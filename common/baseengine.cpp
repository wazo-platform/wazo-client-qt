/* XIVO CTI clients
 * Copyright (C) 2007, 2008  Proformatique
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License version 2 for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Linking the Licensed Program statically or dynamically with other
 * modules is making a combined work based on the Licensed Program. Thus,
 * the terms and conditions of the GNU General Public License version 2
 * cover the whole combination.
 *
 * In addition, as a special exception, the copyright holders of the
 * Licensed Program give you permission to combine the Licensed Program
 * with free software programs or libraries that are released under the
 * GNU Library General Public License version 2.0 or GNU Lesser General
 * Public License version 2.1 or any later version of the GNU Lesser
 * General Public License, and with code included in the standard release
 * of OpenSSL under a version of the OpenSSL license (with original SSLeay
 * license) which is identical to the one that was published in year 2003,
 * or modified versions of such code, with unchanged license. You may copy
 * and distribute such a system following the terms of the GNU GPL
 * version 2 for the Licensed Program and the licenses of the other code
 * concerned, provided that you include the source code of that other code
 * when and as the GNU GPL version 2 requires distribution of source code.
*/

/* $Revision$
 * $Date$
 */

#include <QClipboard>
#include <QCryptographicHash>
#include <QDebug>
#include <QFile>
#include <QMessageBox>
#include <QSettings>
#include <QTcpSocket>
#include <QTime>
#include <QTimerEvent>

#include "JsonToVariant.h"

#include "baseengine.h"
#include "xivoconsts.h"
#include "servercommand.h"

/*! \brief Constructor.
 *
 * Construct the BaseEngine object and load settings.
 * The TcpSocket Object used to communicate with the server
 * is created and connected to the right slots/signals
 *
 * This constructor initialize the UDP socket and
 * the TCP listening socket.
 * It also connects signals with the right slots.
 */
BaseEngine::BaseEngine(QSettings * settings,
                       QObject * parent)
        : QObject(parent),
	  m_serverhost(""), m_loginport(0), m_ctiport(0),
          m_userid(""), m_useridopt(""), m_company(""), m_password(""), m_phonenumber(""),
          m_sessionid(""), m_state(ENotLogged),
          m_pendingkeepalivemsg(0)
{
	m_ka_timerid = 0;
	m_try_timerid = 0;
	m_timer = -1;
	m_sbsocket   = new QTcpSocket(this);
        m_filesocket = new QTcpSocket(this);
        m_settings = settings;
	loadSettings();
	deleteRemovables();

        /*  QTcpSocket signals :
            void connected ()
            void disconnected ()
            void error ( QAbstractSocket::SocketError socketError )
            void hostFound ()
            void stateChanged ( QAbstractSocket::SocketState socketState )
        */
        /* Signals inherited from QIODevice :
           void aboutToClose ()
           void bytesWritten ( qint64 bytes )
           void readyRead ()
        */

	// Socket for TCP connections
        m_sbsocket->setProperty("socket", "cticommands");
	connect( m_sbsocket, SIGNAL(connected()),
                 this, SLOT(socketConnected()));
	connect( m_sbsocket, SIGNAL(disconnected()),
                 this, SLOT(socketDisconnected()));
	connect( m_sbsocket, SIGNAL(hostFound()),
                 this, SLOT(socketHostFound()));
	connect( m_sbsocket, SIGNAL(error(QAbstractSocket::SocketError)),
                 this, SLOT(socketError(QAbstractSocket::SocketError)));
	connect( m_sbsocket, SIGNAL(stateChanged(QAbstractSocket::SocketState)),
                 this, SLOT(socketStateChanged(QAbstractSocket::SocketState)));
	connect( m_sbsocket, SIGNAL(readyRead()),
                 this, SLOT(socketReadyRead()));
        
        m_filesocket->setProperty("socket", "filetransfers");
        connect( m_filesocket, SIGNAL(connected()),
                 this, SLOT(socketConnected()) );
	connect( m_filesocket, SIGNAL(disconnected()),
                 this, SLOT(socketDisconnected()));
	connect( m_filesocket, SIGNAL(hostFound()),
	         this, SLOT(socketHostFound()) );
	connect( m_filesocket, SIGNAL(error(QAbstractSocket::SocketError)),
                 this, SLOT(socketError(QAbstractSocket::SocketError)));
	connect( m_filesocket, SIGNAL(stateChanged(QAbstractSocket::SocketState)),
                 this, SLOT(socketStateChanged(QAbstractSocket::SocketState)));
	connect( m_filesocket, SIGNAL(readyRead()),
                 this, SLOT(socketReadyRead()));
        
	if(m_autoconnect)
		start();
}

/*! \brief Destructor
 */
BaseEngine::~BaseEngine()
{
        qDebug() << "BaseEngine::~BaseEngine()";
}

QSettings * BaseEngine::getSettings()
{
        return m_settings;
}

/*!
 * Load Settings from the registery/configuration file
 * Use default values when settings are not found.
 */
void BaseEngine::loadSettings()
{
        //qDebug() << "BaseEngine::loadSettings()";
	m_systrayed = m_settings->value("display/systrayed", false).toBool();
	m_tablimit = m_settings->value("display/tablimit", 5).toInt();

//         QStringList usergroups = m_settings->childGroups();
//         for(int i = 0 ; i < usergroups.size(); i++) {
//                 QString groupname = usergroups[i];
//                 if (groupname.startsWith("engine")) {
//                         qDebug() << "valid engine name =" << groupname;
//                 }
//         }

        m_settings->beginGroup("engine");
	m_serverhost = m_settings->value("serverhost", "192.168.0.254").toString();
	m_loginport  = m_settings->value("loginport", 5000).toUInt();
	m_ctiport    = m_settings->value("serverport", 5003).toUInt();

	m_userid      = m_settings->value("userid").toString();
	m_useridopt   = m_settings->value("useridopt").toString();
        if(m_useridopt.size() > 0)
                m_useridwithopt = m_userid + "%" + m_useridopt;
        else
                m_useridwithopt = m_userid;
	m_company     = m_settings->value("company").toString();
	m_password    = m_settings->value("password").toString();
	m_loginkind   = m_settings->value("loginkind", 0).toUInt();
	m_keeppass    = m_settings->value("keeppass", 0).toUInt();
	m_phonenumber = m_settings->value("phonenumber").toString();
        setFullId();

	m_autoconnect = m_settings->value("autoconnect", false).toBool();
	m_trytoreconnect = m_settings->value("trytoreconnect", false).toBool();
	m_trytoreconnectinterval = m_settings->value("trytoreconnectinterval", 20*1000).toUInt();
	m_keepaliveinterval = m_settings->value("keepaliveinterval", 20*1000).toUInt();
        m_checked_lastconnwins = m_settings->value("lastconnwins", false).toBool();
	m_availstate = m_settings->value("availstate", "available").toString();
        m_settings->endGroup();
        
        m_settings->beginGroup("user-gui");
	m_historysize = m_settings->value("historysize", 8).toUInt();
	m_contactssize = m_settings->value("contactssize", 45).toUInt();
	m_contactscolumns = m_settings->value("contactscolumns", 2).toUInt();
	m_queuelevels["green"] = m_settings->value("queuelevel-green", 5).toUInt();
	m_queuelevels["orange"] = m_settings->value("queuelevel-orange", 10).toUInt();
	m_queuelevels["red"] = m_settings->value("queuelevel-red", 15).toUInt();
        m_settings->endGroup();
        
        m_settings->beginGroup("user-functions");
        foreach(QString function, CheckFunctions)
                m_checked_function[function] = m_settings->value(function, false).toBool();
        m_settings->endGroup();
}

/*!
 * Save Settings to the registery/configuration file
 */
void BaseEngine::saveSettings()
{
        //qDebug() << "BaseEngine::saveSettings()";
        // information
	m_settings->setValue("version/xivo", __xivo_version__);
	m_settings->setValue("version/svn", __current_client_version__);

	m_settings->setValue("display/systrayed", m_systrayed);
        m_settings->setValue("display/tablimit", m_tablimit);

        // m_settings->beginGroup("engine." + m_userid);
        m_settings->beginGroup("engine");
	m_settings->setValue("serverhost", m_serverhost);
	m_settings->setValue("loginport",  m_loginport);
	m_settings->setValue("serverport", m_ctiport);

	m_settings->setValue("userid",     m_userid);
	m_settings->setValue("useridopt",  m_useridopt);
	m_settings->setValue("company",    m_company);
        if(m_keeppass > 0)
                m_settings->setValue("password", m_password);
        else
                m_settings->remove("password");
	m_settings->setValue("loginkind",  m_loginkind);
	m_settings->setValue("keeppass",   m_keeppass);
	m_settings->setValue("phonenumber", m_phonenumber);

	m_settings->setValue("autoconnect", m_autoconnect);
	m_settings->setValue("trytoreconnect", m_trytoreconnect);
	m_settings->setValue("trytoreconnectinterval", m_trytoreconnectinterval);
	m_settings->setValue("keepaliveinterval", m_keepaliveinterval);
        m_settings->setValue("lastconnwins", m_checked_lastconnwins);
	m_settings->setValue("availstate", m_availstate);
        m_settings->endGroup();
        
        m_settings->beginGroup("user-gui");
	m_settings->setValue("historysize", m_historysize);
	m_settings->setValue("contactssize", m_contactssize);
	m_settings->setValue("contactscolumns", m_contactscolumns);
	m_settings->setValue("queuelevel-green", m_queuelevels["green"]);
	m_settings->setValue("queuelevel-orange", m_queuelevels["orange"]);
	m_settings->setValue("queuelevel-red", m_queuelevels["red"]);
        m_settings->endGroup();
        
        m_settings->beginGroup("user-functions");
        foreach(QString function, CheckFunctions)
                m_settings->setValue(function, m_checked_function[function]);
        m_settings->endGroup();
}

/*!
 *
 */
void BaseEngine::setCheckedFunction(const QString & function, bool b) {
	if(b != m_checked_function[function]) {
		m_checked_function[function] = b;
                if(function == "presence")
                        if((state() == ELogged) && m_enabled_presence) {
                                availAllowChanged(b);
                        }
	}
}

bool BaseEngine::checkedFunction(const QString & function) {
        return m_checked_function[function];
}

void BaseEngine::setEnabledCInfo(bool b) {
        m_enabled_cinfo = b;
}

bool BaseEngine::enabledCInfo() {
        return m_enabled_cinfo;
}

void BaseEngine::setTablimit(int tablimit)
{
	m_tablimit = tablimit;
}

int BaseEngine::tablimit() const
{
	return m_tablimit;
}


void BaseEngine::config_and_start(const QString & login,
                                  const QString & pass,
                                  const QString & phonenum)
{
        setUserId(login);
        setFullId();
        m_password = pass;
        // if phonenum's size is 0, no login as agent
        m_phonenumber = phonenum;
        saveSettings();
        start();
}


/*! \brief Starts the connection to the server
 * This method starts the login process by connection
 * to the server.
 */
void BaseEngine::start()
{
	qDebug() << "BaseEngine::start()" << m_serverhost << m_loginport << m_checked_function;

	// (In case the TCP sockets were attempting to connect ...) aborts them first
	m_sbsocket->abort();

        connectSocket();
}

/*! \brief Closes the connection to the server
 * This method disconnect the engine from the server
 */
void BaseEngine::stop()
{
	qDebug() << "BaseEngine::stop()" << sender() << sender()->property("stopper").toString();
	m_sbsocket->disconnectFromHost();
        
	stopKeepAliveTimer();
	stopTryAgainTimer();
	setState(ENotLogged);
	m_sessionid = "";
        
        m_users.clear();
}

/*! \brief initiate connection to the server
 */
void BaseEngine::connectSocket()
{
        qDebug() << "BaseEngine::connectSocket()" << m_serverhost << m_ctiport;
        m_sbsocket->connectToHost(m_serverhost, m_ctiport);
}

bool BaseEngine::lastconnwins() const
{
        return m_checked_lastconnwins;
}

void BaseEngine::setLastConnWins(bool b)
{
        m_checked_lastconnwins = b;
}

const QStringList & BaseEngine::getCapaXlets() const
{
        return m_capaxlets;
}

const QMap<QString, QVariant> & BaseEngine::getCapaPresence() const
{
        return m_capapresence;
}

void BaseEngine::updateCapaPresence(const QMap<QString, QVariant> & presence)
{
        foreach(QString field, presence.keys())
                if(presence.contains(field))
                        m_capapresence[field] = presence[field];
}

const QStringList & BaseEngine::getCapaFeatures() const
{
        return m_capafeatures;
}

const QString & BaseEngine::getCapaApplication() const
{
        return m_appliname;
}

/*!
 * sets the availability state and call keepLoginAlive() if needed
 */
void BaseEngine::setAvailState(const QString & newstate, bool comesFromServer)
{
        // qDebug() << "BaseEngine::setAvailState" << newstate << comesFromServer;
	if(m_availstate != newstate) {
		m_availstate = newstate;
		m_settings->setValue("engine/availstate", m_availstate);
                if (comesFromServer)
                        changesAvailChecks();
                keepLoginAlive();
	}
}

const QString & BaseEngine::getAvailState() const
{
        return m_availstate;
}

void BaseEngine::setAvailability()
{
        QString availstate = sender()->property("availstate").toString();
	setAvailState(availstate, false);
}

bool BaseEngine::hasFunction(const QString & funcname)
{
        return m_capafuncs.contains(funcname);
}

void BaseEngine::sendCommand(const QString & command)
{
        if(m_sbsocket->state() == QAbstractSocket::ConnectedState)
                m_sbsocket->write((command + "\n").toAscii());
}

/*! \brief parse history command response
 *
 * parse the history command response from the server and
 * trigger the update of the call history panel.
 *
 * \sa Logwidget
 */
void BaseEngine::processHistory(const QStringList & histlist)
{
        foreach (QString hist, histlist) {
                QStringList histline = hist.split(";");
		// DateTime; CallerID; duration; Status?; peer; IN/OUT
		//qDebug() << histlist[i+0] << histlist[i+1]
		//         << histlist[i+2] << histlist[i+3]
		//         << histlist[i+4] << histlist[i+5];
		QDateTime dt = QDateTime::fromString(histline[0], Qt::ISODate);
		int duration = histline[2].toInt();
		QString peer = histline[4];
                QString direction = histline[5];
                QString techdef = histline[6];
		//qDebug() << dt << callerid << duration << peer << direction;
		updateLogEntry(dt, duration, peer, direction, techdef);
	}
}

void BaseEngine::monitorPeerRequest(const QString & userid)
{
        if(m_users.contains(userid)) {
                m_monitored_userid = userid;
                monitorPeer(m_users[userid]);
                m_settings->setValue("monitor/userid", userid);
        }
}

/*! \brief called when the socket is first connected
 */
void BaseEngine::socketConnected()
{
        QString socketname = sender()->property("socket").toString();
	qDebug() << "BaseEngine::socketConnected()" << socketname;
        if(socketname == "cticommands") {
                stopTryAgainTimer();
                /* do the login/identification */
                setMyClientId();
                ServerCommand * sc = new ServerCommand();
                sc->addString("class", "login_id");
                sc->addString("direction", "xivoserver");
                sc->addString("userid", m_userid);
                sc->addString("company", m_company);
                sc->addString("ident", m_clientid);
                sc->addString("version", __current_client_version__);
                sc->addString("xivoversion", __xivo_version__);
                sendCommand(sc->find());
                
        } else if(socketname == "filetransfers") {
                ServerCommand * sc = new ServerCommand();
                sc->addString("class", "filetransfer");
                sc->addString("direction", "xivoserver");
                sc->addString("tdirection", m_filedir);
                sc->addString("fileid", m_fileid);
                if(m_filesocket->state() == QAbstractSocket::ConnectedState)
                        m_filesocket->write((sc->find() + "\n").toAscii());
        }
}

/*! \brief called when the socket is disconnected from the server
 */
void BaseEngine::socketDisconnected()
{
        QString socketname = sender()->property("socket").toString();
        if(socketname == "cticommands") {
                setState(ENotLogged); // calls delogged();
                emitTextMessage(tr("Connection lost with XIVO Daemon"));
                startTryAgainTimer();
                //removePeers();
                //connectSocket();
        }
}

/*! \brief cat host found socket signal
 *
 * This slot is connected to the hostFound() signal of the m_sbsocket
 */
void BaseEngine::socketHostFound()
{
        QString socketname = sender()->property("socket").toString();
        qDebug() << "BaseEngine::socketHostFound()" << socketname;
}

/*! \brief catch socket errors
 */
void BaseEngine::socketError(QAbstractSocket::SocketError socketError)
{
        QString socketname = sender()->property("socket").toString();
        qDebug() << "BaseEngine::socketError()" << socketError << socketname;
	switch(socketError) {
	case QAbstractSocket::ConnectionRefusedError:
		emitTextMessage(tr("Connection refused"));
		if(m_timer != -1) {
			killTimer(m_timer);
		 	m_timer = -1;
                }
		//m_timer = startTimer(2000);
		break;
        case QAbstractSocket::RemoteHostClosedError:
                popupError("connection_closed");
                break;
	case QAbstractSocket::HostNotFoundError:
		emitTextMessage(tr("Host not found"));
		break;
	case QAbstractSocket::UnknownSocketError:
                popupError("socket_error:" + QString::number(socketError));
		break;
	default:
                popupError("socket_error:" + QString::number(socketError));
		break;
	}
}

/*! \brief receive signals of socket state change
 *
 * useless...
 */
void BaseEngine::socketStateChanged(QAbstractSocket::SocketState socketState)
{
        QString socketname = sender()->property("socket").toString();
        if(socketname == "cticommands")
                if(socketState == QAbstractSocket::ConnectedState) {
                        if(m_timer != -1) {
                                killTimer(m_timer);
                                m_timer = -1;
                        }
                        //startTimer(3000);
                }
}


UserInfo * BaseEngine::findUserFromPhone(const QString & astid,
                                         const QString & context,
                                         const QString & tech,
                                         const QString & phonenum)
{
        foreach(UserInfo * uinfo, m_users)
                if(uinfo->hasPhone(astid, context, tech + "/" + phonenum))
                        return uinfo;
        return NULL;
}

UserInfo * BaseEngine::findUserFromAgent(const QString & astid,
                                         const QString & agentnum)
{
        foreach(UserInfo * uinfo, m_users)
                if(uinfo->hasAgent(astid, agentnum))
                        return uinfo;
        return NULL;
}

void BaseEngine::updatePeerAndCallerid(const QStringList & liststatus)
{
	const int nfields0 = 8; // 0th order size (per-phone/line informations)
	const int nfields1 = 6;  // 1st order size (per-channel informations)

	// liststatus[0] is a dummy field, only used for debug on the daemon side
	// p/(asteriskid)/(context)/(protocol)/(phoneid)/(phonenum)
        // ("ful", "xivo", "default", "SIP", "147", "Timeout", "0")

	if(liststatus.count() < nfields0) { // not valid
		qDebug() << "BaseEngine::updatePeerAndCallerid() : Bad data from the server (not enough arguments) :" << liststatus;
		return;
	}
        // BaseEngine::updatePeerAndCallerid() : Bad data from the server (not enough arguments) : ("ful", "xivo-obelisk", "default", "SIP", "100", "Unavailable", "0")

	//<who>:<asterisk_id>:<tech(SIP/IAX/...)>:<phoneid>:<numero>:<contexte>:<dispo>:<etat SIP/XML>:<etat VM>:<etat Queues>: <nombre de liaisons>:
        QString astid    = liststatus[1];
	QString context  = liststatus[2];
        QString tech     = liststatus[3];
	QString phoneid  = liststatus[4];
	QString hintstatus = liststatus[5];

        UserInfo * ui = findUserFromPhone(astid, context, tech, phoneid);
        if(ui) {
                // qDebug() << liststatus;
                ui->updatePhoneStatus(tech + "/" + phoneid, hintstatus);
        }
        
        QHash<QString, QStringList> channs;
        
	int nchans = liststatus[nfields0 - 1].toInt();
	if(liststatus.size() == nfields0 + nfields1 * nchans) {
		for(int i = 0; i < nchans; i++) {
			//  <channel>:<etat du channel>:<nb de secondes dans cet etat>:<to/from>:<channel en liaison>:<numero en liaison>
			// "SIP/103-08248b38" "On the phone" "0" ">" "SIP/103-0827c0b8" "103"
			int refn = nfields0 + nfields1 * i;
                        
                        channs[liststatus[refn]] = (QStringList()
                                                    << liststatus[refn + 1]
                                                    << liststatus[refn + 2]
                                                    << liststatus[refn + 3]
                                                    << liststatus[refn + 4]
                                                    << liststatus[refn + 5]);
                        
			hintstatus = liststatus[refn + 1];
		}
	}
        
        if(ui)
                updatePeer(ui, hintstatus, channs);
}

void BaseEngine::removePeerAndCallerid(const QStringList & liststatus)
{
	const int nfields0 = 10; // 0th order size (per-phone/line informations)
	if(liststatus.count() < nfields0) { // not valid
		qDebug() << "BaseEngine::removePeerAndCallerid() : Bad data from the server :" << liststatus;
		return;
	}

        QString astid   = liststatus[1];
	QString context = liststatus[5];
	QString phonenum = liststatus[4];
        QString tech = "";
        UserInfo * ui = findUserFromPhone(astid, context, tech, phonenum);
        if(ui) {
                removePeer(ui->userid());
                m_users.remove(ui->userid());
        }
}

bool BaseEngine::parseCommand(const QString & line)
{
        JsonQt::JsonToVariant parser;
        QVariant data = parser.parse(line.trimmed());
        QMap<QString, QVariant> datamap = data.toMap();
        QString direction = datamap["direction"].toString();
        
        ServerCommand * sc = new ServerCommand(line.trimmed());
        
        if(direction == "client") {
                QString thisclass = datamap["class"].toString();
                if (thisclass == "callcampaign") {
                        QString payload = sc->find("payload");
                        requestFileListResult(payload);
                } else if (thisclass == "parkcall") {
                        parkingEvent(sc->find("payload"));
                        
                } else if (thisclass == "sheet") {
                        QString payload;
                        QByteArray qba = QByteArray::fromBase64(sc->find("payload").toAscii());
                        if(datamap["compressed"].toString().size() > 0) {
                                payload = QString::fromUtf8(qUncompress(qba));
                        } else {
                                payload = QString::fromUtf8(qba);
                        }
                        // will eventually call the XML parser
                        displayFiche(payload, false);
                        
                } else if (thisclass == "queues") {
                        QString function = datamap["function"].toString();
                        if(function == "sendlist")
                                foreach(QString payload, sc->getSubList("payload"))
                                        if(hasFunction("nojoinleave"))
                                                newQueueList(false, payload);
                                        else
                                                newQueueList(true, payload);
                        else if(function == "update") {
                                setQueueStatus(datamap["payload"].toString());
                        } else if(function == "del") {
                                qDebug() << thisclass << "del" << sc->getString("astid") << sc->getStringList("deltalist");
                        } else if(function == "add") {
                                qDebug() << thisclass << "add" << sc->getString("astid") << sc->getStringList("deltalist");
                        }
                        
                } else if (thisclass == "agents") {
                        QString function = datamap["function"].toString();
                        if(function == "sendlist") {
                                foreach(QString payload, sc->getSubList("payload"))
                                        newAgentList(payload);
                        } else if(function == "update") {
                                QStringList liststatus = datamap["payload"].toStringList();
                                if(liststatus.size() > 2) {
                                        QString astid = liststatus[1];
                                        QString agentid = liststatus[2];
                                        if (agentid.startsWith("Agent/")) {
                                                QString agentnum = agentid.mid(6);
                                                liststatus[2] = agentnum;
                                                UserInfo * ui = findUserFromAgent(astid, agentnum);
                                                if(ui)
                                                        updatePeerAgent(ui->userid(), "agentstatus", liststatus);
                                                else // (useful ?) in order to transfer the replies to unmatched agents
                                                        updatePeerAgent("", "agentstatus", liststatus);
                                        } else
                                                qDebug() << "update-agents agentnum" << agentid;
                                }
                        } else if(function == "del") {
                                qDebug() << thisclass << "del" << sc->getString("astid") << sc->getStringList("deltalist");
                        } else if(function == "add") {
                                qDebug() << thisclass << "add" << sc->getString("astid") << sc->getStringList("deltalist");
                        }
                        
                } else if (thisclass == "agent-status") {
                        QStringList liststatus = datamap["payload"].toStringList();
                        if((liststatus.size() > 1) && (liststatus[0] == m_agent_watched_astid) && (liststatus[1] == m_agent_watched_agentid))
                                changeWatchedAgentSignal(liststatus);
                        
                } else if (thisclass == "queue-status") {
                        QStringList liststatus = datamap["payload"].toStringList();
                        if((liststatus.size() > 1) && (liststatus[0] == m_queue_watched_astid) && (liststatus[1] == m_queue_watched_queueid))
                                changeWatchedQueueSignal(liststatus);
                        
                } else if (thisclass == "history") {
                        // QCryptographicHash histohash(QCryptographicHash::Sha1);
                        // QByteArray res = histohash.hash(command_args.toAscii(), QCryptographicHash::Sha1).toHex();
                        processHistory(datamap["payload"].toStringList());
                        
                } else if (thisclass == "meetme") {
                        meetmeEvent(datamap["payload"].toStringList());
                        
                } else if (thisclass == "serverdown") {
                        qDebug() << thisclass << datamap["mode"].toString();
                        
                } else if (thisclass == "disconn") {
                        qDebug() << thisclass;
                        
                } else if (thisclass == "directory") {
                        directoryResponse(datamap["payload"].toString());
                        
                } else if (thisclass == "faxsend") {
                        m_filedir = datamap["tdirection"].toString();
                        m_fileid = datamap["fileid"].toString();
                        m_filesocket->connectToHost(m_serverhost, m_ctiport);
                        qDebug() << m_filedir << m_fileid;
                        
                } else if (thisclass == "faxprogress") {
                        ackFax(datamap["status"].toString(),
                               datamap["reason"].toString());
                        
                } else if (thisclass == "presence") {
                        QString id = datamap["company"].toString() + "/" + datamap["userid"].toString();
                        //qDebug() << presencestatus << m_users.size();
                        if(m_users.contains(id)) {
                                QString presencestatus = datamap["capapresence"].toMap()["state"].toString();
                                m_users[id]->setAvailState(presencestatus);
                                updatePeerAgent(id, "imstatus", presencestatus.split("/"));
                                updateAgentPresence(m_users[id]->agentid(), presencestatus);
                                if (id == m_fullid) {
                                        updateCapaPresence(datamap["capapresence"].toMap());
                                        updatePresence(m_capapresence);
                                        localUserInfoDefined(m_users[m_fullid]);
                                }
                        }
                        
                } else if (thisclass == "call") {
                        qDebug() << thisclass << "caller =" << sc->find("caller") << "called =" << sc->find("called");
                        
                } else if (thisclass == "users") {
                        QString function = datamap["function"].toString();
                        if (function == "sendlist") {
                                QStringList listpeers = datamap["payload"].toStringList();
                                for(int i = 0 ; i < listpeers.size() ; i += 13) {
                                        // qDebug() << "users-list" << listpeers[i] << listpeers[i+1] << listpeers[i+2]
                                        //<< listpeers[i+3] << listpeers[i+4] << listpeers[i+5]
                                        //<< listpeers[i+6] << listpeers[i+7] << listpeers[i+8] << listpeers[i+9];
                                        QString iduser = listpeers[i+1] + "/" + listpeers[i];
                                        if(! m_users.contains(iduser)) {
                                                m_users[iduser] = new UserInfo(iduser);
                                                m_users[iduser]->setFullName(listpeers[i+2]);
                                                newUser(m_users[iduser]);
                                        }
                                        QString imstatus = listpeers[i+3];
                                        m_users[iduser]->setAvailState(imstatus);
                                        m_users[iduser]->setNumber(listpeers[i+7]);
                                        m_users[iduser]->setPhones(listpeers[i+5], listpeers[i+6], listpeers[i+8]);
                                        m_users[iduser]->setAgent(listpeers[i+9]);
                                        m_users[iduser]->setMWI(listpeers[i+10], listpeers[i+11], listpeers[i+12]);
                                        updatePeerAgent(iduser, "imstatus", imstatus.split("/"));
                                        updateAgentPresence(m_users[iduser]->agentid(), imstatus);
                                }
                                
                                m_monitored_userid = m_fullid;
                                QString fullname_mine = "No One";
                                if(m_users.contains(m_fullid)) {
                                        fullname_mine = m_users[m_fullid]->fullname();
                                        localUserInfoDefined(m_users[m_fullid]);
                                }
                                
                                // Who do we monitor ?
                                // First look at the last monitored one
                                QString fullid_watched = m_settings->value("monitor/userid").toString();
                                QString fullname_watched = "";
                                // If there was nobody, let's watch ourselves.
                                if(fullid_watched.isEmpty()) {
                                        fullid_watched = m_fullid;
                                        fullname_watched = fullname_mine;
                                } else {
                                        if(m_users.contains(fullid_watched))
                                                fullname_watched = m_users[fullid_watched]->fullname();
                                        // If the CallerId value is empty, fallback to ourselves.
                                        if(fullname_watched.isEmpty()) {
                                                fullid_watched = m_fullid;
                                                fullname_watched = fullname_mine;
                                        }
                                }
                        
                                monitorPeerRequest(fullid_watched);
                                emitTextMessage(tr("Received status for %1 users").arg(m_users.size()));
                        } else if (function == "update") {
                                QStringList userupdate = datamap["user"].toStringList();
                                if(userupdate.size() == 2) {
                                        QString iduser = userupdate[0] + "/" + userupdate[1];
                                        if(m_users.contains(iduser) && (iduser == m_fullid)) {
                                                QString subclass = sc->getString("subclass");
                                                if(subclass == "mwi") {
                                                        QStringList payload = datamap["payload"].toStringList();
                                                        m_users[iduser]->setMWI(payload[0], payload[1], payload[2]);
                                                        localUserInfoDefined(m_users[m_fullid]);
                                                }
                                        }
                                }
                        }
                        
                } else if (thisclass == "message") {
                        QStringList message = datamap["payload"].toStringList();
                        // message[0] : emitter name
                        if(message.size() == 2)
                                emitTextMessage(message[0] + tr(" said : ") + message[1]);
                        else
                                emitTextMessage(tr("Unknown") + tr(" said : ") + message[0]);
                        
                } else if (thisclass == "features") {
                        QString function = sc->getString("function");
                        if (function == "update") {
                                QStringList featuresupdate_list = datamap["payload"].toStringList();
                                if(featuresupdate_list.size() == 5)
                                        if(m_monitored_userid == featuresupdate_list[0])
                                                initFeatureFields(featuresupdate_list[1], featuresupdate_list[2]);
                                
                        } else if (function == "get") {
                                QStringList features_list = datamap["payload"].toStringList();
                                if(features_list.size() > 2) {
                                        QString id = features_list[0];
                                        if(id == m_monitored_userid) {
                                                resetFeatures();
                                                if(features_list.size() > 2)
                                                        for(int i = 1 ; i < features_list.size() - 1; i += 2)
                                                                initFeatureFields(features_list[i], features_list[i+1]);
                                                emitTextMessage(tr("Received Services Data for ") + m_monitored_userid);
                                        }
                                }
                                
                        } else if (function == "put") {
                                QStringList features_list = datamap["payload"].toStringList();
                                if(features_list.size() > 1) {
                                        QString id = features_list[0];
                                        if(id == m_monitored_userid) {
                                                QString ret = features_list[1];
                                                if(ret == "OK") {
                                                        featurePutIsOK();
                                                        emitTextMessage("");
                                                } else {
                                                        featurePutIsKO();
                                                        emitTextMessage(tr("Could not modify the Services data.") + " " + tr("Maybe Asterisk is down."));
                                                }
                                        }
                                }
                        }
                        
                } else if (thisclass == "phones") {
                        QString function = datamap["function"].toString();
                        if (function == "sendlist") {
                                QHashIterator<QString, QString> listpeers(sc->getSubHash("payload"));
                                while (listpeers.hasNext()) {
                                        listpeers.next();
                                        QString astid = listpeers.key();
                                        ServerCommand * ssc = new ServerCommand(listpeers.value());
                                        foreach(QString phonestatus, ssc->getSubList("")) {
                                                ServerCommand * ssc2 = new ServerCommand(phonestatus);
                                                qDebug() << astid << ssc2->getStringList("statusbase") << ssc2->getStringList("statusextended");
                                                // updatePeerAndCallerid(liststatus);
                                        }
                                }
                                callsUpdated();
                                peersReceived();
                        } else if (function == "update") {
                                QStringList statusbase = datamap["statusbase"].toStringList();
                                QStringList statusextended = sc->getSubList("statusextended");
                                qDebug() << statusbase << statusextended;
                                // updatePeerAndCallerid(liststatus);
                                callsUpdated();
                        } else if (function == "add") {
                                QStringList listpeers = datamap["payload"].toStringList();
                                for(int i = 1 ; i < listpeers.size() - 1; i++) {
                                        QStringList liststatus = listpeers[i].split(":");
                                        updatePeerAndCallerid(liststatus);
                                }
                                if(listpeers[0] == "0") {
                                        qDebug() << "phones-add completed";
                                } else {
                                        sendCommand("phones-add " + listpeers[0]);
                                }
                        } else if (function == "del") {
                                QStringList listpeers = datamap["payload"].toStringList();
                                for(int i = 1 ; i < listpeers.size() - 1; i++) {
                                        QStringList liststatus = listpeers[i].split(":");
                                        removePeerAndCallerid(liststatus);
                                }
                                if(listpeers[0] == "0") {
                                        qDebug() << "phones-del completed";
                                } else {
                                        sendCommand("phones-del " + listpeers[0]);
                                }
                        } else if (function == "signal-deloradd") {
                                QStringList listpeers = datamap["payload"].toStringList();
                                // qDebug() << "phones-signal-deloradd" << listpeers;
                                //emitTextMessage(tr("New phone list on %1 : - %2 + %3 = %4 total").arg(listpeers[0],
                                //listpeers[1],
                                //listpeers[2],
                                //listpeers[3]));
                                if(listpeers[1].toInt() > 0)
                                        sendCommand("phones-del");
                                if(listpeers[2].toInt() > 0)
                                        sendCommand("phones-add");
                        }
                        
                } else if (thisclass == "login_id_ok") {
                        
                        m_version_server = sc->getString("version").toInt();
                        m_xivover_server = sc->getString("xivoversion");
                        
                        if(m_version_server < REQUIRED_SERVER_VERSION) {
                                stop();
                                popupError("version_server:" + QString::number(m_version_server) + ";" + QString::number(REQUIRED_SERVER_VERSION));
                        } else {
                                QString tohash = sc->getString("sessionid") + ":" + m_password;
                                QCryptographicHash hidepass(QCryptographicHash::Sha1);
                                QByteArray res = hidepass.hash(tohash.toAscii(), QCryptographicHash::Sha1).toHex();
                                ServerCommand * sc2 = new ServerCommand();
                                sc2->addString("class", "login_pass");
                                sc2->addString("direction", "xivoserver");
                                sc2->addString("hashedpassword", res);
                                sendCommand(sc2->find());
                        }
                        
                } else if (thisclass == "loginko") {
                        stop();
                        popupError(sc->getString("errorstring"));
                        
                } else if (thisclass == "login_pass_ok") {
                        QStringList capas = datamap["capalist"].toString().split(",");
                        ServerCommand * sc2 = new ServerCommand();
                        sc2->addString("class", "login_capas");
                        sc2->addString("direction", "xivoserver");
                        if (capas.size() == 1)
                                sc2->addString("capaid", capas[0]);
                        else {
                                if(m_useridopt.size() > 0) {
                                        if(capas.contains(m_useridopt))
                                                sc2->addString("capaid", m_useridopt);
                                        else
                                                sc2->addString("capaid", capas[0]);
                                } else
                                        sc2->addString("capaid", capas[0]);
                        }
                        
                        if(m_loginkind > 0) {
                                sc2->addString("loginkind", "agent");
                                sc2->addString("phonenumber", m_phonenumber);
                        } else
                                sc2->addString("loginkind", "user");
                        if(m_checked_function["presence"])
                                sc2->addString("state", m_availstate);
                        else
                                sc2->addString("state", __nopresence__);
                        if(m_checked_lastconnwins)
                                sc2->addString("lastconnwins", "true");
                        else
                                sc2->addString("lastconnwins", "false");
                        sendCommand(sc2->find());

                } else if (thisclass == "login_capas_ok") {
                        m_capafuncs = datamap["capafuncs"].toString().split(",");
                        m_capaxlets = datamap["capaxlets"].toStringList();
                        m_appliname = datamap["appliname"].toString();
                        updateCapaPresence(datamap["capapresence"].toMap());
                        m_forced_state = datamap["capapresence"].toMap()["state"].toString();
                        // m_capafeatures = sc->getStringList("capas_features");
                        
                        qDebug() << "clientXlets" << XletList;
                        qDebug() << "m_capaxlets" << m_capaxlets;
                        qDebug() << "m_capafuncs" << m_capafuncs;
                        qDebug() << "m_appliname" << m_appliname;

                        // XXXX m_capafuncs => config file
                        foreach(QString function, CheckFunctions)
                                if(! hasFunction(function)) {
                                        m_checked_function[function] = false;
                                        m_settings->remove("functions/" + function);
                                }
                        
                        if((m_capafuncs.size() == 1) && (m_capafuncs[0].size() == 0)) {
                                stop();
                                popupError("no_capability");
                        } else {
                                setState(ELogged); // calls logged()
                                setAvailState(m_forced_state, true);
                                m_ka_timerid = startTimer(m_keepaliveinterval);
                                askCallerIds();
                        }

                } else {
                        qDebug() << "unknown server command class" << thisclass;
                }
        }
        
        return true;
}

void BaseEngine::agentAction(const QString & action)
{
        // qDebug() << "BaseEngine::agentAction" << action;
        ServerCommand * sc = new ServerCommand();
        sc->addString("class", "agent");
        sc->addString("direction", "xivoserver");
        sc->addStringList("command", action.split(" "));
        sendCommand(sc->find());
}

void BaseEngine::meetmeAction(const QString & action)
{
        ServerCommand * sc = new ServerCommand();
        sc->addString("class", "meetme");
        sc->addString("direction", "xivoserver");
        sc->addStringList("command", action.split(" "));
        sendCommand(sc->find());
}

void BaseEngine::requestFileList(const QString & action)
{
        ServerCommand * sc = new ServerCommand();
        sc->addString("class", "callcampaign");
        sc->addString("direction", "xivoserver");
        sc->addStringList("command", action.split(" "));
        sendCommand(sc->find());
}

void BaseEngine::sendFaxCommand(const QString & filename,
                                const QString & number,
                                Qt::CheckState hide)
{
        QFile * qf = new QFile(filename);
        qf->open(QIODevice::ReadOnly);
        m_filedata = new QByteArray();
        m_filedata->append(qf->readAll());
        qf->close();
        m_faxsize = m_filedata->size();
        if(m_filedata->size() > 0) {
                ServerCommand * sc = new ServerCommand();
                sc->addString("class", "faxsend");
                sc->addString("direction", "xivoserver");
                sc->addString("size", QString::number(m_faxsize));
                sc->addString("number", number);
                sc->addString("hide", QString::number(hide));
                sendCommand(sc->find());
        } else
                ackFax("ko", "file");
}

void BaseEngine::popupError(const QString & errorid)
{
        QString errormsg = QString(tr("Server has sent an Error."));
        if(errorid.toLower() == "connection_refused")
                errormsg = tr("You are not allowed to connect to the XIVO daemon on %1.").arg(m_serverhost);

        else if(errorid.toLower() == "number_of_arguments")
                errormsg = tr("The number of arguments sent is incorrect.\n"
                              "Maybe a version issue ?");

        else if(errorid.toLower() == "user_not_found")
                errormsg = tr("Your registration name <%1@%2> is not known.").arg(m_userid, m_company);

        else if(errorid.toLower() == "session_expired")
                errormsg = tr("Your session has expired.");

        else if(errorid.toLower() == "login_password")
                errormsg = tr("You entered a wrong login / password.");

        else if(errorid.toLower() == "no_keepalive_from_server")
                errormsg = tr("The XIVO daemon on %1 did not reply to the last keepalive.").arg(m_serverhost);

        else if(errorid.toLower() == "connection_closed")
                errormsg = tr("The XIVO daemon on %1 has just closed the connection.").arg(m_serverhost);
        
        else if(errorid.startsWith("socket_error:")) {
                QStringList ipinfo = errorid.split(":");
                errormsg = tr("Socket Error number %1.").arg(ipinfo[1]);
        }
        
        else if(errorid.toLower() == "server_stopped")
                errormsg = tr("The XIVO daemon on %1 has just been stopped.").arg(m_serverhost);
        
        else if(errorid.toLower() == "server_reloaded")
                errormsg = tr("The XIVO daemon on %1 has just been reloaded.").arg(m_serverhost);
        
        else if(errorid.startsWith("already_connected:")) {
                QStringList ipinfo = errorid.split(":");
                errormsg = tr("You are already connected from %1:%2.").arg(ipinfo[1], ipinfo[2]);
        }

        else if(errorid.toLower() == "uninit_phone")
                errormsg = tr("Your phone <%1> has not been provisioned on XIVO.").arg(m_userid);

        else if(errorid.toLower() == "no_capability")
                errormsg = tr("No capability allowed.");

        else if(errorid.startsWith("xcusers:")) {
                QStringList userslist = errorid.split(":")[1].split(";");
                errormsg = tr("Max number (%1) of XIVO Clients already reached.").arg(userslist[0]);
        }
        else if(errorid.startsWith("sbusers:")) {
                QStringList userslist = errorid.split(":")[1].split(";");
                errormsg = tr("Max number (%1) of XIVO Switchboards already reached.").arg(userslist[0]);
        }
        else if(errorid.startsWith("missing:")) {
                errormsg = tr("Missing Argument(s)");
        }
        else if(errorid.startsWith("version_client:")) {
                QStringList versionslist = errorid.split(":")[1].split(";");
                if(versionslist.size() >= 2)
                        errormsg = tr("Your client version (%1) is too old for this server.\n"
                                      "Please upgrade it to %2 at least.").arg(__current_client_version__,
                                                                               versionslist[1]);
                else
                        errormsg = tr("Your client version (%1) is too old for this server.\n"
                                      "Please upgrade it.").arg(__current_client_version__);
        }
        else if(errorid.startsWith("version_server:")) {
                QStringList versionslist = errorid.split(":")[1].split(";");
                if(versionslist.size() >= 2)
                        errormsg = tr("Your server version (%1) is too old for this client.\n"
                                      "Please upgrade it to %2 at least.").arg(versionslist[0], __current_client_version__);
                else
                        errormsg = tr("Your server version (%1) is too old for this client.\n"
                                      "Please upgrade it.").arg(versionslist[0]);
        }

        // logs a message before sending any popup that would block
        emitTextMessage(tr("Error") + " : " + errormsg);
        if(! m_trytoreconnect)
                QMessageBox::critical(NULL, tr("XIVO CTI Error"), errormsg);
}

/*! \brief called when data are ready to be read on the socket.
 *
 * Read and process the data from the server.
 */
void BaseEngine::socketReadyRead()
{
        // qDebug() << "BaseEngine::socketReadyRead()";
        QString socketname = sender()->property("socket").toString();
        if(socketname == "cticommands")
                while(m_sbsocket->canReadLine()) {
                        QByteArray data  = m_sbsocket->readLine();
                        // qDebug() << "BaseEngine::socketReadyRead() data.size() = " << data.size();
                        QString line = QString::fromUtf8(data);
                        
                        if(line.startsWith("<ui version=")) {
                                // we get here when receiving a sheet as a Qt4 .ui form
                                qDebug() << "BaseEngine::socketReadyRead() (Customer Info)" << line.size();
                                displayFiche(line, true);
                        } else
                                parseCommand(line);
                }
        else if(socketname == "filetransfers") {
                while(m_filesocket->canReadLine()) {
                        QByteArray data = m_filesocket->readLine();
                        QString line = QString::fromUtf8(data);
                        ServerCommand * sc = new ServerCommand(line.trimmed());
                        
                        if(sc->getString("class") == "fileref") {
                                if(m_filedir == "download") {
                                        qDebug() << sc->getString("filename");
                                        qDebug() << sc->getString("payload").size();
                                } else {
                                        qDebug() << "sending fax contents" << sc->getString("fileid");
                                        if(m_faxsize > 0)
                                                m_filesocket->write(* m_filedata);
                                        delete m_filedata;
                                }
                                m_filesocket->close();
                                m_faxsize = 0;
                                m_fileid = "";
                        }
                }
        }
}

void BaseEngine::actionFromFiche(const QStringList & infos)
{
        qDebug() << "BaseEngine::actionFromFiche" << infos;
        ServerCommand * sc = new ServerCommand();
        sc->addString("class", "actionfiche");
        sc->addString("direction", "xivoserver");
        sc->addStringList("buttonaction", infos);
        sendCommand(sc->find());
}

/*! \brief transfers to the typed number
 */
void BaseEngine::transferToNumber(const QString & chan)
{
        if(m_numbertodial.size() > 0) {
                qDebug() << "BaseEngine::transferToNumber()" << chan << m_numbertodial;
                transferCall(chan, m_numbertodial);
        }
}

/*! \brief send an originate command to the server
 */
void BaseEngine::textEdited(const QString & text)
{
        m_numbertodial = text;
}

void BaseEngine::copyNumber(const QString & dst)
{
        pasteToDialPanel(dst);
}

/*! \brief send an originate command to the server
 */
void BaseEngine::originateCall(const QString & src, const QString & dst)
{
	// qDebug() << "BaseEngine::originateCall()" << src << dst;
        ServerCommand * sc = new ServerCommand();
        sc->addString("class", "originate");
        sc->addString("direction", "xivoserver");
        sc->addString("source", src);
        sc->addString("destination", dst);
        sendCommand(sc->find());
}

/*! \brief send a transfer call command to the server
 */
void BaseEngine::transferCall(const QString & src, const QString & dst)
{
	// qDebug() << "BaseEngine::transferCall()" << src << dst;
        ServerCommand * sc = new ServerCommand();
        sc->addString("class", "transfer");
        sc->addString("direction", "xivoserver");
        sc->addString("source", src);
        sc->addString("destination", dst);
        sendCommand(sc->find());
}

/*! \brief send an attended transfer call command to the server
 */
void BaseEngine::atxferCall(const QString & src, const QString & dst)
{
	// qDebug() << "BaseEngine::atxferCall()" << src << dst;
        ServerCommand * sc = new ServerCommand();
        sc->addString("class", "atxfer");
        sc->addString("direction", "xivoserver");
        sc->addString("source", src);
        sc->addString("destination", dst);
        sendCommand(sc->find());
}

/*! \brief send a transfer call command to the server
 */
void BaseEngine::parkCall(const QString & src)
{
	// qDebug() << "BaseEngine::parkCall()" << src;
        QStringList srclist = src.split("/");
        ServerCommand * sc = new ServerCommand();
        sc->addString("class", "transfer");
        sc->addString("direction", "xivoserver");
        sc->addString("source", src);
        sc->addString("destination",
                      "p/" + srclist[1] + "/" + srclist[2] + "///special:parkthecall");
        sendCommand(sc->find());
}

/*! \brief intercept a call (a channel)
 *
 * The channel is transfered to "Me"
 *
 * \sa transferCall
 */
void BaseEngine::interceptCall(const UserInfo *, const QString & channel)
{
	// qDebug() << "BaseEngine::interceptCall()" << ui << channel;
        ServerCommand * sc = new ServerCommand();
        sc->addString("class", "transfer");
        sc->addString("direction", "xivoserver");
        sc->addString("source", channel);
        sc->addString("destination", "user:special:me");
        sendCommand(sc->find());
}

/*! \brief hang up a channel
 *
 * send a hang up command to the server
 */
void BaseEngine::hangupCall(const UserInfo * ui, const QString & channel)
{
	// qDebug() << "BaseEngine::hangupCall()" << ui << channel;
        ServerCommand * sc = new ServerCommand();
        sc->addString("class", "hangup");
        sc->addString("direction", "xivoserver");
        sc->addString("astid", ui->astid());
        sc->addString("channel", channel);
        sendCommand(sc->find());
}

/*! \brief hang up a channel
 *
 * send a hang up command to the server
 */
void BaseEngine::simplehangupCall(const UserInfo * ui, const QString & channel)
{
	// qDebug() << "BaseEngine::simplehangupCall()" << channel;
        ServerCommand * sc = new ServerCommand();
        sc->addString("class", "simplehangup");
        sc->addString("direction", "xivoserver");
        sc->addString("astid", ui->astid());
        sc->addString("channel", channel);
        sendCommand(sc->find());
}

/*! \brief pick up a channel
 *
 * send a pick up command to the server
 */
void BaseEngine::pickUp(const UserInfo * ui)
{
	// qDebug() << "BaseEngine::pickUp()" << ui;
        ServerCommand * sc = new ServerCommand();
        sc->addString("class", "pickup");
        sc->addString("direction", "xivoserver");
        sc->addString("astid", ui->astid());
        sc->addString("phonenum", ui->phonenum());
        sendCommand(sc->find());
}

/*! \brief send the directory search command to the server
 *
 * \sa directoryResponse()
 */
void BaseEngine::searchDirectory(const QString & text)
{
        // qDebug() << "BaseEngine::searchDirectory()" << text;
        ServerCommand * sc = new ServerCommand();
        sc->addString("class", "directory-search");
        sc->addString("direction", "xivoserver");
        sc->addString("pattern", text);
        sendCommand(sc->find());
}

/*! \brief ask history for an extension 
 */
void BaseEngine::requestHistory(const QString & peer, int mode)
{
	/* mode = 0 : Out calls
	 * mode = 1 : In calls
	 * mode = 2 : Missed calls */
	if(mode >= 0) {
                // qDebug() << "BaseEngine::requestHistory()" << peer;
                ServerCommand * sc = new ServerCommand();
                sc->addString("class", "history");
                sc->addString("direction", "xivoserver");
                sc->addString("peer", peer);
                sc->addString("size", QString::number(m_historysize));
                sc->addString("mode", QString::number(mode));
                sendCommand(sc->find());
        }
}

// === Getter and Setters ===
/* \brief set server address
 *
 * Set server host name and server port
 */
void BaseEngine::setAddress(const QString & host, quint16 port)
{
        qDebug() << "BaseEngine::setAddress()" << port;
	m_serverhost = host;
	m_ctiport = port;
}

/*! \brief get server IP address */
const QString & BaseEngine::serverip() const
{
	return m_serverhost;
}

/*! \brief get server port */
const quint16 & BaseEngine::sbPort() const
{
	return m_ctiport;
}

const QString & BaseEngine::company() const
{
        return m_company;
}

void BaseEngine::setServerip(const QString & serverip)
{
	m_serverhost = serverip;
}

void BaseEngine::setCompany(const QString & companyname)
{
	m_company = companyname;
}

const quint16 & BaseEngine::loginPort() const
{
	return m_loginport;
}

void BaseEngine::setLoginPort(const quint16 & port)
{
	m_loginport = port;
}

const QString & BaseEngine::userId() const
{
        if(m_useridopt.size() > 0)
                return m_useridwithopt;
        else
                return m_userid;
}

void BaseEngine::setUserId(const QString & userid)
{
        QStringList useridsplit = userid.split("%");
	m_userid = useridsplit[0];
        m_useridwithopt = userid;
        if(useridsplit.size() > 1)
                m_useridopt = useridsplit[1];
        else
                m_useridopt = "";
}

void BaseEngine::setFullId()
{
        m_fullid = m_company + "/" + m_userid;
}

const QString & BaseEngine::phonenumber() const
{
	return m_phonenumber;
}

void BaseEngine::setPhonenumber(const QString & phonenumber)
{
	m_phonenumber = phonenumber;
}

const int & BaseEngine::loginkind() const
{
	return m_loginkind;
}

void BaseEngine::setLoginKind(const int loginkind)
{
	m_loginkind = loginkind;
}

const int & BaseEngine::keeppass() const
{
	return m_keeppass;
}

void BaseEngine::setKeepPass(const int keeppass)
{
	m_keeppass = keeppass;
}

const QString & BaseEngine::password() const
{
	return m_password;
}

void BaseEngine::setPassword(const QString & password)
{
	m_password = password;
}

void BaseEngine::setTrytoreconnect(bool b)
{
	m_trytoreconnect = b;
}

bool BaseEngine::trytoreconnect() const
{
	return m_trytoreconnect;
}

void BaseEngine::initFeatureFields(const QString & field, const QString & value)
{
        //        qDebug() << field << value;
        bool isenabled = (value.split(":")[0] == "1");
	if((field == "enablevoicemail") || (field == "vm"))
		optChanged("enablevm", isenabled);
	else if((field == "enablednd") || (field == "dnd"))
		optChanged("enablednd", isenabled);
	else if(field == "callfilter")
		optChanged("incallfilter", isenabled);
	else if(field == "callrecord")
		optChanged("incallrec", isenabled);
	else if((field == "unc") || (field == "busy") || (field == "rna"))
		forwardUpdated(field, isenabled, value.split(":")[1]);
}

void BaseEngine::stopKeepAliveTimer()
{
	if( m_ka_timerid > 0 ) {
		killTimer(m_ka_timerid);
		m_ka_timerid = 0;
	}
}

void BaseEngine::stopTryAgainTimer()
{
	if( m_try_timerid > 0 ) {
		killTimer(m_try_timerid);
		m_try_timerid = 0;
	}
}

void BaseEngine::startTryAgainTimer()
{
	if( m_try_timerid == 0 && m_trytoreconnect )
		m_try_timerid = startTimer(m_trytoreconnectinterval);
}

void BaseEngine::setHistorySize(uint size)
{
	m_historysize = size;
}

uint BaseEngine::historySize() const
{
	return m_historysize;
}

void BaseEngine::setContactsSize(uint size)
{
	m_contactssize = size;
}

uint BaseEngine::contactsSize() const
{
	return m_contactssize;
}

void BaseEngine::setContactsColumns(uint columns)
{
	m_contactscolumns = columns;
}

uint BaseEngine::contactsColumns() const
{
	return m_contactscolumns;
}

void BaseEngine::setQueueLevel(const QString & field, uint value)
{
	m_queuelevels[field] = value;
}

uint BaseEngine::queueLevel(const QString & field) const
{
	return m_queuelevels[field];
}

uint BaseEngine::trytoreconnectinterval() const
{
	return m_trytoreconnectinterval;
}

/*!
 * Setter for property m_trytoreconnectinterval
 * Restart timer if the value changed.
 *
 * \sa trytoreconnectinterval
 */
void BaseEngine::setTrytoreconnectinterval(uint i)
{
	if( m_trytoreconnectinterval != i )
	{
		m_trytoreconnectinterval = i;
		if(m_try_timerid > 0)
		{
			killTimer(m_try_timerid);
			m_try_timerid = startTimer(m_trytoreconnectinterval);
		}
	}
}

/*! \brief implement timer event
 *
 * does nothing
 */
void BaseEngine::timerEvent(QTimerEvent * event)
{
	int timerId = event->timerId();
        // qDebug() << "BaseEngine::timerEvent() timerId's" << timerId << m_ka_timerid << m_try_timerid;
	if(timerId == m_ka_timerid) {
                keepLoginAlive();
                event->accept();
        } else if(timerId == m_try_timerid) {
                emitTextMessage(tr("Attempting to reconnect to server"));
		start();
		event->accept();
	} else {
		event->ignore();
                qDebug() << "BaseEngine::timerEvent() ghost timer" << timerId << "will be stopped";
                killTimer(timerId);
	}
}

void BaseEngine::deleteRemovables()
{
	m_removable.clear();
}

void BaseEngine::addRemovable(const QMetaObject * metaobject)
{
	m_removable.append(metaobject);
}

bool BaseEngine::isRemovable(const QMetaObject * metaobject)
{
	for(int i = 0; i < m_removable.count() ; i++)
		if (metaobject == m_removable[i])
			return true;
	return false;
}

void BaseEngine::featurePutOpt(const QString & capa, bool b)
{
        ServerCommand * sc = new ServerCommand();
        sc->addString("class", "featuresput");
        sc->addString("direction", "xivoserver");
        sc->addString("userid", m_monitored_userid);
        if(capa == "enablevm")
                sc->addString("function", "enablevoicemail");
        else if(capa == "incallrec")
                sc->addString("function", "callrecord");
        else if(capa == "incallfilter")
                sc->addString("function", "callfilter");
        else if(capa == "enablednd")
                sc->addString("function", "enablednd");
        sc->addString("value", QString(b ? "1" : "0"));
        sendCommand(sc->find());
}

void BaseEngine::featurePutForward(const QString & capa, bool b, const QString & dst)
{
        ServerCommand * sc = new ServerCommand();
        sc->addString("class", "featuresput");
        sc->addString("direction", "xivoserver");
        sc->addString("userid", m_monitored_userid);
        if(capa == "fwdunc")
                sc->addString("function", "enableunc");
        else if(capa == "fwdbusy")
                sc->addString("function", "enablebusy");
        else if(capa == "fwdrna")
                sc->addString("function", "enablerna");
        sc->addString("value", QString(b ? "1" : "0"));
        sc->addString("destination", dst);
        sendCommand(sc->find());
}

void BaseEngine::askFeatures()
{
        qDebug() << "BaseEngine::askFeatures()" << m_monitored_userid;
        QString featurestoget = "user:special:me";
        if(m_monitored_userid.size() > 0)
                featurestoget = m_monitored_userid;
        ServerCommand * sc = new ServerCommand();
        sc->addString("class", "featuresget");
        sc->addString("direction", "xivoserver");
        sc->addString("userid", featurestoget);
        sendCommand(sc->find());
}

void BaseEngine::askCallerIds()
{
        qDebug() << "BaseEngine::askCallerIds()";
        ServerCommand * sc1 = new ServerCommand();
        sc1->addString("class", "users");
        sc1->addString("function", "getlist");
        sc1->addString("direction", "xivoserver");
        ServerCommand * sc2 = new ServerCommand();
        sc2->addString("class", "phones");
        sc2->addString("function", "getlist");
        sc2->addString("direction", "xivoserver");
        ServerCommand * sc3 = new ServerCommand();
        sc3->addString("class", "queues");
        sc3->addString("function", "getlist");
        sc3->addString("direction", "xivoserver");
        ServerCommand * sc4 = new ServerCommand();
        sc4->addString("class", "agents");
        sc4->addString("function", "getlist");
        sc4->addString("direction", "xivoserver");
        
        sendCommand(sc1->find());
        sendCommand(sc3->find());
        sendCommand(sc4->find());
        sendCommand(sc2->find());
        sendCommand(sc1->find());
}

void BaseEngine::setSystrayed(bool b)
{
	m_systrayed = b;
}

bool BaseEngine::systrayed() const
{
	return m_systrayed;
}

void BaseEngine::setAutoconnect(bool b)
{
	m_autoconnect = b;
}

bool BaseEngine::autoconnect() const
{
	return m_autoconnect;
}

uint BaseEngine::keepaliveinterval() const
{
	return m_keepaliveinterval;
}

/*!
 * Setter for the m_keepaliveinterval property.
 * if the value is changed, existing timer is restarted.
 *
 * \sa keepaliveinterval
 */
void BaseEngine::setKeepaliveinterval(uint i)
{
	if(i != m_keepaliveinterval)
	{
		m_keepaliveinterval = i;
		if(m_ka_timerid > 0)
		{
			killTimer(m_ka_timerid);
			m_ka_timerid = startTimer(m_keepaliveinterval);
		}
	}
}

const BaseEngine::EngineState & BaseEngine::state() const
{
	return m_state;
}

/*!
 * setter for the m_state property.
 * If the state is becoming ELogged, the
 * signal logged() is thrown.
 * If the state is becoming ENotLogged, the
 * signal delogged() is thrown.
 */
void BaseEngine::setState(EngineState state)
{
	if(state != m_state) {
		m_state = state;
		if(state == ELogged) {
                        m_enabled_presence = hasFunction("presence");
			stopTryAgainTimer();
			if(m_checked_function["presence"] && m_enabled_presence)
                                availAllowChanged(true);
			logged();
                        updatePresence(m_capapresence);
		} else if(state == ENotLogged) {
                        m_enabled_presence = false;
			availAllowChanged(false);
			delogged();
		}
	}
}

void BaseEngine::changeWatchedAgentSlot(const QString & astagentid)
{
        // qDebug() << "BaseEngine::changeWatchedAgentSlot()" << astagentid;
        m_agent_watched_astid = astagentid.split(" ")[0];
        m_agent_watched_agentid = astagentid.split(" ")[1];
        ServerCommand * sc = new ServerCommand();
        sc->addString("class", "agent-status");
        sc->addString("direction", "xivoserver");
        sc->addString("astid", m_agent_watched_astid);
        sc->addString("agentid", m_agent_watched_agentid);
        sendCommand(sc->find());
}

void BaseEngine::changeWatchedQueueSlot(const QString & astqueueid)
{
        // qDebug() << "BaseEngine::changeWatchedQueueSlot()" << astqueueid;
        m_queue_watched_astid = astqueueid.split(" ")[0];
        m_queue_watched_queueid = astqueueid.split(" ")[1];
        ServerCommand * sc = new ServerCommand();
        sc->addString("class", "queue-status");
        sc->addString("direction", "xivoserver");
        sc->addString("astid", m_queue_watched_astid);
        sc->addString("queuename", m_queue_watched_queueid);
        sendCommand(sc->find());
}

void BaseEngine::setOSInfos(const QString & osname)
{
        m_osname = osname;
}

/*!
 * Builds a string defining who is the client (SB or XC @ osname)
*/
void BaseEngine::setMyClientId()
{
        m_clientid = "undef@" + m_osname;
}

/*!
 * Send a keep alive message to the login server.
 * The message is sent in a datagram through m_udpsocket
 */ 
void BaseEngine::keepLoginAlive()
{
	// qDebug() << "BaseEngine::keepLoginAlive()" << m_availstate;
	// got to disconnected state if more than xx keepalive messages
	// have been left without response.
	if(m_pendingkeepalivemsg > 1) {
		qDebug() << "m_pendingkeepalivemsg" << m_pendingkeepalivemsg << "=> 0";
		stopKeepAliveTimer();
		setState(ENotLogged);
		m_pendingkeepalivemsg = 0;
                popupError("no_keepalive_from_server");
		startTryAgainTimer();
		return;
	}

        ServerCommand * sc = new ServerCommand();
        sc->addString("class", "availstate");
        sc->addString("direction", "xivoserver");
        sc->addString("availstate", m_availstate);
        sendCommand(sc->find());
}

/*!
 * Send a keep alive message to the login server.
 * The message is sent in a datagram through m_udpsocket
 */ 
void BaseEngine::sendMessage(const QString & message)
{
        ServerCommand * sc = new ServerCommand();
        sc->addString("class", "message");
        sc->addString("direction", "xivoserver");
        sc->addString("message", message);
        sendCommand(sc->find());
}

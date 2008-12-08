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

#include <QCryptographicHash>
#include <QDateTime>
#include <QDebug>
#include <QFile>
#include <QSettings>
#include <QSocketNotifier>
#include <QTcpSocket>
#include <QTimerEvent>

#include "JsonToVariant.h"
#include "VariantToJson.h"
#ifdef USE_OUTLOOK
#include "outlook_engine.h"
#endif

#include "baseengine.h"
#include "xivoconsts.h"

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
	  m_serverhost(""), m_ctiport(0),
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
        
//         m_eventdevice = new QFile("/dev/input/event1");
//         m_eventdevice->open(QIODevice::ReadOnly);
//         m_notifier = new QSocketNotifier(m_eventdevice->handle(), QSocketNotifier::Read, this);
//         connect(m_notifier, SIGNAL(activated(int)),
//                 this, SLOT(readInputEvent(int)));
        
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
        
#ifdef USE_OUTLOOK
	OLEngine()->start_thread();
#endif
        
	if(m_autoconnect)
		start();
}

void BaseEngine::readInputEvent(int) {
        // qDebug() << "BaseEngine::readInputEvent()" << r;
        if(m_eventdevice->isReadable()) {
                QByteArray qba = m_eventdevice->read(1024);
                qDebug() << "BaseEngine::readInputEvent()" << qba.size();
        }
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
        
        m_settings->beginGroup("engine");
	m_serverhost = m_settings->value("serverhost", "demo.xivo.fr").toString();
	m_ctiport    = m_settings->value("serverport", 5003).toUInt();
        
	m_userid      = m_settings->value("userid").toString().trimmed();
	m_useridopt   = m_settings->value("useridopt").toString().trimmed();
        if(m_useridopt.size() > 0)
                m_useridwithopt = m_userid + "%" + m_useridopt;
        else
                m_useridwithopt = m_userid;
	m_company      = m_settings->value("company", "default").toString();
	m_password     = m_settings->value("password").toString();
	m_keeppass     = m_settings->value("keeppass", 0).toUInt();
	m_showagselect = m_settings->value("showagselect", 0).toUInt();
	m_phonenumber  = m_settings->value("phonenumber").toString();
        
	m_autoconnect = m_settings->value("autoconnect", false).toBool();
	m_trytoreconnect = m_settings->value("trytoreconnect", false).toBool();
	m_trytoreconnectinterval = m_settings->value("trytoreconnectinterval", 20*1000).toUInt();
	m_keepaliveinterval = m_settings->value("keepaliveinterval", 20*1000).toUInt();
        m_checked_lastconnwins = m_settings->value("lastconnwins", false).toBool();
	m_availstate = m_settings->value("availstate", "available").toString();
        m_settings->endGroup();
        
        m_settings->beginGroup("user-gui");
	m_historysize = m_settings->value("historysize", 8).toUInt();
        
        QString defaultguioptions = "{\"contacts-max\":30,"
                "\"contacts-width\":4,"
                "\"sheet-tablimit\":4,"
                "\"loginkind\":0,"
                "\"autourl_allowed\":0,"
                "\"queuelevels\":{\"green\":0,\"orange\":4,\"red\":12}}";
        QVariant data;
        try {
                data = JsonQt::JsonToVariant::parse(defaultguioptions);
        }
        catch(JsonQt::ParseException) {
                qDebug() << "BaseEngine::loadSettings() exception catched for" << defaultguioptions;
        }
        m_guioptions["user"] = m_settings->value("guisettings", data);
        m_loginkind = m_guioptions["user"].toMap()["loginkind"].toInt();
        m_settings->endGroup();
        
        m_settings->beginGroup("user-functions");
        foreach (QString function, CheckFunctions)
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
        
        // m_settings->beginGroup("engine." + m_userid);
        m_settings->beginGroup("engine");
	m_settings->setValue("serverhost", m_serverhost);
	m_settings->setValue("serverport", m_ctiport);

	m_settings->setValue("userid",     m_userid);
	m_settings->setValue("useridopt",  m_useridopt);
	m_settings->setValue("company",    m_company);
        if(m_keeppass > 0)
                m_settings->setValue("password", m_password);
        else
                m_settings->remove("password");
	m_settings->setValue("keeppass",   m_keeppass);
	m_settings->setValue("showagselect", m_showagselect);
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
        
	m_settings->setValue("guisettings", m_guioptions["user"]);
        m_settings->endGroup();
        
        m_settings->beginGroup("user-functions");
        foreach (QString function, CheckFunctions)
                m_settings->setValue(function, m_checked_function[function]);
        m_settings->endGroup();
}

/*!
 *
 */
void BaseEngine::setCheckedFunction(const QString & function, bool b) {
	if(b != m_checked_function[function]) {
		m_checked_function[function] = b;
                if ((state() == ELogged) && m_enabled_function[function]) {
                        if(function == "presence")
                                availAllowChanged(b);
                }
	}
}

bool BaseEngine::checkedFunction(const QString & function) {
        return m_checked_function[function];
}

void BaseEngine::setEnabledFunction(const QString & function, bool b) {
        m_enabled_function[function] = b;
}

bool BaseEngine::enabledFunction(const QString & function) {
        return m_enabled_function[function];
}

void BaseEngine::config_and_start(const QString & login,
                                  const QString & pass,
                                  const QString & phonenum)
{
        setUserId(login);
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
	qDebug() << "BaseEngine::start()" << m_serverhost << m_checked_function;

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

const QVariantMap & BaseEngine::getCapaPresence() const
{
        return m_capapresence;
}

const QVariant BaseEngine::getGuiOptions(const QString & arg) const
{
        return m_guioptions[arg];
}

void BaseEngine::setGuiOption(const QString & arg, const QVariant & opt)
{
        m_guioptions[arg] = opt;
}

void BaseEngine::updateCapaPresence(const QVariant & presence)
{
        QVariantMap presencemap = presence.toMap();
        foreach (QString field, presencemap.keys())
                if(presencemap.contains(field))
                        m_capapresence[field] = presencemap[field];
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
        // qDebug() << "BaseEngine::setAvailState() from" << m_availstate << "to" << newstate << comesFromServer;
	if(m_availstate != newstate) {
		m_availstate = newstate;
		m_settings->setValue("engine/availstate", m_availstate);
                if (comesFromServer)
                        changesAvailChecks();
                changeState();
                keepLoginAlive();
	} else {
                if (comesFromServer)
                        changesAvailChecks();
        }
}

const QString & BaseEngine::getAvailState() const
{
        return m_availstate;
}

void BaseEngine::setAvailability()
{
        // qDebug() << "BaseEngine::setAvailability()" << sender();
        QString availstate = sender()->property("availstate").toString();
	setAvailState(availstate, false);
}

void BaseEngine::sendCommand(const QString & command)
{
        if(m_sbsocket->state() == QAbstractSocket::ConnectedState)
                m_sbsocket->write((command + "\n").toAscii());
}

void BaseEngine::sendJsonCommand(const QVariantMap & command)
{
        QString jsoncommand(JsonQt::VariantToJson::parse(command));
        sendCommand(jsoncommand);
}

/*! \brief parse history command response
 *
 * parse the history command response from the server and
 * trigger the update of the call history panel.
 *
 * \sa Logwidget
 */
void BaseEngine::processHistory(const QVariant & histlist)
{
        foreach (QVariant histitem, histlist.toList()) {
                qDebug() << "BaseEngine::processHistory()" << histitem;
 		QDateTime dt = QDateTime::fromString(histitem.toMap()["ts"].toString(), Qt::ISODate);
		int duration = histitem.toMap()["duration"].toInt();
                QString fullname = histitem.toMap()["fullname"].toString();
                QString direction = histitem.toMap()["direction"].toString();
                QString techdef = histitem.toMap()["termin"].toString();
                updateLogEntry(dt, duration, fullname, direction, techdef);
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
                QVariantMap command;
                command["class"] = "login_id";
                command["direction"] = "xivoserver";
                command["userid"] = m_userid;
                command["company"] = m_company;
                command["ident"] = m_clientid;
                command["version"] = __current_client_version__;
                command["xivoversion"] = __xivo_version__;
                sendJsonCommand(command);
        } else if(socketname == "filetransfers") {
                QVariantMap command;
                command["class"] = "filetransfer";
                command["direction"] = "xivoserver";
                command["tdirection"] = m_filedir;
                command["fileid"] = m_fileid;
                QString jsoncommand(JsonQt::VariantToJson::parse(command));
                if(m_filesocket->state() == QAbstractSocket::ConnectedState)
                        m_filesocket->write((jsoncommand + "\n").toAscii());
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
	case QAbstractSocket::NetworkError:
                popupError("network_error");
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
                                         const QString & phoneid)
{
        foreach (UserInfo * uinfo, m_users)
                if(uinfo->hasPhone(astid, phoneid))
                        return uinfo;
        return NULL;
}

UserInfo * BaseEngine::findUserFromAgent(const QString & astid,
                                         const QString & agentnum)
{
        foreach (UserInfo * uinfo, m_users)
                if(uinfo->hasAgent(astid, agentnum))
                        return uinfo;
        return NULL;
}

void BaseEngine::updatePhone(const QString & astid,
                             const QString & phoneid,
                             const QVariant & properties)
{
        // qDebug() << "BaseEngine::updatePhone()" << astid << phoneid << properties;
        UserInfo * ui = findUserFromPhone(astid, phoneid);
        if(ui) {
                ui->updatePhoneStatus(phoneid, properties.toMap()["hintstatus"]);
                updatePeer(ui, phoneid, properties.toMap()["comms"]);
        }
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
//         UserInfo * ui = findUserFromPhone(astid, context, tech, phonenum);
//         if(ui) {
//                 removePeer(ui->userid());
//                 m_users.remove(ui->userid());
//         }
}

bool BaseEngine::parseCommand(const QString & line)
{
        QVariant data;
        try {
                data = JsonQt::JsonToVariant::parse(line.trimmed());
        }
        catch(JsonQt::ParseException) {
                qDebug() << "BaseEngine::parseCommand() exception catched for" << line.trimmed();
                return false;
        }
        
        QVariantMap datamap = data.toMap();
        QString direction = datamap["direction"].toString();
        
        if(direction == "client") {
                QString thisclass = datamap["class"].toString();
                if (thisclass == "callcampaign") {
                        requestFileListResult(datamap["payload"]);
                        
                } else if (thisclass == "parkcall") {
                        parkingEvent(datamap["payload"]);
                        
                } else if (thisclass == "sheet") {
                        QString payload;
                        QByteArray qba = QByteArray::fromBase64(datamap["payload"].toString().toAscii());
                        if(datamap["compressed"].toString().size() > 0) {
                                payload = QString::fromUtf8(qUncompress(qba));
                        } else {
                                payload = QString::fromUtf8(qba);
                        }
                        // will eventually call the XML parser
                        displayFiche(payload, false);
                        
                } else if (thisclass == "queues") {
                        QString function = datamap["function"].toString();
                        if(function == "sendlist") {
                                foreach (QVariant qv, datamap["payload"].toList())
                                        newQueueList(! m_capafuncs.contains("nojoinleave"), qv);
                        }
                        else if(function == "update") {
                                setQueueStatus(datamap["payload"]);
                        } else if(function == "del") {
                                removeQueues(datamap["astid"].toString(),
                                             datamap["deltalist"].toStringList());
                        } else if(function == "add") {
                                QVariantMap command;
                                command["class"] = "queues";
                                command["direction"] = "xivoserver";
                                command["function"] = "getlist";
                                sendJsonCommand(command);
                                // qDebug() << thisclass << "add" << datamap["astid"].toString() << datamap["deltalist"].toStringList();
                        }
                        
                } else if (thisclass == "agents") {
                        QString function = datamap["function"].toString();
                        if(function == "sendlist") {
                                foreach (QVariant qv, datamap["payload"].toList())
                                        newAgentList(qv);
                        } else if(function == "update") {
                                QVariant params = datamap["payload"];
                                QString action = params.toMap()["action"].toString();
                                QString astid = params.toMap()["astid"].toString();
                                QString agent_channel = params.toMap()["agent_channel"].toString();
                                if (agent_channel.startsWith("Agent/")) {
                                        UserInfo * ui = findUserFromAgent(astid, agent_channel.mid(6));
                                        if(ui)
                                                updatePeerAgent(ui->userid(), "agentstatus", params);
                                        else // (useful ?) in order to transfer the replies to unmatched agents
                                                updatePeerAgent("", "agentstatus", params);
                                } else
                                        qDebug() << "update-agents agentnum" << astid << agent_channel;
                        } else if(function == "del") {
                                qDebug() << thisclass << "del" << datamap["astid"].toString() << datamap["deltalist"].toStringList();
                        } else if(function == "add") {
                                qDebug() << thisclass << "add" << datamap["astid"].toString() << datamap["deltalist"].toStringList();
                        }
                        
                } else if (thisclass == "agentrecord") {
                        statusRecord(datamap["agentnum"].toString(), datamap["status"].toString());
                        
                } else if (thisclass == "agent-status") {
                        if((datamap["astid"].toString() == m_agent_watched_astid) &&
                           (datamap["agentnum"].toString() == m_agent_watched_agentid))
                                changeWatchedAgentSignal(m_agent_watched_astid,
                                                         m_agent_watched_agentid,
                                                         datamap["payload"]);
                        
                } else if (thisclass == "queue-status") {
                        if((datamap["astid"].toString() == m_queue_watched_astid) &&
                           (datamap["queuename"].toString() == m_queue_watched_queueid))
                                changeWatchedQueueSignal(m_queue_watched_astid,
                                                         m_queue_watched_queueid,
                                                         datamap["payload"]);
                        
                } else if (thisclass == "history") {
                        // QCryptographicHash histohash(QCryptographicHash::Sha1);
                        // QByteArray res = histohash.hash(command_args.toAscii(), QCryptographicHash::Sha1).toHex();
                        processHistory(datamap["payload"]);
                        
                } else if (thisclass == "getguisettings") {
                        setGuiOptions(datamap["payload"]);
                        
                } else if (thisclass == "meetme") {
                        QString function = datamap["function"].toString();
                        int timeref = datamap["timenow"].toInt();
                        if (function == "sendlist")
                                meetmeInit(timeref, datamap["payload"]);
                        else if (function == "update")
                                meetmeEvent(timeref, datamap["payload"]);
                        
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
                        
                } else if (thisclass == "filelist") {
                        serverFileList(datamap["filelist"].toStringList());
                        
                } else if (thisclass == "presence") {
                        // QString id = datamap["company"].toString() + "/" + datamap["userid"].toString();
                        QString id = datamap["astid"].toString() + "/" + datamap["xivo_userid"].toString();
                        // qDebug() << thisclass << m_users.size() << id;
                        if(m_users.contains(id)) {
                                m_users[id]->setAvailState(datamap["capapresence"].toMap()["state"]);
                                updatePeerAgent(id, "imstatus", QStringList());
                                updateAgentPresence(m_users[id]->agentid(), datamap["capapresence"].toMap()["state"]);
                                m_counters = datamap["presencecounter"];
                                updateCounter(m_counters);
                                if (id == m_fullid) {
                                        updateCapaPresence(datamap["capapresence"]);
                                        updatePresence(m_capapresence);
                                        localUserInfoDefined(m_users[m_fullid]);
                                }
                        }
                        
                } else if (thisclass == "users") {
                        QString function = datamap["function"].toString();
                        if (function == "sendlist") {
                                QVariantList listusers = datamap["payload"].toList();
                                foreach(QVariant userprops, listusers) {
                                        QVariantMap uinfo = userprops.toMap();
                                        QString iduser = uinfo["astid"].toString() + "/" + uinfo["xivo_userid"].toString();
                                        
                                        if(! m_users.contains(iduser)) {
                                                m_users[iduser] = new UserInfo(iduser);
                                                m_users[iduser]->setCtiLogin(uinfo["user"].toString());
                                                m_users[iduser]->setFullName(uinfo["fullname"].toString());
                                                newUser(m_users[iduser]);
                                                m_users[iduser]->setPhones(uinfo["astid"].toString(),
                                                                           uinfo["techlist"].toStringList());
                                        }
                                        
                                        m_users[iduser]->setAvailState(uinfo["statedetails"]);
                                        m_users[iduser]->setNumber(uinfo["phonenum"].toString());
                                        m_users[iduser]->setAgent(uinfo["agentnum"].toString());
                                        m_users[iduser]->setMWI(uinfo["mwi"].toStringList());
                                        updatePeerAgent(iduser, "imstatus", QStringList());
                                        updateAgentPresence(m_users[iduser]->agentid(), uinfo["statedetails"]);
                                }
                                
                                peersReceived();
                                // qDebug() << m_fullid << m_users.keys();
                                m_monitored_userid = m_fullid;
                                QString fullname_mine = "No One";
                                if(m_users.contains(m_fullid)) {
                                        fullname_mine = m_users[m_fullid]->fullname();
                                        localUserInfoDefined(m_users[m_fullid]);
                                        updateCounter(m_counters);
                                }
                                
                                // Who do we monitor ?
                                // First look at the last monitored one
                                QString fullid_watched;
                                if(m_capafuncs.contains("switchboard"))
                                        fullid_watched = m_settings->value("monitor/userid").toString();
                                else
                                        fullid_watched = "";
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
                                                QString subclass = datamap["subclass"].toString();
                                                if(subclass == "mwi") {
                                                        m_users[iduser]->setMWI(datamap["payload"].toStringList());
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
                        QString function = datamap["function"].toString();
                        if (function == "update") {
                                QVariantMap featuresupdate_map = datamap["payload"].toMap();
                                if(m_monitored_userid == datamap["userid"].toString())
                                        foreach(QString featurekey, featuresupdate_map.keys())
                                                initFeatureFields(featurekey, featuresupdate_map[featurekey]);
                                
                        } else if (function == "get") {
                                QVariantMap featuresupdate_map = datamap["payload"].toMap();
                                if(m_monitored_userid == datamap["userid"].toString()) {
                                        resetFeatures();
                                        foreach(QString featurekey, featuresupdate_map.keys())
                                                initFeatureFields(featurekey, featuresupdate_map[featurekey]);
                                        emitTextMessage(tr("Received Services Data for ") + m_monitored_userid);
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
                                foreach(QString astid, datamap["payload"].toMap().keys()) {
                                        QVariant values = datamap["payload"].toMap()[astid];
                                        foreach(QString phoneid, values.toMap().keys())
                                                updatePhone(astid, phoneid, values.toMap()[phoneid]);
                                }
                                callsUpdated();
                                peersReceived();
                        } else if (function == "update") {
                                QString astid = datamap["astid"].toString();
                                QString phoneid = datamap["phoneid"].toString();
                                QVariant value = datamap["status"];
                                updatePhone(astid, phoneid, value);
                                callsUpdated();
                        } else if (function == "add") {
                                qDebug() << thisclass << function << datamap["astid"].toString() << datamap["deltalist"].toStringList();
                        } else if (function == "del") {
                                qDebug() << thisclass << function << datamap["astid"].toString() << datamap["deltalist"].toStringList();
                        } else if (function == "signal-deloradd") {
                                QStringList listpeers = datamap["payload"].toStringList();
                                // qDebug() << "phones-signal-deloradd" << listpeers;
                                //emitTextMessage(tr("New phone list on %1 : - %2 + %3 = %4 total").arg(listpeers[0],
                                //listpeers ));
                                if(listpeers[1].toInt() > 0)
                                        sendCommand("phones-del");
                                if(listpeers[2].toInt() > 0)
                                        sendCommand("phones-add");
                        }
                        
                } else if (thisclass == "login_id_ok") {
                        
                        m_version_server = datamap["version"].toInt();
                        m_xivover_server = datamap["xivoversion"].toString();
                        
                        if(m_version_server < REQUIRED_SERVER_VERSION) {
                                stop();
                                popupError("version_server:" + QString::number(m_version_server) + ";" + QString::number(REQUIRED_SERVER_VERSION));
                        } else {
                                QString tohash = datamap["sessionid"].toString() + ":" + m_password;
                                QCryptographicHash hidepass(QCryptographicHash::Sha1);
                                QByteArray res = hidepass.hash(tohash.toAscii(), QCryptographicHash::Sha1).toHex();
                                QVariantMap command;
                                command["class"] = "login_pass";
                                command["direction"] = "xivoserver";
                                command["hashedpassword"] = QString(res);
                                sendJsonCommand(command);
                        }
                        
                } else if (thisclass == "loginko") {
                        stop();
                        popupError(datamap["errorstring"].toString());
                        
                } else if (thisclass == "login_pass_ok") {
                        QStringList capas = datamap["capalist"].toStringList();
                        QVariantMap command;
                        command["class"] = "login_capas";
                        command["direction"] = "xivoserver";
                        if (capas.size() == 1)
                                command["capaid"] = capas[0];
                        else {
                                if(m_useridopt.size() > 0) {
                                        if(capas.contains(m_useridopt))
                                                command["capaid"] = m_useridopt;
                                        else
                                                command["capaid"] = capas[0];
                                } else
                                        command["capaid"] = capas[0];
                        }
                        
                        switch(m_loginkind) {
                        case 0:
                                command["loginkind"] = "user";
                                break;
                        case 2:
                                command["agentlogin"] = "now";
                        case 1:
                                command["loginkind"] = "agent";
                                command["phonenumber"] = m_phonenumber;
                                break;
                        }
                        
                        if(m_checked_function["presence"])
                                command["state"] = m_availstate;
                        else
                                command["state"] = __nopresence__;
                        command["lastconnwins"] = m_checked_lastconnwins;
                        sendJsonCommand(command);
                        
                } else if (thisclass == "login_capas_ok") {
                        m_astid = datamap["astid"].toString();
                        m_xivo_userid = datamap["xivo_userid"].toString();
                        setFullId();
                        m_capafuncs = datamap["capafuncs"].toStringList();
                        m_capaxlets = datamap["capaxlets"].toStringList();
                        m_appliname = datamap["appliname"].toString();
                        updateCapaPresence(datamap["capapresence"]);
                        m_forced_state = datamap["capapresence"].toMap()["state"].toString();
                        m_counters = datamap["presencecounter"];
                        m_guioptions["server_gui"] = datamap["guisettings"];
                        QVariantMap tmp;
                        QStringList todisp;
                        m_checked_function["switchboard"] = true;
                        foreach (QString function, m_capafuncs)
                                if(m_checked_function.contains(function) && m_checked_function[function])
                                        todisp.append(function);
                        tmp["functions"] = todisp;
                        tmp["xlets"] = datamap["capaxlets"];
                        tmp["presence"] = datamap["capapresence"];
                        m_guioptions["server_funcs"] = tmp;
                        
                        qDebug() << "clientXlets" << XletList;
                        qDebug() << "m_capaxlets" << m_capaxlets;
                        qDebug() << "m_capafuncs" << m_capafuncs;
                        qDebug() << "m_appliname" << m_appliname;
                        qDebug() << "m_counters"  << m_counters;
                        
                        // XXXX m_capafuncs => config file
                        // m_enabled_function's purposes are :
                        // - to keep track of the user's true rights
                        foreach (QString function, CheckFunctions)
                                if(m_capafuncs.contains(function))
                                        m_enabled_function[function] = true;
                                else
                                        m_enabled_function[function] = false;
                        
                        if(m_capafuncs.size() == 0) {
                                stop();
                                popupError("no_capability");
                        } else {
                                setState(ELogged); // calls logged()
                                setAvailState(m_forced_state, true);
                                m_ka_timerid = startTimer(m_keepaliveinterval);
                                askCallerIds();
                        }

                } else {
                        qDebug() << "unknown server command class" << thisclass << datamap;
                }
        }
        
        return true;
}

void BaseEngine::agentAction(const QString & action)
{
        QVariantMap command;
        command["class"] = "agent";
        command["direction"] = "xivoserver";
        command["command"] = action.split(" ");
        sendJsonCommand(command);
}

void BaseEngine::meetmeAction(const QString & function, const QString & functionargs)
{
        QVariantMap command;
        command["class"] = "meetme";
        command["direction"] = "xivoserver";
        command["function"] = function;
        command["functionargs"] = functionargs.split(" ");
        sendJsonCommand(command);
}

void BaseEngine::requestFileList(const QString & action)
{
        QVariantMap command;
        command["class"] = "callcampaign";
        command["direction"] = "xivoserver";
        command["command"] = action.split(" ");
        sendJsonCommand(command);
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
                QVariantMap command;
                command["class"] = "faxsend";
                command["direction"] = "xivoserver";
                command["size"] = QString::number(m_faxsize);
                command["number"] = number;
                command["hide"] = QString::number(hide);
                sendJsonCommand(command);
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
        
        else if(errorid.startsWith("capaid_undefined:")) {
                QStringList capainfo = errorid.split(":");
                errormsg = tr("Your profile name <%1> is not defined.").arg(capainfo[1]);
        }
        
        else if(errorid.toLower() == "login_password")
                errormsg = tr("You entered a wrong login / password.");
        
        else if(errorid.toLower() == "no_keepalive_from_server")
                errormsg = tr("The XIVO daemon on %1 did not reply to the last keepalive.").arg(m_serverhost);

        else if(errorid.toLower() == "connection_closed")
                errormsg = tr("The XIVO daemon on %1 has just closed the connection.").arg(m_serverhost);
        
        else if(errorid.toLower() == "network_error")
                errormsg = tr("An error occurred with the network (network cable accidentally plugged out ?).");
        
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

        else if(errorid.startsWith("toomuchusers:")) {
                QStringList userslist = errorid.split(":")[1].split(";");
                errormsg = tr("Max number (%1) of XIVO Clients already reached.").arg(userslist[0]);
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
                emitMessageBox(errormsg);
}

void BaseEngine::saveToFile(const QString & filename)
{
        qDebug() << "BaseEngine::saveToFile()" << filename << m_downloaded.size();
        QFile outputfile(filename);
        outputfile.open(QIODevice::WriteOnly);
        outputfile.write(m_downloaded);
        outputfile.close();
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
                        QVariant jsondata;
                        try {
                                jsondata = JsonQt::JsonToVariant::parse(line.trimmed());
                        }
                        catch(JsonQt::ParseException) {
                                qDebug() << "BaseEngine::socketReadyRead() exception catched for" << line.trimmed();
                        }
                        QVariantMap jsondatamap = jsondata.toMap();
                        if(jsondatamap["class"].toString() == "fileref") {
                                if(m_filedir == "download") {
                                        m_downloaded = QByteArray::fromBase64(jsondatamap["payload"].toByteArray());
                                        qDebug() << jsondatamap["filename"].toString() << m_downloaded.size();
                                        fileReceived();
                                } else {
                                        qDebug() << "sending fax contents" << jsondatamap["fileid"].toString();
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

void BaseEngine::actionFromFiche(const QVariant & infos)
{
        // qDebug() << "BaseEngine::actionFromFiche()" << infos;
        QVariantMap command;
        command["class"] = "actionfiche";
        command["direction"] = "xivoserver";
        command["infos"] = infos;
        sendJsonCommand(command);
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
void BaseEngine::actionCall(const QString & action,
                            const QString & src,
                            const QString & dst)
{
        qDebug() << "BaseEngine::actionCall()" << action << src << dst;
        
        QVariantMap command;
        command["direction"] = "xivoserver";
        command["class"] = action;
        
        if((action == "originate") || (action == "transfer") || (action == "atxfer")) {
                command["source"] = src;
                if((dst == "ext:special:dialxlet") && (! m_numbertodial.isEmpty()))
                        command["destination"] = m_numbertodial;
                else
                        command["destination"] = dst;
                sendJsonCommand(command);
        } else if((action == "hangup") || (action == "simplehangup")) {
                command["source"] = src;
                sendJsonCommand(command);
        } else if(action == "pickup") {
                sendJsonCommand(command);
        }
}

/*! \brief send the directory search command to the server
 *
 * \sa directoryResponse()
 */
void BaseEngine::searchDirectory(const QString & text)
{
        // qDebug() << "BaseEngine::searchDirectory()" << text;
        QVariantMap command;
        command["class"] = "directory-search";
        command["direction"] = "xivoserver";
        command["pattern"] = text;
        sendJsonCommand(command);
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
                QVariantMap command;
                command["class"] = "history";
                command["direction"] = "xivoserver";
                command["peer"] = peer;
                command["size"] = QString::number(m_historysize);
                command["mode"] = QString::number(mode);
                sendJsonCommand(command);
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
	m_userid = useridsplit[0].trimmed();
        m_useridwithopt = userid;
        if(useridsplit.size() > 1)
                m_useridopt = useridsplit[1].trimmed();
        else
                m_useridopt = "";
}

void BaseEngine::setFullId()
{
        // m_fullid = m_company + "/" + m_userid;
        m_fullid = m_astid + "/" + m_xivo_userid;
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
        if(loginkind != m_loginkind) {
                m_loginkind = loginkind;
                QVariantMap tmpqvm = m_guioptions["user"].toMap();
                tmpqvm["loginkind"] = m_loginkind;
                m_guioptions["user"] = tmpqvm;
        }
}

const int & BaseEngine::showagselect() const
{
	return m_showagselect;
}

void BaseEngine::setShowAgentSelect(const int showagselect)
{
	m_showagselect = showagselect;
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

void BaseEngine::initFeatureFields(const QString & field, const QVariant & value)
{
        //        qDebug() << field << value;
        bool isenabled = value.toMap()["enabled"].toBool();
        QString number = value.toMap()["number"].toString();
	if((field == "enablevoicemail") || (field == "vm"))
		optChanged("enablevm", isenabled);
	else if((field == "enablednd") || (field == "dnd"))
		optChanged("enablednd", isenabled);
	else if(field == "callfilter")
		optChanged("incallfilter", isenabled);
	else if(field == "callrecord")
		optChanged("incallrec", isenabled);
	else if((field == "unc") || (field == "busy") || (field == "rna"))
		forwardUpdated(field, isenabled, number);
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
        QVariantMap command;
        command["class"] = "featuresput";
        command["direction"] = "xivoserver";
        command["userid"] = m_monitored_userid;
        if(capa == "enablevm")
                command["function"] = "enablevoicemail";
        else if(capa == "incallrec")
                command["function"] = "callrecord";
        else if(capa == "incallfilter")
                command["function"] = "callfilter";
        else if(capa == "enablednd")
                command["function"] = "enablednd";
        command["value"] = QString(b ? "1" : "0");
        sendJsonCommand(command);
}

void BaseEngine::featurePutForward(const QString & capa, bool b, const QString & dst)
{
        QVariantMap command;
        command["class"] = "featuresput";
        command["direction"] = "xivoserver";
        command["userid"] = m_monitored_userid;
        if(capa == "fwdunc")
                command["function"] = "enableunc";
        else if(capa == "fwdbusy")
                command["function"] = "enablebusy";
        else if(capa == "fwdrna")
                command["function"] = "enablerna";
        command["value"] = QString(b ? "1" : "0");
        command["destination"] = dst;
        sendJsonCommand(command);
}

void BaseEngine::askFeatures()
{
        qDebug() << "BaseEngine::askFeatures()" << m_monitored_userid;
        QString featurestoget = "user:special:me";
        if(! m_monitored_userid.isEmpty())
                featurestoget = m_monitored_userid;
        QVariantMap command;
        command["class"] = "featuresget";
        command["direction"] = "xivoserver";
        command["userid"] = featurestoget;
        sendJsonCommand(command);
}

void BaseEngine::askCallerIds()
{
        // qDebug() << "BaseEngine::askCallerIds()";
        QVariantMap command;
        command["direction"] = "xivoserver";
        command["function"] = "getlist";
        QStringList getlists = (QStringList() << "users" << "queues" << "agents" << "phones" << "meetme" << "users");
        foreach(QString kind, getlists) {
                command.remove("class");
                command["class"] = kind;
                sendJsonCommand(command);
        }
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
			stopTryAgainTimer();
			if(m_checked_function["presence"] && m_enabled_function["presence"])
                                availAllowChanged(true);
			logged();
                        updatePresence(m_capapresence);
		} else if(state == ENotLogged) {
			availAllowChanged(false);
			delogged();
                        // reset some variables when disconnecting
                        m_agent_watched_astid = "";
                        m_agent_watched_agentid = "";
		}
	}
}

void BaseEngine::changeWatchedAgentSlot(const QString & astagentid, bool force)
{
        // qDebug() << "BaseEngine::changeWatchedAgentSlot()" << astagentid;
        if(force || ((m_agent_watched_astid.size() == 0) && (m_agent_watched_agentid.size() == 0))) {
                m_agent_watched_astid = astagentid.split(" ")[0];
                m_agent_watched_agentid = astagentid.split(" ")[1];
                QVariantMap command;
                command["class"] = "agent-status";
                command["direction"] = "xivoserver";
                command["astid"] = m_agent_watched_astid;
                command["agentid"] = m_agent_watched_agentid;
                sendJsonCommand(command);
        }
}

void BaseEngine::changeWatchedQueueSlot(const QString & astqueueid)
{
        // qDebug() << "BaseEngine::changeWatchedQueueSlot()" << astqueueid;
        m_queue_watched_astid = astqueueid.split(" ")[0];
        m_queue_watched_queueid = astqueueid.split(" ")[1];
        QVariantMap command;
        command["class"] = "queue-status";
        command["direction"] = "xivoserver";
        command["astid"] = m_queue_watched_astid;
        command["queuename"] = m_queue_watched_queueid;
        sendJsonCommand(command);
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

        QVariantMap command;
        command["class"] = "keepalive";
        command["direction"] = "xivoserver";
        sendJsonCommand(command);
}

void BaseEngine::changeState()
{
        QVariantMap command;
        command["class"] = "availstate";
        command["direction"] = "xivoserver";
        command["availstate"] = m_availstate;
        sendJsonCommand(command);
}

/*!
 * Send a keep alive message to the login server.
 * The message is sent in a datagram through m_udpsocket
 */ 
void BaseEngine::sendMessage(const QString & message)
{
        QVariantMap command;
        command["class"] = "message";
        command["direction"] = "xivoserver";
        command["message"] = message;
        sendJsonCommand(command);
}

void BaseEngine::saveQueueGroups(const QVariant & queuegroups)
{
	m_settings->setValue("agentpanel/queuegroups", queuegroups);
}

void BaseEngine::loadQueueGroups()
{
        setGroups(m_settings->value("agentpanel/queuegroups"));

}

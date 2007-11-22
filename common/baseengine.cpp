/* XIVO CTI clients
Copyright (C) 2007  Proformatique

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
*/

/* $Revision$
 * $Date$
 */

#include <QBuffer>
#include <QDebug>
#include <QMessageBox>
#include <QSettings>
#include <QStringList>
#include <QTcpSocket>
#include <QTime>
#include <QTimerEvent>

#include "baseengine.h"
#include "logeltwidget.h"
#include "popup.h"
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
BaseEngine::BaseEngine(QObject * parent)
        : QObject(parent),
	  m_serverhost(""), m_loginport(0), m_sbport(0),
          m_asterisk(""), m_protocol(""), m_userid(""), m_passwd(""),
          m_checked_presence(false), m_checked_cinfo(false),
          m_sessionid(""), m_state(ENotLogged),
	  m_listenport(0), m_pendingkeepalivemsg(0)
{
	m_ka_timerid = 0;
	m_try_timerid = 0;
	m_timer = -1;
	m_sbsocket     = new QTcpSocket(this);
	m_loginsocket  = new QTcpSocket(this);
        m_faxsocket    = new QTcpSocket(this);
	m_udpsocket    = new QUdpSocket(this);
	m_listenserver = new QTcpServer(this);
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

	// Socket for Login in UDP connections
        m_loginsocket->setProperty("socket", "login_udp");
	connect( m_loginsocket, SIGNAL(connected()),
	         this, SLOT(socketConnected()) );
	connect( m_loginsocket, SIGNAL(hostFound()),
	         this, SLOT(socketHostFound()) );
        connect( m_loginsocket, SIGNAL(readyRead()),
	         this, SLOT(processLoginDialog()) );

	// Socket for TCP connections
        m_sbsocket->setProperty("socket", "tcp_nat");
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

        m_faxsocket->setProperty("socket", "fax");
        connect( m_faxsocket, SIGNAL(connected()),
                 this, SLOT(socketConnected()) );
	connect( m_faxsocket, SIGNAL(disconnected()),
                 this, SLOT(socketDisconnected()));
	connect( m_faxsocket, SIGNAL(hostFound()),
	         this, SLOT(socketHostFound()) );

	// init listen server for profile push
	connect( m_listenserver, SIGNAL(newConnection()),
	         this, SLOT(handleProfilePush()) );
        
	// init UDP socket used for keep alive
	connect( m_udpsocket, SIGNAL(readyRead()),
		 this, SLOT(readKeepLoginAliveDatagrams()) );
        
	if(m_autoconnect)
		start();
}

/*! \brief Destructor
 */
BaseEngine::~BaseEngine()
{
        qDebug() << "BaseEngine::~BaseEngine()";
}

/*!
 * Load Settings from the registery/configuration file
 * Use default values when settings are not found.
 */
void BaseEngine::loadSettings()
{
        //qDebug() << "BaseEngine::loadSettings()";
	QSettings settings;
        settings.beginGroup("engine");
	m_serverhost = settings.value("serverhost", "192.168.0.254").toString();
	m_loginport  = settings.value("loginport", 5000).toUInt();
	m_sbport     = settings.value("serverport", 5003).toUInt();

        m_checked_presence = settings.value("fct_presence", false).toBool();
        m_checked_cinfo    = settings.value("fct_cinfo",    false).toBool();

	m_asterisk   = settings.value("asterisk", "xivo").toString();
	m_protocol   = settings.value("protocol", "sip").toString();
	m_userid     = settings.value("userid").toString();
	m_passwd     = settings.value("passwd").toString();

	m_autoconnect = settings.value("autoconnect", false).toBool();
	m_trytoreconnect = settings.value("trytoreconnect", false).toBool();
	m_trytoreconnectinterval = settings.value("trytoreconnectinterval", 20*1000).toUInt();
	m_keepaliveinterval = settings.value("keepaliveinterval", 20*1000).toUInt();

	m_historysize = settings.value("historysize", 8).toUInt();
	m_tcpmode = settings.value("tcpmode", false).toBool();
        m_checked_lastconnwins = settings.value("lastconnwins", false).toBool();

	m_availstate = settings.value("availstate", "available").toString();
        settings.endGroup();
}

/*!
 * Save Settings to the registery/configuration file
 */
void BaseEngine::saveSettings()
{
        //qDebug() << "BaseEngine::saveSettings()";
	QSettings settings;
        settings.beginGroup("engine");
	settings.setValue("serverhost", m_serverhost);
	settings.setValue("loginport",  m_loginport);
	settings.setValue("serverport", m_sbport);

	settings.setValue("fct_presence", m_checked_presence);
	settings.setValue("fct_cinfo",    m_checked_cinfo);

	settings.setValue("asterisk",   m_asterisk);
	settings.setValue("protocol",   m_protocol);
	settings.setValue("userid",     m_userid);
	settings.setValue("passwd",     m_passwd);

	settings.setValue("autoconnect", m_autoconnect);
	settings.setValue("trytoreconnect", m_trytoreconnect);
	settings.setValue("trytoreconnectinterval", m_trytoreconnectinterval);
	settings.setValue("keepaliveinterval", m_keepaliveinterval);

	settings.setValue("historysize", m_historysize);
	settings.setValue("tcpmode", m_tcpmode);
        settings.setValue("lastconnwins", m_checked_lastconnwins);

	settings.setValue("availstate", m_availstate);
        settings.endGroup();
}

/*!
 *
 */
void BaseEngine::setCheckedPresence(bool b) {
	if(b != m_checked_presence) {
		m_checked_presence = b;
		if((state() == ELogged) && m_enabled_presence) {
			availAllowChanged(b);
                }
	}
}

/*!
 *
 */
bool BaseEngine::checkedPresence() {
        return m_checked_presence;
}

/*!
 *
 */
void BaseEngine::setCheckedCInfo(bool b) {
	if(b != m_checked_cinfo)
		m_checked_cinfo = b;
}

/*!
 *
 */
bool BaseEngine::checkedCInfo() {
        return m_checked_cinfo;
}

/*!
 *
 */
void BaseEngine::setEnabledCInfo(bool b) {
        m_enabled_cinfo = b;
}

/*!
 *
 */
bool BaseEngine::enabledCInfo() {
        return m_enabled_cinfo;
}

void BaseEngine::initListenSocket()
{
	qDebug() << "BaseEngine::initListenSocket()";
	if (!m_listenserver->listen())
	{
		QMessageBox::critical(NULL, tr("XIVO CTI Critical error"),
                                      tr("Unable to start the server: %1.")
                                      .arg(m_listenserver->errorString()));
                m_listenserver->close();
		return;
	}
	m_listenport = m_listenserver->serverPort();
	qDebug() << "BaseEngine::initListenSocket()" << m_listenport;
}

/*! \brief Starts the connection to the server
 * This method starts the login process by connection
 * to the server.
 */
void BaseEngine::start()
{
	qDebug() << "BaseEngine::start()" << m_serverhost << m_loginport << m_checked_presence << m_checked_cinfo;

	// (In case the TCP sockets were attempting to connect ...) aborts them first
	m_sbsocket->abort();
	m_loginsocket->abort();
        m_listenserver->close();
        m_udpsocket->abort();

        if(! m_tcpmode) {
                if(m_checked_cinfo)
                        initListenSocket();
                m_udpsocket->bind();
        }

        connectSocket();
}

/*! \brief Closes the connection to the server
 * This method disconnect the engine from the server
 */
void BaseEngine::stop()
{
	qDebug() << "BaseEngine::stop()";

        if(! m_tcpmode) {
                if(m_sessionid != "") {
                        QString outline = "STOP ";
                        outline.append(m_asterisk + "/" + m_protocol.toLower() + m_userid);
                        outline.append(" SESSIONID ");
                        outline.append(m_sessionid);
                        outline.append("\r\n");
                        m_udpsocket->writeDatagram( outline.toAscii(),
                                                    m_serveraddress, m_loginport + 1 );
                }
                m_udpsocket->close();
        }

        m_listenserver->close();
        m_listenport = 0;

	m_sbsocket->disconnectFromHost();

	stopKeepAliveTimer();
	stopTryAgainTimer();
	setState(ENotLogged);
	m_sessionid = "";

        m_callerids.clear();
}

/*! \brief initiate connection to the server
 */
void BaseEngine::connectSocket()
{
        if(m_tcpmode) {
                qDebug() << "BaseEngine::connectSocket()" << m_serverhost << m_sbport;
                m_sbsocket->connectToHost(m_serverhost, m_sbport);
        } else {
                qDebug() << "BaseEngine::connectSocket()" << m_serverhost << m_loginport;
                m_loginsocket->connectToHost(m_serverhost, m_loginport);
        }
}

bool BaseEngine::tcpmode() const
{
        return m_tcpmode;
}

void BaseEngine::setTcpmode(bool b)
{
        m_tcpmode = b;
}

bool BaseEngine::lastconnwins() const
{
        return m_checked_lastconnwins;
}

void BaseEngine::setLastConnWins(bool b)
{
        m_checked_lastconnwins = b;
}

const QStringList & BaseEngine::getCapabilities() const
{
        return m_capabilities;
}

/*!
 * sets the availability state and call keepLoginAlive() if needed
 *
 * \sa setAvailable()
 * \sa setAway()
 * \sa setBeRightBack()
 * \sa setOutToLunch()
 * \sa setDoNotDisturb()
 */
void BaseEngine::setAvailState(const QString & newstate, bool comesFromServer)
{
	if(m_availstate != newstate) {
		QSettings settings;
		m_availstate = newstate;
		settings.setValue("engine/availstate", m_availstate);
                if (comesFromServer)
                        changesAvailChecks();
                keepLoginAlive();
                sendCommand("availstate " + m_availstate);
	}
}

const QString & BaseEngine::getAvailState() const
{
        return m_availstate;
}

void BaseEngine::setAvailable()
{
	setAvailState("available", false);
}

void BaseEngine::setAway()
{
	setAvailState("away", false);
}

void BaseEngine::setBeRightBack()
{
	setAvailState("berightback", false);
}

void BaseEngine::setOutToLunch()
{
	setAvailState("outtolunch", false);
}

void BaseEngine::setDoNotDisturb()
{
	setAvailState("donotdisturb", false);
}

/*! \brief send a command to the server 
 * The m_pendingcommand is sent on the socket.
 *
 * \sa m_pendingcommand
 */
void BaseEngine::sendTCPCommand()
{
        // qDebug() << "BaseEngine::sendTCPCommand()" << m_pendingcommand;
	m_sbsocket->write((m_pendingcommand + "\r\n"/*"\n"*/).toAscii());
}

void BaseEngine::sendUDPCommand(const QString & command)
{
	// qDebug() << "BaseEngine::sendUDPCommand()" << command;
	if(m_state == ELogged) {
		QString outline = "COMMAND ";
		outline.append(m_asterisk + "/" + m_protocol.toLower() + m_userid);
		outline.append(" SESSIONID " + m_sessionid + " " + command);
		qDebug() << "BaseEngine::sendUDPCommand()" << outline;
                outline.append("\r\n");
		m_udpsocket->writeDatagram( outline.toAscii(),
					    m_serveraddress, m_loginport + 1 );
	} else {
		qDebug() << "not logged in : command not sent :" << command;
	}
}

void BaseEngine::sendCommand(const QString & command)
{
        if(m_tcpmode) {
                if(m_sbsocket->state() == QAbstractSocket::ConnectedState) {
                        m_pendingcommand = command;
                        sendTCPCommand();
                }
        } else
                sendUDPCommand(command);
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
	int i;
	for(i=0; i+6<=histlist.size(); i+=6)
	{
		// DateTime; CallerID; duration; Status?; peer; IN/OUT
		//qDebug() << histlist[i+0] << histlist[i+1]
		//         << histlist[i+2] << histlist[i+3]
		//         << histlist[i+4] << histlist[i+5];
		QDateTime dt = QDateTime::fromString(histlist[i+0], Qt::ISODate);
		QString callerid = histlist[i+1];
		int duration = histlist[i+2].toInt();
		QString status = histlist[i+3];
		QString peer = histlist[i+4];
		LogEltWidget::Direction d;
		d = (histlist[i+5] == "IN") ? LogEltWidget::InCall : LogEltWidget::OutCall;
		//qDebug() << dt << callerid << duration << peer << d;
		updateLogEntry(dt, duration, peer, (int)d);
	}
}


/*! \brief called when the socket is first connected
 */
void BaseEngine::socketConnected()
{
        QString socname = this->sender()->property("socket").toString();
	qDebug() << "BaseEngine::socketConnected()" << socname;
        if(socname == "login_udp") {
                identifyToTheServer();
        } else if(socname == "tcp_nat") {
                stopTryAgainTimer();
                /* do the login/identification ? */
                setMyClientId();
                m_pendingcommand = "login astid=" + m_asterisk + ";proto="
                        + m_protocol + ";userid=" + m_userid + ";";
                if(m_checked_presence)
                        m_pendingcommand += "state=" + m_availstate + ";";
                else
                        m_pendingcommand += "state=unknown;";
                m_pendingcommand += "state=" + m_availstate + ";";
                m_pendingcommand += "ident=" + m_clientid   + ";";
                m_pendingcommand += "passwd=" + m_passwd + ";version=" + __current_client_version__ + ";";
                if(m_checked_lastconnwins)
                        m_pendingcommand += "lastconnwins=true";
                else
                        m_pendingcommand += "lastconnwins=false";
                // login <asterisk> <techno> <id>
                sendTCPCommand();
        } else if(socname == "fax") {
                if(m_faxsize > 0) {
                        QString stp = m_faxid + "\n";
                        m_faxsocket->write(stp.toAscii(), stp.size());
                        m_faxsocket->write(* m_faxdata);
                }
                m_faxsocket->close();
                m_faxsize = 0;
                m_faxid = "";
                delete m_faxdata;
        }
}

/*! \brief called when the socket is disconnected from the server
 */
void BaseEngine::socketDisconnected()
{
        QString socname = this->sender()->property("socket").toString();
	qDebug() << "BaseEngine::socketDisconnected()" << socname;
        if(socname == "tcp_nat") {
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
        QString socname = this->sender()->property("socket").toString();
        qDebug() << "BaseEngine::socketHostFound()" << socname;
}

/*! \brief catch socket errors
 */
void BaseEngine::socketError(QAbstractSocket::SocketError socketError)
{
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
		emitTextMessage(tr("Unknown socket error"));
		break;
	default:
                qDebug() << "BaseEngine::socketError()" << socketError;
		break;
	}
}

/*! \brief receive signals of socket state change
 *
 * useless...
 */
void BaseEngine::socketStateChanged(QAbstractSocket::SocketState socketState)
{
	qDebug() << "BaseEngine::socketStateChanged(" << socketState << ")";
	if(socketState == QAbstractSocket::ConnectedState) {
		if(m_timer != -1)
		{
			killTimer(m_timer);
			m_timer = -1;
		}
		//startTimer(3000);
	}
}


void BaseEngine::updatePeerAndCallerid(const QStringList & liststatus)
{
	const int nfields0 = 14; // 0th order size (per-phone/line informations)
	const int nfields1 = 6;  // 1st order size (per-channel informations)
	QStringList chanIds;
	QStringList chanStates;
	QStringList chanOthers;

	// liststatus[0] is a dummy field, only used for debug on the daemon side
	// p/(asteriskid)/(context)/(protocol)/(phoneid)/(phonenum)

	if(liststatus.count() < nfields0) { // not valid
		qDebug() << "Bad data from the server :" << liststatus;
		return;
	}

	//<who>:<asterisk_id>:<tech(SIP/IAX/...)>:<phoneid>:<numero>:<contexte>:<dispo>:<etat SIP/XML>:<etat VM>:<etat Queues>: <nombre de liaisons>:
        QString astid   = liststatus[1];
	QString context = liststatus[5];

	QString pname   = "p/" + astid + "/" + context + "/"
		+ liststatus[2].toLower() + "/" + liststatus[3] + "/" + liststatus[4];
	QString InstMessAvail   = liststatus[6];
	QString SIPPresStatus   = liststatus[7];
	QString VoiceMailStatus = liststatus[8];
	QString QueueStatus     = liststatus[9];
	m_callerids[pname]      = liststatus[10];

	int nchans = liststatus[nfields0 - 1].toInt();
	if(liststatus.size() == nfields0 + nfields1 * nchans) {
		for(int i = 0; i < nchans; i++) {
			//  <channel>:<etat du channel>:<nb de secondes dans cet etat>:<to/from>:<channel en liaison>:<numero en liaison>
			int refn = nfields0 + nfields1 * i;
			QString displayedNum;
			
			SIPPresStatus = liststatus[refn + 1];
			chanIds << ("c/" + astid + "/" + context + "/" + liststatus[refn]);
			chanStates << liststatus[refn + 1];
                        QString peerid = liststatus[refn + 5];
			if((peerid == "") ||
			   (peerid == "<Unknown>") ||
			   (peerid == "<unknown>") ||
			   (peerid == "anonymous") ||
			   (peerid == "(null)"))
				displayedNum = tr("Unknown Number");
			else
				displayedNum = peerid;

			chanOthers << displayedNum;
                        if(m_is_a_switchboard)
                                updateCall("c/" + astid + "/" + context + "/" + liststatus[refn],
                                           liststatus[refn + 1],
                                           liststatus[refn + 2].toInt(), liststatus[refn + 3],
                                           liststatus[refn + 4], displayedNum,
                                           pname);
		}
	}

        updatePeer(pname, m_callerids[pname],
                   InstMessAvail, SIPPresStatus, VoiceMailStatus, QueueStatus,
                   chanIds, chanStates, chanOthers);
        if(m_is_a_switchboard)
                if(   (m_userid == liststatus[3])
                      && (m_dialcontext == liststatus[5]))
                        updateMyCalls(chanIds, chanStates, chanOthers);
}


void BaseEngine::removePeerAndCallerid(const QStringList & liststatus)
{
	const int nfields0 = 10; // 0th order size (per-phone/line informations)
	if(liststatus.count() < nfields0) { // not valid
		qDebug() << "Bad data from the server :" << liststatus;
		return;
	}

        QString astid   = liststatus[1];
	QString context = liststatus[5];
	QString pname   = "p/" + astid + "/" + context + "/"
		+ liststatus[2].toLower() + "/" + liststatus[3] + "/" + liststatus[4];
        removePeer(pname);
        m_callerids.remove(pname);
}


bool BaseEngine::parseCommand(const QStringList & listitems)
{
	QSettings settings;
        // qDebug() << "BaseEngine::parseCommand listitems[0].toLower() =" << listitems[0].toLower() << listitems.size();
        if((listitems[0].toLower() == QString("phones-list")) && (listitems.size() == 2)) {
                QStringList listpeers = listitems[1].split(";");
                for(int i = 1 ; i < listpeers.size() - 1; i++) {
                        QStringList liststatus = listpeers[i].split(":");
                        updatePeerAndCallerid(liststatus);
                }

                if(listpeers[0] == "0") {
                        callsUpdated();
                        peersReceived();

                        QString myfullid   = "p/" + m_asterisk + "/" + m_dialcontext + "/" + m_protocol + "/" + m_userid + "/" + m_extension;
                        QString myfullname = m_callerids[myfullid];
                        localUserDefined(myfullname);

                        // Who do we monitor ?
                        // First look at the last monitored one
                        QString fullid_watched = settings.value("monitor/peer").toString();

                        // If there was nobody, let's watch ourselves.
                        if(fullid_watched.isEmpty())
                                monitorPeer(myfullid, myfullname);
                        else {
                                QString fullname_watched = m_callerids[fullid_watched];
                                // If the CallerId value is empty, fallback to ourselves.
                                if(fullname_watched.isEmpty())
                                        monitorPeer(myfullid, myfullname);
                                else {
                                        monitorPeer(fullid_watched, fullname_watched);
                                }
                        }
                        emitTextMessage(tr("Received status for %1 phones").arg(m_callerids.size()));
                } else {
                        sendCommand("phones-list " + listpeers[0]);
                }
        } else if((listitems[0].toLower() == QString("phones-update")) && (listitems.size() == 2)) {
                QStringList liststatus = listitems[1].split(":");
                updatePeerAndCallerid(liststatus);
                callsUpdated();

        } else if((listitems[0].toLower() == QString("phones-add")) && (listitems.size() == 2)) {
                QStringList listpeers = listitems[1].split(";");
                for(int i = 1 ; i < listpeers.size() - 1; i++) {
                        QStringList liststatus = listpeers[i].split(":");
                        updatePeerAndCallerid(liststatus);
                }

                if(listpeers[0] == "0") {
                        qDebug() << "phones-add completed";
                } else {
                        sendCommand("phones-add " + listpeers[0]);
                }

        } else if((listitems[0].toLower() == QString("phones-del")) && (listitems.size() == 2)) {
                QStringList listpeers = listitems[1].split(";");
                for(int i = 1 ; i < listpeers.size() - 1; i++) {
                        QStringList liststatus = listpeers[i].split(":");
                        removePeerAndCallerid(liststatus);
                }

                if(listpeers[0] == "0") {
                        qDebug() << "phones-del completed";
                } else {
                        sendCommand("phones-del " + listpeers[0]);
                }

        } else if((listitems[0].toLower() == QString("phones-signal-deloradd")) && (listitems.size() == 2)) {
                QStringList listpeers = listitems[1].split(";");
                qDebug() << listpeers;
                emitTextMessage(tr("New phone list on %1 : - %2 + %3 = %4 total").arg(listpeers[0],
                                                                                      listpeers[1],
                                                                                      listpeers[2],
                                                                                      listpeers[3]));
                if(listpeers[1].toInt() > 0)
                        sendCommand("phones-del");
                if(listpeers[2].toInt() > 0)
                        sendCommand("phones-add");

        } else if(listitems[0].toLower() == "history") {
                processHistory(listitems[1].split(";"));
        } else if(listitems[0].toLower() == "directory-response") {
                directoryResponse(listitems[1]);
        } else if(listitems[0].toLower() == QString("message")) {
                QTime currentTime = QTime::currentTime();
                QStringList message = listitems[1].split("::");
                // message[0] : emitter name
                if(m_is_a_switchboard)
                        if(message.size() == 2)
                                emitTextMessage(message[0] + tr(" said : ") + message[1]);
                        else
                                emitTextMessage(tr("Unknown") + tr(" said : ") + listitems[1]);
        } else if((listitems[0].toLower() == QString("featuresupdate")) && (listitems.size() == 2)) {
                QStringList featuresupdate_list = listitems[1].split(";");
                qDebug() << featuresupdate_list;
                if(featuresupdate_list.size() == 5)
                        if((m_monitored_asterisk == featuresupdate_list[0]) &&
                           (m_monitored_context  == featuresupdate_list[1]) &&
                           (m_monitored_userid   == featuresupdate_list[2]))
                                initFeatureFields(featuresupdate_list[3], featuresupdate_list[4]);
        } else if((listitems[0].toLower() == QString("featuresget")) && (listitems.size() == 2)) {
                if(listitems[1] != "KO") {
                        QStringList features_list = listitems[1].split(";");
                        resetFeatures();
                        if(features_list.size() > 1)
                                for(int i=0; i<features_list.size()-1; i+=2)
                                        initFeatureFields(features_list[i], features_list[i+1]);
                        emitTextMessage(tr("Received Services Data for ") + m_monitored_asterisk + "/" + m_monitored_userid);
                } else
                        emitTextMessage(tr("Could not retrieve the Services data.") + " " + tr("Maybe Asterisk is down."));

        } else if((listitems[0].toLower() == QString("parkedcall")) && (listitems.size() == 2)) {
                parkingEvent(listitems[0], listitems[1]);
        } else if(listitems[0].toLower() == QString("unparkedcall")) {
                parkingEvent(listitems[0], listitems[1]);
        } else if(listitems[0].toLower() == QString("parkedcalltimeout")) {
                parkingEvent(listitems[0], listitems[1]);
        } else if(listitems[0].toLower() == QString("parkedcallgiveup")) {
                parkingEvent(listitems[0], listitems[1]);

        } else if(listitems[0].toLower() == QString("featuresput")) {
                QString ret = listitems[1].split(";")[0];
                if(ret == "OK") {
                        featurePutIsOK();
                        emitTextMessage("");
                } else {
                        featurePutIsKO();
                        emitTextMessage(tr("Could not modify the Services data.") + " " + tr("Maybe Asterisk is down."));
                }
        } else if(listitems[0].toLower() == QString("faxsend")) {
                quint16 port_fax = listitems[1].toInt();
                m_faxsocket->connectToHost(m_serverhost, port_fax);
        } else if(listitems[0].toLower() == QString("faxsent")) {
                ackFax(listitems[1]);
        } else if((listitems[0] != "") && (listitems[0] != "______") && (listitems[0] != "phones-noupdate"))
                qDebug() << "unknown command" << listitems[0];

        return true;
}

void BaseEngine::sendFaxCommand(const QString & filename, const QString & number,
                                Qt::CheckState hide)
{
        QFile * qf = new QFile(filename);
        qf->open(QIODevice::ReadOnly);
        m_faxdata = new QByteArray();
        m_faxdata->append(qf->readAll());
        qf->close();
        m_faxsize = m_faxdata->size();
        if(m_faxdata->size() > 0) {
                m_faxid = "size=" + QString::number(m_faxsize) +
                        " number=" + number +
                        " hide=" + QString::number(hide) +
                        " astid=" + m_asterisk +
                        " context=" + m_dialcontext;
                sendCommand("faxsend " + m_faxid);
        } else
                ackFax("ko;file");
}

void BaseEngine::popupError(const QString & errorid)
{
        QString errormsg = QString(tr("Server has sent an Error."));
        if(errorid.toLower() == "asterisk_name")
                errormsg = tr("The XIVO Id <%1> is unknown by the XIVO CTI Server.").arg(m_asterisk);

        else if(errorid.toLower() == "connection_refused")
                errormsg = tr("You are not allowed to connect to the Server.");

        else if(errorid.toLower() == "number_of_arguments")
                errormsg = tr("The number of arguments sent is incorrect.\n"
                              "Maybe a version issue ?");

        else if(errorid.toLower() == "user_not_found")
                errormsg = tr("Your registration name <%1,%2> is not known on XIVO Id <%3>.").arg(m_protocol, m_userid, m_asterisk);

        else if(errorid.toLower() == "session_expired")
                errormsg = tr("Your session has expired.");

        else if(errorid.toLower() == "login_passwd")
                errormsg = tr("You entered a wrong login / password.");

        else if(errorid.toLower() == "no_keepalive_from_server")
                errormsg = tr("The server did not reply to the last keepalive.");

        else if(errorid.toLower() == "connection_closed")
                errormsg = tr("The server has just closed the connection.");

        else if(errorid.toLower() == "server_stopped")
                errormsg = tr("The server has just been stopped.");

        else if(errorid.toLower() == "server_reloaded")
                errormsg = tr("The server has just been reloaded.");

        else if(errorid.toLower() == "already_connected")
                errormsg = tr("You are already connected.");

        else if(errorid.toLower() == "uninit_phone")
                errormsg = tr("Your phone <%1,%2> has not been provisioned on XIVO Id <%3>.").arg(m_protocol, m_userid, m_asterisk);

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
        //qDebug() << "BaseEngine::socketReadyRead()";
	//QByteArray data = m_sbsocket->readAll();

	while(m_sbsocket->canReadLine()) {
		QByteArray data  = m_sbsocket->readLine();
                // qDebug() << "BaseEngine::socketReadyRead() data.size() = " << data.size();
		QString line     = QString::fromUtf8(data);
		QStringList list = line.trimmed().split("=");

		if(line.startsWith("<?xml") || line.startsWith("<ui version=")) {
                        // we get here when receiving a customer info in tcp mode
                        qDebug() << "BaseEngine::socketReadyRead() (Customer Info)" << line.size();

                        QBuffer * inputstream = new QBuffer(this);
                        inputstream->open(QIODevice::ReadWrite);
                        inputstream->write(line.toUtf8());
                        inputstream->close();
                        // Get Data and Popup the profile if ok
                        bool qtui = false;
                        if(line.startsWith("<ui version="))
                                qtui = true;
                        Popup * popup = new Popup(inputstream, qtui);
                        connect( popup, SIGNAL(destroyed(QObject *)),
                                 this, SLOT(popupDestroyed(QObject *)) );
                        connect( popup, SIGNAL(save(const QString &)),
                                 this, SLOT(addToDataBase(const QString &)) );
                        connect( popup, SIGNAL(wantsToBeShown(Popup *)),
                                 this, SLOT(profileToBeShown(Popup *)) );

                } else if (list.size() == 2) {
                        if(list[0].toLower() == "loginok") {
				QStringList params = list[1].split(";");
                                m_version_server = -1;
                                QHash<QString, QString> params_list;
                                for(int i = 0 ; i < params.size(); i++) {
                                        QStringList params_couple = params[i].split(":");
                                        if(params_couple.size() == 2)
                                                params_list[params_couple[0]] = params_couple[1];
                                }
                                qDebug() << params_list;
                                m_dialcontext    = params_list["context"];
                                m_extension      = params_list["phonenum"];
                                m_capabilities   = params_list["capas"].split(",");
                                m_version_server = params_list["version"].toInt();
                                m_xivover_server = params_list["xivoversion"];
                                m_forced_state   = params_list["state"];
                                
                                if(m_version_server < REQUIRED_SERVER_VERSION) {
                                        stop();
                                        popupError("version_server:" + QString::number(m_version_server) + ";" + QString::number(REQUIRED_SERVER_VERSION));
                                } else if((m_capabilities.size() == 1) && (m_capabilities[0].size() == 0)) {
                                        stop();
                                        popupError("no_capability");
                                } else {
                                        setState(ELogged); // calls logged()
                                        setAvailState(m_forced_state, true);
                                        m_ka_timerid = startTimer(m_keepaliveinterval);
                                        if(m_is_a_switchboard) /* ask here because not ready when the widget builds up */
                                                askCallerIds();
                                }
			} else if(list[0].toLower() == "loginko") {
                                stop();
                                popupError(list[1]);
			} else
                                parseCommand(list);
                }
	}
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
	qDebug() << "BaseEngine::originateCall()" << src << dst;
	QStringList dstlist = dst.split("/");
	if(dstlist.size() >= 6)
                sendCommand("originate " + src + " " + dst);
        else if(dstlist.size() == 1)
                originateCallGoodAsterisk(src, dst);
}

/*! \brief send an originate command to the server
 */
void BaseEngine::originateCallGoodAsterisk(const QString & src, const QString & dst)
{
	qDebug() << "BaseEngine::originateCallGoodAsterisk()" << src << dst;
	QStringList srclist = src.split("/");
	QStringList dstlist = dst.split("/");
        if((dstlist.size() == 1) && (srclist.size() >= 5))
                sendCommand("originate " + src +
                            " p/" + srclist[1] + "/" + srclist[2] + "///" + dst);
}

/*! \brief originate from locally logged user to the full defined dst p////
 *         (typ. when calling a peerwidget displayed)
 */
void BaseEngine::dialFullChannel(const QString & dst)
{
	qDebug() << "BaseEngine::dialFullChannel()" << dst;
	QStringList dstlist = dst.split("/");
        QString src = "p/" + m_asterisk + "/" + m_dialcontext + "/" + m_protocol + "/" + m_userid + "/" + m_extension;
        if(dstlist.size() == 6)
                originateCall(src, dst);
        else if(dstlist.size() == 1)
                originateCallGoodAsterisk(src, dst);
}

/*! \brief send a transfer call command to the server
 */
void BaseEngine::transferCall(const QString & src, const QString & dst)
{
	qDebug() << "BaseEngine::transferCall()" << src << dst;
	QStringList dstlist = dst.split("/");
	if(dstlist.size() >= 6)
                sendCommand("transfer " + src + " "
                            + dstlist[0] + "/" + dstlist[1] + "/" + m_dialcontext + "/"
                            + dstlist[3] + "/" + dstlist[4] + "/" + dstlist[5]);
	else
                sendCommand("transfer " + src + " p/" + m_asterisk + "/"
                            + m_dialcontext + "/" + "/" + "/" + dst);
}

/*! \brief send an attended transfer call command to the server
 */
void BaseEngine::atxferCall(const QString & src, const QString & dst)
{
	qDebug() << "BaseEngine::atxferCall()" << src << dst;
	QStringList dstlist = dst.split("/");
	if(dstlist.size() >= 6)
                sendCommand("atxfer " + src + " "
                            + dstlist[0] + "/" + dstlist[1] + "/" + m_dialcontext + "/"
                            + dstlist[3] + "/" + dstlist[4] + "/" + dstlist[5]);
	else
                sendCommand("atxfer " + src + " p/" + m_asterisk + "/"
                            + m_dialcontext + "/" + "/" + "/" + dst);
}

/*! \brief send a transfer call command to the server
 */
void BaseEngine::parkCall(const QString & src)
{
	qDebug() << "BaseEngine::parkCall()" << src;
        QStringList srclist = src.split("/");
        sendCommand("transfer " + src + " p/" + srclist[1] + "/"
                    + srclist[2] + "/" + "/" + "/special:parkthecall");
}

/*! \brief intercept a call (a channel)
 *
 * The channel is transfered to "Me"
 *
 * \sa transferCall
 */
void BaseEngine::interceptCall(const QString & src)
{
	qDebug() << "BaseEngine::interceptCall()" << src;
        sendCommand("transfer " + src + " p/"
                    + m_asterisk + "/" + m_dialcontext + "/"
                    + m_protocol + "/" + "/" + m_extension);
}

/*! \brief hang up a channel
 *
 * send a hang up command to the server
 */
void BaseEngine::hangUp(const QString & channel)
{
	qDebug() << "BaseEngine::hangUp()" << channel;
	sendCommand("hangup " + channel);
}

/*! \brief send the directory search command to the server
 *
 * \sa directoryResponse()
 */
void BaseEngine::searchDirectory(const QString & text)
{
	qDebug() << "BaseEngine::searchDirectory()" << text;
        sendCommand("directory-search " + text);
}

/*! \brief ask history for an extension 
 */
void BaseEngine::requestHistory(const QString & peer, int mode)
{
	/* mode = 0 : Out calls
	 * mode = 1 : In calls
	 * mode = 2 : Missed calls */
	if(mode >= 0) {
                qDebug() << "BaseEngine::requestHistory()" << peer;
                sendCommand("history " + peer + " " + QString::number(m_historysize) + " " + QString::number(mode));
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
	m_sbport = port;
}

/*! \brief get server IP address */
const QString & BaseEngine::serverip() const
{
	return m_serverhost;
}

/*! \brief get server port */
const quint16 BaseEngine::sbPort() const
{
	return m_sbport;
}

const QString & BaseEngine::serverast() const
{
        return m_asterisk;
}

void BaseEngine::setServerip(const QString & serverip)
{
	m_serverhost = serverip;
}

void BaseEngine::setServerAst(const QString & serverast)
{
	m_asterisk = serverast;
}

const quint16 BaseEngine::loginPort() const
{
	return m_loginport;
}

void BaseEngine::setLoginPort(const quint16 & port)
{
	m_loginport = port;
}

const QString & BaseEngine::userId() const
{
	return m_userid;
}

void BaseEngine::setUserId(const QString & userid)
{
	m_userid = userid;
}

const QString & BaseEngine::protocol() const
{
	return m_protocol;
}

void BaseEngine::setProtocol(const QString & protocol)
{
	m_protocol = protocol;
}

const QString & BaseEngine::password() const
{
	return m_passwd;
}

void BaseEngine::setPassword(const QString & passwd)
{
	m_passwd = passwd;
}

const QString & BaseEngine::phoneNum() const
{
	return m_extension;
}

const QString & BaseEngine::dialContext() const
{
	return m_dialcontext;
}

const QString & BaseEngine::fullName() const
{
	return m_fullname;
}

void BaseEngine::setDialContext(const QString & dialcontext)
{
	m_dialcontext = dialcontext;
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
		voiceMailChanged(isenabled);
	else if((field == "enablednd") || (field == "dnd"))
		dndChanged(isenabled);
	else if(field == "callfilter")
		callFilteringChanged(isenabled);
	else if(field == "callrecord")
		callRecordingChanged(isenabled);
	else if(field == "unc")
		uncondForwardUpdated(isenabled, value.split(":")[1]);
	else if(field == "busy")
		forwardOnBusyUpdated(isenabled, value.split(":")[1]);
	else if(field == "rna")
		forwardOnUnavailableUpdated(isenabled, value.split(":")[1]);
}

/*!
 * Process incoming UDP datagrams which are likely to be 
 * response from keep alive messages.
 * If the response is not 'OK', goes to
 * the "not connected" state.
 */
void BaseEngine::readKeepLoginAliveDatagrams()
{
        char * buffer;
        int size, nread;
	// qDebug() << "BaseEngine::readKeepLoginAliveDatagrams()";
	while( m_udpsocket->hasPendingDatagrams() )
	{
                size = m_udpsocket->pendingDatagramSize();
                buffer = new char[size + 1];
                nread = m_udpsocket->readDatagram(buffer, size);
                if(nread != size)
                        qDebug() << "BaseEngine::readKeepLoginAliveDatagrams() warning :" << nread << "!=" << size;
                if(nread > 0) {
                        buffer[nread] = '\0';
                        QString line     = QString::fromUtf8(buffer);
                        QStringList qsl  = line.trimmed().split(" ");
                        QStringList list = line.trimmed().split("=");
                        
                        QString reply = qsl[0];
                        
                        if(reply == "DISC") {
                                setState(ENotLogged);
                                qDebug() << qsl; //reply;
                        } else if(reply == "ERROR") {
                                qDebug() << "BaseEngine::readKeepLoginAliveDatagrams()" << qsl;
                                stopKeepAliveTimer();
                                stop();
                                if(qsl.size() > 1)
                                        popupError(qsl[1]);
                                else
                                        popupError("");
                                m_pendingkeepalivemsg = 0;
                                startTryAgainTimer();
                        } else if(reply != "OK")
                                parseCommand(list);
                        m_pendingkeepalivemsg = 0;
                }

                delete [] buffer;
	}
}

void BaseEngine::stopKeepAliveTimer()
{
	if( m_ka_timerid > 0 )
	{
		killTimer(m_ka_timerid);
		m_ka_timerid = 0;
	}
}

void BaseEngine::stopTryAgainTimer()
{
	if( m_try_timerid > 0 )
	{
		killTimer(m_try_timerid);
		m_try_timerid = 0;
	}
}

void BaseEngine::startTryAgainTimer()
{
	if( m_try_timerid == 0 && m_trytoreconnect )
	{
		m_try_timerid = startTimer(m_trytoreconnectinterval);
	}
}

void BaseEngine::setHistorySize(uint size)
{
	m_historysize = size;
}

uint BaseEngine::historysize() const
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
        //qDebug() << "BaseEngine::timerEvent() timerId=" << timerId << m_ka_timerid << m_try_timerid;
	if(timerId == m_ka_timerid) {
                keepLoginAlive();
                event->accept();
        } else if(timerId == m_try_timerid) {
                emitTextMessage(tr("Attempting to reconnect to server"));
		start();
		event->accept();
	} else {
		event->ignore();
	}
}

void BaseEngine::setIsASwitchboard(bool b)
{
	m_is_a_switchboard = b;
}

bool BaseEngine::isASwitchboard()
{
	return m_is_a_switchboard;
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

void BaseEngine::featurePutVoiceMail(bool b)
{
        sendCommand("featuresput " + m_monitored_asterisk + " " + m_monitored_context + " " + m_monitored_userid + " enablevoicemail " + QString(b ? "1" : "0"));
}

void BaseEngine::featurePutCallRecording(bool b)
{
	sendCommand("featuresput " + m_monitored_asterisk + " " + m_monitored_context + " " + m_monitored_userid + " callrecord " + QString(b ? "1" : "0"));
}

void BaseEngine::featurePutCallFiltering(bool b)
{
	sendCommand("featuresput " + m_monitored_asterisk + " " + m_monitored_context + " " + m_monitored_userid + " callfilter " + QString(b ? "1" : "0"));
}

void BaseEngine::featurePutDnd(bool b)
{
	sendCommand("featuresput " + m_monitored_asterisk + " " + m_monitored_context + " " + m_monitored_userid + " enablednd " + QString(b ? "1" : "0"));
}

void BaseEngine::featurePutForwardOnUnavailable(bool b, const QString & dst)
{
	sendCommand("featuresput " + m_monitored_asterisk + " " + m_monitored_context + " " + m_monitored_userid + " enablerna " + QString(b ? "1" : "0"));
	sendCommand("featuresput " + m_monitored_asterisk + " " + m_monitored_context + " " + m_monitored_userid + " destrna " + dst);
}

void BaseEngine::featurePutForwardOnBusy(bool b, const QString & dst)
{
	sendCommand("featuresput " + m_monitored_asterisk + " " + m_monitored_context + " " + m_monitored_userid + " enablebusy " + QString(b ? "1" : "0"));
	sendCommand("featuresput " + m_monitored_asterisk + " " + m_monitored_context + " " + m_monitored_userid + " destbusy " + dst);
}

void BaseEngine::featurePutUncondForward(bool b, const QString & dst)
{
	sendCommand("featuresput " + m_monitored_asterisk + " " + m_monitored_context + " " + m_monitored_userid + " enableunc " + QString(b ? "1" : "0"));
	sendCommand("featuresput " + m_monitored_asterisk + " " + m_monitored_context + " " + m_monitored_userid + " destunc " + dst);
}

void BaseEngine::askFeatures(const QString & peer)
{
//        qDebug() << "BaseEngine::askFeatures()" << peer << m_asterisk << m_dialcontext << m_userid;
        m_monitored_asterisk = m_asterisk;
        m_monitored_context  = m_dialcontext;
        m_monitored_userid   = m_userid;
        QStringList peerp = peer.split("/");
        if(peerp.size() == 6) {
                m_monitored_asterisk = peerp[1];
                m_monitored_context  = peerp[2];
                m_monitored_userid   = peerp[5];
        }
        sendCommand("featuresget " + m_monitored_asterisk + " " + m_monitored_context + " " + m_monitored_userid);
}

void BaseEngine::askCallerIds()
{
        qDebug() << "BaseEngine::askCallerIds()";
	sendCommand("phones-list");
}

void BaseEngine::addToDataBase(const QString & dbdetails)
{
        qDebug() << "BaseEngine::addToDataBase()" << dbdetails;
        if (dbdetails.size() > 0)
                sendCommand("database " + dbdetails);
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

const BaseEngine::EngineState BaseEngine::state() const
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
                        m_enabled_presence = m_capabilities.contains("presence");
			stopTryAgainTimer();
			if(m_checked_presence && m_enabled_presence)
                                availAllowChanged(true);
			logged();
		} else if(state == ENotLogged) {
                        m_enabled_presence = false;
			availAllowChanged(false);
			delogged();
		}
	}
}

/*!
 * Builds a string defining who is the client (SB or XC @ (X11, WIN, MAC))
*/
void BaseEngine::setMyClientId()
{
        QString whatami;
        if(m_is_a_switchboard)
                whatami = QString("SB");
        else
                whatami = QString("XC");

#if defined(Q_WS_X11)
        m_clientid = QString(whatami + "@X11");
#elif defined(Q_WS_WIN)
        m_clientid = QString(whatami + "@WIN");
#elif defined(Q_WS_MAC)
        m_clientid = QString(whatami + "@MAC");
#else
        m_clientid = QString(whatami + "@unknown");
#endif
}

/*!
 * Perform the first login step once the TCP connection is established.
 */
void BaseEngine::identifyToTheServer()
{
	QString outline;
	m_serveraddress = m_loginsocket->peerAddress();
	qDebug() << "BaseEngine::identifyToTheServer()" << m_serveraddress;
        setMyClientId();
	outline.append("LOGIN " + m_asterisk + "/" + m_protocol.toLower() + m_userid + \
                       " " + m_clientid + " " + __current_client_version__);
	qDebug() << "BaseEngine::identifyToTheServer()" << outline;
	outline.append("\r\n");
	m_loginsocket->write(outline.toAscii());
	m_loginsocket->flush();
}

/*!
 * Perform the following of the login process after identifyToTheServer()
 * made the first step.
 * Theses steps are : sending the password, sending the port,
 *   just reading the session id from server response.
 * The state is changed accordingly.
 */
void BaseEngine::processLoginDialog()
{
	char buffer[256];
	int len;
	qDebug() << "BaseEngine::processLoginDialog()";
	if(!m_loginsocket->canReadLine())
	{
		qDebug() << "no line ready to be read";
		return;
	}
	len = m_loginsocket->readLine(buffer, sizeof(buffer));
	if(len < 0)
	{
		qDebug() << "readLine() returned -1, closing socket";
		m_loginsocket->close();
		setState(ENotLogged); // calls delogged()
		return;
	}
	QString readLine = QString::fromAscii(buffer);
	QString outline;
	if(readLine.startsWith("Send PASS"))
	{
		outline = "PASS ";
		outline.append(m_passwd);
	}
	else if(readLine.startsWith("Send PORT"))
	{
		if(m_tcpmode) {
			outline = "TCPMODE";
		} else {
			outline = "PORT ";
			outline.append(QString::number(m_listenport));
		}
	}
	else if(readLine.startsWith("Send STATE"))
	{
		outline = "STATE ";
                if(m_checked_presence)
                        outline.append(m_availstate);
                else
                        outline.append("unknown");
	}
	else if(readLine.startsWith("OK SESSIONID"))
	{
                m_version_server = -1;
		readLine.remove(QChar('\r')).remove(QChar('\n'));
		QStringList sessionResp = readLine.split(" ");
		qDebug() << sessionResp;
		if(sessionResp.size() == 4) {
			m_sessionid =    sessionResp[2];
                        QStringList params = sessionResp[3].split(";");
                        QHash<QString, QString> params_list;
                        for(int i = 0 ; i < params.size(); i++) {
                                QStringList params_couple = params[i].split(":");
                                if(params_couple.size() == 2)
                                        params_list[params_couple[0]] = params_couple[1];
                        }
                        qDebug() << "BaseEngine::socketReadyRead()" << m_sessionid << params_list;
                        m_dialcontext    = params_list["context"];
                        m_extension      = params_list["phonenum"];
                        m_capabilities   = params_list["capas"].split(",");
                        m_version_server = params_list["version"].toInt();
                        m_xivover_server = params_list["xivoversion"];
                        m_forced_state   = params_list["state"];
                }

                m_loginsocket->close();
                if(m_version_server < REQUIRED_SERVER_VERSION) {
                        stop();
                        popupError("version_server:" + QString::number(m_version_server) + ";" + QString::number(REQUIRED_SERVER_VERSION));
                        return;
                }
                if((m_capabilities.size() == 1) && (m_capabilities[0].size() == 0)) {
                        stop();
                        popupError("no_capability");
                        return;
                }
                setState(ELogged);
                setAvailState(m_forced_state, true);
                // start the keepalive timer
                m_ka_timerid = startTimer(m_keepaliveinterval);
                if(m_is_a_switchboard) /* ask here because not ready when the widget builds up */
                        askCallerIds();
                return;
	}
	else if(readLine.startsWith("ERROR")) {
                readLine.remove(QChar('\r')).remove(QChar('\n'));
                QStringList sessionResp = readLine.split(" ");
		qDebug() << "ERROR response received from server" << readLine;
		m_loginsocket->close();
		setState(ENotLogged);
                if(sessionResp.size() > 1)
                        popupError(sessionResp[1]);
                else
                        popupError("");
		return;
        }
	else
	{
		readLine.remove(QChar('\r')).remove(QChar('\n'));
		qDebug() << "Response from server not recognized, closing" << readLine;
		m_loginsocket->close();
		setState(ENotLogged);
		return;
	}
	qDebug() << "BaseEngine::processLoginDialog() : " << outline;
	outline.append("\r\n");
	m_loginsocket->write(outline.toAscii());
	m_loginsocket->flush();
}

/*!
 * This slot method is called when a pending connection is
 * waiting on the m_listenserver.
 * It processes the incoming data and create a popup to display it.
 */
void BaseEngine::handleProfilePush()
{
	qDebug() << "BaseEngine::handleProfilePush()";
	m_connection = m_listenserver->nextPendingConnection();
	connect( m_connection, SIGNAL(disconnected()),
	         m_connection, SLOT(deleteLater()) );
	connect( m_connection, SIGNAL(readyRead()),
	         this, SLOT(readProfile()) );
	// signals on the socket : connected() disconnected()
	// error() hostFound() stateChanged()
	// iodevice : readyRead() aboutToClose() bytesWritten()
}

void BaseEngine::readProfile()
{
	qDebug() << "BaseEngine::readProfile()" << m_connection->peerAddress().toString() << m_connection->peerPort();
	while(m_connection->canReadLine()) {
		QByteArray data = m_connection->readLine();
		QString line = QString::fromUtf8(data);
		if(line.startsWith("<?xml") || line.startsWith("<ui version=")) {
                        QBuffer * inputstream = new QBuffer(this);
                        inputstream->open(QIODevice::ReadWrite);
                        inputstream->write(line.toUtf8());
                        inputstream->close();
                        // Get Data and Popup the profile if ok
                        bool qtui = false;
                        if(line.startsWith("<ui version="))
                                qtui = true;
                        Popup * popup = new Popup(inputstream, qtui);
                        connect( popup, SIGNAL(destroyed(QObject *)),
                                 this, SLOT(popupDestroyed(QObject *)) );
                        connect( popup, SIGNAL(save(const QString &)),
                                 this, SLOT(addToDataBase(const QString &)) );
                        connect( popup, SIGNAL(wantsToBeShown(Popup *)),
                                 this, SLOT(profileToBeShown(Popup *)) );
                }
        }
}


void BaseEngine::popupDestroyed(QObject * obj)
{
	qDebug() << "BaseEngine::popupDestroyed()" << obj;
	//obj->dumpObjectTree();
}

void BaseEngine::profileToBeShown(Popup * popup)
{
        qDebug() << "BaseEngine::profileToBeShown()";
	newProfile( popup );
}

/*!
 * Send a keep alive message to the login server.
 * The message is sent in a datagram through m_udpsocket
 */ 
void BaseEngine::keepLoginAlive()
{
	//qDebug() << "BaseEngine::keepLoginAlive()";
	// got to disconnected state if more than xx keepalive messages
	// have been left without response.
	if(m_pendingkeepalivemsg > 1)
	{
		qDebug() << "m_pendingkeepalivemsg" << m_pendingkeepalivemsg << "=> 0";
		stopKeepAliveTimer();
		setState(ENotLogged);
		m_pendingkeepalivemsg = 0;
                popupError("no_keepalive_from_server");
		startTryAgainTimer();
		return;
	}
        if(m_tcpmode)
                sendCommand("availstate " + m_availstate);
        else
                if(m_state == ELogged) {
                        QString outline = "ALIVE ";
                        outline.append(m_asterisk + "/" + m_protocol.toLower() + m_userid);
                        outline.append(" SESSIONID ");
                        outline.append(m_sessionid);
                        outline.append(" STATE ");
                        if(m_checked_presence && m_enabled_presence)
                                outline.append(m_availstate);
                        else
                                outline.append("unknown");
                        qDebug() << "BaseEngine::keepLoginAlive()" << outline;
                        outline.append("\r\n");
                        m_udpsocket->writeDatagram( outline.toAscii(),
                                                    m_serveraddress, m_loginport + 1 );
                        m_pendingkeepalivemsg++;
                        // if the last keepalive msg has not been answered, send this one
                        // twice
                        if(m_pendingkeepalivemsg > 1) {
                                m_udpsocket->writeDatagram( outline.toAscii(),
                                                            m_serveraddress, m_loginport + 1 );
                                m_pendingkeepalivemsg++;
                        }
                }
}

/*!
 * Send a keep alive message to the login server.
 * The message is sent in a datagram through m_udpsocket
 */ 
void BaseEngine::sendMessage(const QString & txt)
{
	sendCommand("message " + txt);
}

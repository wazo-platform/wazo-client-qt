/* XiVO Client
 * Copyright (C) 2007-2009, Proformatique
 *
 * This file is part of XiVO Client.
 *
 * XiVO Client is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version, with a Section 7 Additional
 * Permission as follows:
 *   This notice constitutes a grant of such permission as is necessary
 *   to combine or link this software, or a modified version of it, with
 *   the OpenSSL project's "OpenSSL" library, or a derivative work of it,
 *   and to copy, modify, and distribute the resulting work. This is an
 *   extension of the special permission given by Trolltech to link the
 *   Qt code with the OpenSSL library (see
 *   <http://doc.trolltech.com/4.4/gpl.html>). The OpenSSL library is
 *   licensed under a dual license: the OpenSSL License and the original
 *   SSLeay license.
 *
 * XiVO Client is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with XiVO Client.  If not, see <http://www.gnu.org/licenses/>.
 */

/* $Revision$
 * $Date$
 */

#include <QCryptographicHash>
#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QSettings>
//#include <QSocketNotifier>
#include <QTcpSocket>
#include <QTimerEvent>

#include "JsonToVariant.h"
#include "VariantToJson.h"

#include "baseengine.h"
#include "xivoconsts.h"
#include "userinfo.h"
#include "phoneinfo.h"
#include "agentinfo.h"
#include "queueinfo.h"

/*! \brief Constructor.
 *
 * Construct the BaseEngine object and load settings.
 * The TcpSocket Object used to communicate with the server
 * is created and connected to the right slots/signals
 *
 * This constructor initialize the UDP socket and
 * the TCP listening socket.
 * It also connects signals with the right slots.
 * Take ownership of settings object.
 */
BaseEngine::BaseEngine(QSettings * settings,
                       QObject * parent)
    : QObject(parent),
      m_serverhost(""), m_ctiport(0),
      m_userid(""), m_useridopt(""), m_company(""), m_password(""), m_agentphonenumber(""),
      m_sessionid(""), m_state(ENotLogged),
      m_pendingkeepalivemsg(0), m_logfile(NULL),
      m_byte_counter(0), m_attempt_loggedin(false),
      m_rate_bytes(0), m_rate_msec(0), m_rate_samples(0),
      m_forced_to_disconnect(false)
{
    settings->setParent( this );
    m_timerid_keepalive = 0;
    m_timerid_tryreconnect = 0;
    m_timer = -1;
    m_settings = settings;
    loadSettings();
    
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
    QStringList socknames = (QStringList() << "cticommands" << "filetransfers");
    foreach(QString sockname, socknames) {
        m_tcpsocket[sockname] = new QTcpSocket(this);
        m_tcpsocket[sockname]->setProperty("socket", sockname);
        connect( m_tcpsocket[sockname], SIGNAL(connected()),
                 this, SLOT(socketConnected()));
        connect( m_tcpsocket[sockname], SIGNAL(disconnected()),
                 this, SLOT(socketDisconnected()));
        connect( m_tcpsocket[sockname], SIGNAL(hostFound()),
                 this, SLOT(socketHostFound()));
        connect( m_tcpsocket[sockname], SIGNAL(error(QAbstractSocket::SocketError)),
                 this, SLOT(socketError(QAbstractSocket::SocketError)));
        connect( m_tcpsocket[sockname], SIGNAL(stateChanged(QAbstractSocket::SocketState)),
             this, SLOT(socketStateChanged(QAbstractSocket::SocketState)));
        connect( m_tcpsocket[sockname], SIGNAL(readyRead()),
                 this, SLOT(socketReadyRead()));
    }
    
    if(m_autoconnect)
        start();
}

/*! */
void BaseEngine::readInputEvent(int) {
    // does nothing ??
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
    // clean m_users and m_phones
    clearUserList();
    clearPhoneList();
    clearAgentList();
    clearQueueList();
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
    QString profile = m_settings->value("profile/default", "").toString();
    if(profile.isEmpty())
        m_profilename = "engine";
    else
        m_profilename = "engine-" + profile;
    m_settings->beginGroup(m_profilename);
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
    m_showagselect = m_settings->value("showagselect", 2).toUInt();
    m_agentphonenumber  = m_settings->value("agentphonenumber").toString();
    
    m_autoconnect = m_settings->value("autoconnect", false).toBool();
    m_trytoreconnect = m_settings->value("trytoreconnect", false).toBool();
    m_trytoreconnectinterval = m_settings->value("trytoreconnectinterval", 20*1000).toUInt();
    m_keepaliveinterval = m_settings->value("keepaliveinterval", 20*1000).toUInt();
    m_checked_lastconnwins = m_settings->value("lastconnwins", false).toBool();
    m_availstate = m_settings->value("availstate", "available").toString();
    m_settings->endGroup();
    
    m_settings->beginGroup("user-gui");
    m_historysize = m_settings->value("historysize", 8).toUInt();
    
    QString defaultguioptions;
    QFile defaultguioptions_file(":/common/guioptions.json");
    if(defaultguioptions_file.exists()) {
        defaultguioptions_file.open(QFile::ReadOnly);
        defaultguioptions = defaultguioptions_file.readAll();
        defaultguioptions_file.close();
    }
    QVariant data;
    try {
        data = JsonQt::JsonToVariant::parse(defaultguioptions);
    }
    catch(JsonQt::ParseException) {
        qDebug() << "BaseEngine::loadSettings() exception catched for" << defaultguioptions;
    }
    
    m_guioptions["client_gui"] = m_settings->value("guisettings", data);
    m_loginkind = m_guioptions.value("client_gui").toMap().value("loginkind").toInt();
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
    m_settings->beginGroup(m_profilename);
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
    m_settings->setValue("agentphonenumber", m_agentphonenumber);
    
    m_settings->setValue("autoconnect", m_autoconnect);
    m_settings->setValue("trytoreconnect", m_trytoreconnect);
    m_settings->setValue("trytoreconnectinterval", m_trytoreconnectinterval);
    m_settings->setValue("keepaliveinterval", m_keepaliveinterval);
    m_settings->setValue("lastconnwins", m_checked_lastconnwins);
    m_settings->setValue("availstate", m_availstate);
    m_settings->endGroup();
    
    m_settings->beginGroup("user-gui");
    m_settings->setValue("historysize", m_historysize);
    
    m_settings->setValue("guisettings", m_guioptions.value("client_gui"));
    m_settings->endGroup();
    
    m_settings->beginGroup("user-functions");
    foreach (QString function, CheckFunctions)
        m_settings->setValue(function, m_checked_function[function]);
    m_settings->endGroup();
}

/*!
 *
 */
void BaseEngine::setCheckedFunction(const QString & function, bool b)
{
    if(b != m_checked_function[function]) {
        m_checked_function[function] = b;
        if ((state() == ELogged) && m_enabled_function[function]) {
            if(function == "presence")
                availAllowChanged(b);
        }
    }
}

bool BaseEngine::checkedFunction(const QString & function)
{
    return m_checked_function[function];
}

void BaseEngine::setEnabledFunction(const QString & function, bool b)
{
    m_enabled_function[function] = b;
}

bool BaseEngine::enabledFunction(const QString & function)
{
    return m_enabled_function[function];
}

void BaseEngine::setLogFile(const QString & logfilename)
{
    if (! logfilename.isEmpty()) {
        m_logfile = new QFile( this );
        QDir::setCurrent(QDir::homePath());
        m_logfile->setFileName(logfilename);
        m_logfile->open(QIODevice::Append);
    }
}

void BaseEngine::logAction(const QString & logstring)
{
    if(m_logfile != NULL) {
        QString tolog = QDateTime::currentDateTime().toString(Qt::ISODate) + " " + logstring + "\n";
        m_logfile->write(tolog.toUtf8());
        m_logfile->flush();
    }
}

/*! \brief set login/pass and then starts */
void BaseEngine::config_and_start(const QString & login,
                                  const QString & pass,
                                  const QString & agentphonenumber)
{
    setUserId(login);
    m_password = pass;
    // if agentphonenumber's size is 0, no login as agent
    m_agentphonenumber = agentphonenumber;
    saveSettings();
    start();
}

void BaseEngine::powerEvent(const QString & eventinfo)
{
    QVariantMap command;
    command["class"] = "powerevent";
    command["direction"] = "xivoserver";
    command["value"] = eventinfo;
    sendJsonCommand(command);
}

/*! \brief Starts the connection to the server
 * This method starts the login process by connection
 * to the server.
 */
void BaseEngine::start()
{
    qDebug() << "BaseEngine::start()" << m_serverhost << m_checked_function;
    
    // (In case the TCP sockets were attempting to connect ...) aborts them first
    m_tcpsocket.value("cticommands")->abort();
    
    connectSocket();
    m_byte_counter = 0;
    m_time.start();
}

/*! \brief Closes the connection to the server
 * This method disconnect the engine from the server
 */
void BaseEngine::stop()
{
    QString stopper = sender()->property("stopper").toString();
    qDebug() << "BaseEngine::stop()" << sender() << stopper;
    if(m_attempt_loggedin) {
            QVariantMap command;
            command["class"] = "logout";
            command["direction"] = "xivoserver";
            command["stopper"] = stopper;
            sendJsonCommand(command);
            m_settings->setValue("lastlogout/stopper", stopper);
            m_settings->setValue("lastlogout/datetime", QDateTime::currentDateTime().toString(Qt::ISODate));
            m_attempt_loggedin = false;
    }
    
    m_tcpsocket.value("cticommands")->flush();
    m_tcpsocket.value("cticommands")->disconnectFromHost();
    
    stopKeepAliveTimer();
    stopTryAgainTimer();
    setState(ENotLogged);
    m_sessionid = "";
    
    clearUserList();
    clearPhoneList();
    clearAgentList();
    clearQueueList();
    if(m_time.isValid())
    {
        int elapsed = m_time.elapsed();
        qDebug() << m_byte_counter << "bytes received in" << elapsed << "ms : "
                 << (elapsed?QString::number((1000*m_byte_counter)/elapsed):QString("infinite"))
                 << "Bytes/Second";
    }
}


/*! \brief clear the content of m_users
 *
 * Delete all contained UserInfo objects
 */
void BaseEngine::clearUserList()
{
    QHashIterator<QString, UserInfo *> iter = QHashIterator<QString, UserInfo *>(m_users);
    while( iter.hasNext() )
        {
            iter.next();
            delete iter.value();
        }
    m_users.clear();
}

/*! \brief clear the content of m_phones
 *
 * Delete all contained PhoneInfo objects
 */
void BaseEngine::clearPhoneList()
{
    QHashIterator<QString, PhoneInfo *> iter = QHashIterator<QString, PhoneInfo *>(m_phones);
    while( iter.hasNext() )
        {
            iter.next();
            delete iter.value();
        }
    m_phones.clear();
}

/*! \brief clear the content of m_agents
 *
 * Delete all contained AgentInfo objects
 */
void BaseEngine::clearAgentList()
{
    QHashIterator<QString, AgentInfo *> iter = QHashIterator<QString, AgentInfo *>(m_agents);
    while( iter.hasNext() )
        {
            iter.next();
            delete iter.value();
        }
    m_agents.clear();
}

/*! \brief clear the content of m_queues
 *
 * Delete all contained QueueInfo objects
 */
void BaseEngine::clearQueueList()
{
    QHashIterator<QString, QueueInfo *> iter = QHashIterator<QString, QueueInfo *>(m_queues);
    while( iter.hasNext() )
        {
            iter.next();
            delete iter.value();
        }
    m_queues.clear();
}

QHash<QString, AgentInfo *> BaseEngine::agents()
{
    return m_agents;
}

QHash<QString, QueueInfo *> BaseEngine::queues()
{
    return m_queues;
}

QHash<QString, PhoneInfo *> BaseEngine::phones()
{
    return m_phones;
}

QHash<QString, UserInfo *> BaseEngine::users()
{
    return m_users;
}

/*! \brief initiate connection to the server
 */
void BaseEngine::connectSocket()
{
    if(! m_userid.isEmpty()) {
        qDebug() << "BaseEngine::connectSocket()" << m_serverhost << m_ctiport;
        m_tcpsocket.value("cticommands")->connectToHost(m_serverhost, m_ctiport);
    }
}

bool BaseEngine::lastconnwins() const
{
    return m_checked_lastconnwins;
}

void BaseEngine::setLastConnWins(bool b)
{
    m_checked_lastconnwins = b;
}

/*! \brief gets m_capaxlets */
const QStringList & BaseEngine::getCapaXlets() const
{
    return m_capaxlets;
}

/*! \brief gets m_capapresence */
const QVariantMap & BaseEngine::getCapaPresence() const
{
    return m_capapresence;
}

const QVariantMap BaseEngine::getGuiOptions(const QString & arg) const
{
    return m_guioptions.value(arg).toMap();
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
        else
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

/*! \brief set availability */
void BaseEngine::setAvailability()
{
    // qDebug() << "BaseEngine::setAvailability()" << sender();
    QString availstate = sender()->property("availstate").toString();
    setAvailState(availstate, false);
}

/*! \brief send command to XiVO CTI server */
void BaseEngine::sendCommand(const QString & command)
{
    if(m_tcpsocket.value("cticommands")->state() == QAbstractSocket::ConnectedState)
        m_tcpsocket.value("cticommands")->write((command + "\n").toAscii());
}

/*! \brief encode json and then send command to XiVO CTI server */
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
        // qDebug() << "BaseEngine::processHistory()" << histitem;
        QDateTime dt = QDateTime::fromString(histitem.toMap()["ts"].toString(), Qt::ISODate);
        int duration = histitem.toMap()["duration"].toInt();
        QString fullname = histitem.toMap()["fullname"].toString();
        QString direction = histitem.toMap()["direction"].toString();
        QString techdef = histitem.toMap()["termin"].toString();
        updateLogEntry(dt, duration, fullname, direction, techdef);
    }
}

/*! \brief set monitored peer id */
void BaseEngine::monitorPeerRequest(const QString & userid)
{
    // qDebug() << "BaseEngine::monitorPeerRequest()" << userid;
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
        m_attempt_loggedin = false;
        QVariantMap command;
        command["class"] = "login_id";
        command["direction"] = "xivoserver";
        command["userid"] = m_userid;
        command["company"] = m_company;
        command["ident"] = m_clientid;
        command["version"] = __current_client_version__;
        command["xivoversion"] = __xivo_version__;
        
        command["lastlogout-stopper"] = m_settings->value("lastlogout/stopper").toString();
        command["lastlogout-datetime"] = m_settings->value("lastlogout/datetime").toString();
        m_settings->remove("lastlogout/stopper");
        m_settings->remove("lastlogout/datetime");
        
        sendJsonCommand(command);
    } else if(socketname == "filetransfers") {
        QVariantMap command;
        command["class"] = "filetransfer";
        command["direction"] = "xivoserver";
        command["tdirection"] = m_filedir;
        command["fileid"] = m_fileid;
        QString jsoncommand(JsonQt::VariantToJson::parse(command));
        if(m_tcpsocket["filetransfers"]->state() == QAbstractSocket::ConnectedState)
            m_tcpsocket["filetransfers"]->write((jsoncommand + "\n").toAscii());
    }
}

/*! \brief called when the socket is disconnected from the server
 */
void BaseEngine::socketDisconnected()
{
    QString socketname = sender()->property("socket").toString();
    if(socketname == "cticommands") {
        setState(ENotLogged); // calls delogged();
        emitTextMessage(tr("Connection lost with XiVO CTI server"));
        startTryAgainTimer();
        //removePeers();
        //connectSocket();
    }
}

/*! \brief cat host found socket signal
 *
 * This slot is connected to the hostFound() signal of the m_tcpsocket.value("cticommands")
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
        stopKeepAliveTimer();
        popupError("connection_closed");
        break;
    case QAbstractSocket::HostNotFoundError:
/*! \brief set monitored peer id */
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
    // qDebug() << "BaseEngine::socketStateChanged()" << socketname;
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
                                         const QString & agentnumber)
{
    foreach (UserInfo * uinfo, m_users)
        if(uinfo->hasAgentNumber(astid, agentnumber))
            return uinfo;
    return NULL;
}


void BaseEngine::updatePhone(const QString & astid,
                             const QString & phoneid,
                             const QMap<QString, QVariant> & properties)
{
    //if(phoneid == "sip.default.121.121")
    // qDebug() << "BaseEngine::updatePhone()" << astid << phoneid << properties;
    //qDebug() << map.keys();
    QString key = astid + "." + phoneid;
    UserInfo * ui = findUserFromPhone(astid, phoneid);
    if( ! m_phones.contains( key ) )
    {
        m_phones[key] = new PhoneInfo(astid, properties);
        if(ui)
            ui->updatePhone( m_phones[key] );
    } else {
        m_phones[key]->update(properties);
    }
    if(ui)
    {
        emit userUpdated(ui);
    }
}

QStringList BaseEngine::updateQueue(const QString & astid,
                                    const QString & queueid,
                                    const QMap<QString, QVariant> & properties)
{
    // qDebug() << "BaseEngine::updateQueue" << astid << queueid << properties;
    QStringList keychanges;
    QString key = QString("queue:%1/%2").arg(astid).arg(queueid);
    if( ! m_queues.contains( key ) ) {
        m_queues[key] = new QueueInfo(astid, queueid, properties);
        keychanges << key;
    } else {
        bool haschanged = m_queues[key]->update(properties);
        if (haschanged)
            keychanges << key;
    }
    return keychanges;
}

QStringList BaseEngine::updateQueueAgent(const QString & astid,
                                         const QString & queueid,
                                         const QMap<QString, QVariant> & properties)
{
    QStringList keychanges;
    QString key = QString("queue:%1/%2").arg(astid).arg(queueid);
    if( ! m_queues.contains( key ) ) {
        m_queues[key] = new QueueInfo(astid, queueid, properties);
        keychanges << key;
    } else {
        bool haschanged = m_queues[key]->updateAgent(properties);
        if (haschanged)
            keychanges << key;
    }
    return keychanges;
}

QStringList BaseEngine::updateAgent(const QString & astid,
                                    const QString & agentid,
                                    const QMap<QString, QVariant> & properties)
{
    QStringList keychanges;
    QString key = QString("agent:%1/%2").arg(astid).arg(agentid);
    if( ! m_agents.contains( key ) ) {
        m_agents[key] = new AgentInfo(astid, agentid, properties);
        keychanges << key;
    } else {
        bool haschanged = m_agents[key]->update(properties);
        if (haschanged)
            keychanges << key;
    }
    return keychanges;
}

QStringList BaseEngine::updateAgentQueue(const QString & astid,
                                         const QString & agentid,
                                         const QMap<QString, QVariant> & properties)
{
    QStringList keychanges;
    QString key = QString("agent:%1/%2").arg(astid).arg(agentid);
    if( ! m_agents.contains( key ) ) {
        m_agents[key] = new AgentInfo(astid, agentid, properties);
        keychanges << key;
    } else {
        bool haschanged = m_agents[key]->updateQueue(properties);
        if (haschanged)
            keychanges << key;
    }
    return keychanges;
}

double BaseEngine::timeServer() const
{
    return m_timesrv;
}

const QDateTime & BaseEngine::timeClient() const
{
    return m_timeclt;
}

double BaseEngine::timeDeltaServerClient() const
{
    return m_timeclt.toTime_t()-m_timesrv;
}

/*! \brief parse JSON and then process command */
void BaseEngine::parseCommand(const QString & line)
{
    QVariant data;
    try {
        QTime jsondecodetime;
        jsondecodetime.start();
        data = JsonQt::JsonToVariant::parse(line.trimmed());
        m_rate_msec += jsondecodetime.elapsed();
        m_rate_bytes += line.trimmed().size();
        m_rate_samples ++;
    }
    catch(JsonQt::ParseException) {
        qDebug() << "BaseEngine::parseCommand() exception catched for" << line.trimmed();
        data = QVariant(QVariant::Invalid);
    }
    
    if(! data.isValid())
        return;
    
    QVariantMap datamap = data.toMap();
    QString direction = datamap["direction"].toString();
    QString function = datamap["function"].toString();
    
    if(direction == "client") {
        QString thisclass = datamap["class"].toString();
        m_timesrv = datamap["timenow"].toDouble();
        m_timeclt = QDateTime::currentDateTime();
        // qDebug() << datamap["timenow"].toString() << "BaseEngine message received"
        // << thisclass << datamap["function"].toString()
        // << datamap["phoneid"].toString();
        if (thisclass == "callcampaign") {
            requestFileListResult(datamap["payload"]);
            
        } else if (thisclass == "parkcall") {
            parkingEvent(datamap["payload"]);
            
        } else if (thisclass == "sheet") {
            // TODO : use id better than just channel name
            qDebug() << "** sheet" << datamap;
            QString channel = datamap["channel"].toString();
            if(function == "getownership") {
                gotSheetOwnership(channel);
            } else if(function == "loseownership") {
                lostSheetOwnership(channel);
            } else if(function == "entryadded") {
                sheetEntryAdded(channel, datamap["entry"].toMap());
            } if(datamap.contains("payload")) {
                QString payload;
                QByteArray qba = QByteArray::fromBase64(datamap["payload"].toString().toAscii());
                if(datamap["compressed"].toBool())
                    payload = QString::fromUtf8(qUncompress(qba));
                else
                    payload = QString::fromUtf8(qba);
                // will eventually call the XML parser
                displayFiche(payload, false, channel);
                if(datamap.contains("entries")) {
                    QVariantList entries = datamap["entries"].toList();
                    foreach(QVariant entry, entries) {
                        sheetEntryAdded(channel, entry.toMap());
                    }
                }
            }
        } else if (thisclass == "queues") {
            //qDebug() << "*** queues !!!" << function << line.length();
            if(function == "sendlist") {
                QStringList kk;
                QMap<QString, QVariant> payload = datamap["payload"].toMap();
                foreach(QString astid, payload.keys()) {
                    QMap<QString, QVariant> values = payload[astid].toMap();
                    foreach(QString queueid, values.keys()) {
                        kk += updateQueue(astid, queueid, values[queueid].toMap());
                    }
                }
                if(! kk.isEmpty())
                    emit newQueueList(kk);
            } else if(function == "update") {
                QStringList kk;
                QMap<QString, QVariant> payload = datamap["payload"].toMap();
                foreach(QString astid, payload.keys()) {
                    QMap<QString, QVariant> values = payload[astid].toMap();
                    foreach(QString queueid, values.keys()) {
                        kk += updateQueueAgent(astid, queueid, values[queueid].toMap());
                    }
                }
                if(! kk.isEmpty())
                    emit newQueueList(kk);
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
            
        } else if (thisclass == "groups") {
            // qDebug() << "___" << thisclass << datamap;
            
        } else if (thisclass == "agents") {
            //qDebug() << "*** agents !!!" << function << line.length();
            if(function == "sendlist") {
                QStringList kk;
                QMap<QString, QVariant> payload = datamap.value("payload").toMap();
                foreach(QString astid, payload.keys()) {
                    QMap<QString, QVariant> values = payload[astid].toMap();
                    foreach(QString agentid, values.keys()) {
                        kk += updateAgent(astid, agentid, values[agentid].toMap());
                    }
                }
                if(! kk.isEmpty())
                    emit newAgentList(kk);
                
            } else if(function == "update") {
                QStringList kk;
                QMap<QString, QVariant> payload = datamap.value("payload").toMap();
                foreach(QString astid, payload.keys()) {
                    QMap<QString, QVariant> values = payload[astid].toMap();
                    foreach(QString agentid, values.keys()) {
                        kk += updateAgentQueue(astid, agentid, values[agentid].toMap());
                    }
                }
                if(! kk.isEmpty())
                    emit newAgentList(kk);
                
            } else if(function == "update-old") {
                QVariant params = datamap["payload"];
                QString action = params.toMap()["action"].toString();
                QString astid = params.toMap()["astid"].toString();
                QString agentid = params.toMap()["agent_id"].toString();
                QString agent_channel = params.toMap()["agent_channel"].toString();
                QVariantMap properties = params.toMap()["properties"].toMap();
                // updateAgent(astid, agentid, properties);
                if (agent_channel.startsWith("Agent/")) {
                    UserInfo * ui = findUserFromAgent(astid, agent_channel.mid(6));
                    if(ui)
                        emit updatePeerAgent(m_timesrv, ui->userid(), "agentstatus", params);
                    else // (useful ?) in order to transfer the replies to unmatched agents
                        emit updatePeerAgent(m_timesrv, "", "agentstatus", params);
                } else
                    qDebug() << "update-agents agentnum" << astid << agent_channel;
            } else if(function == "del") {
                qDebug() << thisclass << "del" << datamap["astid"].toString() << datamap["deltalist"].toStringList();
            } else if(function == "add") {
                qDebug() << thisclass << "add" << datamap["astid"].toString() << datamap["deltalist"].toStringList();
            }
            
        } else if (thisclass == "agentrecord") {
            statusRecord(datamap["astid"].toString(),
                         datamap["agentid"].toString(),
                         datamap["status"].toString());
            
        } else if (thisclass == "agentlisten") {
            statusListen(datamap["astid"].toString(),
                         datamap["agentid"].toString(),
                         datamap["status"].toString());
            
        } else if (thisclass == "history") {
            // QCryptographicHash histohash(QCryptographicHash::Sha1);
            // QByteArray res = histohash.hash(command_args.toAscii(), QCryptographicHash::Sha1).toHex();
            processHistory(datamap["payload"]);
            
        } else if (thisclass == "endinit") {
            qDebug() << "I should have received everything";
            
        } else if (thisclass == "meetme") {
            //qDebug() << "**** MEETME **** " << function << datamap["payload"];
            if (function == "sendlist")
            {
                QVariantMap map1 = datamap["payload"].toMap();
                foreach(QString astid, map1.keys()) {
                    m_meetme[astid].clear();
                    QVariantMap map2 = map1[astid].toMap();
                    foreach(QString meetmeid, map2.keys()) {
                        QVariantMap map3 = map2[meetmeid].toMap();
                        m_meetme[astid][meetmeid].m_context = map3["context"].toString();
                        m_meetme[astid][meetmeid].m_name = map3["name"].toString();
                        m_meetme[astid][meetmeid].m_number = map3["number"].toString();
                        m_meetme[astid][meetmeid].m_pin = map3["pin"].toString();
                        m_meetme[astid][meetmeid].m_adminpin = map3["admin-pin"].toString();
                        m_meetme[astid][meetmeid].m_adminid = map3["adminid"].toString();
                        m_meetme[astid][meetmeid].m_uniqueids = map3["uniqueids"].toMap();
                    }
                }
                meetmeInit(m_timesrv, datamap["payload"]);
            }
            else if (function == "update")
            {
                QVariantMap map = datamap["payload"].toMap();
                QString action = map["action"].toString();
                QString astid = map["astid"].toString();
                QString meetmeid = map["meetmeid"].toString();
                QString uniqueid = map["uniqueid"].toString();
                if(!meetmeid.isEmpty() && !astid.isEmpty()) {
                    if(action == QString("join")) {
                        m_meetme[astid][meetmeid].m_uniqueids[uniqueid] = map["details"].toMap();
                    } else if(action == "leave") {
                        m_meetme[astid][meetmeid].m_uniqueids.remove(uniqueid);
                    }
                }
                meetmeEvent(m_timesrv, datamap["payload"]);
            }
            //qDebug() << "**** MEETME " << m_meetme;
        } else if (thisclass == "serverdown") {
            qDebug() << thisclass << datamap["mode"].toString();
            
        } else if (thisclass == "disconn") {
            qDebug() << thisclass;
            
        } else if (thisclass == "directory") {
            directoryResponse(datamap["headers"].toStringList(),
                              datamap["resultlist"].toStringList());
            
        } else if (thisclass == "faxsend") {
            m_filedir = datamap["tdirection"].toString();
            m_fileid = datamap["fileid"].toString();
            m_tcpsocket["filetransfers"]->connectToHost(m_serverhost, m_ctiport);
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
                QString stateid = datamap.value("capapresence").toMap().value("state").toMap().value("stateid").toString();
                QVariantMap changeme = m_guioptions.value("server_gui").toMap().value("autochangestate").toMap();
                if(changeme.count()) {
                    if(stateid == changeme["statesrc"].toString()) {
                        // QTimer::singleShot() could be used.
                        m_timerid_changestate = startTimer(changeme["seconds"].toInt() * 1000);
                        m_changestate_newstate = changeme["statedst"].toString();
                    } else if(changeme.contains(stateid)) {
                        // if(stateid == changeme["statesrc"].toString()) {
                        m_timerid_changestate = startTimer(changeme[stateid].toMap()["delaymsec"].toInt());
                        m_changestate_newstate = changeme[stateid].toMap()["newstate"].toString();
                    }
                }
                m_users[id]->setAvailState(datamap["capapresence"].toMap()["state"]);
                emit updatePeerAgent(m_timesrv, id, "imstatus", QStringList());
                emit updateAgentPresence(m_users[id]->astid(),
                                         m_users[id]->agentid(),
                                         datamap["capapresence"].toMap()["state"]);
                m_counters = datamap["presencecounter"];
                emit updateCounter(m_counters);
                if (id == m_fullid) {
                    updateCapaPresence(datamap["capapresence"]);
                    updatePresence(m_capapresence);
                    emit localUserInfoDefined(m_users[m_fullid]);
                }
            }
            
        } else if (thisclass == "users") {
            if (function == "sendlist") {
                QVariantList listusers = datamap["payload"].toList();
                foreach(QVariant userprops, listusers) {
                    QVariantMap uinfo = userprops.toMap();
                    //qDebug() << "-------------" << uinfo;
                    QString iduser = uinfo["astid"].toString() + "/" + uinfo["xivo_userid"].toString();
                    
                    if(! m_users.contains(iduser)) {
                        m_users[iduser] = new UserInfo(iduser);
                        m_users[iduser]->setCtiLogin(uinfo["user"].toString());
                        m_users[iduser]->setFullName(uinfo["fullname"].toString());
                        //emit newUser(m_users[iduser]);
                        m_users[iduser]->setPhones(uinfo["astid"].toString(),
                                                   uinfo["techlist"].toStringList(),
                                                   m_phones);
                    }
                    
                    m_users[iduser]->setAvailState(uinfo["statedetails"]);
                    m_users[iduser]->setPhoneNumber(uinfo["phonenum"].toString());
                    m_users[iduser]->setAgentId(uinfo["agentid"].toString());
                    m_users[iduser]->setContext(uinfo["context"].toString());
                    
                    m_users[iduser]->setMWI(uinfo["mwi"].toStringList());
                    m_users[iduser]->setSimultCalls(uinfo["simultcalls"].toInt());
                    m_users[iduser]->setVoiceMailNumber(uinfo["voicemailnum"].toString());
                    m_users[iduser]->setAgentNumber(uinfo["agentnumber"].toString());
                    //m_users[iduser]->setContext(uinfo["context"].toString());
                    emit updatePeerAgent(m_timesrv, iduser, "imstatus", QStringList());
                    emit updateAgentPresence(m_users[iduser]->astid(),
                                             m_users[iduser]->agentid(),
                                             uinfo["statedetails"]);
                    emit userUpdated( m_users[iduser] );
                }
                
                peersReceived();
                m_monitored_userid = m_fullid;
                QString fullname_mine = "No One";
                if(m_users.contains(m_fullid)) {
                    fullname_mine = m_users[m_fullid]->fullname();
                    emit localUserInfoDefined(m_users[m_fullid]);
                    emit updateCounter(m_counters);
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
                
                
                // emitTextMessage(tr("Received status for %1 users").arg(m_users.size()));
                // XXX this information might not be relevant (to be filtered according to context, ...)
            } else if (function == "update") {
                QStringList userupdate = datamap["user"].toStringList();
                if(userupdate.size() == 2) {
                    QString iduser = userupdate[0] + "/" + userupdate[1];
                    if(m_users.contains(iduser) && (iduser == m_fullid)) {
                        QString subclass = datamap["subclass"].toString();
                        if(subclass == "mwi") {
                            m_users[iduser]->setMWI(datamap["payload"].toStringList());
                            emit localUserInfoDefined(m_users[m_fullid]);
                        }
                        emit userUpdated(m_users[iduser]);
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
            if (function == "update") {
                QVariantMap featuresupdate_map = datamap["payload"].toMap();
                if(m_monitored_userid == datamap["userid"].toString())
                    foreach(QString featurekey, featuresupdate_map.keys())
                        initFeatureFields(featurekey, featuresupdate_map[featurekey]);
                
            } else if (function == "get") {
                QVariantMap featuresget_map = datamap["payload"].toMap();
                if(m_monitored_userid == datamap["userid"].toString()) {
                    resetFeatures();
                    foreach(QString featurekey, featuresget_map.keys())
                        initFeatureFields(featurekey, featuresget_map[featurekey]);
                    emitTextMessage(tr("Received Services Data"));
                }
                
            } else if (function == "put") {
                QVariantMap featuresput_map = datamap["payload"].toMap();
                if(m_monitored_userid == datamap["userid"].toString()) {
                    if(featuresput_map.isEmpty()) {
                        featurePutIsKO();
                        emitTextMessage(tr("Could not modify the Services data.") + " " + tr("Maybe Asterisk is down."));
                    } else {
                        featurePutIsOK();
                        foreach(QString featurekey, featuresput_map.keys())
                            initFeatureFields(featurekey, featuresput_map[featurekey]);
                        emitTextMessage("");
                    }
                }
            }
            
        } else if (thisclass == "phones") {
            if (function == "sendlist") {
                QMap<QString, QVariant> payload = datamap["payload"].toMap();
                foreach(QString astid, payload.keys()) {
                    QMap<QString, QVariant> values = payload[astid].toMap();
                    foreach(QString phoneid, values.keys())
                        updatePhone(astid, phoneid, values[phoneid].toMap());
                }
                //emit callsUpdated();
                emit peersReceived();
            } else if (function == "update") {
                QString astid = datamap["astid"].toString();
                QString phoneid = datamap["phoneid"].toString();
                QVariantMap value = datamap["status"].toMap();
                updatePhone(astid, phoneid, value);
                //callsUpdated();
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
                        
        } else if (thisclass == "trunks") {
            // if (function == "update") {
            // QString astid = datamap["astid"].toString();
            // QString trunkid = datamap["trunkid"].toString();
            // QVariant value = datamap["status"];
            // updatePhone(astid, trunkid, value);
            // callsUpdated();
            // }
            qDebug() << "BaseEngine::parseQVariantCommand()" << thisclass << "not yet supported";
            
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
                command["agentphonenumber"] = m_agentphonenumber;
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
            
            // Put the values of client_gui, then those of server_gui into merged_gui.
            // The first ones are overrided by the second ones.
            QVariantMap tmpa;
            QMapIterator<QString, QVariant> cg1(m_guioptions.value("client_gui").toMap());
            while (cg1.hasNext()) {
                cg1.next();
                tmpa[cg1.key()] = cg1.value();
            }
            QMapIterator<QString, QVariant> cg2(m_guioptions.value("server_gui").toMap());
            while (cg2.hasNext()) {
                cg2.next();
                tmpa[cg2.key()] = cg2.value();
            }
            m_guioptions["merged_gui"] = tmpa;
            
            QVariantMap tmp;
            QStringList todisp;
            m_checked_function["switchboard"] = true;
            foreach (QString function, m_capafuncs)
                if(m_checked_function.contains(function) && m_checked_function[function])
                    todisp.append(function);
            tmp["functions"] = todisp;
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
                m_timerid_keepalive = startTimer(m_keepaliveinterval);
                askCallerIds();
                m_attempt_loggedin = true;
            }
        } else if (thisclass == "disconnect") {
            qDebug() << "disconnect" << datamap;
            QString type = datamap["type"].toString();
            stop();
            if(type=="force") {
                m_forced_to_disconnect = true;// disable autoreconnect
                popupError("forcedisconnected");
            } else {
                popupError("disconnected");
            }
        } else {
            qDebug() << "BaseEngine::parseQVariantCommand() : unknown server command class" << thisclass << datamap;
        }
    }
}

/*! \brief send meetme command to the CTI server */
void BaseEngine::meetmeAction(const QString & function, const QString & functionargs)
{
    QVariantMap command;
    command["class"] = "meetme";
    command["direction"] = "xivoserver";
    command["function"] = function;
    command["functionargs"] = functionargs.split(" ");
    sendJsonCommand(command);
}

/*! \brief send callcampaign command to the CTI server */
void BaseEngine::requestFileList(const QString & action)
{
    QVariantMap command;
    command["class"] = "callcampaign";
    command["direction"] = "xivoserver";
    command["command"] = action.split(" ");
    sendJsonCommand(command);
}

/*! \brief Send fax to CTI Server */
void BaseEngine::sendFaxCommand(const QString & filename,
                                const QString & number,
                                Qt::CheckState hide)
{
    QFile * qf = new QFile(filename);
    qf->open(QIODevice::ReadOnly);
    m_filedata = QByteArray();
    m_filedata.append(qf->readAll());
    qf->close();
    delete qf;
    qf = NULL;
    m_faxsize = m_filedata.size();
    if(m_filedata.size() > 0) {
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

/*! \brief select message and then display a messagebox
 *
 * TODO : replace string errorids by an enum ?
 */
void BaseEngine::popupError(const QString & errorid)
{
    QString errormsg = QString(tr("Server has sent an Error."));
    if(errorid.toLower() == "connection_refused")
        errormsg = tr("You are not allowed to connect\n"
                      "to the XiVO CTI server on %1:%2.")
            .arg(m_serverhost).arg(m_ctiport);
    
    else if(errorid.toLower() == "number_of_arguments")
        errormsg = tr("The number of arguments sent is incorrect.\n"
                      "Maybe a version issue ?");
    
    else if(errorid.toLower() == "user_not_found")
        errormsg = tr("Your registration name <%1@%2>\n"
                      "is not known by the XiVO CTI server on %3:%4.")
            .arg(m_userid).arg(m_company)
            .arg(m_serverhost).arg(m_ctiport);
    
    else if(errorid.toLower() == "session_expired")
        errormsg = tr("Your session has expired.");
    
    else if(errorid.startsWith("capaid_undefined:")) {
        QStringList capainfo = errorid.split(":");
        errormsg = tr("Your profile name <%1> is not defined.").arg(capainfo[1]);
    }
    
    else if(errorid.toLower() == "login_password")
        errormsg = tr("You entered a wrong login / password.");
    
    else if(errorid.toLower() == "no_keepalive_from_server")
        errormsg = tr("The XiVO CTI server on %1:%2 did not reply to the last keepalive.")
            .arg(m_serverhost).arg(m_ctiport);
    
    else if(errorid.toLower() == "connection_closed")
        errormsg = tr("The XiVO CTI server on %1:%2 has just closed the connection.")
            .arg(m_serverhost).arg(m_ctiport);
    
    else if(errorid.toLower() == "network_error")
        errormsg = tr("An error occurred with the network (network cable accidentally plugged out ?).");
    
    else if(errorid.startsWith("socket_error:")) {
        QStringList ipinfo = errorid.split(":");
        errormsg = tr("Socket Error number %1.").arg(ipinfo[1]);
    }
    
    else if(errorid.toLower() == "server_stopped")
        errormsg = tr("The XiVO CTI server on %1:%2 has just been stopped.")
            .arg(m_serverhost).arg(m_ctiport);
    
    else if(errorid.toLower() == "server_reloaded")
        errormsg = tr("The XiVO CTI server on %1:%2 has just been reloaded.")
            .arg(m_serverhost).arg(m_ctiport);
    
    else if(errorid.startsWith("already_connected:")) {
        QStringList ipinfo = errorid.split(":");
        errormsg = tr("You are already connected from %1:%2.").arg(ipinfo[1]).arg(ipinfo[2]);
    }
    
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
                          "Please upgrade it to %2 at least.")
                .arg(__current_client_version__)
                .arg(versionslist[1]);
        else
            errormsg = tr("Your client version (%1) is too old for this server.\n"
                          "Please upgrade it.").arg(__current_client_version__);
    }
    else if(errorid.startsWith("xivoversion_client:")) {
        QStringList versionslist = errorid.split(":")[1].split(";");
        if(versionslist.size() >= 2)
            errormsg = tr("Your client's major version (%1)\n"
                          "is not the same as the server's (%2).")
                .arg(__xivo_version__)
                .arg(versionslist[1]);
    }
    else if(errorid.startsWith("version_server:")) {
        QStringList versionslist = errorid.split(":")[1].split(";");
        if(versionslist.size() >= 2)
            errormsg = tr("Your server version (%1) is too old for this client.\n"
                          "Please upgrade it to %2 at least.")
                .arg(versionslist[0])
                .arg(__current_client_version__);
        else
            errormsg = tr("Your server version (%1) is too old for this client.\n"
                          "Please upgrade it.").arg(versionslist[0]);
    } else if (errorid == "disconnected") {
        errormsg = tr("You were disconnected by the server.");
    } else if (errorid == "forcedisconnected") {
        errormsg = tr("You were forced to disconnect by the server.");
    }
    
    // logs a message before sending any popup that would block
    emitTextMessage(tr("Error") + " : " + errormsg);
    if(!m_trytoreconnect || m_forced_to_disconnect)
        emitMessageBox(errormsg);
}

/*! \brief save BaseEngine::m_downloaded to a file
 *  \sa BaseEngine::m_downloaded
 */
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
        while(m_tcpsocket.value("cticommands")->canReadLine()) {
            QByteArray data  = m_tcpsocket.value("cticommands")->readLine();
            m_byte_counter += data.size();
            // qDebug() << "BaseEngine::socketReadyRead() data.size() = " << data.size();
            QString line = QString::fromUtf8(data);
            
            if(line.startsWith("<ui version=")) {
                // we get here when receiving a sheet as a Qt4 .ui form
                qDebug() << "BaseEngine::socketReadyRead() (Customer Info)" << line.size();
                displayFiche(line, true, QString());
            } else
                parseCommand(line);
        }
    else if(socketname == "filetransfers") {
        while(m_tcpsocket["filetransfers"]->canReadLine()) {
            QByteArray data = m_tcpsocket["filetransfers"]->readLine();
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
                    QByteArray fax64 = m_filedata.toBase64();
                    qDebug() << "sending fax contents" << jsondatamap["fileid"].toString() << m_faxsize << fax64.size();
                    if(m_faxsize > 0) {
                        m_tcpsocket["filetransfers"]->write(fax64 + "\n");
                        m_tcpsocket["filetransfers"]->flush();
                    }
                    m_filedata.clear();
                }
                m_tcpsocket["filetransfers"]->disconnectFromHost();
                m_faxsize = 0;
                m_fileid = "";
            }
        }
    }
}

/*! \brief forward actions from fiche to XiVO CTI server
 *
 * Build and send JSON command
 */
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
            command["destination"] = "ext:" + m_numbertodial;
        else
            command["destination"] = dst;
        sendJsonCommand(command);
    } else if(action == "hangup") {
        QVariantMap ipbxcommand;
        ipbxcommand["command"] = action;
        ipbxcommand["channelids"] = src;
        ipbxCommand(ipbxcommand);
    } else if(action == "answer") {
        QVariantMap ipbxcommand;
        ipbxcommand["command"] = action;
        ipbxcommand["phoneids"] = src;
        ipbxCommand(ipbxcommand);
    } else if(action == "refuse") {
        QVariantMap ipbxcommand;
        ipbxcommand["command"] = action;
        ipbxcommand["channelids"] = src;
        ipbxCommand(ipbxcommand);
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
void BaseEngine::requestHistory(const QString & peer, int mode, const QDateTime & moreRecent)
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
        if(moreRecent.isValid()) {
            //command["morerecentthan"] = QString::number(moreRecent.toTime_t());
            command["morerecentthan"] = moreRecent.toString(Qt::ISODate);
        }
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
    // qDebug() << "BaseEngine::setFullId()" << m_astid << m_xivo_userid;
    // m_fullid = m_company + "/" + m_userid;
    m_fullid = m_astid + "/" + m_xivo_userid;
}

const QString & BaseEngine::agentphonenumber() const
{
    return m_agentphonenumber;
}

void BaseEngine::setAgentPhoneNumber(const QString & agentphonenumber)
{
    m_agentphonenumber = agentphonenumber;
}

const int & BaseEngine::loginkind() const
{
    return m_loginkind;
}

void BaseEngine::setLoginKind(const int loginkind)
{
    if(loginkind != m_loginkind) {
        QVariantMap tmpqvm = m_guioptions.value("client_gui").toMap();
        tmpqvm["loginkind"] = loginkind;
        m_guioptions["client_gui"] = tmpqvm;
        
        m_loginkind = loginkind;
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
    if((field == "enablevoicemail") || (field == "vm"))
        optChanged("enablevm", isenabled);
    else if((field == "enablednd") || (field == "dnd"))
        optChanged("enablednd", isenabled);
    else if(field == "callfilter")
        optChanged("incallfilter", isenabled);
    else if(field == "callrecord")
        optChanged("incallrec", isenabled);
    else if((field == "unc") || (field == "busy") || (field == "rna"))
        forwardUpdated(field, value);
    else if((field == "enableunc") || (field == "enablebusy") || (field == "enablerna"))
        forwardUpdated(field.mid(6), value);
}

void BaseEngine::stopKeepAliveTimer()
{
    if( m_timerid_keepalive > 0 ) {
        killTimer(m_timerid_keepalive);
        m_timerid_keepalive = 0;
    }
}

void BaseEngine::stopTryAgainTimer()
{
    if( m_timerid_tryreconnect > 0 ) {
        killTimer(m_timerid_tryreconnect);
        m_timerid_tryreconnect = 0;
    }
}

void BaseEngine::startTryAgainTimer()
{
    if( m_timerid_tryreconnect == 0 && m_trytoreconnect && !m_forced_to_disconnect)
        m_timerid_tryreconnect = startTimer(m_trytoreconnectinterval);
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
            if(m_timerid_tryreconnect > 0)
                {
                    killTimer(m_timerid_tryreconnect);
                    m_timerid_tryreconnect = startTimer(m_trytoreconnectinterval);
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
    // qDebug() << "BaseEngine::timerEvent() timerId's" << timerId << m_timerid_keepalive << m_timerid_tryreconnect;
    if(timerId == m_timerid_keepalive) {
        keepLoginAlive();
        event->accept();
    } else if(timerId == m_timerid_tryreconnect) {
        emitTextMessage(tr("Attempting to reconnect to server"));
        start();
        event->accept();
    } else if(timerId == m_timerid_changestate) {
        setAvailState(m_changestate_newstate, false);
        killTimer(timerId);
        m_timerid_changestate = -1;
        event->accept();
    } else {
        event->ignore();
        qDebug() << "BaseEngine::timerEvent() ghost timer" << timerId << "will be stopped";
        killTimer(timerId);
    }
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

/*! \brief send a lot of getlist commands to the CTI server
 *
 * send getlist for "users", "queues", "agents", "phones",
 * "meetme", "users", "endinit"
 *
 * \todo rename this method to something more meaningful
 *
 */
void BaseEngine::askCallerIds()
{
    // qDebug() << "BaseEngine::askCallerIds()";
    QVariantMap command;
    command["direction"] = "xivoserver";
    command["function"] = "getlist";
    QStringList getlists = (QStringList() << "users" << "queues" << "agents" << "phones" << "meetme" << "users" << "endinit");
    foreach(QString kind, getlists) {
        command.remove("class");
        command["class"] = kind;
        sendJsonCommand(command);
    }
    if(m_loginkind == 2) {
        QVariantMap ipbxcommand;
        ipbxcommand["command"] = "agentlogin";
        ipbxcommand["agentids"] = "agent:special:me";
        ipbxcommand["agentphonenumber"] = m_agentphonenumber;
        ipbxCommand(ipbxcommand);
    }
}

void BaseEngine::ipbxCommand(const QVariantMap & ipbxcommand)
{
    if(! ipbxcommand.contains("command"))
        return;
    QVariantMap command;
    command["class"] = "ipbxcommand";
    command["direction"] = "xivoserver";
    command["details"] = ipbxcommand;
    sendJsonCommand(command);
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
            if(m_timerid_keepalive > 0)
                {
                    killTimer(m_timerid_keepalive);
                    m_timerid_keepalive = startTimer(m_keepaliveinterval);
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

void BaseEngine::changeWatchedAgentSlot(const QString & agentid, bool force)
{
    // qDebug() << "BaseEngine::changeWatchedAgentSlot" << agentid << force;
    if((force || (agentid.size() > 0)) && (m_agents.contains(agentid))) {
        m_agent_watched_agentid = agentid;
        emit changeWatchedAgentSignal(agentid);
    }
}

void BaseEngine::changeWatchedQueueSlot(const QString & queueid)
{
    // qDebug() << "BaseEngine::changeWatchedQueueSlot" << queueid;
    m_queue_watched_queueid = queueid;
    emit changeWatchedQueueSignal(queueid);
}

/*! \brief sets m_osname */
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
    if(m_rate_bytes > 100000) {
        command["rate-bytes"] = m_rate_bytes;
        command["rate-msec"] = m_rate_msec;
        command["rate-samples"] = m_rate_samples;
        m_rate_bytes = 0;
        m_rate_msec = 0;
        m_rate_samples = 0;
    }
    sendJsonCommand(command);
}

/*! \brief send m_availstate to CTI server */
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

/*! \brief send error message to the CTI Server */
void BaseEngine::shouldNotOccur(const QString & classmethod, const QString & message)
{
    qDebug() << "BE::sNO" << classmethod << message;
    QVariantMap command;
    command["class"] = "logclienterror";
    command["direction"] = "xivoserver";
    command["classmethod"] = classmethod;
    command["message"] = message;
    sendJsonCommand(command);
}

/*! \brief save parameter in settings (.ini file) */
void BaseEngine::saveQueueGroups(const QVariant & queuegroups)
{
    m_settings->setValue("agentpanel/queuegroups", queuegroups);
}

/*! \brief load parameters from settings */
void BaseEngine::loadQueueGroups()
{
    emit setQueueGroups(m_settings->value("agentpanel/queuegroups"));
}

/*! \brief save queue order in settings (.ini file) */
void BaseEngine::saveQueueOrder(const QVariant & queueorder)
{
    m_settings->setValue("queuepanel/queueorder", queueorder);
}

/*! \brief load queue order from settings */
void BaseEngine::loadQueueOrder()
{
    emit setQueueOrder(m_settings->value("queuepanel/queueorder"));
}

/*! \brief get pointer to the currently logged user
 *
 * Return NULL if not available */
UserInfo * BaseEngine::getXivoClientUser()
{
    //qDebug() << "BaseEngine::getXivoClientUser()" << m_astid << m_xivo_userid;
    if( m_users.contains( m_fullid ) )
        return m_users.value( m_fullid );
    return NULL;
}

/*! \brief send new remark about a sheet */
void BaseEngine::sendNewRemark(const QString & id, const QString & text)
{
    QVariantMap command;
    command["class"] = "sheet";
    command["direction"] = "xivoserver";
    command["function"] = "addentry";
    command["channel"] = id;
    command["text"] = text;
    sendJsonCommand(command);
}

/*! \brief receive the message from other instance of this application
 *
 * \see QtSingleApplication
 */
void BaseEngine::handleOtherInstanceMessage(const QString & msg)
{
    qDebug() << "BaseEngine::handleOtherInstanceMessage()" << msg;
    QRegExp re("^(tel|callto):([-0-9\\. +]*[0-9])", Qt::CaseInsensitive);
    int pos = re.indexIn(msg);
    if(pos >= 0) {
        QString phonenum = re.cap(2);
        phonenum.remove('.').remove(' ').remove('-');
        qDebug() << "  trying to dial" << phonenum;
        actionCall("originate", "user:special:me", "ext:"+phonenum);
    }
}

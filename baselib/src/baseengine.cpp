/* XiVO Client
 * Copyright (C) 2007-2011, Proformatique
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
#include <QDesktopServices>
#include <QDir>
#include <QFileInfo>
#include <QLocale>
#include <QProcess>
#include <QTcpSocket>
#include <QTranslator>
#include <QUrl>
#include <QLibraryInfo>

#include "JsonToVariant.h"
#include "VariantToJson.h"
#include "xivoconsts.h"

#include "baseengine.h"
#include <cticonn.h>


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

BASELIB_EXPORT BaseEngine *b_engine;
static const QStringList CheckFunctions = (QStringList() << "presence" << "customerinfo");
static CtiConn *m_ctiConn;

BaseEngine::BaseEngine(QSettings *settings,
                       const QString &osInfo)
    : QObject(NULL),
      m_serverhost(""), m_ctiport(0),
      m_userid(""), m_useridopt(""), m_company(""), m_password(""), m_agentphonenumber(""),
      m_sessionid(""), m_state(ENotLogged),
      m_pendingkeepalivemsg(0), m_logfile(NULL),
      m_byte_counter(0), m_attempt_loggedin(false),
      m_rate_bytes(0), m_rate_msec(0), m_rate_samples(0),
      m_forced_to_disconnect(false), m_tree(new DStore())
{
    b_engine = this;
    settings->setParent(this);
    m_timerid_keepalive = 0;
    m_timerid_changestate = 0;
    m_timerid_tryreconnect = 0;
    setOSInfos(osInfo);
    m_settings = settings;
    loadSettings();

    // TCP connection with CTI Server
    m_ctiserversocket = new QTcpSocket(this);
    m_ctiConn = new CtiConn(m_ctiserversocket);

    connect(m_ctiserversocket, SIGNAL(connected()),
            this, SLOT(ctiSocketConnected()));
    connect(m_ctiserversocket, SIGNAL(readyRead()),
            this, SLOT(ctiSocketReadyRead()));

    // TCP connection for file transfer
    // (this could be moved to some other class)
    m_filetransfersocket = new QTcpSocket(this);
    connect(m_filetransfersocket, SIGNAL(connected()),
            this, SLOT(filetransferSocketConnected()));
    connect(m_filetransfersocket, SIGNAL(readyRead()),
            this, SLOT(filetransferSocketReadyRead()));

    if (m_autoconnect)
        start();
    translationFiles = \
        (QStringList() << ":/xivoclient_%1"
                       << ":/baselib/baselib_%1"
                       << QLibraryInfo::location(QLibraryInfo::TranslationsPath) + "/qt_%1" );
}

/*! \brief Destructor
 */
BaseEngine::~BaseEngine()
{
    qDebug() << Q_FUNC_INFO;
    clearUserList();
    clearPhoneList();
    clearAgentList();
    clearQueueList();
}

QSettings* BaseEngine::getSettings()
{
    return m_settings;
}

/*!
 * Load Settings from the registery/configuration file
 * Use default values when settings are not found.
 */
void BaseEngine::loadSettings()
{
    // qDebug() << Q_FUNC_INFO;
    m_systrayed = m_settings->value("display/systrayed", false).toBool();
    QString profile = m_settings->value("profile/lastused").toString();
    m_profilename_write = "engine-" + profile;

    QString settingsversion = m_settings->value("version/xivo", __xivo_version__).toString();

    // this is used to make a migration from 1.0 to 1.1
    if (settingsversion == "1.0")
        m_profilename_read = "engine";
    else
        m_profilename_read = "engine-" + profile;

    m_settings->beginGroup(m_profilename_read);
        m_serverhost = m_settings->value("serverhost", "demo.xivo.fr").toString();
        m_ctiport    = m_settings->value("serverport", 5003).toUInt();

        m_userid      = m_settings->value("userid").toString().trimmed();
        m_useridopt   = m_settings->value("useridopt").toString().trimmed();
        if (m_useridopt.size() > 0)
            m_useridwithopt = m_userid + "%" + m_useridopt;
        else
            m_useridwithopt = m_userid;
        m_company      = m_settings->value("company", "default").toString();
        m_password     = m_settings->value("password").toString();
        m_keeppass     = m_settings->value("keeppass", 0).toUInt();
        m_showagselect = m_settings->value("showagselect", 2).toUInt();
        m_agentphonenumber  = m_settings->value("agentphonenumber").toString();

        m_forcelocale = m_settings->value("forcelocale", false).toString();
        m_autoconnect = m_settings->value("autoconnect", false).toBool();
        m_trytoreconnect = m_settings->value("trytoreconnect", false).toBool();
        m_trytoreconnectinterval = m_settings->value("trytoreconnectinterval", 20*1000).toUInt();
        m_keepaliveinterval = m_settings->value("keepaliveinterval", 20*1000).toUInt();
        m_checked_lastconnwins = m_settings->value("lastconnwins", false).toBool();
        m_availstate = m_settings->value("availstate", "available").toString();

        m_settings->beginGroup("user-gui");
            m_historysize = m_settings->value("historysize", 8).toUInt();

            QString defaultguioptions;
            QFile defaultguioptions_file(":/common/guioptions.json");
            if (defaultguioptions_file.exists()) {
                defaultguioptions_file.open(QFile::ReadOnly);
                defaultguioptions = defaultguioptions_file.readAll();
                defaultguioptions_file.close();
            }
            QVariant data;
            try {
                data = JsonQt::JsonToVariant::parse(defaultguioptions);
            } catch(JsonQt::ParseException) {
                qDebug() << Q_FUNC_INFO << "exception catched for" << defaultguioptions;
            }

            QVariantMap guisetting_map = data.toMap();
            guisetting_map.insert("xlet_operator_keyanswer"        , QVariant(Qt::Key_F1));
            guisetting_map.insert("xlet_operator_keyhangup"        , QVariant(Qt::Key_F2));
            guisetting_map.insert("xlet_operator_keydtransfer"     , QVariant(Qt::Key_F3));
            guisetting_map.insert("xlet_operator_keyitransfer"     , QVariant(Qt::Key_F4));
            guisetting_map.insert("xlet_operator_keyilink"         , QVariant(Qt::Key_F5));
            guisetting_map.insert("xlet_operator_keyicancel"       , QVariant(Qt::Key_F6));
            guisetting_map.insert("xlet_operator_keypark"          , QVariant(Qt::Key_F7));
            guisetting_map.insert("xlet_operator_keyatxferfinalize", QVariant(Qt::Key_F8));
            guisetting_map.insert("xlet_operator_keyatxfercancel"  , QVariant(Qt::Key_F9));
            data.setValue(guisetting_map);
        m_settings->endGroup();
    m_settings->endGroup();

    // this is used to make a migration from 1.0 to 1.1
    if (settingsversion != "1.0")
        m_settings->beginGroup(m_profilename_read);
    m_settings->beginGroup("user-gui");
    m_guioptions["client_gui"] = m_settings->value("guisettings", data);
    m_settings->endGroup();
    if (settingsversion != "1.0")
        m_settings->endGroup();

    m_loginkind = m_guioptions.value("client_gui").toMap().value("loginkind").toInt();

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
    if (m_settings->value("userid").toString() != m_userid) {
        m_settings->setValue("monitor/userid", QString(""));
    }

    m_settings->setValue("version/xivo", __xivo_version__);
    m_settings->setValue("version/git_hash", __git_hash__);
    m_settings->setValue("version/git_date", __git_date__);
    m_settings->setValue("display/systrayed", m_systrayed);

    m_settings->beginGroup(m_profilename_write);
        m_settings->setValue("serverhost", m_serverhost);
        m_settings->setValue("serverport", m_ctiport);
        m_settings->setValue("userid", m_userid);
        m_settings->setValue("useridopt", m_useridopt);
        m_settings->setValue("company", m_company);
        m_settings->setValue("keeppass", m_keeppass);
        m_settings->setValue("showagselect", m_showagselect);
        m_settings->setValue("agentphonenumber", m_agentphonenumber);
        m_settings->setValue("forcelocale", m_forcelocale);
        m_settings->setValue("autoconnect", m_autoconnect);
        m_settings->setValue("trytoreconnect", m_trytoreconnect);
        m_settings->setValue("trytoreconnectinterval", m_trytoreconnectinterval);
        m_settings->setValue("keepaliveinterval", m_keepaliveinterval);
        m_settings->setValue("lastconnwins", m_checked_lastconnwins);
        m_settings->setValue("availstate", m_availstate);

        if (m_keeppass > 0)
            m_settings->setValue("password", m_password);
        else
            m_settings->remove("password");

        m_settings->beginGroup("user-gui");
            m_settings->setValue("historysize", m_historysize);
            m_settings->setValue("guisettings", m_guioptions.value("client_gui"));
        m_settings->endGroup();
    m_settings->endGroup();

    m_settings->beginGroup("user-functions");
        foreach (QString function, CheckFunctions)
            m_settings->setValue(function, m_checked_function[function]);
    m_settings->endGroup();

    emit settingChanged(getGuiOptions("client_gui"));
}

void BaseEngine::setCheckedFunction(const QString & function, bool b)
{
    if (b != m_checked_function[function]) {
        m_checked_function[function] = b;
        if ((state() == ELogged) && m_enabled_function[function]) {
            if (function == "presence")
                availAllowChanged(b);
        }
    }
}

void BaseEngine::pasteToDial(const QString &toPaste)
{
    emit pasteToDialPanel(toPaste);
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
    return m_capafuncs.contains(function);
}

void BaseEngine::setLogFile(const QString & logfilename)
{
    if (! logfilename.isEmpty()) {
        m_logfile = new QFile(this);
        QDir::setCurrent(QDir::homePath());
        m_logfile->setFileName(logfilename);
        m_logfile->open(QIODevice::Append);
    }
}

void BaseEngine::logAction(const QString & logstring)
{
    if (m_logfile != NULL) {
        QString tolog = QDateTime::currentDateTime().toString(Qt::ISODate) + " " + logstring + "\n";
        m_logfile->write(tolog.toUtf8());
        m_logfile->flush();
    }
}

/*! \brief set login/pass and then starts */
void BaseEngine::configAndStart(const QString &login,
                                const QString &pass,
                                const QString &agentphonenumber)
{
    setUserId(login);
    m_password = pass;
    // if agentphonenumber's size is 0, no login as agent
    m_agentphonenumber = agentphonenumber;
    saveSettings();
    start();
}

/*! \brief send power event to the server.
 *
 * \todo check if this is usefull. If not, remove it.
 */
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
    qDebug() << Q_FUNC_INFO << m_serverhost << m_ctiport << m_checked_function;

    // (In case the TCP sockets were attempting to connect ...) aborts them first
    m_ctiserversocket->abort();

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
    qDebug() << Q_FUNC_INFO << sender() << stopper;
    if (m_attempt_loggedin) {
        QVariantMap command;
        command["class"] = "logout";
        command["direction"] = "xivoserver";
        command["stopper"] = stopper;
        sendJsonCommand(command);
        m_settings->setValue("lastlogout/stopper", stopper);
        m_settings->setValue("lastlogout/datetime", QDateTime::currentDateTime().toString(Qt::ISODate));
        m_attempt_loggedin = false;
    }

    m_ctiserversocket->flush();
    m_ctiserversocket->disconnectFromHost();

    stopKeepAliveTimer();
    stopTryAgainTimer();
    setState(ENotLogged);
    m_sessionid = "";

    clearUserList();
    clearPhoneList();
    clearAgentList();
    clearQueueList();
    if (m_time.isValid())
    {
        int elapsed = m_time.elapsed();
        qDebug() << Q_FUNC_INFO
                 << m_byte_counter << "bytes received in" << elapsed << "ms : "
                 << (elapsed?QString::number((1000*m_byte_counter)/elapsed):QString("infinite"))
                 << "Bytes/Second";
    }

    /* cleaning the registered callbacks */
    {
        QHashIterator<QString, e_callback*> i(m_class_event_cb);
        while (i.hasNext()) {
            i.next();
            qDebug() << Q_FUNC_INFO << "cleaning callback" << i.key();
            delete i.value();
        }
        m_class_event_cb.clear();
    }

    delete m_tree;
    m_tree = new DStore();
}

void BaseEngine::addToDataBase(QVariantMap & qv)
{
    QVariantMap command;
    command["class"] = "database";
    command["direction"] = "xivoserver";
    command["items"] = qv;
    sendJsonCommand(command);
}

/*! \brief clear the content of m_users
 *
 * Delete all contained UserInfo objects
 */
void BaseEngine::clearUserList()
{
    QHashIterator<QString, UserInfo *> iter = QHashIterator<QString, UserInfo *>(m_users);
    while (iter.hasNext()) {
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
    while (iter.hasNext()) {
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
    while (iter.hasNext()) {
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
    while (iter.hasNext()) {
        iter.next();
        delete iter.value();
    }
    m_queues.clear();
}

/*! \brief initiate connection to the server
 */
void BaseEngine::connectSocket()
{
    if (m_userid.length()) {
        m_ctiserversocket->connectToHost(m_serverhost, m_ctiport);
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

bool BaseEngine::hasCapaFun(QString & capa)
{
    return m_capafuncs.contains(capa);
}

/*! \brief gets m_capaxlets */
const QStringList& BaseEngine::getCapaXlets() const
{
    return m_capaxlets;
}

/*! \brief gets m_capapresence */
const QVariantMap& BaseEngine::getCapaPresence() const
{
    return m_capapresence;
}

const QVariantMap BaseEngine::getGuiOptions(const QString & arg) const
{
    return m_guioptions.value(arg).toMap();
}

void BaseEngine::setGuiOption(const QString &arg, const QVariant &opt)
{
    m_guioptions[arg].clear();
    m_guioptions[arg] = opt;

    m_settings->beginGroup(m_profilename_write);
        m_settings->beginGroup("user-gui");
            m_settings->setValue("guisettings", m_guioptions.value(arg));
        m_settings->endGroup();
    m_settings->endGroup();

    saveSettings();
}

void BaseEngine::updateCapaPresence(const QVariant & presence)
{
    QVariantMap presencemap = presence.toMap();
    foreach (QString field, presencemap.keys()) {
        if (presencemap.contains(field)) {
            m_capapresence[field] = presencemap.value(field);
        }

        if (field == "names") {
            QVariantMap crap = presencemap.value(field).toMap();
            foreach (QString stateName, crap.keys()) {
                QVariantMap fill = crap.value(stateName).toMap();
                fill["id"] = fill.value("stateid");
                fill.remove("stateid");
                tree()->populate(QString("statedetails/%0").arg(stateName), fill);
            }
        } else if (field == "allowed") {
            QVariantMap map = presencemap.value(field).toMap();
            QVariantMap fill;
            foreach (QString stateName, map.keys()) {
                if (map.value(stateName).toBool()) {
                    fill[stateName] = QVariant();
                }
            }

            tree()->populate(QString("statedetails/%0/allowed")
                             .arg(presencemap.value("state").toMap().value("stateid").toString()),
                             fill);
        }
    }
    // qDebug() << DStoreNode::pp(*tree()->root());
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
    // qDebug() << Q_FUNC_INFO << "from" << m_availstate << "to" << newstate << comesFromServer;
    if (m_availstate != newstate) {
        m_availstate = newstate;
        //m_settings->setValue("engine/availstate", m_availstate);
        m_settings->setValue(QString("%1/availstate").arg(m_profilename_write),
                             m_availstate);
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
    // qDebug() << Q_FUNC_INFO << sender();
    QString availstate = sender()->property("availstate").toString();
    setAvailState(availstate, false);
}

/*! \brief send command to XiVO CTI server */
void BaseEngine::sendCommand(const QString & command)
{
    if (m_ctiserversocket->state() == QAbstractSocket::ConnectedState)
        m_ctiserversocket->write((command + "\n").toAscii());
}

/*! \brief encode json and then send command to XiVO CTI server */
void BaseEngine::sendJsonCommand(const QVariantMap & command)
{
    QString jsoncommand(JsonQt::VariantToJson::parse(command));
    sendCommand(jsoncommand);
}

/*! \brief set monitored peer id */
void BaseEngine::monitorPeerRequest(const QString & userid)
{
    // qDebug() << Q_FUNC_INFO << userid;
    if (m_users.contains(userid)) {
        m_monitored_userid = userid;
        emit monitorPeer(m_users[userid]);
        m_settings->setValue("monitor/userid", userid);
    }
}

/*! \brief called when the socket is first connected
 *
 * currently send the identification to login
 * \todo read correctly the banner
 */
void BaseEngine::ctiSocketConnected()
{
    stopTryAgainTimer();
    /* do the login/identification */
    m_attempt_loggedin = false;
    QVariantMap command;
    command["class"] = "login_id";
    command["direction"] = "xivoserver";
    command["userid"] = m_userid;
    command["company"] = m_company;
    command["ident"] = m_clientid;
    command["version"] = "9999";
    command["xivoversion"] = __xivo_version__;
    command["git_hash"] = __git_hash__;
    command["git_date"] = __git_date__;
    // for debuging purposes :
    command["lastlogout-stopper"] = m_settings->value("lastlogout/stopper").toString();
    command["lastlogout-datetime"] = m_settings->value("lastlogout/datetime").toString();
    m_settings->remove("lastlogout/stopper");
    m_settings->remove("lastlogout/datetime");

    sendJsonCommand(command);
}

/*! \brief send filetransfer command
 */
void BaseEngine::filetransferSocketConnected()
{
    QVariantMap command;
    command["class"] = "filetransfer";
    command["direction"] = "xivoserver";
    command["tdirection"] = m_filedir;
    command["fileid"] = m_fileid;
    QString jsoncommand(JsonQt::VariantToJson::parse(command));
    // ??? useless test ???
    if (m_filetransfersocket->state() == QAbstractSocket::ConnectedState)
        m_filetransfersocket->write((jsoncommand + "\n").toAscii());
}

UserInfo * BaseEngine::findUserFromPhone(const QString & astid,
                                         const QString & phoneid)
{
    foreach (UserInfo * uinfo, m_users)
        if (uinfo->hasPhone(astid, phoneid))
            return uinfo;
    return NULL;
}

UserInfo * BaseEngine::findUserFromAgent(const QString & astid,
                                         const QString & agentnumber)
{
    foreach (UserInfo * uinfo, m_users)
        if (uinfo->hasAgentNumber(astid, agentnumber))
            return uinfo;
    return NULL;
}


void BaseEngine::updatePhone(const QString & astid,
                             const QString & phoneid,
                             const QMap<QString, QVariant> & properties)
{
    //if (phoneid == "sip.default.121.121")
    // qDebug() << Q_FUNC_INFO << astid << phoneid << properties;
    // qDebug() << map.keys();
    QString key = astid + "." + phoneid;
    UserInfo * ui = findUserFromPhone(astid, phoneid);
    if (! m_phones.contains(key)) {
        m_phones[key] = new PhoneInfo(astid, properties);
        if (ui) {
            ui->updatePhone(m_phones[key]);
        }
    } else {
        m_phones[key]->update(properties);
    }
    if (ui) {
        emit userUpdated(ui);
    }
}

QStringList BaseEngine::updateQueue(const QString & astid,
                                    const QString & queueid,
                                    const QMap<QString, QVariant> & properties)
{
    // qDebug() << Q_FUNC_INFO << astid << queueid << properties;
    QStringList keychanges;
    QString key = QString("queue:%1/%2").arg(astid).arg(queueid);
    if (!m_queues.contains(key)) {
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
    if (!m_queues.contains(key)) {
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
    if (! m_agents.contains(key)) {
        m_agents[key] = new AgentInfo(astid, agentid, properties);
        keychanges << key;
    } else {
        bool haschanged = m_agents[key]->update(properties);
        if (haschanged)
            keychanges << key;
    }
    return keychanges;
}

QStringList BaseEngine::updateAgentQueue(const QString &astid,
                                         const QString &agentid,
                                         const QVariantMap &properties)
{
    QStringList keychanges;
    QString key = QString("agent:%1/%2").arg(astid).arg(agentid);
    if (! m_agents.contains(key)) {
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
    return (m_timeclt.toTime_t() - m_timesrv);
}

/* until cti protocol get changed the following function
 * function are there to fill the tree manually
 * { */


void addMobilePhone(DStore *tree, const QString &userId, const QString &number)
{
    if (number.isEmpty())
        return ;

    QVariantMap mobilePhoneList =
        tree->extractVMap(QString("mobilephones/*[user-id=@%0]").arg(userId));

    if (mobilePhoneList.size() == 0) {
        int id = tree->extractVMap(QString("mobilephones")).size();
        QVariantMap info;
        info["user-id"] = userId;
        info["number"] = number;
        tree->populate(QString("mobilephones/%0").arg(id), info);
    }
}

void addUpdateUserInTree(DStore *tree, const QVariantMap & uinfo)
{
    QVariantMap info;
    info["id"] = uinfo.value("xivo_userid");
    info["fullname"] = uinfo.value("fullname");
    info["state-id"] = uinfo.value("statedetails").toMap().value("stateid");
    tree->populate(QString("users/%0").arg(info.value("id").toString()), info);
    addMobilePhone(tree, info.value("id").toString(), uinfo.value("mobilephone").toString());
    info.clear();
}

void addUpdateConfMemberInTree(DStore *tree, const QVariantMap & cinfo)
{
    QString id = cinfo.value("details").toMap().value("usernum").toString();
    QString confId = cinfo.value("meetmeid").toString();
    QString path = QString("confrooms/%0/in/%1").arg(confId).arg(id);
    QString action = cinfo.value("action").toString();
    QVariantMap info;

    if ( (action == "join") || (action == "mutestatus") || (action == "auth") ) {
        info["id"] = id;
        QVariantMap details = cinfo.value("details").toMap();
        info["phonenum"] = details.value("phonenum");
        info["time-start"] = details.value("time_start");
        info["user-id"] = details.value("userid").toString();
        info["authed"] = details.value("authed").toBool();
        info["mute"] = (details.value("mutestatus").toString() == "on");
        info["admin"] = details.value("admin").toBool();
        tree->populate(path ,info);
    } else if (action == "leave") {
        tree->rmPath(path);
    } else if (action == "changeroompausedstate") {
        path = QString("confrooms/%0").arg(confId);
        info["paused"] = cinfo.value("paused");
        tree->populate(path ,info);
    } else {
        qDebug() << Q_FUNC_INFO << "unknown meetme action: " << action;
    }
}

void addUpdateConfRoomInTree(DStore *tree,
                             const QString & id,
                             const QVariantMap & cinfo)
{
    QVariantMap info;
    info["id"] = id;
    info["name"] = cinfo.value("roomname");
    info["pin"] = cinfo.value("pin");
    info["in"] = QVariantMap();
    info["number"] = cinfo.value("roomnumber");
    info["moderated"] = cinfo.value("moderated");

    tree->populate(QString("confrooms/%0").arg(id), info);

    QVariantMap userIn = cinfo.value("uniqueids").toMap();
    foreach (QString uniqueId , userIn.keys()) {
        QVariantMap userToInsert;
        userToInsert["details"] = userIn.value(uniqueId).toMap();
        userToInsert["meetmeid"] = id;
        userToInsert["action"] = "join";
        userToInsert["uniqueid"] = uniqueId;

        addUpdateConfMemberInTree(tree, userToInsert);
    }
}

/* } */

void BaseEngine::emitMessage(const QString & msg)
{
    emit emitTextMessage(msg);
}

/*! \brief parse JSON and then process command */
void BaseEngine::parseCommand(const QString &line)
{
    QVariant data;
    try {
        QTime jsondecodetime;
        jsondecodetime.start();
        data = JsonQt::JsonToVariant::parse(line.trimmed());
        m_rate_msec += jsondecodetime.elapsed();
        m_rate_bytes += line.trimmed().size();
        m_rate_samples ++;
    } catch(JsonQt::ParseException) {
        qDebug() << Q_FUNC_INFO << "exception catched for" << line.trimmed();
        data = QVariant(QVariant::Invalid);
    }

    if (! data.isValid())
        return;

    QVariantMap datamap = data.toMap();
    QString direction = datamap.value("direction").toString();
    QString function = datamap.value("function").toString();

    if (direction == "client") {
        QString thisclass = datamap.value("class").toString();
        m_timesrv = datamap.value("timenow").toDouble();
        m_timeclt = QDateTime::currentDateTime();

        if (callClassEventCallback(thisclass, datamap))  // a class callback was called,
            return;                                      // so zap the 500 loc of if-else soup

        // qDebug() << Q_FUNC_INFO << datamap.value("timenow").toString() << "BaseEngine message received"
        // << thisclass << datamap.value("function").toString()
        // << datamap.value("phoneid").toString();
        if (thisclass == "callcampaign") {
            requestFileListResult(datamap.value("payload"));

        } else if (thisclass == "parkcall") {
            QString eventkind = datamap.value("eventkind").toString();
            QString astid = datamap.value("astid").toString();
            QString parkingbay = datamap.value("parkingbay").toString();
            // update local list for astid & parkingbay according to eventkind
            if (eventkind == "parkedcall") {
                if (m_parking.contains(astid) == false)
                    m_parking[astid].clear();
                if (! astid.isEmpty() && ! parkingbay.isEmpty()) {
                    if (m_parking[astid].contains(parkingbay) == false)
                        m_parking[astid][parkingbay] = new ParkingInfo();
                    m_parking[astid][parkingbay]->update(datamap.value("payload").toMap());
                }
            }
            parkingEvent(eventkind, astid, parkingbay, datamap.value("payload"));

        } else if (thisclass == "sheet") {
            // TODO : use id better than just channel name
            // qDebug() << Q_FUNC_INFO << "sheet" << datamap;
            QString channel = datamap.value("channel").toString();
            if (function == "getownership") {
                gotSheetOwnership(channel);
            } else if (function == "loseownership") {
                lostSheetOwnership(channel);
            } else if (function == "entryadded") {
                sheetEntryAdded(channel, datamap.value("entry").toMap());
            } if (datamap.contains("payload")) {
                QString payload;
                QByteArray qba = QByteArray::fromBase64(datamap.value("payload").toString().toAscii());
                if (datamap.value("compressed").toBool())
                    payload = QString::fromUtf8(qUncompress(qba));
                else
                    payload = QString::fromUtf8(qba);
                // will eventually call the XML parser
                displayFiche(payload, false, channel);
                if (datamap.contains("entries")) {
                    QVariantList entries = datamap.value("entries").toList();
                    foreach (QVariant entry, entries) {
                        sheetEntryAdded(channel, entry.toMap());
                    }
                }
            }
        } else if (thisclass == "queues") {
            // qDebug() << Q_FUNC_INFO << "queues" << function << line.length();
            if (function == "sendlist") {
                QStringList kk;
                QMap<QString, QVariant> payload = datamap.value("payload").toMap();
                foreach (QString astid, payload.keys()) {
                    QMap<QString, QVariant> values = payload[astid].toMap();
                    foreach (QString queueid, values.keys()) {
                        kk += updateQueue(astid, queueid, values[queueid].toMap());
                    }
                }
                if (! kk.isEmpty())
                    emit newQueueList(kk);
            } else if (function == "update") {
                QStringList kk;
                QMap<QString, QVariant> payload = datamap.value("payload").toMap();
                foreach (QString astid, payload.keys()) {
                    QMap<QString, QVariant> values = payload[astid].toMap();
                    foreach (QString queueid, values.keys()) {
                        kk += updateQueueAgent(astid, queueid, values[queueid].toMap());
                    }
                }
                if (! kk.isEmpty())
                    emit newQueueList(kk);
            } else if (function == "del") {
                removeQueues(datamap.value("astid").toString(),
                             datamap.value("deltalist").toStringList());
            } else if (function == "add") {
                QVariantMap command;
                command["class"] = "queues";
                command["direction"] = "xivoserver";
                command["function"] = "getlist";
                sendJsonCommand(command);
                // qDebug() << thisclass << "add" << datamap.value("astid").toString() << datamap.value("deltalist"].toStringList();
            }

        } else if (thisclass == "groups") {
            // qDebug() << "___" << thisclass << datamap;

        } else if (thisclass == "agents") {
            //qDebug() << "*** agents !!!" << function << line.length();
            if (function == "sendlist") {
                QStringList kk;
                QMap<QString, QVariant> payload = datamap.value("payload").toMap();
                foreach (QString astid, payload.keys()) {
                    QMap<QString, QVariant> values = payload[astid].toMap();
                    foreach (QString agentid, values.keys()) {
                        kk += updateAgent(astid, agentid, values[agentid].toMap());
                    }
                }
                if (! kk.isEmpty())
                    emit newAgentList(kk);

            } else if (function == "update") {
                QStringList kk;
                QMap<QString, QVariant> payload = datamap.value("payload").toMap();
                foreach (QString astid, payload.keys()) {
                    QMap<QString, QVariant> values = payload[astid].toMap();
                    foreach (QString agentid, values.keys()) {
                        kk += updateAgentQueue(astid, agentid, values[agentid].toMap());
                    }
                }
                if (! kk.isEmpty())
                    emit newAgentList(kk);

            } else if (function == "update-old") {
                QVariant params = datamap.value("payload");
                QString action = params.toMap().value("action").toString();
                QString astid = params.toMap().value("astid").toString();
                QString agentid = params.toMap().value("agent_id").toString();
                QString agent_channel = params.toMap().value("agent_channel").toString();
                QVariantMap properties = params.toMap().value("properties").toMap();
                // updateAgent(astid, agentid, properties);
                if (agent_channel.startsWith("Agent/")) {
                    UserInfo * ui = findUserFromAgent(astid, agent_channel.mid(6));
                    if (ui)
                        emit updatePeerAgent(m_timesrv, ui->userid(), "agentstatus", params);
                    else // (useful ?) in order to transfer the replies to unmatched agents
                        emit updatePeerAgent(m_timesrv, "", "agentstatus", params);
                } else
                    qDebug() << "update-agents agentnum" << astid << agent_channel;
            } else if (function == "del") {
                qDebug() << thisclass << "del" << datamap.value("astid").toString() << datamap.value("deltalist").toStringList();
            } else if (function == "add") {
                qDebug() << thisclass << "add" << datamap.value("astid").toString() << datamap.value("deltalist").toStringList();
            }

        } else if (thisclass == "agentrecord") {
            statusRecord(datamap.value("astid").toString(),
                         datamap.value("agentid").toString(),
                         datamap.value("status").toString());

        } else if (thisclass == "agentlisten") {
            statusListen(datamap.value("astid").toString(),
                         datamap.value("agentid").toString(),
                         datamap.value("status").toString());

        } else if (thisclass == "endinit") {
            qDebug() << Q_FUNC_INFO << "I should have received everything";

        } else if (thisclass == "meetme") {
            if (function == "sendlist") {
                QVariantMap map1 = datamap.value("payload").toMap();
                foreach (QString astid, map1.keys()) {
                    QVariantMap map2 = map1[astid].toMap();
                    foreach (QString meetmeid, map2.keys()) {
                        QVariantMap map3 = map2[meetmeid].toMap();
                        addUpdateConfRoomInTree(tree(), meetmeid, map3);
                    }
                }
            } else if (function == "update") {
                QVariantMap map = datamap.value("payload").toMap();
                addUpdateConfMemberInTree(tree(), map);
            }
        } else if (thisclass == "serverdown") {
            qDebug() << Q_FUNC_INFO << thisclass << datamap.value("mode").toString();

        } else if (thisclass == "disconn") {
            qDebug() << Q_FUNC_INFO << thisclass;

        } else if (thisclass == "directory") {
            emit directoryResponse(datamap.value("headers").toStringList(),
                                   datamap.value("resultlist").toStringList());

        } else if (thisclass == "faxsend") {
            m_filedir = datamap.value("tdirection").toString();
            m_fileid = datamap.value("fileid").toString();
            m_filetransfersocket->connectToHost(m_serverhost, m_ctiport);
            qDebug() << Q_FUNC_INFO << m_filedir << m_fileid;

        } else if (thisclass == "faxprogress") {
            ackFax(datamap.value("status").toString(),
                   datamap.value("reason").toString());

        } else if (thisclass == "filelist") {
            serverFileList(datamap.value("filelist").toStringList());

        } else if (thisclass == "presence") {
            // QString id = datamap.value("company").toString() + "/" + datamap.value("userid").toString();
            QString id = datamap.value("astid").toString() + "/" + datamap.value("xivo_userid").toString();
            // qDebug() << Q_FUNC_INFO << thisclass << m_users.size() << id;
            if (m_users.contains(id)) {
                QString stateid = datamap.value("capapresence").toMap().value("state").toMap().value("stateid").toString();
                QVariantMap changeme = m_guioptions.value("server_gui").toMap().value("presence.autochangestate").toMap();
                if (changeme.count() && (id == m_fullid)) {
                    if (changeme.contains(stateid)) {
                        // if (stateid == changeme["statesrc"].toString()) {
                        QVariantMap changemeconf = changeme[stateid].toMap();
                        m_timerid_changestate = startTimer(changemeconf.value("delaymsec").toInt());
                        m_changestate_newstate = changemeconf.value("newstate").toString();
                        m_changestate_oldstate = stateid;
                    }
                }
                m_users[id]->setAvailState(datamap.value("capapresence").toMap().value("state"));
                emit updatePeerAgent(m_timesrv, id, "imstatus", QStringList());
                emit updateAgentPresence(m_users[id]->astid(),
                                         m_users[id]->agentid(),
                                         datamap.value("capapresence").toMap().value("state"));
                m_counters = datamap.value("presencecounter");
                emit updateCounter(m_counters);
                if (id == m_fullid) {
                    updateCapaPresence(datamap.value("capapresence"));
                    updatePresence(m_capapresence);
                    emit localUserInfoDefined(m_users[m_fullid]);
                }
            }

        } else if (thisclass == "users") {
            if (function == "sendlist") {
                QVariantList listusers = datamap.value("payload").toList();
                foreach (QVariant userprops, listusers) {
                    QVariantMap uinfo = userprops.toMap();
                    //qDebug() << "-------------" << uinfo;

                    addUpdateUserInTree(tree(), uinfo);

                    QString iduser = QString("%1/%2")
                        .arg(uinfo.value("astid").toString())
                        .arg(uinfo.value("xivo_userid").toString());
                    if (! m_users.contains(iduser)) {
                        m_users[iduser] = new UserInfo(iduser);
                        m_users[iduser]->setCtiLogin(uinfo.value("user").toString());
                        m_users[iduser]->setFullName(uinfo.value("fullname").toString());
                        //emit newUser(m_users[iduser]);
                        m_users[iduser]->setPhones(uinfo.value("astid").toString(),
                                                   uinfo.value("techlist").toStringList(),
                                                   m_phones);
                    }

                    m_users[iduser]->setAvailState(uinfo.value("statedetails"));
                    m_users[iduser]->setPhoneNumber(uinfo.value("phonenum").toString());
                    m_users[iduser]->setMobileNumber(uinfo.value("mobilenum").toString());
                    m_users[iduser]->setAgentId(uinfo.value("agentid").toString());
                    m_users[iduser]->setContext(uinfo.value("context").toString());

                    m_users[iduser]->setMWI(uinfo.value("mwi").toStringList());
                    m_users[iduser]->setSimultCalls(uinfo.value("simultcalls").toInt());
                    m_users[iduser]->setVoiceMailNumber(uinfo.value("voicemailnum").toString());
                    m_users[iduser]->setAgentNumber(uinfo.value("agentnumber").toString());
                    //m_users[iduser]->setContext(uinfo.value("context").toString());
                    emit updatePeerAgent(m_timesrv, iduser, "imstatus", QStringList());
                    emit updateAgentPresence(m_users[iduser]->astid(),
                                             m_users[iduser]->agentid(),
                                             uinfo.value("statedetails"));
                    emit userUpdated(m_users[iduser]);
                }

                peersReceived();
                m_monitored_userid = m_fullid;
                QString fullname_mine = "No One";
                if (m_users.contains(m_fullid)) {
                    fullname_mine = m_users[m_fullid]->fullname();
                    emit localUserInfoDefined(m_users[m_fullid]);
                    emit updateCounter(m_counters);
                }

                // Who do we monitor ?
                // First look at the last monitored one
                QString fullid_watched;
                if (m_capafuncs.contains("switchboard")) {
                    fullid_watched = m_settings->value("monitor/userid").toString();
                } else {
                    fullid_watched = "";
                }
                QString fullname_watched = "";
                // If there was nobody, let's watch ourselves.
                if (fullid_watched.isEmpty()) {
                    fullid_watched = m_fullid;
                    fullname_watched = fullname_mine;
                } else {
                    if (m_users.contains(fullid_watched)) {
                        fullname_watched = m_users[fullid_watched]->fullname();
                    }
                    // If the CallerId value is empty, fallback to ourselves.
                    if (fullname_watched.isEmpty()) {
                        fullid_watched = m_fullid;
                        fullname_watched = fullname_mine;
                    }
                }
                monitorPeerRequest(fullid_watched);
            } else if (function == "update") {
                QStringList userupdate = datamap.value("user").toStringList();
                if (userupdate.size() == 2) {
                    QString iduser = userupdate[0] + "/" + userupdate[1];
                    if (m_users.contains(iduser) && (iduser == m_fullid)) {
                        QString subclass = datamap.value("subclass").toString();
                        if (subclass == "mwi") {
                            m_users[iduser]->setMWI(datamap.value("payload").toStringList());
                            emit localUserInfoDefined(m_users[m_fullid]);
                        }
                        emit userUpdated(m_users[iduser]);
                    }
                }
            }

        } else if (thisclass == "features") {
            if (function == "update") {
                QVariantMap featuresupdate_map = datamap.value("payload").toMap();
                if (m_monitored_userid == datamap.value("userid").toString())
                    foreach (QString featurekey, featuresupdate_map.keys())
                        initFeatureFields(featurekey, featuresupdate_map.value(featurekey));

            } else if (function == "get") {
                QVariantMap featuresget_map = datamap.value("payload").toMap();
                if (m_monitored_userid == datamap.value("userid").toString()) {
                    resetFeatures();
                    foreach (QString featurekey, featuresget_map.keys()) {
                        initFeatureFields(featurekey, featuresget_map.value(featurekey));
                    }
                    emit emitTextMessage(tr("Received Services Data"));
                }

            } else if (function == "put") {
                QVariantMap featuresput_map = datamap.value("payload").toMap();
                if (m_monitored_userid == datamap.value("userid").toString()) {
                    if (featuresput_map.isEmpty()) {
                        featurePutIsKO();
                        emit emitTextMessage(tr("Could not modify the Services data.") + " " + tr("Maybe Asterisk is down."));
                    } else {
                        featurePutIsOK();
                        foreach (QString featurekey, featuresput_map.keys()) {
                            initFeatureFields(featurekey, featuresput_map.value(featurekey));
                        }
                        emit emitTextMessage("");
                    }
                }
            }

        } else if (thisclass == "phones") {
            if (function == "sendlist") {
                QMap<QString, QVariant> payload = datamap.value("payload").toMap();
                foreach (QString astid, payload.keys()) {
                    QMap<QString, QVariant> values = payload[astid].toMap();
                    foreach (QString phoneid, values.keys())
                        updatePhone(astid, phoneid, values[phoneid].toMap());
                }
                //emit callsUpdated();
                emit peersReceived();
            } else if (function == "update") {
                QString astid = datamap.value("astid").toString();
                QString phoneid = datamap.value("phoneid").toString();
                QVariantMap value = datamap.value("status").toMap();
                updatePhone(astid, phoneid, value);
                //callsUpdated();
            } else if (function == "add") {
                qDebug() << thisclass << function << datamap.value("astid").toString() << datamap.value("deltalist").toStringList();
            } else if (function == "del") {
                qDebug() << thisclass << function << datamap.value("astid").toString() << datamap.value("deltalist").toStringList();
            } else if (function == "signal-deloradd") {
                QStringList listpeers = datamap.value("payload").toStringList();
                // qDebug() << "phones-signal-deloradd" << listpeers;
                //emit emitTextMessage(tr("New phone list on %1 : - %2 + %3 = %4 total").arg(listpeers[0],
                //listpeers ));
                if (listpeers[1].toInt() > 0)
                    sendCommand("phones-del");
                if (listpeers[2].toInt() > 0)
                    sendCommand("phones-add");
            }

        } else if (thisclass == "trunks") {
            // if (function == "update") {
            // QString astid = datamap.value("astid").toString();
            // QString trunkid = datamap.value("trunkid").toString();
            // QVariant value = datamap.value("status");
            // updatePhone(astid, trunkid, value);
            // callsUpdated();
            // }
            qDebug() << Q_FUNC_INFO << thisclass << "not yet supported";

        } else if (thisclass == "login_id_ok") {
            QString tohash = datamap.value("sessionid").toString() + ":" + m_password;
            QCryptographicHash hidepass(QCryptographicHash::Sha1);
            QByteArray res = hidepass.hash(tohash.toAscii(), QCryptographicHash::Sha1).toHex();
            QVariantMap command;
            command["class"] = "login_pass";
            command["direction"] = "xivoserver";
            command["hashedpassword"] = QString(res);
            sendJsonCommand(command);

        } else if (thisclass == "loginko") {
            stop();
            popupError(datamap.value("errorstring").toString());

        } else if (thisclass == "login_pass_ok") {
            QStringList capas = datamap.value("capalist").toStringList();
            QVariantMap command;
            command["class"] = "login_capas";
            command["direction"] = "xivoserver";
            if (capas.size() == 1)
                command["capaid"] = capas[0];
            else {
                if (m_useridopt.size() > 0) {
                    if (capas.contains(m_useridopt))
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

            if (m_checked_function["presence"])
                command["state"] = m_availstate;
            else
                command["state"] = __nopresence__;
            command["lastconnwins"] = m_checked_lastconnwins;
            sendJsonCommand(command);

        } else if (thisclass == "login_capas_ok") {
            //qDebug() << "login_capas_ok" << datamap.keys();
            m_astid = datamap.value("astid").toString();
            m_xivo_userid = datamap.value("xivo_userid").toString();
            m_fullid = m_astid + "/" + m_xivo_userid;
            m_capafuncs = datamap.value("capafuncs").toStringList();
            m_capaxlets = datamap.value("capaxlets").toStringList();
            m_appliname = datamap.value("appliname").toString();
            updateCapaPresence(datamap.value("capapresence"));
            m_forced_state = datamap.value("capapresence").toMap().value("state").toString();
            m_counters = datamap.value("presencecounter");
            m_guioptions["server_gui"] = datamap.value("guisettings");
            //qDebug() << "======== guisettings ======== " << datamap.value("guisettings");

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
            tmpa["services"] = datamap.value("capaservices");
            m_guioptions["merged_gui"] = tmpa;

            QVariantMap tmp;
            QStringList todisp;
            m_checked_function["switchboard"] = true;
            foreach (QString function, m_capafuncs)
                if (m_checked_function.contains(function) && m_checked_function[function])
                    todisp.append(function);
            tmp["functions"] = todisp;
            m_guioptions["server_funcs"] = tmp;

            //qDebug() << "clientXlets" << XletList;
            qDebug() << "\n";
            qDebug() << "m_capaxlets" << m_capaxlets;
            qDebug() << "m_capafuncs" << m_capafuncs;
            qDebug() << "m_appliname" << m_appliname;
            qDebug() << "m_counters"  << m_counters;
            qDebug() << "\n";

            QString urltolaunch = m_guioptions.value("merged_gui").toMap().value("loginwindow.url").toString();
            if (! urltolaunch.isEmpty()) {
                urltolaunch.replace("{xc-username}", m_userid);
                urltolaunch.replace("{xc-password}", m_password);
                this->sendUrlToBrowser(urltolaunch);
            }

            // XXXX m_capafuncs => config file
            // m_enabled_function's purposes are :
            // - to keep track of the user's true rights
            foreach (QString function, CheckFunctions)
                if (m_capafuncs.contains(function))
                    m_enabled_function[function] = true;
                else
                    m_enabled_function[function] = false;

            if (m_capafuncs.size() == 0) {
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
            QString type = datamap.value("type").toString();
            stop();
            if (type=="force") {
                m_forced_to_disconnect = true;// disable autoreconnect
                popupError("forcedisconnected");
            } else {
                popupError("disconnected");
            }
        } else {
            qDebug() << Q_FUNC_INFO << "unknown server command class" << thisclass << datamap;
        }
    }
}

/*! \brief send meetme command to the CTI server */
void BaseEngine::meetmeAction(const QString &function, const QString &functionargs)
{
    qDebug() <<"meetmeAction" << function << " -- arg: " << functionargs;
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
    if (m_filedata.size() > 0) {
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

    // errors sent by the server (login phase)
    if (errorid.toLower() == "user_not_found") {
        errormsg = tr("Your registration name <%1@%2> "
                      "is not known by the XiVO CTI server on %3:%4.")
            .arg(m_userid).arg(m_company)
            .arg(m_serverhost).arg(m_ctiport);
    } else if (errorid.toLower() == "login_password") {
        errormsg = tr("You entered a wrong login / password.");
    } else if (errorid.startsWith("capaid_undefined:")) {
        QStringList capainfo = errorid.split(":");
        errormsg = tr("Your profile identifier <%1> is not defined.").arg(capainfo[1]);

    // keepalive (internal)
    } else if (errorid.toLower() == "no_keepalive_from_server") {
        errormsg = tr("The XiVO CTI server on %1:%2 did not reply to the last keepalive.")
            .arg(m_serverhost).arg(m_ctiport);

    // socket errors - while attempting to connect
    } else if (errorid.toLower() == "socket_error_hostnotfound") {
        errormsg = tr("You defined an IP address %1 that is probably an unresolved host name.")
            .arg(m_serverhost);
    } else if (errorid.toLower() == "socket_error_timeout") {
        errormsg = tr("Socket timeout (~ 60 s) : you probably attempted to reach, "
                      "via a gateway, an IP address %1 that does not exist.")
            .arg(m_serverhost);
    } else if (errorid.toLower() == "socket_error_connectionrefused") {
        errormsg = tr("There seems to be a machine running on this IP address %1, "
                      "and either no CTI server is running, or your port %2 is wrong.")
            .arg(m_serverhost).arg(m_ctiport);
    } else if (errorid.toLower() == "socket_error_network") {
        errormsg = tr("An error occurred on the network while attempting to join the IP address %1 :\n"
                      "- no external route defined to access this IP address (~ no timeout)\n"
                      "- this IP address is routed but there is no machine (~ 5 s timeout)\n"
                      "- a cable has been unplugged on your LAN on the way to this IP address (~ 30 s timeout).")
            .arg(m_serverhost);
    } else if (errorid.toLower() == "socket_error_unknown") {
        errormsg = tr("An unknown socket error has occured while attempting to join the IP address:port %1:%2.")
            .arg(m_serverhost).arg(m_ctiport);
    } else if (errorid.startsWith("socket_error_unmanagedyet:")) {
        QStringList ipinfo = errorid.split(":");
        errormsg = tr("An unmanaged (number %1) socket error has occured while attempting to join the IP address:port %1:%2.")
            .arg(ipinfo[1]).arg(m_serverhost).arg(m_ctiport);

    // socket errors - once connected
    } else if (errorid.toLower() == "socket_error_remotehostclosed") {
        errormsg = tr("The XiVO CTI server on %1:%2 has just closed the connection.")
            .arg(m_serverhost).arg(m_ctiport);

    } else if (errorid.toLower() == "server_stopped") {
        errormsg = tr("The XiVO CTI server on %1:%2 has just been stopped.")
            .arg(m_serverhost).arg(m_ctiport);
    } else if (errorid.toLower() == "server_reloaded") {
        errormsg = tr("The XiVO CTI server on %1:%2 has just been reloaded.")
            .arg(m_serverhost).arg(m_ctiport);
    } else if (errorid.startsWith("already_connected:")) {
        QStringList ipinfo = errorid.split(":");
        errormsg = tr("You are already connected from %1:%2.").arg(ipinfo[1]).arg(ipinfo[2]);
    } else if (errorid.toLower() == "no_capability") {
        errormsg = tr("No capability allowed.");
    } else if (errorid.startsWith("toomuchusers:")) {
        QStringList userslist = errorid.split(":")[1].split(";");
        errormsg = tr("Max number (%1) of XiVO Clients already reached.").arg(userslist[0]);
    } else if (errorid.startsWith("missing:")) {
        errormsg = tr("Missing Argument(s)");
    } else if (errorid.startsWith("version_client:")) {
        QStringList versionslist = errorid.split(":")[1].split(";");
        if (versionslist.size() >= 2) {
            errormsg = tr("Your client version (%1) is too old for this server.\n"
                          "Please upgrade it to %2 at least.")
                .arg(__git_hash__)
                .arg(versionslist[1]);
        } else {
            errormsg = tr("Your client version (%1) is too old for this server.\n"
                          "Please upgrade it.").arg(__git_hash__);
        }
    } else if (errorid.startsWith("xivoversion_client:")) {
        QStringList versionslist = errorid.split(":")[1].split(";");

        if (versionslist.size() >= 2)
            errormsg = tr("Your client's major version (%1)\n"
                          "is not the same as the server's (%2).")
                .arg(__xivo_version__)
                .arg(versionslist[1]);
    } else if (errorid.startsWith("version_server:")) {
        QStringList versionslist = errorid.split(":")[1].split(";");
        if (versionslist.size() >= 2) {
            errormsg = tr("Your server version (%1) is too old for this client.\n"
                          "Please upgrade it to %2 at least.")
                .arg(versionslist[0])
                .arg(__git_hash__);
        } else {
            errormsg = tr("Your server version (%1) is too old for this client.\n"
                          "Please upgrade it.").arg(versionslist[0]);
        }
    } else if (errorid == "disconnected") {
        errormsg = tr("You were disconnected by the server.");
    } else if (errorid == "forcedisconnected") {
        errormsg = tr("You were forced to disconnect by the server.");
    }

    // logs a message before sending any popup that would block
    emit emitTextMessage(tr("ERROR") + " : " + errormsg);
    if (!m_trytoreconnect || m_forced_to_disconnect)
        emitMessageBox(errormsg);
}

/*! \brief save BaseEngine::m_downloaded to a file
 *  \sa BaseEngine::m_downloaded
 */
void BaseEngine::saveToFile(const QString & filename)
{
    qDebug() << Q_FUNC_INFO << filename << m_downloaded.size();
    QFile outputfile(filename);
    outputfile.open(QIODevice::WriteOnly);
    outputfile.write(m_downloaded);
    outputfile.close();
}

/*! \brief called when data are ready to be read on the socket.
 *
 * Read and process the data from the server.
 */
void BaseEngine::ctiSocketReadyRead()
{
    while(m_ctiserversocket->canReadLine()) {
        QByteArray data  = m_ctiserversocket->readLine();
        m_byte_counter += data.size();
        // qDebug() << Q_FUNC_INFO << "data.size() = " << data.size();
        QString line = QString::fromUtf8(data);

        if (line.startsWith("<ui version=")) {
            // we get here when receiving a sheet as a Qt4 .ui form
            qDebug() << Q_FUNC_INFO << "(Customer Info)" << line.size();
            displayFiche(line, true, QString());
        } else
            parseCommand(line);
    }
}

/*! \brief read data from the file transfer socket
 *
 * Read text data, the file is encapsulated into JSON as a base 64 string.
 */
void BaseEngine::filetransferSocketReadyRead()
{
    while(m_filetransfersocket->canReadLine()) {
        QByteArray data = m_filetransfersocket->readLine();
        QString line = QString::fromUtf8(data);
        QVariant jsondata;
        try {
            jsondata = JsonQt::JsonToVariant::parse(line.trimmed());
        } catch(JsonQt::ParseException) {
            qDebug() << Q_FUNC_INFO << "exception catched for" << line.trimmed();
        }
        QVariantMap jsondatamap = jsondata.toMap();
        if (jsondatamap.value("class").toString() == "fileref") {
            if (m_filedir == "download") {
                m_downloaded = QByteArray::fromBase64(jsondatamap.value("payload").toByteArray());
                qDebug() << jsondatamap.value("filename").toString() << m_downloaded.size();
                fileReceived();
            } else {
                QByteArray fax64 = m_filedata.toBase64();
                qDebug() << "sending fax contents" << jsondatamap.value("fileid").toString()
                         << m_faxsize << fax64.size();
                if (m_faxsize > 0) {
                    m_filetransfersocket->write(fax64 + "\n");
                    m_filetransfersocket->flush();
                }
                m_filedata.clear();
            }
            m_filetransfersocket->disconnectFromHost();
            m_faxsize = 0;
            m_fileid = "";
        }
    }
}

/*! \brief forward actions from fiche to XiVO CTI server
 *
 * Build and send JSON command
 */
void BaseEngine::actionFromFiche(const QVariant & infos)
{
    // qDebug() << Q_FUNC_INFO << infos;
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

/*! \brief send telephony command to the server
 *
 * \param action originate/transfer/atxfer/hangup/answer/refuse
 */
void BaseEngine::actionCall(const QString & action,
                            const QString & src,
                            const QString & dst)
{
    qDebug() << Q_FUNC_INFO << action << src << dst;

    QVariantMap command;
    command["direction"] = "xivoserver";
    command["class"] = action;

    if ((action == "originate") || (action == "transfer") || (action == "atxfer")) {
        command["source"] = src;
        if ((dst == "ext:special:dialxlet") && (! m_numbertodial.isEmpty()))
            command["destination"] = "ext:" + m_numbertodial;
        else
            command["destination"] = dst;
        sendJsonCommand(command);
    } else if ((action == "hangup") || (action == "transfercancel")) {
        QVariantMap ipbxcommand;
        ipbxcommand["command"] = action;
        ipbxcommand["channelids"] = src;
        ipbxCommand(ipbxcommand);
    } else if (action == "answer") {
        QVariantMap ipbxcommand;
        ipbxcommand["command"] = action;
        ipbxcommand["phoneids"] = src;
        ipbxCommand(ipbxcommand);
    } else if (action == "refuse") {
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
    // qDebug() << Q_FUNC_INFO << text;
    QVariantMap command;
    command["class"] = "directory-search";
    command["direction"] = "xivoserver";
    command["pattern"] = text;
    sendJsonCommand(command);
}

// === Getter and Setters ===
/* \brief set server address
 *
 * Set server host name and server port
 */
void BaseEngine::setAddress(const QString & host, quint16 port)
{
    // qDebug() << Q_FUNC_INFO << host << port;
    m_serverhost = host;
    m_ctiport = port;
}

/*! \brief get server IP address */
const QString & BaseEngine::serverip() const
{
    return m_serverhost;
}

/*! \brief get server port */
quint16 BaseEngine::sbPort()
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

const QString& BaseEngine::userId() const
{
    if (m_useridopt.size() > 0)
        return m_useridwithopt;
    else
        return m_userid;
}

void BaseEngine::setUserId(const QString &userid)
{
    QStringList useridsplit = userid.split("%");
    m_userid = useridsplit[0].trimmed();
    m_useridwithopt = userid;
    if (useridsplit.size() > 1) {
        m_useridopt = useridsplit[1].trimmed();
    } else {
        m_useridopt = "";
    }
}

const QString & BaseEngine::agentphonenumber() const
{
    return m_agentphonenumber;
}

void BaseEngine::setAgentPhoneNumber(const QString & agentphonenumber)
{
    m_agentphonenumber = agentphonenumber;
}

int BaseEngine::loginkind()
{
    return m_loginkind;
}

void BaseEngine::setLoginKind(const int loginkind)
{
    if (loginkind != m_loginkind) {
        QVariantMap tmpqvm = m_guioptions.value("client_gui").toMap();
        tmpqvm["loginkind"] = loginkind;
        m_guioptions["client_gui"] = tmpqvm;

        m_loginkind = loginkind;
    }
}

int BaseEngine::showagselect()
{
    return m_showagselect;
}

void BaseEngine::setShowAgentSelect(const int showagselect)
{
    m_showagselect = showagselect;
}

int BaseEngine::keeppass()
{
    return m_keeppass;
}

void BaseEngine::setKeepPass(int keeppass)
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
    bool isenabled = value.toMap().value("enabled").toBool();
    if ((field == "enablevoicemail") || (field == "vm"))
        optChanged("enablevm", isenabled);
    else if ((field == "enablednd") || (field == "dnd"))
        optChanged("enablednd", isenabled);
    else if (field == "incallfilter")
        optChanged("incallfilter", isenabled);
    else if (field == "callrecord")
        optChanged("callrecord", isenabled);
    else if ((field == "unc") || (field == "busy") || (field == "rna"))
        forwardUpdated(field, value);
    else if ((field == "enableunc") || (field == "enablebusy") || (field == "enablerna"))
        forwardUpdated(field.mid(6), value);
}

void BaseEngine::stopKeepAliveTimer()
{
    if (m_timerid_keepalive > 0) {
        killTimer(m_timerid_keepalive);
        m_timerid_keepalive = 0;
    }
}

void BaseEngine::stopTryAgainTimer()
{
    if (m_timerid_tryreconnect > 0) {
        killTimer(m_timerid_tryreconnect);
        m_timerid_tryreconnect = 0;
    }
}

void BaseEngine::startTryAgainTimer()
{
    if (m_timerid_tryreconnect == 0 && m_trytoreconnect && !m_forced_to_disconnect)
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
    if (m_trytoreconnectinterval != i) {
        m_trytoreconnectinterval = i;
        if (m_timerid_tryreconnect > 0) {
            killTimer(m_timerid_tryreconnect);
            m_timerid_tryreconnect = startTimer(m_trytoreconnectinterval);
        }
    }
}

/*! \brief implement timer event
 *
 */
void BaseEngine::timerEvent(QTimerEvent *event)
{
    int timerId = event->timerId();
    if (timerId == m_timerid_keepalive) {
        keepLoginAlive();
    } else if (timerId == m_timerid_tryreconnect) {
        emit emitTextMessage(tr("Attempting to reconnect to server"));
        start();
    } else if (timerId == m_timerid_changestate) {
        if (m_availstate == m_changestate_oldstate)
            setAvailState(m_changestate_newstate, false);
        killTimer(timerId);
        m_timerid_changestate = 0;
    } else {
        qDebug() << Q_FUNC_INFO << "ghost timer" << timerId << "will be stopped";
        killTimer(timerId);
    }
}

/*! \brief send a feature put command to the cti server
 *
 * ???
 */
void BaseEngine::featurePutOpt(const QString &capa, bool b)
{
    QVariantMap command;
    command["class"] = "featuresput";
    command["direction"] = "xivoserver";
    command["userid"] = m_monitored_userid;
    if (capa == "enablevm")
        command["function"] = "enablevoicemail";
    else if (capa == "callrecord")
        command["function"] = "callrecord";
    else if (capa == "incallfilter")
        command["function"] = "incallfilter";
    else if (capa == "enablednd")
        command["function"] = "enablednd";
    command["value"] = QString(b ? "1" : "0");
    sendJsonCommand(command);
}

/*! \brief send a feature put command to the cti server
 *
 */
void BaseEngine::featurePutForward(const QString & capa, bool b, const QString & dst)
{
    QVariantMap command;
    command["class"] = "featuresput";
    command["direction"] = "xivoserver";
    command["userid"] = m_monitored_userid;
    if (capa == "fwdunc")
        command["function"] = "enableunc";
    else if (capa == "fwdbusy")
        command["function"] = "enablebusy";
    else if (capa == "fwdrna")
        command["function"] = "enablerna";
    command["value"] = QString(b ? "1" : "0");
    command["destination"] = dst;
    sendJsonCommand(command);
}

/*! \brief send a featursget command to the cti server */
void BaseEngine::askFeatures()
{
    qDebug() << Q_FUNC_INFO << m_monitored_userid;
    QString featurestoget = "user:special:me";
    if (! m_monitored_userid.isEmpty()) {
        featurestoget = m_monitored_userid;
    }
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
    // qDebug() << Q_FUNC_INFO;
    QVariantMap command;
    command["direction"] = "xivoserver";
    command["function"] = "getlist";
    QStringList getlists = (QStringList() << "users" << "queues" << "agents" << "phones" << "meetme" << "users" << "parking" << "endinit");
    foreach (QString kind, getlists) {
        command.remove("class");
        command["class"] = kind;
        sendJsonCommand(command);
    }
    if (m_loginkind == 2) {
        QVariantMap ipbxcommand;
        ipbxcommand["command"] = "agentlogin";
        ipbxcommand["agentids"] = "agent:special:me";
        ipbxcommand["agentphonenumber"] = m_agentphonenumber;
        ipbxCommand(ipbxcommand);
    }
}

/*! \brief send an ipbxcommand command to the cti server */
void BaseEngine::ipbxCommand(const QVariantMap & ipbxcommand)
{
    if (! ipbxcommand.contains("command")) {
        return ;
    }
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

void BaseEngine::setForcelocale(QString b)
{
    m_forcelocale = b;
    this->changeTranslation(b);
}

QString BaseEngine::forcelocale() const
{
    return m_forcelocale;
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
    if (i != m_keepaliveinterval) {
        m_keepaliveinterval = i;
        if (m_timerid_keepalive > 0) {
            killTimer(m_timerid_keepalive);
            m_timerid_keepalive = startTimer(m_keepaliveinterval);
        }
    }
}

BaseEngine::EngineState BaseEngine::state()
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
    if (state != m_state) {
        m_state = state;
        if (state == ELogged) {
            stopTryAgainTimer();
            if (m_checked_function["presence"] && m_enabled_function["presence"]) {
                availAllowChanged(true);
            }
            emit logged();
            updatePresence(m_capapresence);
        } else if (state == ENotLogged) {
            availAllowChanged(false);
            emit delogged();
            // reset some variables when disconnecting
            m_agent_watched_astid = "";
            m_agent_watched_agentid = "";
        }
    }
}

void BaseEngine::changeWatchedAgentSlot(const QString & agentid, bool force)
{
    // qDebug() << Q_FUNC_INFO << agentid << force;
    if ((force || (agentid.size() > 0)) && (m_agents.contains(agentid))) {
        m_agent_watched_agentid = agentid;
        emit changeWatchedAgentSignal(agentid);
    }
}

void BaseEngine::changeWatchedQueueSlot(const QString & queueid)
{
    m_queue_watched_queueid = queueid;
    emit changeWatchedQueueSignal(queueid);
}

/*! \brief sets m_osname
 *
 * also builds a string defining who is the client (SB or XC @ osname)
 */
void BaseEngine::setOSInfos(const QString & osname)
{
    m_osname = osname;
    m_clientid = "undef@" + m_osname;
}

/*!
 * Send a keep alive message to the login server.
 * The message is sent in a datagram through m_udpsocket
 */
void BaseEngine::keepLoginAlive()
{
    // qDebug() << Q_FUNC_INFO << m_availstate;
    // got to disconnected state if more than xx keepalive messages
    // have been left without response.
    if (m_pendingkeepalivemsg > 1) {
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
    if (m_rate_bytes > 100000) {
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

/*! \brief send error message to the CTI Server */
void BaseEngine::logClientWarning(const QString & classmethod,
                                  const QString & message)
{
    // qDebug() << Q_FUNC_INFO << classmethod << message;
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

/*! \brief get pointer to the currently logged user
 *
 * Return NULL if not available */
UserInfo* BaseEngine::getXivoClientUser()
{
    // qDebug() << Q_FUNC_INFO << m_astid << m_xivo_userid << "aaaa" << m_fullid;
    if (m_users.contains(m_fullid)) {
        return m_users.value(m_fullid);
    }
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
 * Dial the number if the message is tel:<i>number</i>.
 * \see QtSingleApplication
 */
void BaseEngine::handleOtherInstanceMessage(const QString & msg)
{
    qDebug() << Q_FUNC_INFO << msg;
    // callto://number is unofficial and used by Skype
    // tel:number is in RFC 3966
    // callto:number is unofficial (read 7.3. in RFC 3966)
    // we support tel:number and callto:number
    QRegExp re("^(tel|callto):([-0-9\\. +]*[0-9])", Qt::CaseInsensitive);
    // todo : handle also other commands
    int pos = re.indexIn(msg);
    if (pos >= 0) {
        QString phonenum = re.cap(2);
        phonenum.remove('.').remove(' ').remove('-');
        qDebug() << Q_FUNC_INFO << "trying to dial" << phonenum;
        actionCall("originate", "user:special:me", QString("ext:%1").arg(phonenum));
    }
}

int BaseEngine::callClassEventCallback(QString class_event, const QVariantMap & map)
{
    QList< e_callback* > values = m_class_event_cb.values(class_event);
    e_callback * p;
    int i;

    for (i=0;i<values.size();++i) {
        p = values.at(i);
        p->cb(map, p->udata);
    }

    return values.size();
}

void BaseEngine::registerClassEvent(const QString & class_event,
                                    void (*cb)(const QVariantMap &, void *),
                                    void * udata)
{
    e_callback * e_call = new e_callback;

    e_call->cb = cb;
    e_call->udata = udata;

    m_class_event_cb.insert(class_event, e_call);
}

void BaseEngine::registerTranslation(const QString &path)
{
    QString locale = QLocale::system().name();
    QTranslator *translator = new QTranslator;
    translator->load(path.arg(locale));
    qApp->installTranslator(translator);
}

void BaseEngine::changeTranslation(const QString &locale)
{
    QVector<QTranslator *> new_translators;

    int i;
    for(i=0;i<translationFiles.size();++i) {
        if (locale != "en_US") {
            new_translators.append(new QTranslator);
            new_translators.at(i)->load(translationFiles.at(i).arg(locale));
            qApp->installTranslator(new_translators.at(i));
        } else if (!translators.isEmpty()) {
            qApp->removeTranslator(translators.at(i));
        }
    }

    translators = new_translators;
}

void BaseEngine::sendUrlToBrowser(const QString & value)
{
    // qDebug() << Q_FUNC_INFO << value;
#ifdef Q_WS_WIN
    QSettings settings("HKEY_CLASSES_ROOT\\HTTP\\shell\\open\\command", QSettings::NativeFormat);
    QString command = settings.value(".").toString();
    QRegExp rx("\"(.+)\"");
    if (rx.indexIn(command) != -1)
        command = rx.capturedTexts()[1];
    QUrl url(value);
    QFileInfo browserFileInfo(command);
    if (browserFileInfo.fileName() == "iexplore.exe") {
        QProcess::startDetached(browserFileInfo.absoluteFilePath(), QStringList() << "-new" << url.toEncoded());
    } else {
        QDesktopServices::openUrl(url);
    }
#else
    QDesktopServices::openUrl(QUrl(value));
#endif
}

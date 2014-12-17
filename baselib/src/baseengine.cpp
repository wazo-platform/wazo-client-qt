/* XiVO Client
 * Copyright (C) 2007-2014 Avencall
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

#include <QApplication>
#include <QCryptographicHash>
#include <QDateTime>
#include <QDebug>
#include <QDesktopServices>
#include <QDir>
#include <QFileInfo>
#include <QHostAddress>
#include <QLocale>
#include <QSettings>
#include <QProcess>
#include <QTcpSocket>
#include <QTranslator>
#include <QUrl>
#include <QUrlQuery>
#include <QLibraryInfo>
#include <QSslError>
#include <QSslSocket>
#include <QUdpSocket>

#include <QJsonDocument>

#include <storage/agentinfo.h>
#include <storage/channelinfo.h>
#include <storage/phoneinfo.h>
#include <storage/queueinfo.h>
#include <storage/queuememberinfo.h>
#include <storage/userinfo.h>
#include <storage/voicemailinfo.h>

#include "ipbxlistener.h"
#include "xivoconsts.h"
#include "baseengine.h"
#include "cti_server.h"
#include "phonenumber.h"
#include "message_factory.h"


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

BASELIB_EXPORT BaseEngine * b_engine;
static const QStringList CheckFunctions = (QStringList()
                                           << "presence"
                                           << "customerinfo");
static const QStringList GenLists = (QStringList()
                                     << "users"
                                     << "phones"
                                     << "agents"
                                     << "queues"
                                     << "voicemails"
                                     << "queuemembers");
static CTIServer * m_cti_server;

BaseEngine::BaseEngine(QSettings *settings, const QString &osInfo)
    : QObject(NULL),
      m_sessionid(""),
      m_state(ENotLogged),
      m_pendingkeepalivemsg(0),
      m_logfile(NULL),
      m_attempt_loggedin(false),
      m_forced_to_disconnect(false)
{
    settings->setParent(this);
    m_timerid_keepalive = 0;
    m_timerid_changestate = 0;
    m_timerid_tryreconnect = 0;
    setOSInfos(osInfo);
    m_settings = settings;
    loadSettings();

    m_xinfoList.insert("users", newXInfo<UserInfo>);
    m_xinfoList.insert("phones", newXInfo<PhoneInfo>);
    m_xinfoList.insert("agents", newXInfo<AgentInfo>);
    m_xinfoList.insert("queues", newXInfo<QueueInfo>);
    m_xinfoList.insert("voicemails", newXInfo<VoiceMailInfo>);
    m_xinfoList.insert("queuemembers", newXInfo<QueueMemberInfo>);

    // TCP connection with CTI Server
    m_ctiserversocket = new QSslSocket(this);
    m_ctiserversocket->setProtocol(QSsl::TlsV1_0);
    m_cti_server = new CTIServer(m_ctiserversocket);

    connect(m_ctiserversocket, SIGNAL(sslErrors(const QList<QSslError> &)),
            this, SLOT(sslErrors(const QList<QSslError> & )));
    connect(m_ctiserversocket, SIGNAL(connected()),
            this, SLOT(authenticate()));
    connect(m_ctiserversocket, SIGNAL(readyRead()),
            this, SLOT(ctiSocketReadyRead()));
    connect(m_cti_server, SIGNAL(disconnected()),
            this, SLOT(onCTIServerDisconnected()));
    connect(m_cti_server, SIGNAL(failedToConnect(const QString &, const QString &, const QString &)),
            this, SLOT(popupError(const QString &, const QString &, const QString &)));

    connect(&m_init_watcher, SIGNAL(watching()),
            this, SIGNAL(initializing()));
    connect(&m_init_watcher, SIGNAL(sawAll()),
            this, SIGNAL(initialized()));

    connect(m_cti_server, SIGNAL(failedToConnect(const QString &, const QString &, const QString &)),
            this, SIGNAL(doneConnecting()));
    connect(this, SIGNAL(initialized()),
            this, SIGNAL(doneConnecting()));

    // TCP connection for file transfer
    // (this could be moved to some other class)
    m_filetransfersocket = new QTcpSocket(this);
    connect(m_filetransfersocket, SIGNAL(connected()),
            this, SLOT(filetransferSocketConnected()));
    connect(m_filetransfersocket, SIGNAL(readyRead()),
            this, SLOT(filetransferSocketReadyRead()));

    if (m_config["autoconnect"].toBool())
        start();
    setupTranslation();
}

void BaseEngine::sslErrors(const QList<QSslError> & qlse)
{
    qDebug() << Q_FUNC_INFO;
    foreach (QSslError qse, qlse)
        qDebug() << " ssl error" << qse;
    m_ctiserversocket->ignoreSslErrors();
    // "The host name did not match any of the valid hosts for this certificate"
    // "The certificate is self-signed, and untrusted"
    // "The certificate has expired"
    // see http://doc.trolltech.com/4.6/qsslerror.html for a list
}

/*! \brief Destructor
 */
BaseEngine::~BaseEngine()
{
    clearLists();
    clearChannelList();
    deleteTranslators();
}

QSettings* BaseEngine::getSettings()
{
    return m_settings;
}

/*!
 * Load Settings from the registry/configuration file
 * Use default values when settings are not found.
 */
void BaseEngine::loadSettings()
{
    m_config["systrayed"] = m_settings->value("display/systrayed", false).toBool();
    m_config["uniqueinstance"] = m_settings->value("display/unique", true).toBool();
    m_config["qss"] = m_settings->value("display/qss", "none").toString();

    // this part had been commented for Win32, see svn 5882 or git 70eb1793
    // to allow a bit more flexibility, we leave it as a configurable setting,
    // whose default mode will be 'disabled'
#ifdef Q_OS_WIN
    m_config["enableclipboard"] = m_settings->value("display/enableclipboard", false).toBool();
#else
    m_config["enableclipboard"] = m_settings->value("display/enableclipboard", true).toBool();
#endif /* Q_OS_WIN */

    m_config["logfilename"] = "XiVO_Client.log";
    m_config["activate_on_tel"] = m_settings->value("display/activate_on_tel", false).toBool();
    openLogFile ();

    m_config["profilename"] = m_settings->value("profile/lastused").toString();
    m_profilename_write = "engine-" + m_config["profilename"].toString();

    QString settingsversion = m_settings->value("version/xivo", __xivo_version__).toString();

    // this is used to make a migration from 1.0 to 1.1
    if (settingsversion == "1.0")
        m_profilename_read = "engine";
    else
        m_profilename_read = "engine-" + m_config["profilename"].toString();

    m_settings->beginGroup(m_profilename_read);
        m_config["cti_address"] = m_settings->value("serverhost", "demo.xivo.io").toString();
        m_config["cti_port"]    = m_settings->value("serverport", 5003).toUInt();
        m_config["cti_encrypt"] = m_settings->value("encryption", false).toBool();
        m_config["cti_backup_address"] = m_settings->value("backup_server_host", "").toString();
        m_config["cti_backup_port"]    = m_settings->value("backup_server_port", 5003).toUInt();
        m_config["cti_backup_encrypt"] = m_settings->value("backup_server_encryption", false).toBool();

        setUserLogin (m_settings->value("userid").toString(), m_settings->value("useridopt").toString());
        m_config["password"] = m_settings->value("password").toString();
        // keeppass and showagselect are booleans in memory, integers (Qt::checkState) in qsettings/config file (due to compatibility)
        m_config["keeppass"] = (m_settings->value("keeppass", Qt::Unchecked).toUInt() == Qt::Checked);
        m_config["showagselect"] = (m_settings->value("showagselect", Qt::Checked).toUInt() == Qt::Checked);
        m_config["agentphonenumber"] = m_settings->value("agentphonenumber").toString();

        m_config["forcelocale"] = m_settings->value("forcelocale", "default").toString();
        m_config["autoconnect"] = m_settings->value("autoconnect", false).toBool();
        m_config["trytoreconnect"] = m_settings->value("trytoreconnect", false).toBool();
        m_config["trytoreconnectinterval"] = m_settings->value("trytoreconnectinterval", 20*1000).toUInt();
        m_config["keepaliveinterval"] = m_settings->value("keepaliveinterval", 120*1000).toUInt();
        m_availstate = m_settings->value("availstate", "available").toString();
        m_config["displayprofile"] = m_settings->value("displayprofile", false).toBool();
        m_config["dialpanel.history_length"] = m_settings->value("dialpanel/history_length", 10).toInt();

        m_config["switchboard_queue_name"] = m_settings->value("switchboard.queue", "__switchboard").toString();
        m_config["switchboard_hold_queue_name"] = m_settings->value("switchboard.queue_hold", "__switchboard_hold").toString();

        m_config["agent_status_dashboard.main_window_state"] = m_settings->value("agent_status_dashboard.main_window_state");

        m_config["remote_directory_sort_column"] = m_settings->value("remote_directory.sort_column", 0).toInt();
        m_config["remote_directory_sort_order"] = m_settings->value("remote_directory.sort_order", Qt::AscendingOrder).toInt();

        m_settings->beginGroup("user-gui");
            m_config["historysize"] = m_settings->value("historysize", 8).toUInt();
        m_settings->endGroup();
    m_settings->endGroup();

    QByteArray defaultguioptions;
    QFile defaultguioptions_file(":/guioptions.json");
    if (defaultguioptions_file.exists()) {
        defaultguioptions_file.open(QFile::ReadOnly);
        defaultguioptions = defaultguioptions_file.readAll();
        defaultguioptions_file.close();
    }
    QVariant data = parseJson(defaultguioptions);

    // this is used to make a migration from 1.0 to 1.1
    if (settingsversion != "1.0")
        m_settings->beginGroup(m_profilename_read);
            m_settings->beginGroup("user-gui");
                m_config.merge (m_settings->value("guisettings", data).toMap(), "guioptions");
            m_settings->endGroup();
    if (settingsversion != "1.0")
        m_settings->endGroup();

    QMap<QString, bool> enable_function_bydefault;
    foreach (QString function, CheckFunctions)
        enable_function_bydefault[function] = false;
    enable_function_bydefault["presence"] = true;

    m_settings->beginGroup("user-functions");
        foreach (QString function, CheckFunctions)
            m_config["checked_function." + function] =
                    m_settings->value(function,
                                      enable_function_bydefault[function]
                                     ).toBool();
    m_settings->endGroup();
}

QVariant BaseEngine::parseJson(const QByteArray &raw) const
{
    QJsonDocument doc = QJsonDocument::fromJson(raw);
    return doc.toVariant();
}

QByteArray BaseEngine::toJson(const QVariantMap &map) const
{
    return QJsonDocument::fromVariant(map).toJson(QJsonDocument::Compact);
}

/*!
 * Save Settings to the registery/configuration file
 *
 * \todo automatize saving of m_config values
 */
void BaseEngine::saveSettings()
{
    if (m_settings->value("userid").toString() != m_config["userloginsimple"].toString()) {
        m_settings->setValue("monitor/userid", QString(""));
    }

    m_settings->setValue("version/xivo", __xivo_version__);
    m_settings->setValue("version/git_hash", __git_hash__);
    m_settings->setValue("version/git_date", __git_date__);
    m_settings->setValue("display/systrayed", m_config["systrayed"].toBool());
    m_settings->setValue("display/unique", m_config["uniqueinstance"].toBool());
    m_settings->setValue("display/qss", m_config["qss"].toString());
    m_settings->setValue("display/enableclipboard", m_config["enableclipboard"].toBool());
    m_settings->setValue("display/activate_on_tel", m_config["activate_on_tel"].toBool());

    m_settings->beginGroup(m_profilename_write);
        m_settings->setValue("serverhost", m_config["cti_address"].toString());
        m_settings->setValue("serverport", m_config["cti_port"].toUInt());
        m_settings->setValue("encryption", m_config["cti_encrypt"].toBool());
        m_settings->setValue("backup_server_host", m_config["cti_backup_address"].toString());
        m_settings->setValue("backup_server_port", m_config["cti_backup_port"].toUInt());
        m_settings->setValue("backup_server_encryption", m_config["cti_backup_encrypt"].toBool());
        m_settings->setValue("userid", m_config["userloginsimple"].toString());
        m_settings->setValue("useridopt", m_config["userloginopt"].toString());
        // keeppass and showagselect are booleans in memory, but integers (Qt::checkType) in qsettings/config file (due to compatibility)
        m_settings->setValue("keeppass", m_config["keeppass"].toBool() ? Qt::Checked : Qt::Unchecked);
        m_settings->setValue("showagselect", m_config["showagselect"].toBool() ? Qt::Checked : Qt::Unchecked);
        m_settings->setValue("agentphonenumber", m_config["agentphonenumber"].toString());
        m_settings->setValue("forcelocale", m_config["forcelocale"].toString());
        m_settings->setValue("autoconnect", m_config["autoconnect"].toBool());
        m_settings->setValue("trytoreconnect", m_config["trytoreconnect"].toBool());
        m_settings->setValue("trytoreconnectinterval", m_config["trytoreconnectinterval"].toUInt());
        m_settings->setValue("keepaliveinterval", m_config["keepaliveinterval"].toUInt());
        m_settings->setValue("displayprofile", m_config["displayprofile"].toBool());
        m_settings->setValue("dialpanel/history_length", m_config["dialpanel.history_length"].toInt());

        m_settings->setValue("switchboard.queue", m_config["switchboard_queue_name"].toString());
        m_settings->setValue("switchboard.queue_hold", m_config["switchboard_hold_queue_name"].toString());

        m_settings->setValue("agent_status_dashboard.main_window_state", m_config["agent_status_dashboard.main_window_state"]);

        m_settings->setValue("remote_directory.sort_column", m_config["remote_directory_sort_column"]);
        m_settings->setValue("remote_directory.sort_order", m_config["remote_directory_sort_order"]);

        if (m_config["keeppass"].toBool())
            m_settings->setValue("password", m_config["password"].toString());
        else
            m_settings->remove("password");

        m_settings->beginGroup("user-gui");
            m_settings->setValue("historysize", m_config["historysize"].toInt());
            m_settings->setValue("guisettings", m_config.getSubSet("guioptions"));
        m_settings->endGroup();
    m_settings->endGroup();

    m_settings->beginGroup("user-functions");
        foreach (QString function, CheckFunctions)
            m_settings->setValue(function, m_config["checked_function." + function].toBool());
    m_settings->endGroup();

    emit settingsChanged();
}

QVariant BaseEngine::getProfileSetting(const QString & key, const QVariant & bydefault) const
{
    m_settings->beginGroup(m_profilename_read);
        QVariant ret = m_settings->value(key, bydefault);
    m_settings->endGroup();
    return ret;
}

void BaseEngine::setProfileSetting(const QString & key, const QVariant & value)
{
    m_settings->beginGroup(m_profilename_write);
        m_settings->setValue(key, value);
    m_settings->endGroup();
}

void BaseEngine::pasteToDial(const QString & toPaste)
{
    emit pasteToXlets(toPaste);
}

bool BaseEngine::checkedFunction(const QString & function)
{
    return m_config["checked_function." + function].toBool();
}

void BaseEngine::openLogFile()
{
    QString logfilename = m_config["logfilename"].toString();
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

/*! \brief Starts the connection to the server
 * This method starts the login process by connection
 * to the server.
 */
void BaseEngine::startConnection()
{
    qDebug() << "Connecting to" << m_config["cti_address"].toString() << "port" << port_to_use();

    ConnectionConfig connection_config = m_config.getConnectionConfig();
    m_cti_server->connectToServer(connection_config);
}

void BaseEngine::start()
{
    startConnection();
}

void BaseEngine::authenticated()
{
    stopTryAgainTimer();
    emitLogged();
}

void BaseEngine::emitLogged()
{
    if(this->m_state != ELogged) {
        this->m_state = ELogged;
        emit logged();
    }
}

void BaseEngine::authenticate()
{
    stopTryAgainTimer();
    /* do the login/identification */
    m_attempt_loggedin = false;
    QVariantMap command;
    command["class"] = "login_id";
    command["userlogin"] = m_config["userloginsimple"].toString();
    command["company"] = "xivo";
    command["ident"] = m_osname;
    command["version"] = "9999";
    command["xivoversion"] = __cti_protocol_version__;

    // for debuging purposes :
    command["lastlogout-stopper"] = m_settings->value("lastlogout/stopper").toString();
    command["lastlogout-datetime"] = m_settings->value("lastlogout/datetime").toString();
    m_settings->remove("lastlogout/stopper");
    m_settings->remove("lastlogout/datetime");

    sendJsonCommand(command);
}

void BaseEngine::stop()
{
    qDebug() << "Disconnecting";
    stopConnection();
    stopKeepAliveTimer();
    emitDelogged();
    clearInternalData();
}

void BaseEngine::emitDelogged()
{
    if(m_state != ENotLogged) {
        m_state = ENotLogged;
        emit delogged();
    }
}

void BaseEngine::stopConnection()
{
    if (m_attempt_loggedin) {
        QString stopper = sender() ? sender()->property("stopper").toString() : "unknown";
        sendLogout(stopper);
        saveLogoutData(stopper);

        m_attempt_loggedin = false;
    }
    m_cti_server->disconnectFromServer();
}

void BaseEngine::sendLogout(const QString & stopper)
{
    QVariantMap command;
    command["class"] = "logout";
    command["stopper"] = stopper;
    sendJsonCommand(command);
}

void BaseEngine::saveLogoutData(const QString & stopper)
{
    m_settings->setValue("lastlogout/stopper", stopper);
    m_settings->setValue("lastlogout/datetime",
                         QDateTime::currentDateTime().toString(Qt::ISODate));
    m_settings->beginGroup(m_profilename_write);
    m_settings->setValue("availstate", m_availstate);
    m_settings->endGroup();
}

void BaseEngine::clearInternalData()
{
    m_sessionid = "";

    clearLists();
    clearChannelList();

    /* cleaning the registered listeners */
    m_listeners.clear();
}

void BaseEngine::onCTIServerDisconnected()
{
    b_engine->emitMessage(tr("Connection lost with XiVO CTI server"));
    b_engine->startTryAgainTimer();
    this->stop();
}

/*! \brief clear the content of m_users
 *
 * Delete all contained UserInfo objects
 */
void BaseEngine::clearLists()
{
    emit clearingCache();
    foreach (QString listname, m_anylist.keys()) {
        QHashIterator<QString, XInfo *> iter = QHashIterator<QString, XInfo *>(m_anylist.value(listname));
        while (iter.hasNext()) {
            iter.next();
            delete iter.value();
        }
        m_anylist[listname].clear();
    }
}

void BaseEngine::clearChannelList()
{
    QHashIterator<QString, ChannelInfo *> iterc = QHashIterator<QString, ChannelInfo *>(m_channels);
    while (iterc.hasNext()) {
        iterc.next();
        delete iterc.value();
    }
    m_channels.clear();
    QHashIterator<QString, QueueMemberInfo *> iterq = QHashIterator<QString, QueueMemberInfo *>(m_queuemembers);
    while (iterq.hasNext()) {
        iterq.next();
        delete iterq.value();
    }
    m_queuemembers.clear();
}

void BaseEngine::deleteTranslators()
{
    while (! m_translators.isEmpty()) {
        QTranslator * translator_to_delete = m_translators.takeLast();
        delete translator_to_delete;
    }
}

/*! \brief gets m_capaxlets */
const QVariantList & BaseEngine::getCapaXlets() const
{
    return m_capaxlets;
}

const QVariantMap & BaseEngine::getOptionsUserStatus() const
{
    return m_options_userstatus;
}

const QVariantMap & BaseEngine::getOptionsPhoneStatus() const
{
    return m_options_phonestatus;
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
    if (m_availstate != newstate && !comesFromServer) {
        m_availstate = newstate;
        changeState();
    }
}

void BaseEngine::restoreAvailState()
{
    changeState();
    disconnect(m_ctiserversocket, SIGNAL(connected()),
               this, SLOT(restoreAvailState()));
}

/*! \brief send command to XiVO CTI server */
void BaseEngine::sendCommand(const QByteArray &command)
{
    if (m_ctiserversocket->state() == QAbstractSocket::ConnectedState)
        m_ctiserversocket->write(command + "\n");
}

/*! \brief encode json and then send command to XiVO CTI server */
QString BaseEngine::sendJsonCommand(const QVariantMap & cticommand)
{
    if (! cticommand.contains("class"))
        return QString("");
    QVariantMap fullcommand = cticommand;
    fullcommand["commandid"] = qrand();
    QByteArray jsoncommand(toJson(fullcommand));
    sendCommand(jsoncommand);
    return fullcommand["commandid"].toString();
}

/*! \brief send an ipbxcommand command to the cti server */
void BaseEngine::ipbxCommand(const QVariantMap & ipbxcommand)
{
    if (! ipbxcommand.contains("command"))
        return;
    QVariantMap cticommand = ipbxcommand;
    cticommand["class"] = "ipbxcommand";
    sendJsonCommand(cticommand);
}

/*! \brief set monitored peer id */
void BaseEngine::monitorPeerRequest(const QString & xuserid)
{
    if (m_anylist.value("users").contains(xuserid)) {
        m_monitored_xuserid = xuserid;
        emit monitoredUserInfoDefined();
        emit monitorPeerChanged();
        m_settings->setValue("monitor/userid", xuserid);
    }
}


/*! \brief send filetransfer command
 */
void BaseEngine::filetransferSocketConnected()
{
    QVariantMap command;
    command["class"] = "filetransfer";
    command["command"] = "put_announce";
    command["format"] = "base64";
    command["socketref"] = QString("%1:%2")
        .arg(m_filetransfersocket->localAddress().toString())
        .arg(m_filetransfersocket->localPort());
    command["filename"] = m_filename;
    command["fileid"] = m_fileid;
    command["formatted_size"] = m_filedata.size();
    command["file_size"] = m_faxsize;
    sendJsonCommand(command);
}

double BaseEngine::timeDeltaServerClient() const
{
    double delta = m_timeclt.toTime_t() - m_timesrv;
    return delta;
}

QString BaseEngine::timeElapsed(double timestamp) const
{
    QDateTime now = QDateTime::currentDateTime().addSecs(-timeDeltaServerClient());
    int seconds_elapsed = QDateTime::fromTime_t(timestamp).secsTo(now);
    QTime time_elapsed = QTime(0, 0).addSecs(seconds_elapsed);
    if (time_elapsed.hour() == 0) {
        return time_elapsed.toString("mm:ss");
    } else {
        return time_elapsed.toString("hh:mm:ss");
    }
}

const ChannelInfo * BaseEngine::channel(const QString & id) const
{
    return channels().value(id);
}

const UserInfo * BaseEngine::user(const QString & id) const
{
    return static_cast<const UserInfo *> (m_anylist.value("users").value(id));
}

const PhoneInfo * BaseEngine::phone(const QString & id) const
{
    return static_cast<const PhoneInfo *> (m_anylist.value("phones").value(id));
}

const AgentInfo * BaseEngine::agent(const QString & id) const
{
    return static_cast<const AgentInfo *> (m_anylist.value("agents").value(id));
}

const QueueInfo * BaseEngine::queue(const QString & id) const
{
    return static_cast<const QueueInfo *> (m_anylist.value("queues").value(id));
}

const VoiceMailInfo * BaseEngine::voicemail(const QString & id) const
{
    return static_cast<const VoiceMailInfo *> (m_anylist.value("voicemails").value(id));
}

const QueueMemberInfo * BaseEngine::queuemember(const QString & id) const
{
    return static_cast<const QueueMemberInfo *> (m_anylist.value("queuemembers").value(id));
}

void BaseEngine::emitMessage(const QString & msg)
{
    emit emitTextMessage(msg);
}

/*! \brief parse JSON and then process command */
void BaseEngine::parseCommand(const QByteArray &raw)
{
    m_pendingkeepalivemsg = 0;
    QVariant data = parseJson(raw);

    if (data.isNull()) {
        qDebug() << "Invalid json aborting";
        return;
    }

    QVariantMap datamap = data.toMap();
    QString direction = datamap.value("direction").toString();
    QString function = datamap.value("function").toString();
    QString thisclass = datamap.value("class").toString();
    QString replyid = datamap.value("replyid").toString();
    if (datamap.contains("timenow")) {
        m_timesrv = datamap.value("timenow").toDouble();
        m_timeclt = QDateTime::currentDateTime();
    }

    if (forwardToListeners(thisclass, datamap))  // a class callback was called,
        return;                                  // so zap the 500 loc of if-else soup

    if ((thisclass == "keepalive") || (thisclass == "availstate")) {
        // ack from the keepalive and availstate commands previously sent
        return;
    }
    if (thisclass == "sheet") {
        // TODO : use id better than just channel name
        QString channel = datamap.value("channel").toString();

        if (datamap.contains("payload")) {
            QString payload;
            QByteArray qba = QByteArray::fromBase64(datamap.value("payload").toString().toLatin1());
            if (datamap.value("compressed").toBool())
                payload = QString::fromUtf8(qUncompress(qba));
            else
                payload = QString::fromUtf8(qba);
            // will eventually call the XML parser
            emit displayFiche(payload, false, channel);
        }

    } else if (thisclass == "getlist") {
        configsLists(function, datamap);
    } else if (thisclass == "agentlisten") {
        emit statusListen(datamap.value("ipbxid").toString(),
                          datamap.value("agentid").toString(),
                          datamap.value("status").toString());
    } else if (thisclass == "serverdown") {
        qDebug() << Q_FUNC_INFO << thisclass << datamap.value("mode").toString();

    } else if (thisclass == "disconn") {
        qDebug() << Q_FUNC_INFO << thisclass;

    } else if (thisclass == "directory") {
        emit directoryResponse(datamap.value("headers").toStringList(),
                               datamap.value("resultlist").toStringList());

    } else if (thisclass == "faxsend") {
        if (datamap.contains("step"))
            qDebug() << Q_FUNC_INFO << "step" << datamap.value("step").toString();
        else {
            m_fileid = datamap.value("fileid").toString();
            m_filetransfersocket->connectToHost(m_config["cti_address"].toString(), port_to_use());
        }

    } else if (thisclass == "filetransfer") {
        qint64 written = m_filetransfersocket->write(m_filedata + "\n");
        qDebug() << Q_FUNC_INFO << written << datamap;
        m_filetransfersocket->flush();
        m_filetransfersocket->disconnectFromHost();
        m_filedata.clear();

    } else if (thisclass == "faxprogress") {
        emit ackFax(datamap.value("status").toString(), datamap.value("reason").toString());

    } else if (thisclass == "filelist") {
        emit serverFileList(datamap.value("filelist").toStringList());

    } else if (thisclass == "presence") {
        QString id = datamap.value("astid").toString() + "/" + datamap.value("xivo_userid").toString();
        if (m_anylist.value("users").contains(id)) {
            QVariantMap state = datamap.value("capapresence").toMap().value("state").toMap();
            QString stateid = state.value("stateid").toString();
            QVariantMap changeme = m_config["guioptions.presence.autochangestate"].toMap();
            if (changeme.count() && (id == m_xuserid)) {
                if (changeme.contains(stateid)) {
                    QVariantMap changemeconf = changeme[stateid].toMap();
                    m_timerid_changestate = startTimer(changemeconf.value("delaymsec").toInt());
                    m_changestate_newstate = changemeconf.value("newstate").toString();
                    m_changestate_oldstate = stateid;
                }
            }
            UserInfo * ui = (UserInfo *) m_anylist.value("users").value(id);
            ui->setAvailState(stateid);
            if (id == m_xuserid) {
                setAvailState(stateid, true);
                emit updatePresence();
            }
        }

    } else if (thisclass == "featuresput") {
        QString featuresput_status = datamap.value("status").toString();
        if (featuresput_status != "OK") {
            emit servicePutIsKO();
            emit emitTextMessage(tr("Could not modify the Services data.") + " " + tr("Maybe Asterisk is down."));
        } else {
            emit servicePutIsOK(datamap.value("replyid").toString(),
                                datamap.value("warning_string").toString());
            emit emitTextMessage("");
        }
    } else if (thisclass == "ipbxcommand" && datamap.contains("error_string")) {
        popupError(datamap.value("error_string").toString());
    } else if (thisclass == "login_id") {
        if (datamap.contains("error_string")) {
            stop();
            popupError(datamap.value("error_string").toString());
        } else {
            m_sessionid = datamap.value("sessionid").toString();
            QString tohash = QString("%1:%2").arg(m_sessionid).arg(m_config["password"].toString());
            QCryptographicHash hidepass(QCryptographicHash::Sha1);
            QByteArray res = hidepass.hash(tohash.toLatin1(), QCryptographicHash::Sha1).toHex();
            QVariantMap command;
            command["class"] = "login_pass";
            command["hashedpassword"] = QString(res);
            sendJsonCommand(command);
        }
    } else if (thisclass == "login_pass") {
        if (datamap.contains("error_string")) {
            stop();
            popupError(datamap.value("error_string").toString());
        } else {
            QStringList capas = datamap.value("capalist").toStringList();
            QVariantMap command;
            command["class"] = "login_capas";
            if (capas.size() == 1)
                command["capaid"] = capas[0];
            else if (capas.size() == 0) {
                command["capaid"] = "";
            } else {
                if (m_config["userloginopt"].toString().size() > 0) {
                    if (capas.contains(m_config["userloginopt"].toString()))
                        command["capaid"] = m_config["userloginopt"].toString();
                    else
                        command["capaid"] = capas[0];
                } else
                    command["capaid"] = capas[0];
            }

            switch(m_config["guioptions.loginkind"].toInt()) {
            case 0:
                command["loginkind"] = "user";
                break;
            case 2:
                command["agentlogin"] = "now";
            case 1:
                command["loginkind"] = "agent";
                command["agentphonenumber"] = m_config["agentphonenumber"].toString();
                break;
            }

            command["state"] = getInitialPresence();

            /*!
             * \todo To be deleted, when the server will accept it
             */
            command["lastconnwins"] = false;

            sendJsonCommand(command);
        }

    } else if (thisclass == "login_capas") {
        m_ipbxid = datamap.value("ipbxid").toString();
        m_userid = datamap.value("userid").toString();
        m_xuserid = QString("%1/%2").arg(m_ipbxid).arg(m_userid);

        m_appliname = datamap.value("appliname").toString();
        m_capaxlets = datamap.value("capaxlets").toList();

        QVariantMap capas = datamap.value("capas").toMap();
        m_options_userstatus = capas.value("userstatus").toMap();
        m_options_phonestatus = capas.value("phonestatus").toMap();

        // ("ipbxcommands", "regcommands", "services", "functions")
        m_config.merge(capas.value("preferences").toMap());
        m_config["services"] = capas.value("services");

        QVariantMap tmp;
        QStringList todisp;
        m_config["checked_function.switchboard"] = true;
        tmp["functions"] = todisp;
        m_config["guioptions.server_funcs"] = tmp;

        QString urltolaunch = m_config["guioptions.loginwindow.url"].toString();
        if (! urltolaunch.isEmpty()) {
            urltolaunch.replace("{xc-username}", m_config["userloginsimple"].toString());
            urltolaunch.replace("{xc-password}", m_config["password"].toString());
            this->urlAuto(urltolaunch);
        }

        fetchIPBXList();
        this->authenticated();
        m_timerid_keepalive = startTimer(m_config["keepaliveinterval"].toUInt());
        m_attempt_loggedin = true;

    } else if (thisclass == "disconnect") {
        qDebug() << thisclass << datamap;
        QString type = datamap.value("type").toString();
        stop();
        if (type=="force") {
            m_forced_to_disconnect = true; // disable autoreconnect
            popupError("forcedisconnected");
        } else {
            popupError("disconnected");
        }
    } else if (thisclass == "getipbxlist") {
        m_ipbxlist = datamap.value("ipbxlist").toStringList();
        fetchLists();
    } else if (thisclass == "queueentryupdate") {
        const QVariantMap &state = datamap.value("state").toMap();
        const QString &queue_id = state["queue_id"].toString();
        const QVariantList &entry_list = state["entries"].toList();

        emit queueEntryUpdate(queue_id, entry_list);
    } else if (thisclass == "meetme_update") {
        emit meetmeUpdate(datamap.value("config").toMap());
    } else if (thisclass == "meetme_user") {
        m_meetme_membership = datamap["list"].toList();
    } else {
        if (replyid.isEmpty())
            qDebug() << "Unknown server command received:" << thisclass << datamap;
    }
}

bool BaseEngine::isMeetmeMember(const QString &room, int number) const
{
    foreach (const QVariant &item, m_meetme_membership) {
        const QVariantMap &map = item.toMap();
        if (map["room_number"].toString() == room && map["user_number"].toInt() == number)
            return true;
    }
    return false;
}

void BaseEngine::handleGetlistListId(const QString &listname, const QString &ipbxid, const QStringList &listid)
{
    if (! GenLists.contains(listname)) {
        return;
    }
    m_init_watcher.watchList(listname, listid);
    if (! m_anylist.contains(listname))
        m_anylist[listname].clear();

    this->addConfigs(listname, ipbxid, listid);
}

void BaseEngine::addConfigs(const QString &listname, const QString &ipbxid, const QStringList &listid)
{
    if (! GenLists.contains(listname)) {
        return;
    }
    foreach (const QString &id, listid) {
        QString xid = QString("%1/%2").arg(ipbxid).arg(id);
        if (! m_anylist[listname].contains(xid)) {
            newXInfoProto construct = m_xinfoList.value(listname);
            XInfo * xinfo = construct(ipbxid, id);
            m_anylist[listname][xid] = xinfo;
        }
    }
}

void BaseEngine::handleGetlistDelConfig(const QString &listname, const QString &ipbxid, const QStringList &listid)
{
    // Pre delete actions
    foreach (const QString &id, listid) {
        QString xid = QString("%1/%2").arg(ipbxid).arg(id);
        if (listname == "phones")
            emit removePhoneConfig(xid);
        else if (listname == "users")
            emit removeUserConfig(xid);
        else if (listname == "agents")
            emit removeAgentConfig(xid);
        else if (listname == "queues")
            emit removeQueueConfig(xid);
        else if (listname == "queuemembers")
            emit removeQueueMemberConfig(xid);
    }

    // Delete
    foreach (const QString &id, listid) {
        QString xid = QString("%1/%2").arg(ipbxid).arg(id);
        if (GenLists.contains(listname)) {
            if (m_anylist.value(listname).contains(xid)) {
                delete m_anylist[listname][xid];
                m_anylist[listname].remove(xid);
            }
        } else if (listname == "channels") {
            if (m_channels.contains(xid)) {
                delete m_channels[xid];
                m_channels.remove(xid);
            }
        }
        if (listname == "queuemembers") {
            if (m_queuemembers.contains(xid)) {
                delete m_queuemembers[xid];
                m_queuemembers.remove(xid);
            }
        }
    }

    // Post delete
    foreach (const QString &id, listid) {
        QString xid = QString("%1/%2").arg(ipbxid).arg(id);
        if (listname == "queuemembers") {
            emit postRemoveQueueMemberConfig(xid);
        } else if (listname == "queues") {
            emit postRemoveQueueConfig(xid);
        }
    }
}

void BaseEngine::handleGetlistUpdateConfig(
    const QString &listname,
    const QString &ipbxid,
    const QString &id,
    const QVariantMap &data)
{
    QString xid = QString("%1/%2").arg(ipbxid).arg(id);
    QVariantMap config = data.value("config").toMap();
    if (GenLists.contains(listname)) {
        if (! m_anylist.value(listname).contains(xid)) {
            newXInfoProto construct = m_xinfoList.value(listname);
            XInfo * xinfo = construct(ipbxid, id);
            m_anylist[listname][xid] = xinfo;
        }
        if (m_anylist.value(listname).value(xid) != NULL) {
            m_anylist.value(listname)[xid]->updateConfig(config);
        } else {
            qDebug() << "received updateconfig for inexisting" << listname << xid;
        }
        if ((xid == m_xuserid) && (listname == "users")) {
            emit localUserInfoDefined();
        }
    } else {
        qDebug() << "received updateconfig for unknown list" << listname << "id" << xid;
    }

    if (listname == "phones") {
        emit peersReceived();
    }

    // transmission to xlets
    if (listname == "users") {
        emit updateUserConfig(xid);
        emit updateUserConfig(xid,data);
    } else if (listname == "phones")
        emit updatePhoneConfig(xid);
    else if (listname == "agents")
        emit updateAgentConfig(xid);
    else if (listname == "queues")
        emit updateQueueConfig(xid);
    else if (listname == "voicemails")
        emit updateVoiceMailConfig(xid);
    else if (listname == "queuemembers")
        emit updateQueueMemberConfig(xid);
}

void BaseEngine::handleGetlistUpdateStatus(
    const QString &listname,
    const QString &ipbxid,
    const QString &id,
    const QVariantMap &status)
{
    QString xid = QString("%1/%2").arg(ipbxid).arg(id);

    m_init_watcher.sawItem(listname, id);

    if (GenLists.contains(listname)) {
        if (m_anylist.value(listname).contains(xid))
            m_anylist.value(listname).value(xid)->updateStatus(status);
    } else if (listname == "channels") {
        if (! m_channels.contains(xid))
            m_channels[xid] = new ChannelInfo(ipbxid, id);
        m_channels[xid]->updateStatus(status);
    }
    if (listname == "queuemembers") {
        if (! m_queuemembers.contains(xid))
            m_queuemembers[xid] = new QueueMemberInfo(ipbxid, id);
        m_queuemembers[xid]->updateStatus(status);
    }

    if (listname == "users") {
        setAvailState(status["availstate"].toString(), true);
        emit updateUserStatus(xid);
    } else if (listname == "phones") {
        emit updatePhoneStatus(xid);
        if (hasPhone(xid)) {
            foreach (QString cid, phone(xid)->channels()) {
                this->requestStatus("channels", ipbxid, cid);
            }
        }
    } else if (listname == "agents")
        emit updateAgentStatus(xid);
    else if (listname == "queues") {
        emit updateQueueStatus(xid);
    }
    else if (listname == "voicemails")
        emit updateVoiceMailStatus(xid);
    else if (listname == "channels")
        emit updateChannelStatus(xid);
}

void BaseEngine::requestListConfig(const QString &listname, const QString &ipbxid, const QStringList &listid)
{
    QVariantMap command;
    command["class"] = "getlist";
    command["function"] = "updateconfig";
    command["listname"] = listname;
    command["tipbxid"] = ipbxid;
    foreach (const QString &id, listid) {
        command["tid"] = id;
        sendJsonCommand(command);
    }
}

void BaseEngine::requestStatus(const QString &listname, const QString &ipbxid, const QString &id)
{
    QVariantMap command;
    command["class"] = "getlist";
    command["function"] = "updatestatus";
    command["listname"] = listname;
    command["tipbxid"] = ipbxid;
    command["tid"] = id;
    sendJsonCommand(command);
}



void BaseEngine::configsLists(const QString & function, const QVariantMap & datamap)
{
    QString listname = datamap.value("listname").toString();
    QString ipbxid = datamap.value("tipbxid").toString();

    if (function == "listid") {
        QStringList listid = datamap.value("list").toStringList();
        this->handleGetlistListId(listname, ipbxid, listid);
        this->requestListConfig(listname, ipbxid, listid);
    } else if (function == "delconfig") {
        QStringList listid = datamap.value("list").toStringList();
        this->handleGetlistDelConfig(listname, ipbxid, listid);
    } else if (function == "updateconfig") {
        QString id = datamap.value("tid").toString();
        this->handleGetlistUpdateConfig(listname, ipbxid, id, datamap);
        this->requestStatus(listname, ipbxid, id);
    } else if (function == "updatestatus") {
        QString id = datamap.value("tid").toString();
        QVariantMap status = datamap.value("status").toMap();
        this->handleGetlistUpdateStatus(listname, ipbxid, id, status);
    } else if (function == "addconfig") {
        QStringList listid = datamap.value("list").toStringList();
        this->addConfigs(listname, ipbxid, listid);
        this->requestListConfig(listname, ipbxid, listid);
    }
}


/*! \brief send meetme command to the CTI server */
void BaseEngine::meetmeAction(const QString &function, const QString &functionargs)
{
    QVariantMap command;
    command["command"] = "meetme";
    command["function"] = function;
    command["functionargs"] = functionargs.split(" ");
    ipbxCommand(command);
}

/*! \brief Send fax to CTI Server */
void BaseEngine::sendFaxCommand(const QString & filename,
                                const QString & number,
                                Qt::CheckState hide)
{
    m_filename = filename;
    QFile * qf = new QFile(filename);
    bool canopen = qf->open(QIODevice::ReadOnly);

    if (canopen) {
        QByteArray truefiledata = QByteArray();
        truefiledata.append(qf->readAll());
        m_faxsize = truefiledata.size();

        if (m_faxsize > 0) {
            m_filedata = truefiledata.toBase64();
            QVariantMap command;
            command["class"] = "faxsend";
            command["hide"] = QString::number(hide);
            command["filename"] = filename;
            command["destination"] = number;
            sendJsonCommand(command);
        } else
            emit ackFax("ko", "fileempty");
    } else
        emit ackFax("ko", "filenotfound");

    qf->close();
    delete qf;
}

/*! \brief select message and then display a messagebox
 *
 * TODO : replace string errorids by an enum ?
 */
void BaseEngine::popupError(const QString & errorid,
                            const QString & server_address,
                            const QString & server_port)
{
    QString errormsg = QString(tr("Server has sent an Error."));

    // errors sent by the server (login phase)
    if (errorid.toLower() == "user_not_found") {
        errormsg = tr("Your registration name <%1@%2> "
                      "is not known by the XiVO CTI server on %3:%4.")
            .arg(server_address).arg(server_port);
    } else if (errorid.toLower() == "login_password") {
        errormsg = tr("You entered a wrong login / password.");
    } else if (errorid.startsWith("capaid_undefined")) {
        errormsg = tr("You have no profile defined.");

    // keepalive (internal)
    } else if (errorid.toLower() == "no_keepalive_from_server") {
        errormsg = tr("The XiVO CTI server on %1:%2 did not reply to the last keepalive.")
            .arg(server_address).arg(server_port);

    // socket errors - while attempting to connect
    } else if (errorid.toLower() == "socket_error_hostnotfound") {
        errormsg = tr("You defined an IP address %1 that is probably an unresolved host name.")
            .arg(server_address);
    } else if (errorid.toLower() == "socket_error_timeout") {
        errormsg = tr("Socket timeout (~ 60 s) : you probably attempted to reach, "
                      "via a gateway, an IP address %1 that does not exist.")
            .arg(server_address);
    } else if (errorid.toLower() == "socket_error_connectionrefused") {
        errormsg = tr("There seems to be a machine running on this IP address %1, "
                      "and either no CTI server is running, or your port %2 is wrong.")
            .arg(server_address).arg(server_port);
    } else if (errorid.toLower() == "socket_error_network") {
        errormsg = tr("An error occurred on the network while attempting to join the IP address %1 :\n"
                      "- no external route defined to access this IP address (~ no timeout)\n"
                      "- this IP address is routed but there is no machine (~ 5 s timeout)\n"
                      "- a cable has been unplugged on your LAN on the way to this IP address (~ 30 s timeout).")
            .arg(server_address);
    } else if (errorid.toLower() == "socket_error_sslhandshake") {
        errormsg = tr("It seems that the server with IP address %1 does not accept encryption on "
                      "its port %2. Please change either your port or your encryption setting.")
            .arg(server_address).arg(server_port);
    } else if (errorid.toLower() == "socket_error_unknown") {
        errormsg = tr("An unknown socket error has occured while attempting to join the IP address:port %1:%2.")
            .arg(server_address).arg(server_port);
    } else if (errorid.startsWith("socket_error_unmanagedyet:")) {
        QStringList ipinfo = errorid.split(":");
        errormsg = tr("An unmanaged (number %1) socket error has occured while attempting to join the IP address:port %1:%2.")
            .arg(ipinfo[1]).arg(server_address).arg(server_port);

    // socket errors - once connected
    } else if (errorid.toLower() == "socket_error_remotehostclosed") {
        errormsg = tr("The XiVO CTI server on %1:%2 has just closed the connection.")
            .arg(server_address).arg(server_port);

    } else if (errorid.toLower() == "server_stopped") {
        errormsg = tr("The XiVO CTI server on %1:%2 has just been stopped.")
            .arg(server_address).arg(server_port);
    } else if (errorid.toLower() == "server_reloaded") {
        errormsg = tr("The XiVO CTI server on %1:%2 has just been reloaded.")
            .arg(server_address).arg(server_port);
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
                .arg(__cti_protocol_version__)
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
    } else if (errorid == "agent_login_invalid_exten") {
        errormsg = tr("Could not log agent: invalid extension.");
    } else if (errorid == "agent_login_exten_in_use") {
        errormsg = tr("Could not log agent: extension already in use.");
    } else if (errorid.startsWith("unreachable_extension:")) {
        QString extension = errorid.split(":")[1];
        errormsg = tr("Unreachable number: %1").arg(extension);
    }

    // logs a message before sending any popup that would block
    emit emitTextMessage(tr("ERROR") + " : " + errormsg);
    if (!m_config["trytoreconnect"].toBool() || m_forced_to_disconnect)
        emit emitMessageBox(errormsg);
}

/*! \brief save BaseEngine::m_downloaded to a file
 *  \sa BaseEngine::m_downloaded
 */
void BaseEngine::saveToFile(const QString & filename)
{
    qDebug() << "Saving downloaded file" << filename << "size" << m_downloaded.size();
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
    while (m_ctiserversocket->canReadLine()) {
        QByteArray data  = m_ctiserversocket->readLine();
        QString line = QString::fromUtf8(data);

        if (line.startsWith("<ui version=")) {
            // we get here when receiving a sheet as a Qt4 .ui form
            qDebug() << "Incoming sheet, size:" << line.size();
            emit displayFiche(line, true, QString());
        } else {
            data.chop(1);  // remove the \n the the end of the json
            parseCommand(data);
        }
    }
}

/*! \brief read data from the file transfer socket
 *
 * Read text data, the file is encapsulated into JSON as a base 64 string.
 */
void BaseEngine::filetransferSocketReadyRead()
{
    while (m_filetransfersocket->canReadLine()) {
        QByteArray data = m_filetransfersocket->readLine();
        QVariantMap jsondatamap = this->parseJson(data).toMap();
        if (jsondatamap.value("class").toString() == "fileref") {
            if (m_filedir == "download") {
                m_downloaded = QByteArray::fromBase64(jsondatamap.value("payload").toByteArray());
                qDebug() << jsondatamap.value("filename").toString() << m_downloaded.size();
                emit fileReceived();
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
    QVariantMap ipbxcommand;
    ipbxcommand["command"] = action;

    if ((action == "originate") || (action == "transfer") || (action == "atxfer")) {
        ipbxcommand["command"] = action;
        ipbxcommand["source"] = src;
        if ((dst == "ext:special:dialxlet") && (! m_numbertodial.isEmpty()))
            ipbxcommand["destination"] = QString("exten:%1/%2").arg(m_ipbxid).arg(m_numbertodial);
        else
            ipbxcommand["destination"] = dst;
    } else if ((action == "hangup") || (action == "transfercancel")) {
        ipbxcommand["command"] = action;
        ipbxcommand["channelids"] = src;
    } else if (action == "answer") {
        ipbxcommand["command"] = action;
        ipbxcommand["phoneids"] = src;
    } else if (action == "refuse") {
        ipbxcommand["command"] = action;
        ipbxcommand["channelids"] = src;
    } else if (action == "intercept") {
        ipbxcommand["tointercept"] = dst;
        ipbxcommand["catcher"] = src;
    }

    ipbxCommand(ipbxcommand);
}

void BaseEngine::actionDial(const QString &destination)
{
    this->sendJsonCommand(MessageFactory::dial(destination));
}

/*! \brief Receive a number list from xlets and signal this list
 *  to interested xlets
 */
void BaseEngine::receiveNumberSelection(const QStringList & numbers)
{
    emit broadcastNumberSelection(numbers);
}

/*!
 * \return all settings
 */
QVariantMap BaseEngine::getConfig() const
{
    return m_config.toQVariantMap();
}

/*!
 * \return the setting indexed by the parameter
 */
QVariant BaseEngine::getConfig(const QString & setting) const
{
    return m_config[setting];
}

// only used to simplify the writing of one config setting
// use setConfig(const QVariantMap & qvm) if there are multiple values to change
void BaseEngine::setConfig(const QString & setting, QVariant value) {
    QVariantMap qvm;
    qvm[setting] = value;
    setConfig(qvm);
}

// qvm may not contain every key, only the ones that need to be modified
void BaseEngine::setConfig(const QVariantMap & qvm)
{
    bool reload_tryagain = qvm.contains("trytoreconnectinterval") &&
                           m_config["trytoreconnectinterval"].toUInt() != qvm["trytoreconnectinterval"].toUInt();
    bool reload_keepalive = qvm.contains("keepaliveinterval") &&
                            m_config["keepaliveinterval"].toUInt() != qvm["keepaliveinterval"].toUInt();
    bool toggle_presence_enabled = qvm.contains("checked_function.presence") &&
                            m_config["checked_function.presence"].toBool() != qvm["checked_function.presence"].toBool();

    m_config.merge(qvm);

    if (reload_tryagain) {
        stopTryAgainTimer();
        startTryAgainTimer();
    }
    if (reload_keepalive) {
        stopKeepAliveTimer();
        m_timerid_keepalive = startTimer(m_config["keepaliveinterval"].toUInt());
    }

    setUserLogin(m_config["userlogin"].toString());

    if (toggle_presence_enabled) {
        if (m_config["checked_function.presence"].toBool()) {
            setAvailState(__presence_on__, false);
        } else {
            setAvailState(__presence_off__, false);
        }
    }

    saveSettings();
}

// === Getter and Setters ===

void BaseEngine::setUserLogin(const QString & userlogin)
{
    m_config["userlogin"] = userlogin.trimmed();
    QStringList userloginsplit = userlogin.split("%");
    m_config["userloginsimple"] = userloginsplit[0].trimmed();
    if (userloginsplit.size() > 1) {
        m_config["userloginopt"] = userloginsplit[1].trimmed();
    } else {
        m_config["userloginopt"] = "";
    }
}

void BaseEngine::setUserLogin(const QString & userid, const QString & opt)
{
    m_config["userloginsimple"] = userid.trimmed();
    m_config["userloginopt"] = opt.trimmed();
    if (m_config["userloginopt"].toString().isEmpty()) {
        m_config["userlogin"] = m_config["userloginsimple"].toString();
    } else {
        m_config["userlogin"] = m_config["userloginsimple"].toString()
                                + "%" + m_config["userloginopt"].toString();
    }
}

uint BaseEngine::port_to_use() const
{
    if (! m_config["cti_encrypt"].toBool()) {
        return m_config["cti_port"].toUInt();
    } else {
        return m_config["cti_port_encrypted"].toUInt();
    }
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
    if (m_timerid_tryreconnect == 0 && m_config["trytoreconnect"].toBool() && ! m_forced_to_disconnect)
        m_timerid_tryreconnect = startTimer(m_config["trytoreconnectinterval"].toUInt());
}

void BaseEngine::timerEvent(QTimerEvent *event)
{
    int timerId = event->timerId();
    if (timerId == m_timerid_keepalive) {
        keepLoginAlive();
    } else if (timerId == m_timerid_tryreconnect) {
        emit emitTextMessage(tr("Attempting to reconnect to server"));
        connect(m_ctiserversocket, SIGNAL(connected()),
                this, SLOT(restoreAvailState()));
        start();
    } else if (timerId == m_timerid_changestate) {
        if (m_availstate == m_changestate_oldstate)
            setAvailState(m_changestate_newstate, false);
        killTimer(timerId);
        m_timerid_changestate = 0;
    } else {
        qDebug() << "Removing unused timer:" << timerId;
        killTimer(timerId);
    }
}

/*! \brief send a feature put command to the cti server */
void BaseEngine::servicePutOpt(const QString &capa, bool b)
{
    QVariantMap command;
    command["class"] = "featuresput";
    if (capa == "enablevoicemail")
        command["function"] = "enablevoicemail";
    else if (capa == "incallfilter")
        command["function"] = "incallfilter";
    else if (capa == "enablednd")
        command["function"] = "enablednd";
    command["value"] = b;
    sendJsonCommand(command);
}

/*! \brief send a feature put command to the cti server
 *
 * NOTE: we send value (forward target number) BEFORE status (enabled/disabled)
 *       to prevent server disabling back forward if value was empty
 */
QString BaseEngine::servicePutForward(const QString & capa, bool b, const QString & dst)
{
    QVariantMap command, value;
    command["class"]    = "featuresput";
    command["function"] = "fwd";

    value["enable"+capa.mid(3)] = b;
    value["dest"+capa.mid(3)]   = dst;
    command["value"]            = value;
    return sendJsonCommand(command);
}

void BaseEngine::fetchIPBXList() {
    QVariantMap command;
    command["class"] = "getipbxlist";
    sendJsonCommand(command);
}

/*! \brief send a lot of getlist commands to the CTI server
 *
 * send getlist for "users", "queues", "agents", "phones",
 * "users", "endinit"
 */
void BaseEngine::fetchLists()
{
    QVariantMap command;

    command["class"] = "getlist";
    command["function"] = "updateconfig";
    command["listname"] = "users";
    command["tipbxid"] = m_ipbxid;
    command["tid"] = m_userid;
    sendJsonCommand(command);

    command.clear();
    command["class"] = "getlist";
    command["function"] = "listid";
    QStringList getlists;
    getlists = GenLists;

    foreach (QString ipbxid, m_ipbxlist) {
        command["tipbxid"] = ipbxid;
        foreach (QString kind, getlists) {
            command["listname"] = kind;
            sendJsonCommand(command);
        }
    }

    if (m_config["guioptions.loginkind"].toInt() == 2) {
        QVariantMap ipbxcommand;
        ipbxcommand["command"] = "agentlogin";
        ipbxcommand["agentids"] = "agent:special:me";
        ipbxcommand["agentphonenumber"] = m_config["agentphonenumber"].toString();
        ipbxCommand(ipbxcommand);
    }
}

void BaseEngine::inviteConfRoom(const QString &invitee)
{
    QVariantMap command;
    command["class"] = "invite_confroom";
    command["invitee"] = invitee;
    sendJsonCommand(command);
}

BaseEngine::EngineState BaseEngine::state()
{
    return m_state;
}

void BaseEngine::changeWatchedAgent(const QString & agentid, bool force)
{
    if ((force || (agentid.size() > 0)) && (hasAgent(agentid))) {
        emit changeWatchedAgentSignal(agentid);
    }
}

void BaseEngine::changeWatchedQueue(const QString & queueid)
{
    emit changeWatchedQueueSignal(queueid);
}

/*! \brief sets m_osname
 *
 * also builds a string defining who is the client (osname)
 */
void BaseEngine::setOSInfos(const QString & osname)
{
    m_osname = osname;
}

// method to provide the list of phonenumbers of a given xuserid
QStringList BaseEngine::phonenumbers(const UserInfo * userinfo)
{
    QStringList phonenumbers;
    if (userinfo != NULL) {
        foreach (QString xphoneid, userinfo->phonelist()) {
            const PhoneInfo * phoneinfo = phone(xphoneid);
            if (phoneinfo == NULL)
                continue;
            QString phonenumber = phoneinfo->number();
            if ((! phonenumber.isEmpty()) && (! phonenumbers.contains(phonenumber)))
                phonenumbers.append(phonenumber);
        }
    }
    return phonenumbers;
}

void BaseEngine::keepLoginAlive()
{
    if (m_pendingkeepalivemsg > 0) {
        disconnectNoKeepAlive();
    } else {
        sendKeepAliveMsg();
    }
}

void BaseEngine::disconnectNoKeepAlive()
{
    stop();
    popupError("no_keepalive_from_server");
    m_pendingkeepalivemsg = 0;
    startTryAgainTimer();
}

void BaseEngine::sendKeepAliveMsg()
{
    QVariantMap command;
    command["class"] = "keepalive";
    ++m_pendingkeepalivemsg;
    sendJsonCommand(command);
}

/*! \brief send m_availstate to CTI server */
void BaseEngine::changeState()
{
    QVariantMap command;
    command["class"] = "availstate";
    command["availstate"] = m_availstate;
    command["ipbxid"] = m_ipbxid;
    command["userid"] = m_userid;
    sendJsonCommand(command);
}

QString BaseEngine::getInitialPresence() const
{
    if (m_config["checked_function.presence"].toBool()) {
        QString state = m_availstate;
        if (state.isEmpty() || state == __presence_off__)
            state = __presence_on__;
        return state;
    }
    return __presence_off__;
}

/*! \brief get pointer to the currently logged user
 *
 * Return NULL if not available */
UserInfo * BaseEngine::getXivoClientUser()
{
    if (m_anylist.value("users").contains(m_xuserid)) {
        return (UserInfo *) m_anylist.value("users").value(m_xuserid);
    }
    return NULL;
}

UserInfo * BaseEngine::getXivoClientMonitored()
{
    if (m_anylist.value("users").contains(m_monitored_xuserid)) {
        return (UserInfo *) m_anylist.value("users").value(m_monitored_xuserid);
    }
    return NULL;
}

int BaseEngine::forwardToListeners(QString event_dest, const QVariantMap & map)
{
    if (m_listeners.contains(event_dest)) {
        foreach (IPBXListener *listener, m_listeners.values(event_dest)) {
            listener->parseCommand(map);
        }
        return true ;
    } else {
        return false;
    }
}

void BaseEngine::registerListener(const QString & event_to_listen, IPBXListener *xlet)
{
    m_listeners.insert(event_to_listen, xlet);
}

void BaseEngine::registerTranslation(const QString &path)
{
    QString translation_file = path.arg(m_locale);
    QTranslator *translator = this->createTranslator(translation_file);
    m_translators.append(translator);
}

void BaseEngine::setupTranslation()
{
    m_locale = m_config["forcelocale"].toString();

    if (m_locale == "default") {
        m_locale = QLocale::system().name();
    }

    QStringList translation_files = QStringList()
        << QString(":/obj/xivoclient_%1").arg(m_locale)
        << QString(":/obj/baselib_%1").arg(m_locale)
        << QString(":/obj/xletlib_%1").arg(m_locale)
        << QString("%1/qt_%2").arg(QLibraryInfo::location(QLibraryInfo::TranslationsPath), m_locale);

    foreach(QString translation_file, translation_files) {
        if (m_locale != "en_US") {
            QTranslator * new_translator = this->createTranslator(translation_file);
            m_translators.append(new_translator);
        }
    }
}

QTranslator * BaseEngine::createTranslator(const QString & translation_file)
{
    QTranslator * new_translator = new QTranslator;
    new_translator->load(translation_file);
    qApp->installTranslator(new_translator);
    return new_translator;
}

void BaseEngine::sheetSocketConnected()
{
    QString kind = sender()->property("kind").toString();
    QString payload = sender()->property("payload").toString();
    if (kind == "tcpsheet") {
        m_tcpsheetsocket->write(payload.toUtf8() + "\n");
        m_tcpsheetsocket->flush();
        m_tcpsheetsocket->disconnectFromHost();
    }
}

void BaseEngine::urlAuto(const QString & value)
{
    // a list of URI schemes is available there : http://www.iana.org/assignments/uri-schemes.html
    // 'udp' and 'tcp' do not belong to it, however we'll use them ...

    QUrl url(value);

    if ((url.scheme() == "tcp") || (url.scheme() == "udp")) {
        // ability to send a string to any TCP or UDP socket
        // tcp://host:port/?var1=val1&var2=val2
        // udp://host:port/?var1=val1&var2=val2
        // "var1=val1&var2=val2" will be sent through TCP or UDP to host:port
        QString reserialize = url.path();
        QStringList tosend;
        // the reserialize is intended to enable some choice among serialization methods
        // one could for instance send the pairs into json or whatever ...
        if (reserialize == "/") {
            QUrlQuery query(url);
            QPair<QString, QString> pair;
            foreach(pair, query.queryItems())
                tosend.append(QString("%1=%2").arg(pair.first).arg(pair.second));
        }

        if (tosend.length() > 0) {
            if (url.scheme() == "tcp") {
                m_tcpsheetsocket = new QTcpSocket(this);
                m_tcpsheetsocket->setProperty("kind", "tcpsheet");
                m_tcpsheetsocket->setProperty("payload", tosend.join("&"));
                connect(m_tcpsheetsocket, SIGNAL(connected()),
                        this, SLOT(sheetSocketConnected()));
                m_tcpsheetsocket->connectToHost(QHostAddress(url.host()), quint16(url.port()));
            } else if (url.scheme() == "udp") {
                m_udpsheetsocket = new QUdpSocket(this);
                m_udpsheetsocket->writeDatagram(tosend.join("&").toUtf8() + "\n",
                                                QHostAddress(url.host()),
                                                quint16(url.port()));
            }
        }

    } else if (url.scheme().isEmpty())
        // Launch a given application, with arguments.
        // Beware in Windows cases, where there might be a C: appearing in the path ...
        // Actually, a similar behaviour could be expected by defining a scheme for any
        // given application, and letting the OS underneath launch this application.
        QProcess::startDetached(value);

    else {
        // rely on the system's url opening methods (see xdg-open on linux, for instance)
#ifdef Q_OS_WIN
        // in win32 case + iexplore.exe, this should ensure it opens a new tab
        QString key = QString("HKEY_CLASSES_ROOT\\%1\\shell\\open\\command").arg(url.scheme());
        QSettings settings(key, QSettings::NativeFormat);
        QString command = settings.value(".").toString();
        QRegExp rx("\"(.+)\"");
        if (rx.indexIn(command) != -1)
            command = rx.capturedTexts()[1];
        QFileInfo browserFileInfo(command);
        if (browserFileInfo.fileName() == "iexplore.exe") {
            QProcess::startDetached(browserFileInfo.absoluteFilePath(), QStringList() << "-new" << url.toEncoded());
        } else {
            QDesktopServices::openUrl(url);
        }
#else
        QDesktopServices::openUrl(url);
#endif
    }
}

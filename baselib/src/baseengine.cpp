/* XiVO Client
 * Copyright (C) 2007-2011, Avencall
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
#include <QProcess>
#include <QTcpSocket>
#include <QTranslator>
#include <QUrl>
#include <QLibraryInfo>
#include <QSslError>
#include <QSslSocket>
#include <QUdpSocket>

#include <JsonToVariant.h>
#include <VariantToJson.h>
#include "xivoconsts.h"

#include "baseengine.h"
#include "cticonn.h"
#include "phonenumber.h"


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
static const QStringList CheckFunctions = (QStringList() << "presence" << "customerinfo");
static const QStringList GenLists = (QStringList()
                                     << "users" << "phones"
                                     << "agents" << "queues" << "groups" << "meetmes"
                                     << "voicemails" << "incalls" << "queuemembers" << "parkinglots");
static CtiConn * m_ctiConn;

BaseEngine::BaseEngine(QSettings *settings,
                       const QString &osInfo)
    : QObject(NULL),
      m_sessionid(""), m_state(ENotLogged),
      m_pendingkeepalivemsg(0), m_logfile(NULL),
      m_byte_counter(0), m_attempt_loggedin(false),
      m_rate_bytes(0), m_rate_msec(0), m_rate_samples(0),
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
    m_xinfoList.insert("groups", newXInfo<GroupInfo>);
    m_xinfoList.insert("meetmes", newXInfo<MeetmeInfo>);
    m_xinfoList.insert("voicemails", newXInfo<VoiceMailInfo>);
    m_xinfoList.insert("incalls", newXInfo<InCallsInfo>);
    m_xinfoList.insert("queuemembers", newXInfo<QueueMemberInfo>);
    m_xinfoList.insert("parkinglots", newXInfo<ParkingInfo>);

    // TCP connection with CTI Server
    m_ctiserversocket = new QSslSocket(this);
    m_ctiserversocket->setProtocol(QSsl::TlsV1);
    m_ctiConn = new CtiConn(m_ctiserversocket);

    connect(m_ctiserversocket, SIGNAL(encrypted()),
            this, SLOT(encryptedSsl()));
    connect(m_ctiserversocket, SIGNAL(sslErrors(const QList<QSslError> &)),
            this, SLOT(sslErrors(const QList<QSslError> & )));
    connect(m_ctiserversocket, SIGNAL(connected()),
            this, SLOT(ctiSocketConnected()));
    connect(m_ctiserversocket, SIGNAL(readyRead()),
            this, SLOT(ctiSocketReadyRead()));
    connect(m_ctiConn, SIGNAL(failedToConnect(const QString &)),
            this, SLOT(popupError(const QString &)));

    // TCP connection for file transfer
    // (this could be moved to some other class)
    m_filetransfersocket = new QTcpSocket(this);
    connect(m_filetransfersocket, SIGNAL(connected()),
            this, SLOT(filetransferSocketConnected()));
    connect(m_filetransfersocket, SIGNAL(readyRead()),
            this, SLOT(filetransferSocketReadyRead()));

    if (m_config["autoconnect"].toBool())
        start();
    translationFiles = \
        (QStringList() << ":/obj/xivoclient_%1"
                       << ":/obj/baselib_%1"
                       << ":/obj/xletlib_%1"
                       << QLibraryInfo::location(QLibraryInfo::TranslationsPath) + "/qt_%1" );
    changeTranslation();
}

void BaseEngine::encryptedSsl()
{
    qDebug() << Q_FUNC_INFO;
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
    qDebug() << Q_FUNC_INFO;
    clearLists();
    clearChannelList();
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
#ifdef Q_WS_WIN
    m_config["enableclipboard"] = m_settings->value("display/enableclipboard", false).toBool();
#else
    m_config["enableclipboard"] = m_settings->value("display/enableclipboard", true).toBool();
#endif /* Q_WS_WIN */

    m_config["logtofile"] = m_settings->value("display/logtofile", false).toBool();
    m_config["logfilename"] = m_settings->value("display/logfilename", "XiVO_Client.log").toString();
    m_config["activate_on_tel"] = m_settings->value("display/activate_on_tel", false).toBool();
    if (m_config["logtofile"].toBool()) {
        openLogFile ();
    }
    
    m_config["profilename"] = m_settings->value("profile/lastused").toString();
    m_profilename_write = "engine-" + m_config["profilename"].toString();
    
    QString settingsversion = m_settings->value("version/xivo", __xivo_version__).toString();

    // this is used to make a migration from 1.0 to 1.1
    if (settingsversion == "1.0")
        m_profilename_read = "engine";
    else
        m_profilename_read = "engine-" + m_config["profilename"].toString();

    m_settings->beginGroup(m_profilename_read);
        m_config["cti_address"] = m_settings->value("serverhost", "demo.xivo.fr").toString();
        m_config["cti_port"]    = m_settings->value("serverport", 5003).toUInt();
        m_config["cti_port_encrypted"]    = m_settings->value("serverport_encrypted", 5013).toUInt();
        m_config["cti_encrypt"] = m_settings->value("encryption", false).toBool();

        setUserLogin (m_settings->value("userid").toString(), m_settings->value("useridopt").toString());
        m_config["company"] = m_settings->value("company", "default").toString();
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
        
        m_config["doubleclick.searchpanel"] = m_settings->value("doubleclick.searchpanel", "call").toString();
        m_config["doubleclick.switchboard"] = m_settings->value("doubleclick.switchboard", "call").toString();

        m_settings->beginGroup("user-gui");
            m_config["historysize"] = m_settings->value("historysize", 8).toUInt();
        m_settings->endGroup();
    m_settings->endGroup();
    
    QString defaultguioptions;
    QFile defaultguioptions_file(":/guioptions.json");
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
    m_settings->setValue("display/logtofile", m_config["logtofile"].toBool());
    m_settings->setValue("display/logfilename", m_config["logfilename"].toString());
    m_settings->setValue("display/activate_on_tel", m_config["activate_on_tel"].toBool());
    
    m_settings->beginGroup(m_profilename_write);
        m_settings->setValue("serverhost", m_config["cti_address"].toString());
        m_settings->setValue("serverport", m_config["cti_port"].toUInt());
        m_settings->setValue("serverport_encrypted", m_config["cti_port_encrypted"].toUInt());
        m_settings->setValue("encryption", m_config["cti_encrypt"].toBool());
        m_settings->setValue("userid", m_config["userloginsimple"].toString());
        m_settings->setValue("useridopt", m_config["userloginopt"].toString());
        m_settings->setValue("company", m_config["company"].toString());
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
        
        m_settings->setValue("doubleclick.searchpanel", m_config["doubleclick.searchpanel"].toString());
        m_settings->setValue("doubleclick.switchboard", m_config["doubleclick.switchboard"].toString());

        if (m_config["keeppass"].toBool())
            m_settings->setValue("password", m_config["password"].toString());
        else
            m_settings->remove("password");

        m_settings->beginGroup("user-gui");
            m_settings->setValue("historysize", m_config["historysize"].toInt());
            m_settings->setValue("guisettings", m_config.getSubSet("guioptions", BaseConfig::Unmasked));
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
    if (m_config["logtofile"].toBool() && m_logfile != NULL) {
        QString tolog = QDateTime::currentDateTime().toString(Qt::ISODate) + " " + logstring + "\n";
        m_logfile->write(tolog.toUtf8());
        m_logfile->flush();
    }
}

/*! \brief send power event to the server.
 *
 * \todo check if this is usefull. If not, remove it.
 */
void BaseEngine::powerEvent(const QString & eventinfo)
{
    QVariantMap command;
    command["class"] = "powerevent";
    command["value"] = eventinfo;
    sendJsonCommand(command);
}

/*! \brief Starts the connection to the server
 * This method starts the login process by connection
 * to the server.
 */
void BaseEngine::start()
{
    qDebug() << "XiVO client starting:" << m_config["cti_address"].toString()
            << port_to_use() << m_config["cti_encrypt"].toBool()
            << m_config.getSubSet("checked_function");

    // (In case the TCP sockets were attempting to connect ...) aborts them first
    m_ctiserversocket->abort();
    connectSocket();
    m_byte_counter = 0;
    m_time.start();
}

/*! \brief Closes the connection to the server
 * This method disconnect the engine from the server
 */
void BaseEngine::clearInternalData()
{
    if (m_attempt_loggedin) {
        QString stopper = sender() ? sender()->property("stopper").toString() : "unknown";
        QVariantMap command;
        command["class"] = "logout";
        command["stopper"] = stopper;
        sendJsonCommand(command);
        m_settings->setValue("lastlogout/stopper", stopper);
        m_settings->setValue("lastlogout/datetime",
                             QDateTime::currentDateTime().toString(Qt::ISODate));
        m_settings->beginGroup(m_profilename_write);
        m_settings->setValue("availstate", m_availstate);
        m_settings->endGroup();
        m_attempt_loggedin = false;
    }

    m_sessionid = "";

    clearLists();
    clearChannelList();

    if (m_time.isValid()) {
        int elapsed = m_time.elapsed();
        qDebug() << Q_FUNC_INFO
                 << m_byte_counter << "bytes received in" << elapsed << "ms : "
                 << (elapsed?QString::number((1000*m_byte_counter)/elapsed):QString("infinite"))
                 << "Bytes/Second";
    }

    /* cleaning the registered listeners */
    m_listeners.clear();
}

void BaseEngine::stop()
{
    QString stopper = sender()->property("stopper").toString();
    qDebug() << Q_FUNC_INFO << "stopper = " << stopper;
    disconnectAndClean();
}

void BaseEngine::disconnectAndClean()
{
    clearInternalData();
    stopConnection();
    setState(ENotLogged);
}

void BaseEngine::stopConnection()
{
    qDebug() << Q_FUNC_INFO;
    m_ctiserversocket->flush();
    m_ctiserversocket->disconnectFromHost();
    stopKeepAliveTimer();
}

/*! \brief clear the content of m_users
 *
 * Delete all contained UserInfo objects
 */
void BaseEngine::clearLists()
{
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

/*! \brief initiates connection to the server
 */
void BaseEngine::connectSocket()
{
    if (m_config["userloginsimple"].toString().length()) {
        if (m_config["cti_encrypt"].toBool())
            m_ctiserversocket->connectToHostEncrypted(m_config["cti_address"].toString(), m_config["cti_port_encrypted"].toUInt());
        else
            m_ctiserversocket->connectToHost(m_config["cti_address"].toString(), m_config["cti_port"].toUInt());
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

const QStringList & BaseEngine::getCapasRegCommands() const
{
    return m_capas_regcommands;
}

const QStringList & BaseEngine::getCapasIpbxCommands() const
{
    return m_capas_ipbxcommands;
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
        keepLoginAlive();
    }
}

/*!
 * Returns the availstate of the current user
 */
const QString & BaseEngine::getAvailState() const
{
    if (const UserInfo * u = b_engine->user(getFullId())) {
        return u->availstate();
    }
    // If it's too early to have an initialized UserInfo use m_availstate
    qDebug() << Q_FUNC_INFO << "No user defined at this point using available";
    return m_availstate;
}

/*! \brief send command to XiVO CTI server */
void BaseEngine::sendCommand(const QString & command)
{
    if (m_ctiserversocket->state() == QAbstractSocket::ConnectedState)
        m_ctiserversocket->write((command + "\n").toUtf8());
}

/*! \brief encode json and then send command to XiVO CTI server */
QString BaseEngine::sendJsonCommand(const QVariantMap & cticommand)
{
    if (! cticommand.contains("class"))
        return QString("");
    QVariantMap fullcommand = cticommand;
    fullcommand["commandid"] = qrand();
    QString jsoncommand(JsonQt::VariantToJson::parse(fullcommand));
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
    command["userlogin"] = m_config["userloginsimple"].toString();
    command["company"] = m_config["company"].toString();
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

QString BaseEngine::timeElapsed(double timestamp) const
{
    double timespent = QDateTime::fromTime_t(timestamp).secsTo(QDateTime::currentDateTime());
    int timecorr = int(timespent - timeDeltaServerClient());
    int sec = timecorr % 60;
    int min = ((timecorr - sec) / 60) % 60;
    int hou = ((timecorr - sec - min * 60) / 60) / 60;
    QString timefmt;
    if (hou > 0)
        timefmt = QString("%1:%2:%3").arg(hou, 2, 10, QChar('0'))
            .arg(min, 2, 10, QChar('0'))
            .arg(sec, 2, 10, QChar('0'));
    else
        timefmt = QString("%1:%2").arg(min, 2, 10, QChar('0'))
            .arg(sec, 2, 10, QChar('0'));
    return timefmt;
    // to bench : this, against .toString("hh:mm:ss") (see confs)
}

void BaseEngine::emitMessage(const QString & msg)
{
    emit emitTextMessage(msg);
}

/*! \brief parse JSON and then process command */
void BaseEngine::parseCommand(const QString &line)
{
    m_pendingkeepalivemsg = 0;
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
    QString thisclass = datamap.value("class").toString();
    QString replyid = datamap.value("replyid").toString();
    m_timesrv = datamap.value("timenow").toDouble();
    m_timeclt = QDateTime::currentDateTime();

    if (forwardToListeners(thisclass, datamap))  // a class callback was called,
        return;                                  // so zap the 500 loc of if-else soup

    if ((thisclass == "keepalive") || (thisclass == "availstate")) {
        // ack from the keepalive and availstate commands previously sent
        return;
    }
    if (thisclass == "callcampaign") {
        emit requestFileListResult(datamap.value("payload"));
    } else if (thisclass == "sheet") {
        // TODO : use id better than just channel name
        QString channel = datamap.value("channel").toString();
        if (function == "getownership") {
            emit gotSheetOwnership(channel);
        } else if (function == "loseownership") {
            emit lostSheetOwnership(channel);
        } else if (function == "entryadded") {
            emit sheetEntryAdded(channel, datamap.value("entry").toMap());
        }

        if (datamap.contains("payload")) {
            QString payload;
            QByteArray qba = QByteArray::fromBase64(datamap.value("payload").toString().toAscii());
            if (datamap.value("compressed").toBool())
                payload = QString::fromUtf8(qUncompress(qba));
            else
                payload = QString::fromUtf8(qba);
            // will eventually call the XML parser
            emit displayFiche(payload, false, channel);
            if (datamap.contains("entries")) {
                QVariantList entries = datamap.value("entries").toList();
                foreach (QVariant entry, entries) {
                    emit sheetEntryAdded(channel, entry.toMap());
                }
            }
        }

    } else if (thisclass == "getlist") {
        configsLists(thisclass, function, datamap);
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

    } else if (thisclass == "featuresget") {
        QVariantMap featuresget_map = datamap.value("userfeatures").toMap();
        foreach (QString featurekey, featuresget_map.keys()) {
            initFeatureFields(featurekey);
        }
        emit emitTextMessage(tr("Received Services Data"));
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
            disconnectAndClean();
            popupError(datamap.value("error_string").toString());
        } else {
            m_sessionid = datamap.value("sessionid").toString();
            QString tohash = QString("%1:%2").arg(m_sessionid).arg(m_config["password"].toString());
            QCryptographicHash hidepass(QCryptographicHash::Sha1);
            QByteArray res = hidepass.hash(tohash.toAscii(), QCryptographicHash::Sha1).toHex();
            QVariantMap command;
            command["class"] = "login_pass";
            command["hashedpassword"] = QString(res);
            sendJsonCommand(command);
        }
    } else if (thisclass == "login_pass") {
        if (datamap.contains("error_string")) {
            disconnectAndClean();
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
        m_capas_regcommands = capas.value("regcommands").toStringList();
        m_capas_ipbxcommands = capas.value("ipbxcommands").toStringList();

        // ("ipbxcommands", "regcommands", "services", "functions")
        m_config.merge(capas.value("preferences").toMap());
        m_config["services"] = capas.value("services");

        QVariantMap tmp;
        QStringList todisp;
        m_config["checked_function.switchboard"] = true;
        tmp["functions"] = todisp;
        m_config["guioptions.server_funcs"] = tmp;

        qDebug() << "\n";
        qDebug() << "capaxlets" << m_capaxlets;
        qDebug() << "appliname" << m_appliname;
        qDebug() << "\n";

        QString urltolaunch = m_config["guioptions.loginwindow.url"].toString();
        if (! urltolaunch.isEmpty()) {
            urltolaunch.replace("{xc-username}", m_config["userloginsimple"].toString());
            urltolaunch.replace("{xc-password}", m_config["password"].toString());
            this->urlAuto(urltolaunch);
        }

        fetchIPBXList();
        setState(ELogged); // calls logged()
        m_timerid_keepalive = startTimer(m_config["keepaliveinterval"].toUInt());
        m_attempt_loggedin = true;

    } else if (thisclass == "disconnect") {
        qDebug() << thisclass << datamap;
        QString type = datamap.value("type").toString();
        disconnectAndClean();
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
    } else {
        if (replyid.isEmpty())
            qDebug() << "Unknown server command received:" << thisclass << datamap;
    }
}

void BaseEngine::configsLists(const QString & thisclass, const QString & function,
                              const QVariantMap & datamap)
{
    if (thisclass == "getlist") {
        QString listname = datamap.value("listname").toString();
        QString ipbxid = datamap.value("tipbxid").toString();

        if (function == "listid") {
            QStringList listid = datamap.value("list").toStringList();
            foreach (QString id, listid) {
                QString xid = QString("%1/%2").arg(ipbxid).arg(id);
                if (GenLists.contains(listname)) {
                    if (! m_anylist.contains(listname))
                        m_anylist[listname].clear();
                    newXInfoProto construct = m_xinfoList.value(listname);
                    XInfo * xinfo = construct(ipbxid, id);
                    if (! m_anylist[listname].contains(xid)) {
                        m_anylist[listname][xid] = xinfo;
                    }
                }
            }

            QVariantMap command;
            command["class"] = "getlist";
            command["function"] = "updateconfig";
            command["listname"] = listname;
            command["tipbxid"] = ipbxid;
            foreach (QString id, listid) {
                command["tid"] = id;
                sendJsonCommand(command);
            }

        } else if (function == "delconfig") {
            QStringList listid = datamap.value("list").toStringList();
            foreach (QString id, listid) {
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
            foreach (QString id, listid) {
                QString xid = QString("%1/%2").arg(ipbxid).arg(id);
                if (listname == "phones")
                    emit removePhoneConfig(xid);
                else if (listname == "users")
                    emit removeUserConfig(xid);
                else if (listname == "agents")
                    emit removeAgentConfig(xid);
                else if (listname == "queues")
                    emit removeQueueConfig(xid);
                else if (listname == "meetmes")
                    emit removeMeetmeConfig(xid);
                else if (listname == "queuemembers")
                    emit removeQueueMemberConfig(xid);
            }

        } else if (function == "updateconfig") {
            QString id = datamap.value("tid").toString();
            QString xid = QString("%1/%2").arg(ipbxid).arg(id);
            QVariantMap config = datamap.value("config").toMap();
            bool haschanged = false;
            if (GenLists.contains(listname)) {
                if (! m_anylist.value(listname).contains(xid)) {
                    newXInfoProto construct = m_xinfoList.value(listname);
                    XInfo * xinfo = construct(ipbxid, id);
                    m_anylist[listname][xid] = xinfo;
                }
                if (m_anylist.value(listname).value(xid) != NULL) {
                    haschanged = m_anylist.value(listname)[xid]->updateConfig(config);
                } else {
                    qDebug() << "received updateconfig for inexisting" << listname << xid;
                }
                if ((xid == m_xuserid) && (listname == "users")) {
                    emit localUserInfoDefined();
                }
            } else {
                qDebug() << "received " << function << "for unknown list" << listname << "id" << xid;
            }

            if (! haschanged) {
                // qDebug() << "got an unchanged" << function << listname << xid;
            }

            // updating relations ...
            if (listname == "phones") {
                emit peersReceived();
            }

            // transmission to xlets
            if (listname == "users") {
                emit updateUserConfig(xid);
                emit updateUserConfig(xid,datamap);
            } else if (listname == "phones")
                emit updatePhoneConfig(xid);
            else if (listname == "agents")
                emit updateAgentConfig(xid);
            else if (listname == "queues")
                emit updateQueueConfig(xid);
            else if (listname == "voicemails")
                emit updateVoiceMailConfig(xid);
            else if (listname == "meetmes")
                emit updateMeetmesConfig(xid);
            else if (listname == "queuemembers")
                emit updateQueueMemberConfig(xid);
            else if (listname == "parkinglots")
                emit updateParkinglotConfig(xid);

            QVariantMap command;
            command["class"] = "getlist";
            command["function"] = "updatestatus";
            command["listname"] = listname;
            command["tipbxid"] = ipbxid;
            command["tid"] = id;
            sendJsonCommand(command);

        } else if (function == "updatestatus") {
            QString id = datamap.value("tid").toString();
            QString xid = QString("%1/%2").arg(ipbxid).arg(id);
            QVariantMap status = datamap.value("status").toMap();
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
                if (id.startsWith("qa:")) {
                    QString qaids = id.split(":")[1];
                    QStringList parts = qaids.split("-");
                    QString queuexid = QString("%0/%1").arg(ipbxid).arg(parts[0]);
                    QString agentxid = QString("%0/%1").arg(ipbxid).arg(parts[1]);
                    emit updateQueueStatus(queuexid);
                    emit updateAgentStatus(agentxid);
                }
            }

            if (listname == "users") {
                setAvailState(status["availstate"].toString(), true);
                emit updateUserStatus(xid);
            } else if (listname == "phones") {
                emit updatePhoneStatus(xid);
                if (hasPhone(xid)) {
                    QVariantMap command;
                    command["class"] = "getlist";
                    command["function"] = "updatestatus";
                    command["listname"] = "channels";
                    command["tipbxid"] = ipbxid;
                    foreach (QString cid, phone(xid)->channels()) {
                        command["tid"] = cid;
                        sendJsonCommand(command);
                    }
                }
            } else if (listname == "parkinglots") {
                emit updateParkinglotStatus(xid);
            } else if (listname == "agents")
                emit updateAgentStatus(xid);
            else if (listname == "queues") {
                emit updateQueueStatus(xid);
                if (hasQueue(xid)) {
                    QVariantMap command;
                    command["class"] = "getlist";
                    command["function"] = "updatestatus";
                    command["listname"] = "queuemembers";
                    command["tipbxid"] = ipbxid;
                    foreach (QString aid, queue(xid)->agentmembers()) {
                        command["tid"] = aid;
                        sendJsonCommand(command);
                    }
                    foreach (QString pid, queue(xid)->phonemembers()) {
                        command["tid"] = pid;
                        sendJsonCommand(command);
                    }
                }
            }
            else if (listname == "voicemails")
                emit updateVoiceMailStatus(xid);
            else if (listname == "channels")
                emit updateChannelStatus(xid);
            else if (listname == "meetmes")
                emit updateMeetmesStatus(xid);

        } else if (function == "addconfig") {
            QStringList listid = datamap.value("list").toStringList();
            foreach (QString id, listid) {
                QString xid = QString("%1/%2").arg(ipbxid).arg(id);
                if (GenLists.contains(listname)) {
                    newXInfoProto construct = m_xinfoList.value(listname);
                    XInfo * xinfo = construct(ipbxid, id);
                    if (! m_anylist[listname].contains(xid)) {
                        m_anylist[listname][xid] = xinfo;
                    }
                }
            }
            QVariantMap command;
            command["class"] = "getlist";
            command["function"] = "updateconfig";
            command["listname"] = listname;
            command["tipbxid"] = ipbxid;
            foreach (QString id, listid) {
                command["tid"] = id;
                sendJsonCommand(command);
            }
        }
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

/*! \brief send callcampaign command to the CTI server */
void BaseEngine::requestFileList(const QString & action)
{
    QVariantMap command;
    command["class"] = "callcampaign";
    command["command"] = action.split(" ");
    sendJsonCommand(command);
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
void BaseEngine::popupError(const QString & errorid)
{
    QString errormsg = QString(tr("Server has sent an Error."));

    // errors sent by the server (login phase)
    if (errorid.toLower() == "user_not_found") {
        errormsg = tr("Your registration name <%1@%2> "
                      "is not known by the XiVO CTI server on %3:%4.")
            .arg(m_config["userloginsimple"].toString()).arg(m_config["company"].toString())
            .arg(m_config["cti_address"].toString()).arg(port_to_use());
    } else if (errorid.toLower() == "login_password") {
        errormsg = tr("You entered a wrong login / password.");
    } else if (errorid.startsWith("capaid_undefined:")) {
        QStringList capainfo = errorid.split(":");
        errormsg = tr("Your profile identifier <%1> is not defined.").arg(capainfo[1]);

    // keepalive (internal)
    } else if (errorid.toLower() == "no_keepalive_from_server") {
        errormsg = tr("The XiVO CTI server on %1:%2 did not reply to the last keepalive.")
            .arg(m_config["cti_address"].toString()).arg(port_to_use());

    // socket errors - while attempting to connect
    } else if (errorid.toLower() == "socket_error_hostnotfound") {
        errormsg = tr("You defined an IP address %1 that is probably an unresolved host name.")
            .arg(m_config["cti_address"].toString());
    } else if (errorid.toLower() == "socket_error_timeout") {
        errormsg = tr("Socket timeout (~ 60 s) : you probably attempted to reach, "
                      "via a gateway, an IP address %1 that does not exist.")
            .arg(m_config["cti_address"].toString());
    } else if (errorid.toLower() == "socket_error_connectionrefused") {
        errormsg = tr("There seems to be a machine running on this IP address %1, "
                      "and either no CTI server is running, or your port %2 is wrong.")
            .arg(m_config["cti_address"].toString()).arg(port_to_use());
    } else if (errorid.toLower() == "socket_error_network") {
        errormsg = tr("An error occurred on the network while attempting to join the IP address %1 :\n"
                      "- no external route defined to access this IP address (~ no timeout)\n"
                      "- this IP address is routed but there is no machine (~ 5 s timeout)\n"
                      "- a cable has been unplugged on your LAN on the way to this IP address (~ 30 s timeout).")
            .arg(m_config["cti_address"].toString());
    } else if (errorid.toLower() == "socket_error_sslhandshake") {
        errormsg = tr("It seems that the server with IP address %1 does not accept encryption on "
                      "its port %2. Please change either your port or your encryption setting.")
            .arg(m_config["cti_address"].toString()).arg(port_to_use());
    } else if (errorid.toLower() == "socket_error_unknown") {
        errormsg = tr("An unknown socket error has occured while attempting to join the IP address:port %1:%2.")
            .arg(m_config["cti_address"].toString()).arg(port_to_use());
    } else if (errorid.startsWith("socket_error_unmanagedyet:")) {
        QStringList ipinfo = errorid.split(":");
        errormsg = tr("An unmanaged (number %1) socket error has occured while attempting to join the IP address:port %1:%2.")
            .arg(ipinfo[1]).arg(m_config["cti_address"].toString()).arg(port_to_use());

    // socket errors - once connected
    } else if (errorid.toLower() == "socket_error_remotehostclosed") {
        errormsg = tr("The XiVO CTI server on %1:%2 has just closed the connection.")
            .arg(m_config["cti_address"].toString()).arg(port_to_use());

    } else if (errorid.toLower() == "server_stopped") {
        errormsg = tr("The XiVO CTI server on %1:%2 has just been stopped.")
            .arg(m_config["cti_address"].toString()).arg(port_to_use());
    } else if (errorid.toLower() == "server_reloaded") {
        errormsg = tr("The XiVO CTI server on %1:%2 has just been reloaded.")
            .arg(m_config["cti_address"].toString()).arg(port_to_use());
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
    } else if (errorid == "invalid_exten") {
        errormsg = tr("Invalid extension number");
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
        m_byte_counter += data.size();
        QString line = QString::fromUtf8(data);

        if (line.startsWith("<ui version=")) {
            // we get here when receiving a sheet as a Qt4 .ui form
            qDebug() << "Incoming sheet, size:" << line.size();
            emit displayFiche(line, true, QString());
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
    while (m_filetransfersocket->canReadLine()) {
        QByteArray data = m_filetransfersocket->readLine();
        QString line = QString::fromUtf8(data);
        QVariant jsondata;
        try {
            jsondata = JsonQt::JsonToVariant::parse(line.trimmed());
        } catch(JsonQt::ParseException &) {
            qDebug() << Q_FUNC_INFO << "exception catched for" << line.trimmed();
        }
        QVariantMap jsondatamap = jsondata.toMap();
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

/*! \brief forward actions from fiche to XiVO CTI server
 *
 * Build and send JSON command
 */
void BaseEngine::actionFromFiche(const QVariant & infos)
{
    QVariantMap command;
    command["class"] = "actionfiche";
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
    if (! getCapasIpbxCommands().contains(action)) {
        qDebug() << Q_FUNC_INFO << "IGNORE" << action << src << dst;
        return;
    }

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
    } else if (action == "dial") {
        ipbxcommand["command"] = action;
        ipbxcommand["destination"] = dst;
    } else if (action == "parking") {
        ipbxcommand["command"] = action;
        ipbxcommand["source"] = src;
        ipbxcommand["destination"] = dst;
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

/*! \brief make the user dial a number
 *
 * \param action originate/transfer/atxfer/hangup/answer/refuse
 */
void BaseEngine::actionDialNumber(const QString & number)
{
    QVariantMap ipbxcommand;
    ipbxcommand["command"] = "dial";
    ipbxcommand["destination"] = QString("exten:%1/%2").arg(m_ipbxid).arg(number);
    ipbxCommand(ipbxcommand);
}

/*! \brief Receive a number list from xlets and signal this list
 *  to interested xlets
 */
void BaseEngine::receiveNumberSelection(const QStringList & numbers)
{
    emit broadcastNumberSelection(numbers);
}

/*! \brief send the directory search command to the server
 *
 * \sa directoryResponse()
 */
void BaseEngine::searchDirectory(const QString & text)
{
    QVariantMap command;
    command["class"] = "directory";
    command["pattern"] = text;
    sendJsonCommand(command);
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
    bool change_translation = qvm.contains("forcelocale") &&
                            m_config["forcelocale"].toUInt() != qvm["forcelocale"].toUInt();
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
    
    if (change_translation)
        changeTranslation();

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

void BaseEngine::initFeatureFields(const QString & field)
{
    if ( (field == "enablednd") ||
         (field == "enablevoicemail") ||
         (field == "incallfilter") )
        emit optChanged(field);

    // dnd, vm, unc, rna, busy ... ?
    else if ( (field == "enableunc") ||
              (field == "enablebusy") ||
              (field == "enablerna") ||
              (field == "destunc") ||
              (field == "destbusy") ||
              (field == "destrna"))
        emit forwardUpdated(field);
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

void BaseEngine::fetchIPBXList()
{
    QVariantMap command;
    command["class"] = "getipbxlist";
    sendJsonCommand(command);
}

/*! \brief send a lot of getlist commands to the CTI server
 *
 * send getlist for "users", "queues", "agents", "phones",
 * "meetme", "users", "endinit"
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
            emit logged();
            // emit updatePresence();
        } else if (state == ENotLogged) {
            emit delogged();
            // reset some variables when disconnecting
        }
    }
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
    disconnectAndClean();
    popupError("no_keepalive_from_server");
    m_pendingkeepalivemsg = 0;
    startTryAgainTimer();
}

void BaseEngine::sendKeepAliveMsg()
{
    QVariantMap command;
    command["class"] = "keepalive";
    if (m_rate_bytes > 100000) {
        command["rate-bytes"] = m_rate_bytes;
        command["rate-msec"] = m_rate_msec;
        command["rate-samples"] = m_rate_samples;
        m_rate_bytes = 0;
        m_rate_msec = 0;
        m_rate_samples = 0;
    }
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

/*! \brief send message to the CTI Server */
void BaseEngine::logClient(const QString & level,
                           const QString & classmethod,
                           const QString & message)
{
    QVariantMap command;
    command["class"] = "logfromclient";
    command["level"] = level;
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

/*! \brief Retrieves a UserInfo for a given xchannel id
 *
 *  \param xcid The xivo channel id to look for
 *  \return The UserInfo of the channel's owner
 */
const UserInfo * BaseEngine::getUserForXChannelId(const QString & xcid) const
{
    foreach (const QString xuid, b_engine->iterover("users").keys()) {
        const UserInfo * user = b_engine->user(xuid);
        if (user == NULL) {
            qDebug() << Q_FUNC_INFO << "unknown user" << xuid;
            continue;
        }
        foreach (const QString pid, user->phonelist()) {
            const PhoneInfo * phone = b_engine->phone(pid);
            if (phone == NULL) {
                qDebug() << Q_FUNC_INFO << "unknown phone" << pid << "for user" << xuid;
                continue;
            }
            foreach (const QString xchan, phone->xchannels()) {
                if (xchan.endsWith(xcid))
                    return user;
            }
        }
    }
    return NULL;
}

/*! \brief send new remark about a sheet */
void BaseEngine::sendNewRemark(const QString & id, const QString & text)
{
    QVariantMap command;
    command["class"] = "sheet";
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
    qDebug() << Q_FUNC_INFO << m_osname << "got" << msg;
    // tel:number is in RFC 3966
    // callto:number is unofficial (read 7.3. in RFC 3966)
    // callto://number is unofficial and used by Skype
    // we support tel:number and callto:number
    // todo : handle also other commands
    QString phonenum = msg;
    qDebug() << Q_FUNC_INFO << "trying to dial" << phonenum;
    actionDialNumber(phonenum);
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
    QString locale = m_locale;
    QTranslator *translator = new QTranslator;
    translator->load(path.arg(locale));
    qApp->installTranslator(translator);
}

void BaseEngine::changeTranslation(QString locale)
{
    if (locale.isEmpty()) {
        locale = m_config["forcelocale"].toString();
    }
    if (locale == "default") {
        locale = QLocale::system().name();
    }
    
    m_locale = locale;

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
            QPair<QString, QString> pair;
            foreach(pair, url.queryItems())
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
#ifdef Q_WS_WIN
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

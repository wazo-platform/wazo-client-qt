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
#include <QSettings>
#include <QSslError>
#include <QSslSocket>
#include <QUdpSocket>

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

BASELIB_EXPORT BaseEngine * b_engine;
static const QStringList CheckFunctions = (QStringList() << "presence" << "customerinfo");
static const QStringList GenLists = (QStringList()
                                     << "users" << "phones" << "trunks"
                                     << "agents" << "queues" << "groups" << "meetmes"
                                     << "voicemails" << "incalls" << "parkinglots");
static CtiConn * m_ctiConn;

BaseEngine::BaseEngine(QSettings *settings,
                       const QString &osInfo)
    : QObject(NULL),
      m_cti_address(""), m_cti_port(0),
      m_cti_encrypt(false),
      m_userlogin(""), m_userloginopt(""), m_company(""), m_password(""), m_agentphonenumber(""),
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

    m_xinfoList.insert("users", newXInfo<UserInfo>);
    m_xinfoList.insert("phones", newXInfo<PhoneInfo>);
    m_xinfoList.insert("trunks", newXInfo<TrunkInfo>);
    m_xinfoList.insert("agents", newXInfo<AgentInfo>);
    m_xinfoList.insert("queues", newXInfo<QueueInfo>);
    m_xinfoList.insert("groups", newXInfo<GroupInfo>);
    m_xinfoList.insert("meetmes", newXInfo<MeetmeInfo>);
    m_xinfoList.insert("voicemails", newXInfo<VoiceMailInfo>);
    m_xinfoList.insert("incalls", newXInfo<InCallsInfo>);
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
        m_cti_address = m_settings->value("serverhost", "demo.xivo.fr").toString();
        m_cti_port    = m_settings->value("serverport", 5003).toUInt();
        m_cti_encrypt = m_settings->value("encryption", false).toBool();

        m_userlogin      = m_settings->value("userid").toString().trimmed();
        m_userloginopt   = m_settings->value("useridopt").toString().trimmed();
        if (m_userloginopt.size() > 0)
            m_userloginwithopt = m_userlogin + "%" + m_userloginopt;
        else
            m_userloginwithopt = m_userlogin;
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
    if (m_settings->value("userid").toString() != m_userlogin) {
        m_settings->setValue("monitor/userid", QString(""));
    }

    m_settings->setValue("version/xivo", __xivo_version__);
    m_settings->setValue("version/git_hash", __git_hash__);
    m_settings->setValue("version/git_date", __git_date__);
    m_settings->setValue("display/systrayed", m_systrayed);

    m_settings->beginGroup(m_profilename_write);
        m_settings->setValue("serverhost", m_cti_address);
        m_settings->setValue("serverport", m_cti_port);
        m_settings->setValue("encryption", m_cti_encrypt);
        m_settings->setValue("userid", m_userlogin);
        m_settings->setValue("useridopt", m_userloginopt);
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
                emit availAllowChanged(b);
        }
    }
}

void BaseEngine::pasteToDial(const QString & toPaste)
{
    emit pasteToXlets(toPaste);
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
    command["value"] = eventinfo;
    sendJsonCommand(command);
}

/*! \brief Starts the connection to the server
 * This method starts the login process by connection
 * to the server.
 */
void BaseEngine::start()
{
    qDebug() << Q_FUNC_INFO << m_cti_address << m_cti_port << m_cti_encrypt << m_checked_function;

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
    QString stopper = sender()->property("stopper").toString();
    if (m_attempt_loggedin) {
        QVariantMap command;
        command["class"] = "logout";
        command["stopper"] = stopper;
        sendJsonCommand(command);
        m_settings->setValue("lastlogout/stopper", stopper);
        m_settings->setValue("lastlogout/datetime", QDateTime::currentDateTime().toString(Qt::ISODate));
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

void BaseEngine::stop()
{
    QString stopper = sender()->property("stopper").toString();
    qDebug() << Q_FUNC_INFO << "stopper = " << stopper;
    stopConnection();
    // if (stopper != "connection_lost")
    clearInternalData();
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
    if (m_userlogin.length()) {
        if (m_cti_encrypt)
            m_ctiserversocket->connectToHostEncrypted(m_cti_address, m_cti_port);
        else
            m_ctiserversocket->connectToHost(m_cti_address, m_cti_port);
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

const QVariantMap & BaseEngine::getOptionsChannelStatus() const
{
    return m_options_channelstatus;
}

const QVariantMap & BaseEngine::getOptionsAgentStatus() const
{
    return m_options_agentstatus;
}

const QStringList & BaseEngine::getCapasRegCommands() const
{
    return m_capas_regcommands;
}

const QStringList & BaseEngine::getCapasIpbxCommands() const
{
    return m_capas_ipbxcommands;
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
            emit changesAvailChecks();
        else
            changeState();
        keepLoginAlive();
    } else {
        if (comesFromServer)
            emit changesAvailChecks();
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
void BaseEngine::sendJsonCommand(const QVariantMap & cticommand)
{
    if (! cticommand.contains("class"))
        return;
    QVariantMap fullcommand = cticommand;
    fullcommand["commandid"] = qrand();
    QString jsoncommand(JsonQt::VariantToJson::parse(fullcommand));
    sendCommand(jsoncommand);
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
    // qDebug() << Q_FUNC_INFO << userid;
    if (xuserid == m_monitored_xuserid)
    if (m_anylist.value("users").contains(xuserid)) {
        m_monitored_xuserid = xuserid;
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
    command["userlogin"] = m_userlogin;
    command["company"] = m_company;
    command["ident"] = m_osname;
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
    command["command"] = "put_announce";
    command["format"] = "base64";
    command["socketref"] = QString("%1:%2")
        .arg(m_filetransfersocket->localAddress().toString())
        .arg(m_filetransfersocket->localPort());
    command["filename"] = "toto.pdf";
    command["fileid"] = m_fileid;
    command["formatted_size"] = m_filedata.size();
    command["file_size"] = m_faxsize;
    sendJsonCommand(command);

//     // ??? useless test ???
//     if (m_filetransfersocket->state() == QAbstractSocket::ConnectedState)
//         m_filetransfersocket->write((jsoncommand + "\n").toAscii());
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

void BaseEngine::addUpdateConfMemberInTree(DStore *tree, const QString & xid)
{
    const MeetmeInfo * meetmeinfo = meetme(xid);
    if (meetmeinfo == NULL)
        return;

    QString roomid = meetmeinfo->id();
    tree->rmPath(QString("confrooms/%1/in").arg(roomid));
    foreach (QString xchannel, meetmeinfo->xchannels()) {
        const ChannelInfo * channelinfo = m_channels.value(xchannel);
        if (channelinfo == NULL)
            continue;

        QVariantMap info;
        QString id = QString::number(channelinfo->meetme_usernum());
        info["id"] = id;
        info["time-start"] = channelinfo->timestamp();
        info["displayname"] = channelinfo->thisdisplay();
        // info["phonenum"] = details.value("phonenum");
        // info["user-id"] = details.value("userid");
        info["admin"] = channelinfo->meetme_isadmin();
        info["authed"] = channelinfo->meetme_isauthed();
        info["muted"] = channelinfo->meetme_ismuted();
        QString path = QString("confrooms/%1/in/%2").arg(roomid).arg(id);
        tree->populate(path ,info);
    }

    // leave case
    // tree->rmPath(path);
}

void BaseEngine::addUpdateConfRoomInTree(DStore *tree,
                                         const QString & xid)
{
    const MeetmeInfo * meetmeinfo = meetme(xid);
    if (meetmeinfo == NULL)
        return;

    QString roomid = meetmeinfo->id();
    QVariantMap info;
    info["id"] = roomid;
    info["name"] = meetmeinfo->name();
    info["number"] = meetmeinfo->number();
    info["pin_needed"] = meetmeinfo->pin_needed();
    info["admin_moderationmode"] = meetmeinfo->admin_moderationmode();
    info["in"] = QVariantMap();

    tree->populate(QString("confrooms/%0").arg(roomid), info);
}

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
    QString thisclass = datamap.value("class").toString();
    QString replyid = datamap.value("replyid").toString();
    m_timesrv = datamap.value("timenow").toDouble();
    m_timeclt = QDateTime::currentDateTime();

    if (callClassEventCallback(thisclass, datamap))  // a class callback was called,
        return;                                      // so zap the 500 loc of if-else soup

    // qDebug() << Q_FUNC_INFO << datamap.value("timenow").toString() << "BaseEngine message received"
    // << thisclass << datamap.value("function").toString()
    // << datamap.value("phoneid").toString();
    if ((thisclass == "keepalive") || (thisclass == "availstate"))
        // ack from the keepalive and availstate commands previously sent
        return;

        if (thisclass == "callcampaign") {
            emit requestFileListResult(datamap.value("payload"));
        } else if (thisclass == "sheet") {
            // TODO : use id better than just channel name
            // qDebug() << Q_FUNC_INFO << "sheet" << datamap;
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

        } else if (thisclass == "agentrecord") {
            emit statusRecord(datamap.value("ipbxid").toString(),
                              datamap.value("agentid").toString(),
                              datamap.value("status").toString());

        } else if (thisclass == "agentlisten") {
            emit statusListen(datamap.value("ipbxid").toString(),
                              datamap.value("agentid").toString(),
                              datamap.value("status").toString());

        } else if (thisclass == "endinit") {
            qDebug() << Q_FUNC_INFO << "I should have received everything";

        } else if (thisclass == "serverdown") {
            qDebug() << Q_FUNC_INFO << thisclass << datamap.value("mode").toString();

        } else if (thisclass == "disconn") {
            qDebug() << Q_FUNC_INFO << thisclass;

        } else if (thisclass == "directory") {
            emit directoryResponse(datamap.value("headers").toStringList(),
                                   datamap.value("resultlist").toStringList());

        } else if (thisclass == "faxsend") {
            //m_filedir = datamap.value("tdirection").toString();
            if (datamap.contains("step"))
                qDebug() << Q_FUNC_INFO << "step" << datamap.value("step").toString();
            else {
                m_fileid = datamap.value("fileid").toString();
                m_filetransfersocket->connectToHost(m_cti_address, m_cti_port);
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
            // QString id = datamap.value("company").toString() + "/" + datamap.value("userid").toString();
            QString id = datamap.value("astid").toString() + "/" + datamap.value("xivo_userid").toString();
            qDebug() << Q_FUNC_INFO << thisclass << m_anylist.value("users").size() << id << datamap;
            if (m_anylist.value("users").contains(id)) {
                QVariantMap state = datamap.value("capapresence").toMap().value("state").toMap();
                QString stateid = state.value("stateid").toString();
                QVariantMap changeme = m_guioptions.value("server_gui").toMap().value("presence.autochangestate").toMap();
                if (changeme.count() && (id == m_xuserid)) {
                    if (changeme.contains(stateid)) {
                        // if (stateid == changeme["statesrc"].toString()) {
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
            // if (m_monitored_userid == datamap.value("userid").toString()) {
            resetFeatures();
            foreach (QString featurekey, featuresget_map.keys()) {
                initFeatureFields(featurekey);
            }
            emit emitTextMessage(tr("Received Services Data"));
            //}

        } else if (thisclass == "featuresput") {
            QVariantMap featuresput_map = datamap.value("payload").toMap();
            if (m_monitored_xuserid == datamap.value("userid").toString()) {
                if (featuresput_map.isEmpty()) {
                    emit featurePutIsKO();
                    emit emitTextMessage(tr("Could not modify the Services data.") + " " + tr("Maybe Asterisk is down."));
                } else {
                    emit featurePutIsOK();
                    foreach (QString featurekey, featuresput_map.keys()) {
                        initFeatureFields(featurekey);
                    }
                    emit emitTextMessage("");
                }
            }

//         } else if (thisclass == "features") {
//             if (function == "update") {
//                 QVariantMap featuresupdate_map = datamap.value("payload").toMap();
//                 if (m_monitored_xuserid == datamap.value("userid").toString())
//                     foreach (QString featurekey, featuresupdate_map.keys())
//                         initFeatureFields(featurekey);
//             }

        } else if (thisclass == "login_id") {
            if (datamap.contains("error_string")) {
                stopConnection();
                clearInternalData();
                setState(ENotLogged);
                popupError(datamap.value("error_string").toString());
            } else {
                m_sessionid = datamap.value("sessionid").toString();
                QString tohash = QString("%1:%2").arg(m_sessionid).arg(m_password);
                QCryptographicHash hidepass(QCryptographicHash::Sha1);
                QByteArray res = hidepass.hash(tohash.toAscii(), QCryptographicHash::Sha1).toHex();
                QVariantMap command;
                command["class"] = "login_pass";
                command["hashedpassword"] = QString(res);
                sendJsonCommand(command);
            }

        } else if (thisclass == "login_pass") {
            if (datamap.contains("error_string")) {
                stopConnection();
                clearInternalData();
                setState(ENotLogged);
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
                if (m_userloginopt.size() > 0) {
                    if (capas.contains(m_userloginopt))
                        command["capaid"] = m_userloginopt;
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
            }

        } else if (thisclass == "login_capas") {
            // qDebug() << "login_capas_ok" << datamap;
            m_ipbxid = datamap.value("ipbxid").toString();
            m_userid = datamap.value("userid").toString();
            m_xuserid = QString("%1/%2").arg(m_ipbxid).arg(m_userid);

            m_appliname = datamap.value("appliname").toString();
            m_capaxlets = datamap.value("capaxlets").toList();

            QVariantMap capas = datamap.value("capas").toMap();
            m_options_userstatus = capas.value("userstatus").toMap();
            m_options_phonestatus = capas.value("phonestatus").toMap();
            m_options_channelstatus = capas.value("channelstatus").toMap();
            m_options_agentstatus = capas.value("agentstatus").toMap();
            m_capas_regcommands = capas.value("regcommands").toStringList();
            m_capas_ipbxcommands = capas.value("ipbxcommands").toStringList();
            m_capafuncs = capas.value("functions").toStringList();
            // ("agentstatus", "ipbxcommands", "phonestatus", "regcommands", "services", "functions", "userstatus") 

            m_forced_state = datamap.value("presence").toString();
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
            tmpa["services"] = capas.value("services");
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
            qDebug() << "capaxlets" << m_capaxlets;
            qDebug() << "capafuncs" << m_capafuncs;
            qDebug() << "appliname" << m_appliname;
            qDebug() << "\n";

            QString urltolaunch = m_guioptions.value("merged_gui").toMap().value("loginwindow.url").toString();
            if (! urltolaunch.isEmpty()) {
                urltolaunch.replace("{xc-username}", m_userlogin);
                urltolaunch.replace("{xc-password}", m_password);
                this->urlAuto(urltolaunch);
            }

            // XXXX m_capafuncs => config file
            // m_enabled_function's purposes are :
            // - to keep track of the user's true rights
            foreach (QString function, CheckFunctions)
                if (m_capafuncs.contains(function))
                    m_enabled_function[function] = true;
                else
                    m_enabled_function[function] = false;

            // if no capa ?
            // stopConnection();
            // clearInternalData();
            // setState(ENotLogged);
            // popupError("no_capability");

            fetchIPBXList();
            setState(ELogged); // calls logged()
            setAvailState(m_forced_state, true);
            emit updatePresence();
            m_timerid_keepalive = startTimer(m_keepaliveinterval);
            m_attempt_loggedin = true;

        } else if (thisclass == "disconnect") {
            qDebug() << thisclass << datamap;
            QString type = datamap.value("type").toString();
            stopConnection();
            clearInternalData();
            setState(ENotLogged);
            if (type=="force") {
                m_forced_to_disconnect = true; // disable autoreconnect
                popupError("forcedisconnected");
            } else {
                popupError("disconnected");
            }

        } else if (thisclass == "ipbxcommand") {
            qDebug() << Q_FUNC_INFO << thisclass << datamap;

        } else if (thisclass == "getipbxlist") {
            m_ipbxlist = datamap.value("ipbxlist").toStringList();
            fetchLists();

        } else {
            if (replyid.isEmpty())
                qDebug() << Q_FUNC_INFO << "unknown server command class" << thisclass << datamap;
        }
}

void BaseEngine::configsLists(const QString & thisclass, const QString & function,
                              const QVariantMap & datamap)
{
    QVariantMap payload;
    if (function == "sendlist")
        payload = datamap.value("payload").toMap();

    if (thisclass == "getlist") {
        // qDebug() << Q_FUNC_INFO << thisclass << function << datamap;
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
                } else if (listname == "queuemembers") {
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
                    qDebug() << "null for" << listname << xid;
                }
                if ((xid == m_xuserid) && (listname == "users"))
                    emit localUserInfoDefined();
            } else {
                qDebug() << function << listname << xid << haschanged;
            }

            if (! haschanged)
                qDebug() << "got an unchanged" << function << listname << xid;

            // updating relations ...
            if (listname == "phones") {
                //
            }

            // transmission to xlets
            if (listname == "users")
                emit updateUserConfig(xid);
            else if (listname == "phones")
                emit updatePhoneConfig(xid);
            else if (listname == "agents")
                emit updateAgentConfig(xid);
            else if (listname == "queues")
                emit updateQueueConfig(xid);
            else if (listname == "voicemails")
                emit updateVoiceMailConfig(xid);
            else if (listname == "meetmes")
                addUpdateConfRoomInTree(tree(), xid);
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
            bool haschanged = false;
            if (GenLists.contains(listname)) {
                if (m_anylist.value(listname).contains(xid))
                    haschanged = m_anylist.value(listname).value(xid)->updateStatus(status);
            } else if (listname == "channels") {
                if (! m_channels.contains(xid))
                    m_channels[xid] = new ChannelInfo(ipbxid, id);
                haschanged = m_channels[xid]->updateStatus(status);
            } else if (listname == "queuemembers") {
                if (! m_queuemembers.contains(xid))
                    m_queuemembers[xid] = new QueueMemberInfo(ipbxid, id);
                haschanged = m_queuemembers[xid]->updateStatus(status);
            }

            // qDebug() << function << listname << xid << haschanged << status;

            if (listname == "users")
                emit updateUserStatus(xid);
            else if (listname == "phones") {
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
            } else if (haschanged && listname == "parkinglots") {
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
                addUpdateConfMemberInTree(tree(), xid);

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

    } else if (thisclass == "users") {
        if (function == "sendlist") {
            QVariantList listusers = datamap.value("payload").toList();
            foreach (QVariant userprops, listusers) {
                QVariantMap uinfo = userprops.toMap();
                //qDebug() << "-------------" << uinfo;

                addUpdateUserInTree(tree(), uinfo);

            }
        }
    }
}


/*! \brief send meetme command to the CTI server */
void BaseEngine::meetmeAction(const QString &function, const QString &functionargs)
{
    qDebug() <<"meetmeAction" << function << " -- arg: " << functionargs;
    QVariantMap command;
    command["class"] = "meetme";
    command["function"] = function;
    command["functionargs"] = functionargs.split(" ");
    sendJsonCommand(command);
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
            .arg(m_userlogin).arg(m_company)
            .arg(m_cti_address).arg(m_cti_port);
    } else if (errorid.toLower() == "login_password") {
        errormsg = tr("You entered a wrong login / password.");
    } else if (errorid.startsWith("capaid_undefined:")) {
        QStringList capainfo = errorid.split(":");
        errormsg = tr("Your profile identifier <%1> is not defined.").arg(capainfo[1]);

    // keepalive (internal)
    } else if (errorid.toLower() == "no_keepalive_from_server") {
        errormsg = tr("The XiVO CTI server on %1:%2 did not reply to the last keepalive.")
            .arg(m_cti_address).arg(m_cti_port);

    // socket errors - while attempting to connect
    } else if (errorid.toLower() == "socket_error_hostnotfound") {
        errormsg = tr("You defined an IP address %1 that is probably an unresolved host name.")
            .arg(m_cti_address);
    } else if (errorid.toLower() == "socket_error_timeout") {
        errormsg = tr("Socket timeout (~ 60 s) : you probably attempted to reach, "
                      "via a gateway, an IP address %1 that does not exist.")
            .arg(m_cti_address);
    } else if (errorid.toLower() == "socket_error_connectionrefused") {
        errormsg = tr("There seems to be a machine running on this IP address %1, "
                      "and either no CTI server is running, or your port %2 is wrong.")
            .arg(m_cti_address).arg(m_cti_port);
    } else if (errorid.toLower() == "socket_error_network") {
        errormsg = tr("An error occurred on the network while attempting to join the IP address %1 :\n"
                      "- no external route defined to access this IP address (~ no timeout)\n"
                      "- this IP address is routed but there is no machine (~ 5 s timeout)\n"
                      "- a cable has been unplugged on your LAN on the way to this IP address (~ 30 s timeout).")
            .arg(m_cti_address);
    } else if (errorid.toLower() == "socket_error_sslhandshake") {
        errormsg = tr("It seems that the server with IP address %1 does not accept encryption on "
                      "its port %2. Please change either your port or your encryption setting.")
            .arg(m_cti_address).arg(m_cti_port);
    } else if (errorid.toLower() == "socket_error_unknown") {
        errormsg = tr("An unknown socket error has occured while attempting to join the IP address:port %1:%2.")
            .arg(m_cti_address).arg(m_cti_port);
    } else if (errorid.startsWith("socket_error_unmanagedyet:")) {
        QStringList ipinfo = errorid.split(":");
        errormsg = tr("An unmanaged (number %1) socket error has occured while attempting to join the IP address:port %1:%2.")
            .arg(ipinfo[1]).arg(m_cti_address).arg(m_cti_port);

    // socket errors - once connected
    } else if (errorid.toLower() == "socket_error_remotehostclosed") {
        errormsg = tr("The XiVO CTI server on %1:%2 has just closed the connection.")
            .arg(m_cti_address).arg(m_cti_port);

    } else if (errorid.toLower() == "server_stopped") {
        errormsg = tr("The XiVO CTI server on %1:%2 has just been stopped.")
            .arg(m_cti_address).arg(m_cti_port);
    } else if (errorid.toLower() == "server_reloaded") {
        errormsg = tr("The XiVO CTI server on %1:%2 has just been reloaded.")
            .arg(m_cti_address).arg(m_cti_port);
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
        emit emitMessageBox(errormsg);
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
    while (m_ctiserversocket->canReadLine()) {
        QByteArray data  = m_ctiserversocket->readLine();
        m_byte_counter += data.size();
        // qDebug() << Q_FUNC_INFO << "data.size() = " << data.size();
        QString line = QString::fromUtf8(data);

        if (line.startsWith("<ui version=")) {
            // we get here when receiving a sheet as a Qt4 .ui form
            qDebug() << Q_FUNC_INFO << "(Customer Info)" << line.size();
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
        } catch(JsonQt::ParseException) {
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
    // qDebug() << Q_FUNC_INFO << infos;
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
    qDebug() << Q_FUNC_INFO << "ACCEPT" << action << src << dst;

    QVariantMap ipbxcommand;
    ipbxcommand["command"] = action;

    if ((action == "originate") || (action == "transfer") || (action == "atxfer")) {
        ipbxcommand["command"] = action;
        ipbxcommand["source"] = src;
        if ((dst == "ext:special:dialxlet") && (! m_numbertodial.isEmpty()))
            ipbxcommand["destination"] = "ext:" + m_numbertodial;
        else
            ipbxcommand["destination"] = dst;
    } else if ((action == "hangup") || (action == "transfercancel")) {
        ipbxcommand["command"] = action;
        ipbxcommand["channelids"] = src;
    } else if (action == "park") {
        ipbxcommand["command"] = action;
        ipbxcommand["source"] = src;
    } else if (action == "answer") {
        ipbxcommand["command"] = action;
        ipbxcommand["phoneids"] = src;
    } else if (action == "refuse") {
        ipbxcommand["command"] = action;
        ipbxcommand["channelids"] = src;
    }

    ipbxCommand(ipbxcommand);
}

/*! \brief Receive a number list from xlets and signal this list
 *  to interested xlets
 */
void BaseEngine::receiveNumberSelection(const QStringList & numbers)
{
    // qDebug() << Q_FUNC_INFO;
    emit broadcastNumberSelection(numbers);
}

/*! \brief send the directory search command to the server
 *
 * \sa directoryResponse()
 */
void BaseEngine::searchDirectory(const QString & text)
{
    // qDebug() << Q_FUNC_INFO << text;
    QVariantMap command;
    command["class"] = "directory";
    command["pattern"] = text;
    sendJsonCommand(command);
}

// === Getter and Setters ===
/* \brief set server address
 *
 * Set server host name and server port
 */
void BaseEngine::setAddressPort(const QString & address, quint16 port)
{
    // qDebug() << Q_FUNC_INFO << address << port;
    m_cti_address = address;
    m_cti_port = port;
}

void BaseEngine::setEncryption(bool encrypt)
{
    m_cti_encrypt = encrypt;
}

/*! \brief get server IP address */
const QString & BaseEngine::ctiAddress() const
{
    return m_cti_address;
}

/*! \brief get server port */
quint16 BaseEngine::ctiPort() const
{
    return m_cti_port;
}

bool BaseEngine::ctiEncrypt() const
{
    return m_cti_encrypt;
}

const QString & BaseEngine::company() const
{
    return m_company;
}

void BaseEngine::setCompany(const QString & companyname)
{
    m_company = companyname;
}

const QString& BaseEngine::userId() const
{
    if (m_userloginopt.size() > 0)
        return m_userloginwithopt;
    else
        return m_userlogin;
}

void BaseEngine::setUserId(const QString &userid)
{
    QStringList useridsplit = userid.split("%");
    m_userlogin = useridsplit[0].trimmed();
    m_userloginwithopt = userid;
    if (useridsplit.size() > 1) {
        m_userloginopt = useridsplit[1].trimmed();
    } else {
        m_userloginopt = "";
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

void BaseEngine::initFeatureFields(const QString & field)
{
    if ( (field == "callrecord") ||
         (field == "enablednd") ||
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
    qDebug() << Q_FUNC_INFO;
    if (m_timerid_keepalive > 0) {
        killTimer(m_timerid_keepalive);
        m_timerid_keepalive = 0;
    }
}

void BaseEngine::stopTryAgainTimer()
{
    qDebug() << Q_FUNC_INFO;
    if (m_timerid_tryreconnect > 0) {
        killTimer(m_timerid_tryreconnect);
        m_timerid_tryreconnect = 0;
    }
}

void BaseEngine::startTryAgainTimer()
{
    qDebug() << Q_FUNC_INFO;
    if (m_timerid_tryreconnect == 0 && m_trytoreconnect && ! m_forced_to_disconnect)
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
    qDebug() << Q_FUNC_INFO << timerId << m_timerid_tryreconnect;
    if (timerId == m_timerid_keepalive) {
        keepLoginAlive();
    } else if (timerId == m_timerid_tryreconnect) {
        qDebug() << Q_FUNC_INFO << m_timerid_tryreconnect;
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
    if (capa == "enablevoicemail")
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
    if (capa == "fwdunc") {
        command["function"] = "enableunc";
        command["value"] = QString(b ? "1" : "0");
        sendJsonCommand(command);
        command["function"] = "destunc";
        command["value"] = dst;
        sendJsonCommand(command);
    } else if (capa == "fwdbusy") {
        command["function"] = "enablebusy";
        command["value"] = QString(b ? "1" : "0");
        sendJsonCommand(command);
        command["function"] = "destbusy";
        command["value"] = dst;
        sendJsonCommand(command);
    } else if (capa == "fwdrna") {
        command["function"] = "enablerna";
        command["value"] = QString(b ? "1" : "0");
        sendJsonCommand(command);
        command["function"] = "destrna";
        command["value"] = dst;
        sendJsonCommand(command);
    }
}

/*! \brief send a featursget command to the cti server */
void BaseEngine::askFeatures()
{
    qDebug() << Q_FUNC_INFO;
    QVariantMap command;
    command["class"] = "featuresget";
    sendJsonCommand(command);
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
    // qDebug() << Q_FUNC_INFO;
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
    getlists = (QStringList()
                << "users" << "phones" << "trunks"
                << "agents" << "queues" << "groups" << "meetmes"
                << "voicemails" << "incalls" << "parkinglots");

    foreach (QString ipbxid, m_ipbxlist) {
        command["tipbxid"] = ipbxid;
        foreach (QString kind, getlists) {
            command["listname"] = kind;
            sendJsonCommand(command);
        }
    }

    if (m_loginkind == 2) {
        QVariantMap ipbxcommand;
        ipbxcommand["command"] = "agentlogin";
        ipbxcommand["agentids"] = "agent:special:me";
        ipbxcommand["agentphonenumber"] = m_agentphonenumber;
        ipbxCommand(ipbxcommand);
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
                emit availAllowChanged(true);
            }
            emit logged();
            // emit updatePresence();
        } else if (state == ENotLogged) {
            emit availAllowChanged(false);
            emit delogged();
            // reset some variables when disconnecting
        }
    }
}

void BaseEngine::changeWatchedAgent(const QString & agentid, bool force)
{
    // qDebug() << Q_FUNC_INFO << agentid << force;
    if ((force || (agentid.size() > 0)) && (hasAgent(agentid))) {
        emit changeWatchedAgentSignal(agentid);
    }
}

void BaseEngine::changeWatchedQueue(const QString & queueid)
{
    // qDebug() << Q_FUNC_INFO << queueid;
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


/*!
 * Send a keep alive message to the login server.
 * The message is sent in a datagram through m_udpsocket
 */
void BaseEngine::keepLoginAlive()
{
    // qDebug() << Q_FUNC_INFO << m_availstate;

//     // got to disconnected state if more than xx keepalive messages
//     // have been left without response.
//     if (m_pendingkeepalivemsg > 1) {
//         qDebug() << Q_FUNC_INFO << m_pendingkeepalivemsg << "=> 0";
//         stopKeepAliveTimer();
//         setState(ENotLogged);
//         m_pendingkeepalivemsg = 0;
//         popupError("no_keepalive_from_server");
//         startTryAgainTimer();
//         return;
//     }

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

/*! \brief send message to the CTI Server */
void BaseEngine::logClient(const QString & level,
                           const QString & classmethod,
                           const QString & message)
{
    // qDebug() << Q_FUNC_INFO << classmethod << message;
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
    // qDebug() << Q_FUNC_INFO << m_ipbxid << m_userid << m_xuserid;
    if (m_anylist.value("users").contains(m_xuserid)) {
        return (UserInfo *) m_anylist.value("users").value(m_xuserid);
    }
    return NULL;
}

UserInfo * BaseEngine::getXivoClientMonitored()
{
    // qDebug() << Q_FUNC_INFO << m_ipbxid << m_userid << m_xuserid;
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
    // qDebug() << Q_FUNC_INFO << xcid;
    foreach (const QString xuid, b_engine->iterover("users").keys()) {
        const UserInfo * user = b_engine->user(xuid);
        foreach (const QString pid, user->phonelist()) {
            const PhoneInfo * phone = b_engine->phone(pid);
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
        QString key = QString("HKEY_CLASSES_ROOT\\%1\\shell\\open\\command").arg(url.scheme())
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

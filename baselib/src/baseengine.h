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

#ifndef __BASEENGINE_H__
#define __BASEENGINE_H__

#include "baselib_export.h"

#include <QHash>
#include <QMultiHash>
#include <QObject>
#include <QTime>
#include <QSettings>

#include "queue_agent_status.h"
#include "dstore/dstore.h"

#include "channelinfo.h"
#include "incallsinfo.h"
#include "meetmeinfo.h"
#include "parkinginfo.h"
#include "phoneinfo.h"
#include "queueinfo.h"
#include "agentinfo.h"
#include "trunkinfo.h"
#include "groupinfo.h"
#include "voicemailinfo.h"
#include "userinfo.h"
#include "baseconfig.h"
#include "ipbxlistener.h"

class QApplication;
class QDateTime;
class QFile;
class QSettings;
class QSocketNotifier;
class QSslError;
class QSslSocket;
class QTcpSocket;
class QTimerEvent;
class QTranslator;
class QUdpSocket;
class QVariant;

/*! \brief Class which handles connection with the XiVO CTI server
 *
 *  The engine object contains all the code to
 *  handle network connection and requests
 *
 *  Config options (available through getConfig):
 *  - (bool) \b systrayed = main window starts systrayed
 *  - (bool) \b uniqueinstance = forbids multiple clients running simultaneously on the same machine
 *  - (string) \b qss = Qt style sheet, changes appearance of the client. It is the name of a file in \code :/common/ *.qss \endcode, without path nor extension.
 *  - (bool) \b enableclipboard = enable the clipboard or not
 *  - (bool) \b logtofile = enable events logging
 *  - (string) \b logfilename = log file name, no path
 *  - (bool) \b activate_on_tel = activate the window when calling from an extern application through the client
 *  - (string) \b profilename = config profile used
 *  - (string) \b cti_address = CTI server address
 *  - (int) \b cti_port = CTI server port
 *  - (bool) \b cti_encrypt = encrypt communication to CTI server
 *  - (string) \b userlogin = login\%opt
 *  - (string) \b userloginsimple = login
 *  - (string) \b userloginopt = opt
 *  - (string) \b password = password
 *  - (string) \b company = user's company (context)
 *  - (bool) \b keeppass = keep password in config file
 *  - (bool) \b showagselect = show agent combobox on main window
 *  - (string) \b agentphonenumber = agent phone number
 *  - (string) \b forcelocale = language choice. Can be 'default' or 'en_US', 'fr_FR' ... Plugin localization is automatic, you should not have to use this.
 *  - (bool) \b autoconnect = connect on starting client
 *  - (bool) \b trytoreconnect = automatic reconnection
 *  - (int) \b trytoreconnectinterval = reconnection attempts interval (ms)
 *  - (int) \b keepaliveinterval = heartbeat intervals (ms)
 *  - (bool) \b displayprofile = show the config profile used (bottom right of the main window)
 *  - (int) \b historysize = max number of lines displayed in XLetHistoryPlugin
 *  - (bool) \b guioptions.autourl_allowed = enable loading web page on incoming call
 *  - (int) \b guioptions.contacts-max = max number of contacts displayed in SearchPanel
 *  - (int) \b guioptions.contacts-width = number of columns to display contacts in SearchPanel
 *  - (int) \b guioptions.loginkind = index of agent combobox (0 = no agent, 1 = agent not logged, 2 = agent logged)
 *  - (string) \b guioptions.loginwindow.url = url to load when connecting to the server
 *  - (int) \b guioptions.maxwidthwanted = maximum graphical width of contacts (px)
 *  - (int) \b guioptions.presenceindicatorsize = size of contacts presence indicator (px)
 *  - (bool) \b guioptions.queue_displaynu = display queue numbers
 *  - (bool) \b guioptions.queue_longestwait = display longest wait queue
 *  - (map)(int) \b guioptions.queuelevels = number of queued calls threshold to change queue color (indices: green and orange)
 *  - (map)(int) \b guioptions.queuelevels_wait = waiting time threshold to change queue color (indices: green and orange)
 *  - (map)(list) \b guioptions.server_funcs = server enabled functions (index: functions)
 *  - (int) \b guioptions.sheet-tablimit = max number of tab in presence reporting
 *  - (string) \b guioptions.switchboard-elt-type = switchboard display method (values: small, detailed)
 *  - (bool) \b guioptions.xlet_operator_answer_work = show answer button
 *  - (int) \b guioptions.xlet_operator_keyanswer = key code to answer
 *  - (int) \b guioptions.xlet_operator_keyatxfercancel = key code to cancel transfer
 *  - (int) \b guioptions.xlet_operator_keyatxferfinalize = key code to finalize a transfer
 *  - (int) \b guioptions.xlet_operator_keydtransfer = key code to direct transfer
 *  - (int) \b guioptions.xlet_operator_keyhangup = key code to hang up
 *  - (int) \b guioptions.xlet_operator_keyicancel = key code to cancel ?
 *  - (int) \b guioptions.xlet_operator_keyilink = key code to join
 *  - (int) \b guioptions.xlet_operator_keyitransfer = key code to indirect transfer
 *  - (int) \b guioptions.xlet_operator_keypark = key code to park
 *  - (bool) \b checked_function.presence = presence function enabled ?
 *  - (bool) \b checked_function.customerinfo = customerinfo function enabled ?
 */
class BASELIB_EXPORT BaseEngine: public QObject
{
    Q_OBJECT

    public:
        //! Enum for BaseEngine state logged/not logged
        typedef enum {ENotLogged, ELogged} EngineState;

        BaseEngine(QSettings *, const QString &);  //! Constructor
        ~BaseEngine();  //! Destructor
        
        
        // public config and settings
        
        QSettings* getSettings();
        QVariant getProfileSetting(const QString &, const QVariant & = QVariant()) const;  //!< get one setting in current profile
        void setProfileSetting(const QString &, const QVariant &);  //!< set one setting in current profile
        
        QVariantMap getConfig() const;              //!< all BaseEngine settings.
        QVariant getConfig(const QString &) const;  //!< one BaseEngine setting. Use to read a setting.
        void setConfig(const QVariantMap &);                //!< set as many settings as you want


        // public setters/getters for properties

        EngineState state();               //!< Engine state (Logged/Not Logged)
        void setState(EngineState state);  //!< see state()

        const QString& getAvailState() const;  //!< returns availability status
        
        void setCheckedFunction(const QString &, bool b);  //!< set m_checked_function
        bool checkedFunction(const QString &);             //!< get m_checked_function
        void setEnabledFunction(const QString &, bool b);  //!< set m_enabled_function
        bool enabledFunction(const QString &);             //!< on m_capafuncs

        const QVariantList & getCapaXlets() const;
        
        const QStringList & getCapasRegCommands() const;
        const QStringList & getCapasIpbxCommands() const;
        
        const QVariantMap & getOptionsUserStatus() const;
        const QVariantMap & getOptionsPhoneStatus() const;
        const QVariantMap & getOptionsChannelStatus() const;
        const QVariantMap & getOptionsAgentStatus() const;

        const QString & getCapaApplication() const;
        const QString osname() const { return m_osname; };
        const QString ipbxid() const { return m_ipbxid; };

        const QString & xivoUserId() const { return m_userid; };
        const QString & getFullId() const { return m_xuserid; };
        const UserInfo * getUserForXChannelId(const QString & xcid) const;  //!< Returns a user for a given xchannelid
        UserInfo * getXivoClientUser();       //!< Return the user of the XiVO CTI Client
        UserInfo * getXivoClientMonitored();  //!< Return the monitored user
        
        double timeServer() const;
        const QDateTime & timeClient() const;
        double timeDeltaServerClient() const;
        QString timeElapsed(double) const;

        bool hasPhone(const QString & xid) { return m_anylist.value("phones").contains(xid); };
        bool hasAgent(const QString & xid) { return m_anylist.value("agents").contains(xid); };
        bool hasQueue(const QString & xid) { return m_anylist.value("queues").contains(xid); };

        QHash<QString, XInfo *> iterover(const QString & listname) { return m_anylist.value(listname); };

        const ChannelInfo * channel(const QString & id) const      //!< Return the channel to any xlet
            { return channels().value(id); };
        const UserInfo * user(const QString & id) const            //!< Return the user to any Xlet
            { return (const UserInfo *) m_anylist.value("users").value(id); };
        const PhoneInfo * phone(const QString & id) const          //!< Return the phone to any Xlet
            { return (const PhoneInfo *) m_anylist.value("phones").value(id); };
        const TrunkInfo * trunk(const QString & id) const          //!< Return the trunk to any Xlet
            { return (const TrunkInfo *) m_anylist.value("trunks").value(id); };
        const AgentInfo * agent(const QString & id) const          //!< Return the agent to any Xlet
            { return (const AgentInfo *) m_anylist.value("agents").value(id); };
        const QueueInfo * queue(const QString & id) const         //!< Return the queue to any Xlet
            { return (const QueueInfo *) m_anylist.value("queues").value(id); };
        const GroupInfo * group(const QString & id) const          //!< Return the group to any Xlet
            { return (const GroupInfo *) m_anylist.value("groups").value(id); };
        const MeetmeInfo * meetme(const QString & id) const        //!< Return the meetme to any Xlet
            { return (const MeetmeInfo *) m_anylist.value("meetmes").value(id); };
        const ParkingInfo * parkinglot(const QString & id) const   //!< Return the meetme to any Xlet
            { return (const ParkingInfo *) m_anylist.value("parkinglots").value(id); }
        const VoiceMailInfo * voicemail(const QString & id) const  //!< Return the voicemail to any Xlet
            { return (const VoiceMailInfo *) m_anylist.value("voicemails").value(id); };

        const QHash<QString, ChannelInfo *> & channels() const
                { return m_channels; };      //!< Return the channels to any Xlet
        const QHash<QString, QueueMemberInfo *> & queuemembers() const
                { return m_queuemembers; };  //!< Return the queue members to any Xlet
        
        
        // public operations
        
        void pasteToDial(const QString &);
        
        void registerListener(const QString &, IPBXListener *); //!< Register an XLet wanting to listen IPBX messages
        
        void sendJsonCommand(const QVariantMap &);

        QStringList phonenumbers(const UserInfo *);

        void registerTranslation(const QString &);
        void changeTranslation(QString locale = "");
        
        void urlAuto(const QString &);
        
        void changeWatchedAgent(const QString &, bool);
        void changeWatchedQueue(const QString &);
    private:
    
        
        // private getters/setters
        
        void setUserLogin(const QString &);                   //!< see userid()
        void setUserLogin(const QString &, const QString &);  //!< set userid and userid option
        
        uint port_to_use() const;  //!< gives the right port, according to encryption setting
        
        void setOSInfos(const QString &);
        
        
        // private operations
    
        void loadSettings();  //!< load server settings from QSettings (conf file)
        void saveSettings();  //!< save server settings into QSettings (conf file)
        
        int forwardToListeners(QString className, const QVariantMap &map); //!< forward IPBX message to XLets listening
        
        void stopConnection();     //!< stop the engine
        void clearInternalData();  //!< clear the engine internal data
        
        void openLogFile();

    public slots:
    
    
        // public getters/setters slots
        
        void setAvailState(const QString &, bool); //!< set m_availstate
        
        void setAvailability();  //!< set user status from menu
        
        // public operations slots
        
        void start(); //!< start the connection process.
        void stop();  //!< stop the engine
        
        void powerEvent(const QString &);
        
        void handleOtherInstanceMessage(const QString &);
        
        void actionCall(const QString &, const QString &src="", const QString &dst="");
        void actionDialNumber(const QString &);
        
        void receiveNumberSelection(const QStringList &);  //!< relay the selection

        void searchDirectory(const QString &);
        
        void textEdited(const QString &);
        
        void featurePutOpt(const QString &, bool);
        void featurePutForward(const QString &, bool, const QString &);
        void askFeatures();
        
        void fetchIPBXList();
        void fetchLists();
        
        void sendFaxCommand(const QString &, const QString &, Qt::CheckState);
        
        void meetmeAction(const QString &, const QString &);
        
        void requestFileList(const QString &);
        
        void monitorPeerRequest(const QString &);
        
        void saveToFile(const QString &);
        
        void loadQueueGroups();
        void saveQueueGroups(const QVariant &);
        
        void logAction(const QString &);  //!< client side logging
        
        void logClient(const QString &, const QString &, const QString &);  //!< log tricky situations (server side)
        
        void sendNewRemark(const QString &, const QString &);
        
        void ipbxCommand(const QVariantMap &);
        
        void emitMessage(const QString &);
        
        void actionFromFiche(const QVariant &);

        // ssl-related slots
        void encryptedSsl();
        // void sslSocketReadyRead();
        void sslErrors(const QList<QSslError> &);

    private slots:
        void keepLoginAlive();  //!< Keep session alive
        
        void changeState();  //!< Change the presence status
        
        void ctiSocketConnected();
        void ctiSocketReadyRead();
        
        void filetransferSocketConnected();
        void filetransferSocketReadyRead();
        
        void sheetSocketConnected();

    signals:
        void settingsChanged();  //!< signal emitted when the setting are changed
        
        void logged();    //!< signal emitted when the state becomes ELogged
        void delogged();  //!< signal emitted when the state becomes ENotLogged
        
        void availAllowChanged(bool);  //!< presence function has been enabled/disabled
        
        void emitTextMessage(const QString &);  //!< message to be displayed to the user.
        
        void pasteToXlets(const QString &);  //!< Xlets intercept this signal from paste to dial
        
        void parkingEvent(const QString &,
                          const QString &,
                          const QString &,
                          const QVariant &);
                          
        void ackFax(const QString &, const QString &);
        
        void monitorPeerChanged();
        
        void updatePresence();
        
        void requestFileListResult(const QVariant &);  //!< needed by callcampaign
        
        void serverFileList(const QStringList &);  //!< needed by agentdetails
        void fileReceived();                       //!< needed by agentdetails
        
        void statusRecord(const QString &, const QString &, const QString &);
        void statusListen(const QString &, const QString &, const QString &);
        
        void emitMessageBox(const QString &);
        
        void setQueueGroups(const QVariant &);
        
        void peersReceived();  //!< list of peer was received

        void updatePhoneConfig(const QString &);
        void updatePhoneStatus(const QString &);
        void updateUserConfig(const QString &);
        void updateUserStatus(const QString &);
        void updateAgentConfig(const QString &);
        void updateAgentStatus(const QString &);
        void updateQueueConfig(const QString &);
        void updateQueueStatus(const QString &);
        void updateVoiceMailConfig(const QString &);
        void updateVoiceMailStatus(const QString &);
        void updateChannelStatus(const QString &);
        void updateMeetmesConfig(const QString &);
        void updateMeetmesStatus(const QString &);
        void updateParkinglotConfig(const QString &);
        void updateParkinglotStatus(const QString &);
        void removePhoneConfig(const QString &);
        void removeUserConfig(const QString &);
        void removeAgentConfig(const QString &);
        void removeQueueConfig(const QString &);

        void newQueueList(const QStringList &);
        void removeQueues(const QString &, const QStringList &);
        
        void newAgentList(const QStringList &);
        
        void directoryResponse(const QStringList &, const QStringList &);  //! the directory search response has been received.
        
        void disconnectFeatures();
        void connectFeatures();
        void resetFeatures();
        void featurePutIsKO();
        void featurePutIsOK();
        
        void localUserInfoDefined();
        void monitoredUserInfoDefined();
        
        void optChanged(const QString &);
        void forwardUpdated(const QString &);
        
        void changesAvailChecks();
        
        void changeWatchedAgentSignal(const QString &);
        void changeWatchedQueueSignal(const QString &);
        
        void displayFiche(const QString &, bool, const QString &);
        
        void gotSheetOwnership(const QString &id);                     //!< the user logged has now ownership of the sheet
        void lostSheetOwnership(const QString &id);                    //!< the user logged just lost ownership of the sheet
        void sheetEntryAdded(const QString &id, const QVariantMap &);  //!< sheet data added by a user.
        
        void broadcastNumberSelection(const QStringList &);  //!< numbers for a selected peer

    protected:
        void timerEvent(QTimerEvent *);  //!< receive timer events

    private:
        void stopKeepAliveTimer();  //!< Stop the keep alive timer if running
        void startTryAgainTimer();  //!< Start the "try to reconnect" timer
        void stopTryAgainTimer();   //!< Stop the "try to reconnect" timer
        
        void popupError(const QString &);
        
        void initFeatureFields(const QString &);
        
        void connectSocket();
        void sendCommand(const QString &);
        void parseCommand(const QString &);
        void configsLists(const QString &, const QString &, const QVariantMap &);
        void updatePhone(const QString &, const QString &, const QVariantMap &);
        
        void clearLists();
        void clearChannelList();

        // Class Members
        // Parameters given by the User at Login time
        
        BaseConfig m_config;            //!< Stores the configuration values.
        
        QString m_xuserid;              //!< Full Id (userid + company)
        QString m_ipbxid;               //!< IPBX id of the current user
        QString m_userid;               //!< xivo user id of the current user
        QString m_profilename_read;     //!< CTI profile name of the current user
        QString m_profilename_write;    //!< CTI profile name of the current user
        QStringList m_ipbxlist;

        QDateTime m_timeclt;
        double m_timesrv;

        QString m_locale;
        QStringList translationFiles;   //!< List of translation files
        QVector<QTranslator *> translators;   //!< Vector of translators

        QHash<QString, bool> m_enabled_function;  //!< function enabled

        // Replies given by the server
        QStringList m_capafuncs;         //!< List of func capabilities issued by the server after a successful login
        QVariantList m_capaxlets;        //!< List of xlet capabilities issued by the server after a successful login
        QVariantMap m_options_userstatus;    //!< Display Options for User statuses (presence)
        QVariantMap m_options_phonestatus;   //!< Display Options for Phone statuses
        QVariantMap m_options_channelstatus; //!< Display Options for Channel statuses
        QVariantMap m_options_agentstatus;   //!< Display Options for Agent statuses
        QStringList m_capas_regcommands;  //!< Capabilities for regular commands
        QStringList m_capas_ipbxcommands; //!< Capabilities for IPBX commands

        QString m_appliname;            //!< Application name to be displayed
        QString m_sessionid;            //!< Session id obtained after a successful login

        // Status variables
        EngineState m_state;            //!< State of the engine (Logged/Not Logged)
        QString m_availstate;           //!< Availability state to send to the server

        // QSslSocket * m_sslsocket;

        // Internal management
        QSslSocket * m_ctiserversocket;     //!< Connection to the CTI server
        QTcpSocket * m_filetransfersocket;  //!< TCP connection for File transfer.
        QTcpSocket * m_tcpsheetsocket;  //!< TCP connection for Sheet sockets
        QUdpSocket * m_udpsheetsocket;  //!< UDP connection for Sheet sockets
        int m_timerid_keepalive;        //!< timer id for keep alive
        int m_timerid_tryreconnect;     //!< timer id for try to reconnect
        int m_timerid_changestate;      //!< timer id for changing state automatically
        QString m_changestate_oldstate; //!< old state when changing state automatically
        QString m_changestate_newstate; //!< new state when changing state automatically
        int m_pendingkeepalivemsg;      //!< number of keepalivemsg sent without response
        QString m_numbertodial;         //!< Number dialed in
        QString m_osname;               //!< OS informations

        QString m_fileid;
        QString m_filename;
        QString m_filedir;
        QByteArray m_filedata;
        int m_faxsize;

        QString m_monitored_xuserid;  //!< UserId of the Monitored user
        QSettings * m_settings;  //!< Settings (stored in .ini file)
        QFile * m_eventdevice;
        QByteArray m_downloaded;    //!< downloaded data
        QFile * m_logfile;
        
        int m_byte_counter; //!< byte counter for calculating network throughput
        QTime m_time;       //!< time counter for calculating network throughput
        bool m_attempt_loggedin;
        int m_rate_bytes;   //!< byte counter to calculate Json decode throughput
        int m_rate_msec;    //!< time counter to calculate Json decode throughput
        int m_rate_samples; //!< number of Json decode
        bool m_forced_to_disconnect;    //!< set to true when disconnected by server
        
        QMultiHash<QString, IPBXListener*> m_listeners;

        // miscellaneous statuses to share between xlets
        QHash<QString, newXInfoProto> m_xinfoList;  //!< XInfo constructors
        QHash<QString, QHash<QString, XInfo *> > m_anylist;
        QHash<QString, ChannelInfo *> m_channels;  //!< List of Channel informations
        QHash<QString, QueueMemberInfo *> m_queuemembers;  //!< List of Channel informations
        
        

        DStore * m_tree;
    public:
        DStore* tree() { return m_tree; };
        QVariant eV(const QString &req) {
                return tree()->extractVariant(req);
        }
        QVariantMap eVM(const QString &req) {
                return tree()->extractVMap(req);
        }

    friend class CtiConn;
};

extern BASELIB_EXPORT BaseEngine *b_engine;
#define nelem(x)    static_cast<int>(sizeof(x)/sizeof(x)[0])

#endif /* __BASEENGINE_H__ */

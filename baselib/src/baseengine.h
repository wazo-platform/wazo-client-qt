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

#include <baselib_export.h>

#include <QHash>
#include <QObject>
#include <QSettings>
#include <QStringList>
#include <QMultiHash>
#include <QTime>
#include <QDateTime>
#include <QVariant>
#include <QVariantMap>

class QApplication;
class QFile;
class QTimerEvent;
class QTranslator;
class QSslSocket;
class QSslError;

class Xlet;

#include <userinfo.h>
#include <phoneinfo.h>
#include <agentinfo.h>
#include <queueinfo.h>
#include <meetmeinfo.h>
#include <parkinginfo.h>
#include <channelinfo.h>

#include <dstore/src/dstore.h>

struct e_callback {
    void (*cb)(const QVariantMap &, void *);
    void *udata;
};

class QSocketNotifier;
class QTcpSocket;

/*! \brief Class which handles connection with the XiVO CTI server
 *
 *  The engine object contains all the code to
 *  handle network connection and requests */
class BASELIB_EXPORT BaseEngine: public QObject
{
    Q_OBJECT

    public:
        //! Enum for BaseEngine state logged/not logged
        typedef enum {ENotLogged, ELogged} EngineState;

        BaseEngine(QSettings *, const QString &); //! Constructor
        ~BaseEngine(); //! Destructor

        QSettings* getSettings();
        void loadSettings();                   //!< load server settings
        // setter/getter for properties
        //! set address used to connect to the server
        void setAddressPort(const QString &, quint16);
        void setEncryption(bool);
        const QString& ctiAddress() const;     //!< IP address of the login server
        quint16 ctiPort() const;               //!< TCP port of the login server
        bool ctiEncrypt() const;

        const QString & company() const;       //!< name of the user's company
        void setCompany(const QString &);      //!< see company()
        const QString & userId() const;        //!< userid to identify to the server
        void setUserId(const QString &);       //!< see userid()
        const QString & agentphonenumber() const;  //!< agent's phone number
        void setAgentPhoneNumber(const QString &); //!< see agentphonenumber()
        int loginkind();                        //!< loginkind to identify to the server
        void setLoginKind(const int);           //!< see loginkind()
        int keeppass();                         //!< keeppass to identify to the server
        void setKeepPass(int);            //!< see keeppass()
        int showagselect();                     //!< showagselect to identify to the server
        void setShowAgentSelect(const int);     //!< see showagselect()
        const QString & password() const;       //!< password to identify to the sever
        void setPassword(const QString &);      //!< see password()

        QString forcelocale() const;               //!< force locale string
        void setForcelocale(QString);              //!< set force locale
        bool autoconnect() const;               //!< auto connect flag
        void setAutoconnect(bool);              //!< set auto connect flag
        bool trytoreconnect() const;            //!< try to reconnect flag
        void setTrytoreconnect(bool);           //!< set try to reconnect flag
        uint trytoreconnectinterval() const;    //!< try to reconnect interval
        void setTrytoreconnectinterval(uint);   //!< set try to reconnect interval
        uint historySize() const;               //!< history size
        void setHistorySize(uint size);         //!< set history size
        bool systrayed() const;                 //!< systrayed flag
        void setSystrayed(bool);                //!< set systrayed flag

        void saveSettings();                    //!< save server settings

        EngineState state();      //!< Engine state (Logged/Not Logged)
        void setState(EngineState state);       //!< see state()

        const QString& getAvailState() const;        //!< returns availability status
        void setCheckedFunction(const QString &, bool b);        //!< set m_checked_function
        bool checkedFunction(const QString &);                   //!< get m_checked_function
        void setEnabledFunction(const QString &, bool b);        //!< set m_enabled_function
        bool enabledFunction(const QString &);                   //!< on m_capafuncs

        uint keepaliveinterval() const;           //!< keep alive interval
        bool lastconnwins() const;                //!< last connected one wins
        void setLastConnWins(bool b);             //!< last connected user wins

        bool hasCapaFun(QString &);                //!< 0 we don't have the fun, 1 we got it

        const QStringList & getCapaXlets() const;
        const QVariantMap & getCapaPresence() const;
        const QVariantMap & getCapaTermStates() const;

        void setGuiOption(const QString &, const QVariant &);
        const QVariantMap getGuiOptions(const QString &) const;

        const QString& getCapaApplication() const;
        void configAndStart(const QString &, const QString &, const QString &);
        QString osname() const { return m_osname; };

        void setLogFile(const QString &);

        const QString & xivoUserId() const { return m_userid; };
        const QString & getFullId() const { return m_xuserid; };
        UserInfo* getXivoClientUser();  //!< Return the user of the Xivo CTI Client
        double timeServer() const;
        const QDateTime & timeClient() const;
        double timeDeltaServerClient() const;
        QString timeElapsed(double) const;
        int m_historysize;  //!< Number of elements when requestion call log

        void pasteToDial(const QString &);
        void registerClassEvent(const QString &class_function,
                                void (*)(const QVariantMap &map, void *udata), void *udata);
        void sendJsonCommand(const QVariantMap &);

        const QHash<QString, AgentInfo *> agents() const
                { return m_agents; }; //!< Return the agents to any Xlet
        const QHash<QString, QueueInfo *> queues() const
                { return m_queues; }; //!< Return the queues to any Xlet
        const QHash<QString, PhoneInfo *> phones() const
                { return m_phones; }; //!< Return the phones to any Xlet
        const QHash<QString, UserInfo *>  users() const
                { return m_users; };  //!< Return the users to any Xlet
        const QHash<QString, ChannelInfo *>  channels() const
                { return m_channels; };  //!< Return the channels to any Xlet
        const QHash<QString, QHash<QString, ParkingInfo *> > parking() const
                { return m_parking; }; //!< Return the parking to any Xlet

        void registerTranslation(const QString &);
        void changeTranslation(const QString &);
        void sendUrlToBrowser(const QString &);
        void addToDataBase(QVariantMap &);
        void changeWatchedAgent(const QString &, bool);
        void changeWatchedQueue(const QString &);

    private:
        int callClassEventCallback(QString className, const QVariantMap &map);
        void stopConnection();   //!< stop the engine
        void clearInternalData();   //!< clear the engine internal data
        void setOSInfos(const QString &);

        QMultiHash<QString, e_callback* > m_class_event_cb;

    public slots:
        void start();  //!< start the connection process.
        void stop();   //!< stop the engine
        void powerEvent(const QString &);
        //! set m_availstate
        void setAvailState(const QString &, bool);
        void actionCall(const QString &, const QString &src="", const QString &dst="");

        void searchDirectory(const QString &);
        void textEdited(const QString &);
        void setAvailability();  //!< set user status from menu
        void featurePutOpt(const QString &, bool);
        void featurePutForward(const QString &, bool, const QString &);
        void askFeatures();
        void askCallerIds0();
        void askCallerIds();
        void setKeepaliveinterval(uint);  //!< set keep alive interval
        void sendFaxCommand(const QString &, const QString &, Qt::CheckState);
        void meetmeAction(const QString &, const QString &);
        void requestFileList(const QString &);

        void monitorPeerRequest(const QString &);
        void saveToFile(const QString &);
        void saveQueueGroups(const QVariant &);
        void loadQueueGroups();
        void logAction(const QString &);
        void logClientWarning(const QString &,
                              const QString &);  //!< log tricky situations
        void sendNewRemark(const QString &, const QString &);
        void handleOtherInstanceMessage(const QString &);
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
        void filetransferSocketReadyRead();
        void filetransferSocketConnected();

    signals:
        void settingChanged(const QVariantMap &);  //!< signal emitted when the setting are changed
        void logged();    //!< signal emitted when the state becomes ELogged
        void delogged();  //!< signal emitted when the state becomes ENotLogged
        void availAllowChanged(bool);            //!< signal
        void emitTextMessage(const QString &);   //!< message to be displayed to the user.
        void pasteToDialPanel(const QString &);  //!< the dial panel intercept this signal if he exist
        void parkingEvent(const QString &,
                          const QString &,
                          const QString &,
                          const QVariant &);
        void ackFax(const QString &, const QString &);
        void featurePutIsKO();
        void featurePutIsOK();
        void monitorPeer(UserInfo *);
        void requestFileListResult(const QVariant &);
        void updatePresence();
        void serverFileList(const QStringList &);
        void fileReceived();
        void statusRecord(const QString &, const QString &, const QString &);
        void statusListen(const QString &, const QString &, const QString &);
        void emitMessageBox(const QString &);
        void setQueueGroups(const QVariant &);

        //! call list is updated
        //void callsUpdated();
        //! list of peer was received
        void peersReceived();
        //! the server requested a peer remove
        //void removePeer(const QString &);

        void updatePhoneConfig(const QString &);
        void updatePhoneStatus(const QString &);
        void updateUserConfig(const QString &);
        void updateUserStatus(const QString &);
        void updateAgentConfig(const QString &);
        void updateAgentStatus(const QString &);
        void updateQueueConfig(const QString &);
        void updateQueueStatus(const QString &);
        void updateChannelStatus(const QString &);

        void newQueueList(const QStringList &);
        void newAgentList(const QStringList &);
        void directoryResponse(const QStringList &, const QStringList &);  //! the directory search response has been received.
        void disconnectFeatures();
        void connectFeatures();
        void resetFeatures();
        void localUserInfoDefined(const UserInfo *);
        void removeQueues(const QString &, const QStringList &);
        void optChanged(const QString &, bool);
        void forwardUpdated(const QString &, const QVariant &);
        void changesAvailChecks();
        void changeWatchedAgentSignal(const QString &);
        void changeWatchedQueueSignal(const QString &);
        void displayFiche(const QString &, bool, const QString &);
        void gotSheetOwnership(const QString &id);  //! the user logged has now ownership of the sheet
        void lostSheetOwnership(const QString &id);  //! the user logged just lost ownership of the sheet
        void sheetEntryAdded(const QString &id, const QVariantMap &);  //! sheet data added by a user.

    protected:
        void timerEvent(QTimerEvent *);  //!< receive timer events

    private:
        void stopKeepAliveTimer();  //!< Stop the keep alive timer if running
        void popupError(const QString &);
        void startTryAgainTimer();  //!< Start the "try to reconnect" timer
        void stopTryAgainTimer();   //!< Stop the "try to reconnect" timer
        void initFeatureFields(const QString &, const QVariant &);
        void connectSocket();
        void sendCommand(const QString &);
        void parseCommand(const QString &);
        void configsLists(const QString &, const QString &, const QVariantMap &);
        void updatePhone(const QString &, const QString &,
                         const QVariantMap &);
        QStringList updateQueue(const QString &, const QString &,
                                const QVariantMap &);
        QStringList updateQueueAgent(const QString &, const QString &,
                                     const QVariantMap &);
        QStringList updateAgent(const QString &, const QString &,
                                const QVariantMap &);
        QStringList updateAgentQueue(const QString &, const QString &,
                                     const QVariantMap &);
        void clearUserList();
        void clearPhoneList();
        void clearAgentList();
        void clearQueueList();
        void clearChannelList();

        // Class Members
        // Parameters given by the User at Login time
        QString m_cti_address;          //!< IP address to the login server
        quint16 m_cti_port;             //!< TCP port to connect to server
        bool m_cti_encrypt;             //!< Encrypt CTI connection ?

        QString m_userlogin;            //!< User Id
        QString m_userloginopt;         //!< User Id Option (kind of login)
        QString m_userloginwithopt;     //!< User Id Option (kind of login)
        QString m_company;
        QString m_password;             //!< User password for account
        QString m_agentid;              //!< Agent Id
        QString m_agentphonenumber;     //!< Agent's phone number
        int m_loginkind;                //!< Login Kind
        int m_keeppass;                 //!< Keep password ?
        int m_showagselect;             //!< Show agent selection ?
        QString m_xuserid;              //!< Full Id (userid + company)
        QString m_ipbxid;               //!< IPBX id of the current user
        QString m_userid;               //!< xivo user id of the current user
        QString m_profilename_read;     //!< CTI profile name of the current user
        QString m_profilename_write;    //!< CTI profile name of the current user

        QDateTime m_timeclt;
        double m_timesrv;

        QString m_forcelocale;          //!< Force locale string
        QStringList translationFiles;   //!< List of translation files
        QVector<QTranslator *> translators;   //!< Vector of translators
        bool m_autoconnect;             //!< Autoconnect to server at startup
        bool m_trytoreconnect;          //!< "try to reconnect" flag
        bool m_systrayed;               //!< "systrayed at startup" flag
        uint m_trytoreconnectinterval;  //!< Try to reconnect interval (in msec)
        uint m_keepaliveinterval;       //!< Keep alive interval (in msec)

        QHash<QString, bool> m_checked_function;  //!< function checked
        QHash<QString, bool> m_enabled_function;  //!< function enabled
        bool m_checked_lastconnwins;           //!< the last connected account "wins"

        // Replies given by the server
        QStringList m_capafuncs;        //!< List of func capabilities issued by the server after a successful login
        QStringList m_capaxlets;        //!< List of xlet capabilities issued by the server after a successful login
        QVariantMap m_capapresence;     //!< List of capabilities issued by the server for the presence statuses
        QVariantMap m_capatermstates;   //!< List of capabilities issued by the server for the termination statuses
        QVariantMap m_guioptions;       //!< List of GUI options
        QString m_appliname;            //!< Application name to be displayed
        QString m_sessionid;            //!< Session id obtained after a successful login
        QString m_forced_state;         //!< Forced state sent by the server

        // Status variables
        EngineState m_state;            //!< State of the engine (Logged/Not Logged)
        QString m_availstate;           //!< Availability state to send to the server

        // QSslSocket * m_sslsocket;

        // Internal management
        QSslSocket * m_ctiserversocket;     //!< Connection to the CTI server
        QTcpSocket * m_filetransfersocket;  //!< TCP connection for File transfer.
        int m_timerid_keepalive;        //!< timer id for keep alive
        int m_timerid_tryreconnect;     //!< timer id for try to reconnect
        int m_timerid_changestate;      //!< timer id for changing state automatically
        QString m_changestate_oldstate; //!< old state when changing state automatically
        QString m_changestate_newstate; //!< new state when changing state automatically
        int m_pendingkeepalivemsg;      //!< number of keepalivemsg sent without response
        QString m_numbertodial;         //!< Number dialed in
        QString m_osname;               //!< OS informations

        QString m_fileid;
        QString m_filedir;
        QByteArray m_filedata;
        int m_faxsize;

        QString m_monitored_userid;  //!< UserId of the Monitored Phone (on SB, or one's own on XC)
        QSettings * m_settings;  //!< Settings (stored in .ini file)
        QFile * m_eventdevice;
    //    QSocketNotifier * m_notifier;
        QByteArray m_downloaded;    //!< downloaded data
        QFile * m_logfile;
        int m_byte_counter; //!< byte counter for calculating network throughput
        QTime m_time;       //!< time counter for calculating network throughput
        bool m_attempt_loggedin;
        int m_rate_bytes;   //!< byte counter to calculate Json decode throughput
        int m_rate_msec;    //!< time counter to calculate Json decode throughput
        int m_rate_samples; //!< number of Json decode
        bool m_forced_to_disconnect;    //!< set to true when disconnected by server

        // miscellaneous statuses to share between xlets
        QHash<QString, UserInfo *> m_users;    //!< List of User Informations
        QHash<QString, PhoneInfo *> m_phones;  //!< List of Phone informations
        QHash<QString, ChannelInfo *> m_channels;  //!< List of Channel informations

        QHash<QString, AgentInfo *> m_agents;  //!< List of Agent informations
        QHash<QString, QueueInfo *> m_queues;  //!< List of Queue informations
/*         QHash<QString, GroupInfo *> m_groups;  //!< List of Group informations */
/*         QHash<QString, VoicemailInfo *> m_voicemails;  //!< List of Queue informations */
/*         QHash<QString, TrunkInfo *> m_trunks;  //!< List of Queue informations */
        QHash<QString, QHash<QString, ParkingInfo *> > m_parking; //! parking bays

        DStore *m_tree;
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

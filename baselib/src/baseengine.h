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
#include <QMultiHash>
#include <QObject>
#include <QTime>

#include "queue_agent_status.h"
#include "dstore/src/dstore.h"

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

class Xlet;

struct e_callback {
    void (*cb)(const QVariantMap &, void *);
    void *udata;
};

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
        
        QVariantMap getConfig();
        void setConfig(QVariantMap);
        // setter/getter for properties

        QString userLogin() const;        //!< userid to identify to the server
        QString agentphonenumber() const;  //!< agent's phone number
        void setAgentPhoneNumber(const QString &); //!< see agentphonenumber()
        int loginkind();                        //!< loginkind to identify to the server
        void setLoginKind(const int);           //!< see loginkind()
        int keeppass();                         //!< keeppass to identify to the server
        void setKeepPass(int);            //!< see keeppass()
        int showagselect();                     //!< showagselect to identify to the server
        void setShowAgentSelect(const int);     //!< see showagselect()
        QString password() const;       //!< password to identify to the sever
        void setPassword(const QString &);      //!< see password()

        bool systrayed() const;                 //!< systrayed flag
        uint historySize() const;               //!< history size
        QString profileName() const;            //!< Settings profile

        void saveSettings();                    //!< save server settings

        EngineState state();      //!< Engine state (Logged/Not Logged)
        void setState(EngineState state);       //!< see state()

        const QString& getAvailState() const;        //!< returns availability status
        void setCheckedFunction(const QString &, bool b);        //!< set m_checked_function
        bool checkedFunction(const QString &);                   //!< get m_checked_function
        void setEnabledFunction(const QString &, bool b);        //!< set m_enabled_function
        bool enabledFunction(const QString &);                   //!< on m_capafuncs

        const QVariantList & getCapaXlets() const;
        const QVariantMap & getOptionsUserStatus() const;
        const QVariantMap & getOptionsPhoneStatus() const;
        const QVariantMap & getOptionsChannelStatus() const;
        const QVariantMap & getOptionsAgentStatus() const;
        const QStringList & getCapasRegCommands() const;
        const QStringList & getCapasIpbxCommands() const;

        void setGuiOption(const QString &, const QVariant &);
        QVariantMap getGuiOptions(const QString &) const;

        const QString& getCapaApplication() const;
        void configAndStart(const QString &, const QString &, const QString &);
        QString osname() const { return m_osname; };

        void openLogFile();

        const QString & xivoUserId() const { return m_userid; };
        const QString & getFullId() const { return m_xuserid; };
        const UserInfo * getUserForXChannelId(const QString & xcid) const;  //!< Returns a user for a given xchannelid
        UserInfo * getXivoClientUser();  //!< Return the user of the XiVO CTI Client
        UserInfo * getXivoClientMonitored();  //!< Return the monitored user
        double timeServer() const;
        const QDateTime & timeClient() const;
        double timeDeltaServerClient() const;
        QString timeElapsed(double) const;

        void pasteToDial(const QString &);
        void registerClassEvent(const QString &class_function,
                                void (*)(const QVariantMap &map, void *udata), void *udata);
        void sendJsonCommand(const QVariantMap &);

        QHash<QString, newXInfoProto> m_xinfoList;  //!< XInfo constructors

        bool hasPhone(const QString & xid) { return m_anylist.value("phones").contains(xid); };
        bool hasAgent(const QString & xid) { return m_anylist.value("agents").contains(xid); };
        bool hasQueue(const QString & xid) { return m_anylist.value("queues").contains(xid); };

        QHash<QString, XInfo *> iterover(const QString & listname) { return m_anylist.value(listname); };

        const ChannelInfo * channel(const QString & id) const
                { return channels().value(id); };    //! < Return the channe to any xlet
        const UserInfo * user(const QString & id) const
                { return (const UserInfo *) m_anylist.value("users").value(id); };  //!< Return the user to any Xlet
        const PhoneInfo * phone(const QString & id) const
                { return (const PhoneInfo *) m_anylist.value("phones").value(id); };  //!< Return the phone to any Xlet
        const TrunkInfo * trunk(const QString & id) const
                { return (const TrunkInfo *) m_anylist.value("trunks").value(id); };  //!< Return the trunk to any Xlet
        const AgentInfo * agent(const QString & id) const
                { return (const AgentInfo *) m_anylist.value("agents").value(id); };  //!< Return the agent to any Xlet
        const QueueInfo * queue(const QString & id) const
                { return (const QueueInfo *) m_anylist.value("queues").value(id); };  //!< Return the queue to any Xlet
        const GroupInfo * group(const QString & id) const
                { return (const GroupInfo *) m_anylist.value("groups").value(id); };  //!< Return the group to any Xlet
        const MeetmeInfo * meetme(const QString & id) const
                { return (const MeetmeInfo *) m_anylist.value("meetmes").value(id); };  //!< Return the meetme to any Xlet
        const ParkingInfo * parkinglot(const QString & id) const
                { return (const ParkingInfo *) m_anylist.value("parkinglots").value(id); }
        const VoiceMailInfo * voicemail(const QString & id) const
                { return (const VoiceMailInfo *) m_anylist.value("voicemails").value(id); };  //!< Return the voicemail to any Xlet

        const QHash<QString, ChannelInfo *> & channels() const
                { return m_channels; };  //!< Return the channels to any Xlet
        const QHash<QString, QueueMemberInfo *> & queuemembers() const
                { return m_queuemembers; };  //!< Return the channels to any Xlet

        QStringList phonenumbers(const UserInfo *);

        void registerTranslation(const QString &);
        void changeTranslation(const QString &);
        void urlAuto(const QString &);
        void changeWatchedAgent(const QString &, bool);
        void changeWatchedQueue(const QString &);

    private:
        int callClassEventCallback(QString className, const QVariantMap &map);
        void stopConnection();   //!< stop the engine
        void clearInternalData();   //!< clear the engine internal data
        void setOSInfos(const QString &);

        void addUpdateConfRoomInTree(DStore *, const QString &);
        void addUpdateConfMemberInTree(DStore *, const QString &);

        QMultiHash<QString, e_callback* > m_class_event_cb;
        
        void setUserLogin(const QString &);       //!< see userid()
        void setUserLogin(const QString &, const QString &); //!< set userid and userid option

    public slots:
        void start();  //!< start the connection process.
        void stop();   //!< stop the engine
        void powerEvent(const QString &);
        //! set m_availstate
        void setAvailState(const QString &, bool);
        void actionCall(const QString &, const QString &src="", const QString &dst="");
        void receiveNumberSelection(const QStringList &);   //!< relay the selection

        void searchDirectory(const QString &);
        void textEdited(const QString &);
        void setAvailability();  //!< set user status from menu
        void featurePutOpt(const QString &, bool);
        void featurePutForward(const QString &, bool, const QString &);
        void askFeatures();
        void fetchIPBXList();
        void fetchLists();
        void setKeepaliveinterval(uint);  //!< set keep alive interval
        void sendFaxCommand(const QString &, const QString &, Qt::CheckState);
        void meetmeAction(const QString &, const QString &);
        void requestFileList(const QString &);

        void monitorPeerRequest(const QString &);
        void saveToFile(const QString &);
        void saveQueueGroups(const QVariant &);
        void loadQueueGroups();
        void logAction(const QString &);
        void logClient(const QString &, const QString &, const QString &); //!< log tricky situations
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
        void filetransferSocketConnected();
        void filetransferSocketReadyRead();
        void sheetSocketConnected();

    signals:
        void settingChanged(const QVariantMap &);  //!< signal emitted when the setting are changed
        void logged();    //!< signal emitted when the state becomes ELogged
        void delogged();  //!< signal emitted when the state becomes ENotLogged
        void availAllowChanged(bool);            //!< signal
        void emitTextMessage(const QString &);   //!< message to be displayed to the user.
        void pasteToXlets(const QString &); //!< Xlets intercept this signal from paste to dial
        void parkingEvent(const QString &,
                          const QString &,
                          const QString &,
                          const QVariant &);
        void ackFax(const QString &, const QString &);
        void featurePutIsKO();
        void featurePutIsOK();
        void monitorPeerChanged();
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
        void updateVoiceMailConfig(const QString &);
        void updateVoiceMailStatus(const QString &);
        void updateChannelStatus(const QString &);
        void updateParkinglotConfig(const QString &);
        void updateParkinglotStatus(const QString &);
        void removePhoneConfig(const QString &);
        void removeUserConfig(const QString &);
        void removeAgentConfig(const QString &);
        void removeQueueConfig(const QString &);

        void newQueueList(const QStringList &);
        void newAgentList(const QStringList &);
        void directoryResponse(const QStringList &, const QStringList &);  //! the directory search response has been received.
        void disconnectFeatures();
        void connectFeatures();
        void resetFeatures();
        void localUserInfoDefined();
        void monitoredUserInfoDefined();
        void removeQueues(const QString &, const QStringList &);
        void optChanged(const QString &);
        void forwardUpdated(const QString &);
        void changesAvailChecks();
        void changeWatchedAgentSignal(const QString &);
        void changeWatchedQueueSignal(const QString &);
        void displayFiche(const QString &, bool, const QString &);
        void gotSheetOwnership(const QString &id);  //! the user logged has now ownership of the sheet
        void lostSheetOwnership(const QString &id);  //! the user logged just lost ownership of the sheet
        void sheetEntryAdded(const QString &id, const QVariantMap &);  //! sheet data added by a user.
        //! numbers for a selected peer
        void broadcastNumberSelection(const QStringList &);

    protected:
        void timerEvent(QTimerEvent *);  //!< receive timer events

    private:
        void stopKeepAliveTimer();  //!< Stop the keep alive timer if running
        void popupError(const QString &);
        void startTryAgainTimer();  //!< Start the "try to reconnect" timer
        void stopTryAgainTimer();   //!< Stop the "try to reconnect" timer
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
        
        /*! \brief Stores the configuration values.
         * Only BaseEngine can freeze values in it.
         * The frozen values are the one given by the server, and should not be overwritten
         */
        BaseConfig m_config;
        
        int m_loginkind;                //!< Login Kind
        QString m_xuserid;              //!< Full Id (userid + company)
        QString m_ipbxid;               //!< IPBX id of the current user
        QString m_userid;               //!< xivo user id of the current user
        QString m_profilename;          //!< CTI profile name of the current user
        QString m_profilename_read;     //!< CTI profile name of the current user
        QString m_profilename_write;    //!< CTI profile name of the current user
        QStringList m_ipbxlist;

        QDateTime m_timeclt;
        double m_timesrv;

        QStringList translationFiles;   //!< List of translation files
        QVector<QTranslator *> translators;   //!< Vector of translators

        QHash<QString, bool> m_checked_function;  //!< function checked
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

        // QVariantMap m_guioptions;       //!< List of GUI options
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

        // miscellaneous statuses to share between xlets
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

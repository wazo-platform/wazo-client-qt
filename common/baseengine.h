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

#ifndef __BASEENGINE_H__
#define __BASEENGINE_H__

// QT includes.
#include <QAbstractSocket>
#include <QHash>
#include <QHostAddress>
#include <QObject>
#include <QSettings>
#include <QStringList>

#include "userinfo.h"

class QFile;
class QSocketNotifier;
class QTcpSocket;
class QTimerEvent;

/*! \brief Class which handles connection with the XIVO CTI server
 *   + Profile popup engine
 *  The engine object contains all the code to
 *  handle network connection and requests */
class BaseEngine: public QObject
{
	Q_OBJECT
public:
	//! Enum for BaseEngine state logged/not logged
	typedef enum {ENotLogged, ELogged } EngineState;
	//! Constructor
	BaseEngine(QSettings *, QObject * parent = 0);
	//! Destructor
	~BaseEngine();

        QSettings * getSettings();
	void loadSettings();			//!< load server settings
	// setter/getter for properties
	//! set address used to connect to the server
	void setAddress(const QString &, quint16);
	void setServerip(const QString &);	//!< see serverip()
	const quint16 & sbPort() const;
	const QString & serverip() const;	//!< Host of the login server
	const quint16 & loginPort() const;	//!< TCP port for connection to server
	void setLoginPort(const quint16 &);	//!< see loginPort()
        
	void setFullId();
	const QString & company() const;	//!< name of the user's company
	void setCompany(const QString &);	//!< see company()
	const QString & userId() const;		//!< userid to identify to the server
	void setUserId(const QString &);       	//!< see userid()
	const QString & phonenumber() const;	//!< agent's phone number
	void setPhonenumber(const QString &);	//!< see phonenumber()
	const int & loginkind() const;	       	//!< loginkind to identify to the server
	void setLoginKind(const int);	       	//!< see loginkind()
	const int & keeppass() const;		//!< keeppass to identify to the server
	void setKeepPass(const int);		//!< see keeppass()
	const int & showagselect() const;	//!< showagselect to identify to the server
	void setShowAgentSelect(const int);	//!< see showagselect()
	const QString & password() const;	//!< password to identify to the sever
	void setPassword(const QString &);	//!< see password()
        
	bool autoconnect() const;		//!< auto connect flag
	void setAutoconnect(bool);		//!< set auto connect flag
	bool trytoreconnect() const;		//!< try to reconnect flag
	void setTrytoreconnect(bool);		//!< set try to reconnect flag
	uint trytoreconnectinterval() const;	//!< try to reconnect interval
	void setTrytoreconnectinterval(uint);	//!< set try to reconnect interval
	uint historySize() const;		//!< history size
	void setHistorySize(uint size);		//!< set history size
	bool systrayed() const;			//!< systrayed flag
	void setSystrayed(bool);		//!< set systrayed flag

	void saveSettings();			//!< save server settings

	void deleteRemovables();
	void addRemovable(const QMetaObject *);
	bool isRemovable(const QMetaObject *);
	void sendMessage(const QString &);      //!< Sends an instant message
        
	const EngineState & state() const;	//!< Engine state (Logged/Not Logged)
	void setState(EngineState state);	//!< see state()

	const QString & getAvailState() const;	//!< returns availability status
	void setCheckedFunction(const QString &, bool b);	//!< set m_checked_function
	bool checkedFunction(const QString &);			//!< get m_checked_function
	void setEnabledFunction(const QString &, bool b);	//!< set m_enabled_function
	bool enabledFunction(const QString &);			//!< get m_enabled_function
        
	void setMyClientId();			//! set m_clientid
        
	uint keepaliveinterval() const;		//!< keep alive interval
	bool lastconnwins() const;		//!< last connected one wins
	void setLastConnWins(bool b);		//!< last connected user wins
	const QStringList & getCapabilities() const;	//!< returns capabilities
        const QStringList & getCapaXlets() const;
        const QVariantMap & getCapaPresence() const;
        const QVariant getGuiOptions(const QString &) const;
        void updateCapaPresence(const QVariant &);
        const QString     & getCapaApplication() const;
        void config_and_start(const QString &,
                              const QString &, const QString &);
        void setOSInfos(const QString &);
        UserInfo * findUserFromPhone(const QString &,
                                     const QString &);
        UserInfo * findUserFromAgent(const QString &,
                                     const QString &);
        void setGuiOption(const QString &, const QVariant &);
public slots:
	void start();				//!< start the connection process.
	void stop();				//!< stop the engine
	void setAvailState(const QString &, bool);	//! set m_availstate
	void actionCall(const QString &,
                        const QString &,
                        const QString &);
        
	void searchDirectory(const QString &);
	void requestHistory(const QString &, int);
        
        void textEdited(const QString &);
	void setAvailability();			//!< set user status from menu
	void featurePutOpt(const QString &, bool);
	void featurePutForward(const QString &, bool, const QString &);
	void askFeatures();
	void askCallerIds();
	void setKeepaliveinterval(uint);	//!< set keep alive interval
        void copyNumber(const QString &);
        void sendFaxCommand(const QString &, const QString &, Qt::CheckState);
        void agentAction(const QString &);
        void meetmeAction(const QString &, const QString &);
        void requestFileList(const QString &);
        
        void monitorPeerRequest(const QString &);
        void changeWatchedAgentSlot(const QString &, bool);
        void changeWatchedQueueSlot(const QString &);
        void saveToFile(const QString &);
private slots:
	void keepLoginAlive(); //!< Keep session alive
	void changeState(); //!< Change the presence status
        void updatePhone(const QString &,
                         const QString &,
                         const QVariant &);
        void removePeerAndCallerid(const QStringList &);
	void socketConnected();
	void socketDisconnected();
	void socketHostFound();
	void socketError(QAbstractSocket::SocketError);
	void socketStateChanged(QAbstractSocket::SocketState);
	void socketReadyRead();
        void actionFromFiche(const QStringList &);
        void readInputEvent(int);
signals:
	void logged();				//!< signal emitted when the state becomes ELogged
	void delogged();			//!< signal emitted when the state becomes ENotLogged
	void availAllowChanged(bool);		//!< signal 
	void emitTextMessage(const QString &);	//! message to be displayed to the user.
        void pasteToDialPanel(const QString &);
        void parkingEvent(const QVariant &);
        void ackFax(const QString &, const QString &);
        void featurePutIsKO();
        void featurePutIsOK();
        void monitorPeer(UserInfo *);
        void meetmeEvent(const QVariant &);
        void meetmeInit(const QVariant &);
        void requestFileListResult(const QVariant &);
        void updatePresence(const QVariant &);
        void updateCounter(const QVariant &);
        void serverFileList(const QStringList &);
        void fileReceived();
        void statusRecord(const QString &, const QString &);
        void setGuiOptions(const QVariant &);
        void emitMessageBox(const QString &);
        
	//! a call
	void updateCall(UserInfo *,
                        const QString &,
                        const QString &,
                        int,
                        const QString &,
                        const QString &,
                        const QString &);
	//! call list is updated
	void callsUpdated();
	void peersReceived();
	//! the server requested a peer remove
	void removePeer(const QString &);
	//! update informations about a peer
	void updatePeer(UserInfo *,
	                const QString &,
                        const QVariant &);
        void newUser(UserInfo *);
	void updatePeerAgent(const QString &,
                             const QString &,
                             const QStringList &);
	void updateOnlineAgent(const QStringList &);
	//! a log entry has to be updated.
	void updateLogEntry(const QDateTime &, int, const QString &, const QString &, const QString &);
	//! the directory search response has been received.
	void directoryResponse(const QString &);
        void disconnectFeatures();
        void connectFeatures();
        void resetFeatures();
        void localUserInfoDefined(const UserInfo *);
        void setQueueStatus(const QStringList &);
        void removeQueues(const QString &, const QStringList &);
        void newQueueList(bool, const QVariant &);
        void newAgentList(const QVariant &);
	void optChanged(const QString &, bool);
	void forwardUpdated(const QString &, bool, const QString &);
        void changesAvailChecks();
        void changeWatchedAgentSignal(const QString &, const QString &, const QVariant &);
        void changeWatchedQueueSignal(const QString &, const QString &, const QVariant &);
        void updateAgentPresence(const QString &, const QVariant &);
        void displayFiche(const QString &, bool);
protected:
	void timerEvent(QTimerEvent *);		//!< receive timer events
private:
	void stopKeepAliveTimer();	//!< Stop the keep alive timer if running
	void startTryAgainTimer();	//!< Start the "try to reconnect" timer
	void stopTryAgainTimer();	//!< Stop the "try to reconnect" timer
	void processHistory(const QStringList &);
	void initFeatureFields(const QString &, const QVariant &);
	void connectSocket();
        void sendCommand(const QString &);
        void sendJsonCommand(const QVariantMap &);
        bool parseCommand(const QString &);
        void popupError(const QString &);
        
	// Class Members
        
	// GUI client capabilities
	QList<const QMetaObject *> m_removable;
        
	// Parameters given by the User at Login time
	QString m_serverhost;		//!< Host to the login server
	quint16 m_ctiport;		//!< TCP port to connect to server
        
	QString m_userid;		//!< User Id
	QString m_useridopt;		//!< User Id Option (kind of login)
	QString m_useridwithopt;	//!< User Id Option (kind of login)
        QString m_company;
	QString m_password;		//!< User password for account
	QString m_agentid;		//!< Agent Id
	QString m_phonenumber;		//!< Agent's phone
	int m_loginkind;		//!< Login Kind
	int m_keeppass;			//!< Keep password ?
	int m_showagselect;		//!< Show agent selection ?
	QString m_fullid;		//!< Full Id (userid + company)
        QString m_astid;
        QString m_xivo_userid;
        
	bool m_autoconnect;		//!< Autoconnect to server at startup
	bool m_trytoreconnect;		//!< "try to reconnect" flag
	bool m_systrayed;		//!< "systrayed at startup" flag
	uint m_trytoreconnectinterval;	//!< Try to reconnect interval (in msec)
	uint m_keepaliveinterval;	//!< Keep alive interval (in msec)
        
	int m_historysize;
	QHash<QString, bool> m_checked_function;      	//!< function checked
	QHash<QString, bool> m_enabled_function;      	//!< function enabled
	bool m_checked_lastconnwins;   	//!< the last connected account "wins"
        
	// Replies given by the server
	QStringList m_capafuncs;	//!< List of func capabilities issued by the server after a successful login
	QStringList m_capaxlets;	//!< List of xlet capabilities issued by the server after a successful login
	QVariantMap m_capapresence;	//!< List of capabilities issued by the server for the presence statuses
	QVariantMap m_guioptions;	//!< List of GUI options
	QString m_appliname;		//!< Application name to be displayed
	QString m_sessionid;		//!< Session id obtained after a successful login
	QString m_clientid;		//!< Client Identifier
	QString m_forced_state;		//!< Forced state sent by the server
        QHash<QString, UserInfo *> m_users;	//!< List of User Informations
	int m_version_server;		//!< Version issued by the server after a successful login
        QString m_xivover_server;	//!< Server's XIVO version
        
	// Status variables
	EngineState m_state;		//!< State of the engine (Logged/Not Logged)
	QString m_availstate;		//!< Availability state to send to the server

	// Internal management
	QHostAddress m_serveraddress;	//!< Resolved address of the login server
	QTcpSocket * m_sbsocket;	//!< TCP socket to connect to the server (SB mode)
	QTcpSocket * m_filesocket;	//!< TCP socket to send fax data to the server
	QTcpSocket * m_connection;	//!< TCP socket set from TCP server listening for profiles
	ushort m_listenport;		//!< Port where we are listening for profiles
	int m_timer;			//!< timer id
	int m_ka_timerid;		//!< timer id for keep alive
	int m_try_timerid;		//!< timer id for try to reconnect
	int m_pendingkeepalivemsg;	//!< number of keepalivemsg sent without response
        QString m_numbertodial;		//!< Number dialed in
        QString m_osname;		//!< OS informations
        QVariant m_counters;

        QString m_agent_watched_astid;
        QString m_agent_watched_agentid;
        QString m_queue_watched_astid;
        QString m_queue_watched_queueid;

        QString m_fileid;
        QString m_filedir;
        QByteArray * m_filedata;
        int m_faxsize;

        QString m_monitored_userid;	//!< UserId of the Monitored Phone (on SB, or one's own on XC)
        QSettings * m_settings;
        QFile * m_eventdevice;
        QSocketNotifier * m_notifier;
        QByteArray m_downloaded;
};

#endif

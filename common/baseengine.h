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

class QTimerEvent;
class QTcpSocket;

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
	const int & loginkind() const;		//!< loginkind to identify to the server
	void setLoginKind(const int);		//!< see loginkind()
	const int & keeppass() const;		//!< keeppass to identify to the server
	void setKeepPass(const int);		//!< see keeppass()
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
	uint contactsColumns() const;		//!< contacts columns
	void setContactsColumns(uint columns);	//!< set contacts colmuns
        void setQueueLevel(const QString &, uint);
        uint queueLevel(const QString &) const;
	uint contactsSize() const;		//!< contacts size
	void setContactsSize(uint size);	//!< set contacts size
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
	void setAvailState(const QString &, bool);	//! set m_availstate
	void setCheckedPresence(bool b);	//!< set m_checked_presence
	bool checkedPresence();			//!< get m_checked_presence
	void setCheckedCInfo(bool b);		//!< set m_checked_cinfo
	bool checkedCInfo();			//!< get m_checked_cinfo
	void setCheckedAutoUrl(bool b);		//!< set m_checked_autourl
	bool checkedAutoUrl();			//!< get m_checked_autourl
	void setEnabledPresence(bool b);	//!< set m_enabled_presence
	bool enabledPresence();			//!< get m_enabled_presence
	void setEnabledCInfo(bool b);		//!< set m_enabled_cinfo
	bool enabledCInfo();			//!< get m_enabled_cinfo
	int tablimit() const;			//!< get m_tablimit
	void setTablimit(int);			//!< set m_tablimit

        bool hasFunction(const QString &);
	void setMyClientId();			//! set m_clientid

	uint keepaliveinterval() const;		//!< keep alive interval
	bool lastconnwins() const;		//!< last connected one wins
	void setLastConnWins(bool b);		//!< last connected user wins
	const QStringList & getCapabilities() const;	//!< returns capabilities
        const QStringList & getCapaFeatures() const;	//!< returns features capabilities
        const QStringList & getCapaXlets() const;
        const QString     & getCapaApplication() const;
        void config_and_start(const QString &,
                              const QString &, const QString &);
        void setOSInfos(const QString &);
        UserInfo * findUserFromPhone(const QString &,
                                     const QString &,
                                     const QString &,
                                     const QString &);
        UserInfo * findUserFromAgent(const QString &,
                                     const QString &);
protected:
	void timerEvent(QTimerEvent *);		//!< receive timer events
public slots:
	void start();				//!< start the connection process.
	void stop();				//!< stop the engine
	void originateCall(const QString &, const QString &);
	void transferCall(const QString &, const QString &);
        void atxferCall(const QString &, const QString &);
	void interceptCall(const UserInfo *, const QString &);
        void transferToNumber(const QString &);
        void parkCall(const QString &);
	void hangupCall(const UserInfo *, const QString &);
	void simplehangupCall(const UserInfo *, const QString &);
        
	void searchDirectory(const QString &);
	void requestHistory(const QString &, int);
        
        void textEdited(const QString &);
	void setAvailable();			//!< set user status as "available"
	void setAway();				//!< set user status as "away"
	void setBeRightBack();			//!< set user status as "be right back"
	void setOutToLunch();			//!< set user status as "out to lunch"
	void setDoNotDisturb();			//!< set user status as "do not disturb"
	void featurePutVoiceMail(bool);
	void featurePutCallRecording(bool);
	void featurePutCallFiltering(bool);
	void featurePutDnd(bool);
	void featurePutUncondForward(bool, const QString &);
	void featurePutForwardOnBusy(bool, const QString &);
	void featurePutForwardOnUnavailable(bool, const QString &);
	void askFeatures();
	void askCallerIds();
	void setKeepaliveinterval(uint);	//!< set keep alive interval
        void copyNumber(const QString &);
        void sendFaxCommand(const QString &, const QString &, Qt::CheckState);
        void agentAction(const QString &);
        void meetmeAction(const QString &);
	void pickUp(const UserInfo *);

        void monitorPeerRequest(const QString &);
        void changeWatchedAgentSlot(const QString &);
        void changeWatchedQueueSlot(const QString &);
private slots:
	void keepLoginAlive();			//!< Send a UDP datagram to keep session alive
        void updatePeerAndCallerid(const QStringList &);
        void removePeerAndCallerid(const QStringList &);
	void socketConnected();
	void socketDisconnected();
	void socketHostFound();
	void socketError(QAbstractSocket::SocketError);
	void socketStateChanged(QAbstractSocket::SocketState);
	void socketReadyRead();
signals:
	void logged();				//!< signal emitted when the state becomes ELogged
	void delogged();			//!< signal emitted when the state becomes ENotLogged
	void availAllowChanged(bool);		//!< signal 
	void emitTextMessage(const QString &);	//! message to be displayed to the user.
        void pasteToDialPanel(const QString &);
        void parkingEvent(const QString &, const QString &);
        void ackFax(const QString &);
        void featurePutIsKO();
        void featurePutIsOK();
        void monitorPeer(UserInfo *);
        void meetmeEvent(const QStringList &);

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
                        const QHash<QString, QStringList> &);
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
        void setQueueStatus(const QString &);
        void newQueueList(bool, const QString &);
        void newAgentList(const QString &);
	void voiceMailChanged(bool);
	void callRecordingChanged(bool);
	void callFilteringChanged(bool);
	void dndChanged(bool);
	void uncondForwardUpdated(bool, const QString &);
	void uncondForwardUpdated(bool);
	void uncondForwardUpdated(const QString &);
	void forwardOnBusyUpdated(bool, const QString &);
	void forwardOnBusyUpdated(bool);
	void forwardOnBusyUpdated(const QString &);
	void forwardOnUnavailableUpdated(bool, const QString &);
	void forwardOnUnavailableUpdated(bool);
	void forwardOnUnavailableUpdated(const QString &);
        void changesAvailChecks();
        void changeWatchedAgentSignal(const QStringList &);
        void changeWatchedQueueSignal(const QStringList &);
        void updateAgentPresence(const QString &, const QString &);
        void displayFiche(const QString &, bool);
private:
	void stopKeepAliveTimer();	//!< Stop the keep alive timer if running
	void startTryAgainTimer();	//!< Start the "try to reconnect" timer
	void stopTryAgainTimer();	//!< Stop the "try to reconnect" timer
	void processHistory(const QStringList &);
	void initFeatureFields(const QString &,
			       const QString &);
	void connectSocket();
	void sendTCPCommand();
        void sendCommand(const QString &);
        bool parseCommand(const QStringList &);
        void popupError(const QString &);

	// Class Members

	// GUI client capabilities
	QList<const QMetaObject *> m_removable;

	// Parameters given by the User at Login time
	QString m_serverhost;		//!< Host to the login server
	quint16 m_loginport;		//!< TCP port (UDP port for keep alive is +1)
	quint16 m_ctiport;		//!< port to connect to server

	QString m_userid;		//!< User Id
        QString m_company;
	QString m_password;		//!< User password for account
	QString m_agentid;		//!< Agent Id
	QString m_phonenumber;		//!< Agent's phone
	int m_loginkind;		//!< Login Kind
	int m_keeppass;			//!< Keep password ?
	QString m_fullid;		//!< Full Id (userid + company)

	bool m_autoconnect;		//!< Autoconnect to server at startup
	bool m_trytoreconnect;		//!< "try to reconnect" flag
	bool m_systrayed;		//!< "systrayed at startup" flag
	uint m_trytoreconnectinterval;	//!< Try to reconnect interval (in msec)
	uint m_keepaliveinterval;	//!< Keep alive interval (in msec)

	int m_historysize;
	int m_contactssize;
	int m_contactscolumns;
        QHash<QString, int> m_queuelevels;
	bool m_enabled_presence;      	//!< presence is enabled
	bool m_enabled_cinfo;      	//!< customer info is enabled
	bool m_checked_presence;      	//!< presence is checked
	bool m_checked_cinfo;      	//!< customer info is checked
	bool m_checked_lastconnwins;   	//!< the last connected account "wins"
	bool m_checked_autourl;   	//!< allow automatic url display
        int m_tablimit;

	// Replies given by the server
	QStringList m_capafuncs;	//!< List of func capabilities issued by the server after a successful login
	QStringList m_capaxlets;	//!< List of xlet capabilities issued by the server after a successful login
	QStringList m_capafeatures;	//!< List of capabilities issued by the server for the features
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
	QTcpSocket * m_faxsocket;	//!< TCP socket to send fax data to the server
	QTcpSocket * m_connection;	//!< TCP socket set from TCP server listening for profiles
	ushort m_listenport;		//!< Port where we are listening for profiles
	int m_timer;			//!< timer id
	int m_ka_timerid;		//!< timer id for keep alive
	int m_try_timerid;		//!< timer id for try to reconnect
	int m_pendingkeepalivemsg;	//!< number of keepalivemsg sent without response
	QString m_pendingcommand;	//!< command to be sent to the server.
        QString m_numbertodial;		//!< Number dialed in
        QString m_osname;		//!< OS informations

        QString m_agent_watched_astid;
        QString m_agent_watched_agentid;
        QString m_queue_watched_astid;
        QString m_queue_watched_queueid;

        QString m_faxid;
        QByteArray * m_faxdata;
        int m_faxsize;

        QString m_monitored_userid;	//!< UserId of the Monitored Phone (on SB, or one's own on XC)
        QSettings * m_settings;
};

#endif

/*
XIVO CTI Clients
Copyright (C) 2007  Proformatique

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
*/

/* $Revision$
   $Date$
*/

#ifndef __BASEENGINE_H__
#define __BASEENGINE_H__

// QT includes.
#include <QHash>
#include <QObject>
#include <QSettings>
#include <QStringList>
#include <QTcpSocket>
#include <QTcpServer>
#include <QTime>
#include <QTimer>
#include <QUdpSocket>

class Popup;

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
	const quint16 sbPort() const;
	const QString & serverip() const;	//!< Host of the login server
	const quint16 loginPort() const;	//!< TCP port for connection to server
	void setLoginPort(const quint16 &);	//!< see loginPort()

	const QString & serverast() const;	//!< id Name of the Asterisk server
	void setServerAst(const QString &);	//!< see serverast()
	const QString & protocol() const;      	//!< protocol of the user to identify to the server
	void setProtocol(const QString &);     	//!< see protocol()
	const QString & userId() const;		//!< userid to identify to the server
	void setUserId(const QString &);       	//!< see userid()
	const QString & password() const;	//!< password to identify to the sever
	void setPassword(const QString &);	//!< see password()

	const QString & phoneNum() const;	//! get m_extension
	const QString & dialContext() const;	//! get m_dialcontext
	void setDialContext(const QString &);	//! set m_dialcontext

	bool autoconnect() const;		//!< auto connect flag
	void setAutoconnect(bool);		//!< set auto connect flag
	bool trytoreconnect() const;		//!< try to reconnect flag
	void setTrytoreconnect(bool);		//!< set try to reconnect flag
	uint trytoreconnectinterval() const;	//!< try to reconnect interval
	void setTrytoreconnectinterval(uint);	//!< set try to reconnect interval
	uint historySize() const;		//!< history size
	void setHistorySize(uint size);		//!< set history size
	uint contactsSize() const;		//!< contacts size
	void setContactsSize(uint size);	//!< set contacts size

	void saveSettings();			//!< save server settings

	void setIsASwitchboard(bool);
	bool isASwitchboard();
	void deleteRemovables();
	void addRemovable(const QMetaObject *);
	bool isRemovable(const QMetaObject *);
	void sendMessage(const QString &);      //!< Sends an instant message
        
	const EngineState state() const;	//!< Engine state (Logged/Not Logged)
	void setState(EngineState state);	//!< see state()

	const QString & getAvailState() const;	//!< returns availability status
	void setAvailState(const QString &, bool);	//! set m_availstate
	void setCheckedPresence(bool b);	//! set m_checked_presence
	bool checkedPresence();			//! get m_checked_presence
	void setCheckedCInfo(bool b);		//! set m_checked_cinfo
	bool checkedCInfo();			//! get m_checked_cinfo
	void setEnabledPresence(bool b);	//! set m_enabled_presence
	bool enabledPresence();			//! get m_enabled_presence
	void setEnabledCInfo(bool b);		//! set m_enabled_cinfo
	bool enabledCInfo();			//! get m_enabled_cinfo

	void setMyClientId();			//! set m_clientid
        const QString & fullName() const;	//!< returns full name of the logged-in user

	uint keepaliveinterval() const;		//!< keep alive interval
	bool lastconnwins() const;		//!< last connected one wins
	bool tcpmode() const;			//!< get tcp mode flag
	void setTcpmode(bool b);		//!< set tcp mode flag
	void setLastConnWins(bool b);		//!< last connected user wins
	const QStringList & getCapabilities() const;	//!< returns capabilities
protected:
	void timerEvent(QTimerEvent *);		//!< receive timer events
public slots:
	void start();				//!< start the connection process.
	void stop();				//!< stop the engine
	void originateCall(const QString &, const QString &);
	void originateCallGoodAsterisk(const QString &, const QString &);
	void dialFullChannel(const QString &);
	void transferCall(const QString &, const QString &);
        void atxferCall(const QString &, const QString &);
	void interceptCall(const QString &);
	void searchDirectory(const QString &);
	void requestHistory(const QString &, int);
        void transferToNumber(const QString &);
        void parkCall(const QString &);

        void textEdited(const QString &);
	void setAvailable();			//!< set user status as "available"
	void setAway();				//!< set user status as "away"
	void setBeRightBack();			//!< set user status as "be right back"
	void setOutToLunch();			//!< set user status as "out to lunch"
	void setDoNotDisturb();			//!< set user status as "do not disturb"
	void sendUDPCommand(const QString &);
	void featurePutVoiceMail(bool);
	void featurePutCallRecording(bool);
	void featurePutCallFiltering(bool);
	void featurePutDnd(bool);
	void featurePutUncondForward(bool, const QString &);
	void featurePutForwardOnBusy(bool, const QString &);
	void featurePutForwardOnUnavailable(bool, const QString &);
	void askFeatures(const QString &);
	void askCallerIds();
        void addToDataBase(const QString &);
	void setKeepaliveinterval(uint);	//!< set keep alive interval
        void copyNumber(const QString &);
        void sendFaxCommand(const QString &, const QString &, Qt::CheckState);
	void readProfile();
private slots:
	void identifyToTheServer();		//!< perform the first login step
	void processLoginDialog();		//!< perform the following login steps
	void handleProfilePush();		//!< called when receiving a profile
	void keepLoginAlive();			//!< Send a UDP datagram to keep session alive
	void readKeepLoginAliveDatagrams();	//!< handle the responses to keep alive
	void popupDestroyed(QObject *);		//!< know when a profile widget is destroyed *DEBUG*
	void profileToBeShown(Popup *);		//!< a new profile must be displayed
        void updatePeerAndCallerid(const QStringList &);
        void removePeerAndCallerid(const QStringList &);
	void socketConnected();
	void socketDisconnected();
	void socketHostFound();
	void socketError(QAbstractSocket::SocketError);
	void socketStateChanged(QAbstractSocket::SocketState);
	void socketReadyRead();
	void hangUp(const QString &);
signals:
	void logged();				//!< signal emitted when the state becomes ELogged
	void delogged();			//!< signal emitted when the state becomes ENotLogged
	void availAllowChanged(bool);		//!< signal 
	void newProfile(Popup *);		//!< signal emitted when a new profile has to be shown
	void emitTextMessage(const QString &);	//! message to be displayed to the user.
        void pasteToDialPanel(const QString &);
        void parkingEvent(const QString &, const QString &);
        void ackFax(const QString &);
        void featurePutIsKO();
        void featurePutIsOK();

	//! a call
	void updateCall(const QString & channelme,
			const QString & action,
			int time,
			const QString & direction,
			const QString & channelpeer,
			const QString & exten,
			const QString & phone);
	//! "my" calls are updated
	void updateMyCalls(const QStringList &, const QStringList &, const QStringList &);
	//! call list is updated
	void callsUpdated();
	void peersReceived();
	//! the server requested a peer remove
	void removePeer(const QString &);
	//! we want to monitor a given peer (not the one given by the mouse's drag&drop).
	void monitorPeer(const QString &, const QString &);
	//! update informations about a peer
	void updatePeer(const QString &, const QString &,
	                const QString &, const QString &,
	                const QString &, const QString &,
	                const QStringList &, const QStringList &, const QStringList &);
	//! a log entry has to be updated.
	void updateLogEntry(const QDateTime &, int, const QString &, int);
	//! the directory search response has been received.
	void directoryResponse(const QString &);
        void disconnectFeatures();
        void connectFeatures();
        void resetFeatures();
        void localUserDefined(const QString &);
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
private:
	void initListenSocket();	//!< initialize the socket listening to profile
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
	bool m_is_a_switchboard;

	// Parameters given by the User at Login time
	QString m_serverhost;		//!< Host to the login server
	quint16 m_loginport;		//!< TCP port (UDP port for keep alive is +1)
	quint16 m_sbport;		//!< port to connect to server

	QString m_asterisk;		//!< Host to the login server
	QString m_protocol;		//!< User Protocol's login
	QString m_userid;		//!< User Id
	QString m_passwd;		//!< User password for account

	bool m_autoconnect;		//!< Autoconnect to server at startup
	bool m_trytoreconnect;		//!< "try to reconnect" flag
	uint m_trytoreconnectinterval;	//!< Try to reconnect interval (in msec)
	uint m_keepaliveinterval;	//!< Keep alive interval (in msec)

	int m_historysize;
	int m_contactssize;
	bool m_enabled_presence;      	//!< presence is enabled
	bool m_enabled_cinfo;      	//!< customer info is enabled
	bool m_checked_presence;      	//!< presence is checked
	bool m_checked_cinfo;      	//!< customer info is checked
	bool m_checked_lastconnwins;   	//!< the last connected account "wins"
	bool m_tcpmode;			//!< use a unique outgoing TCP connection for everything

	// Replies given by the server
	QString m_extension;		//!< extension for "my phone"
	QString m_dialcontext;		//!< Context of the phone, as returned by the xivo_daemon server
	QStringList m_capabilities;	//!< List of capabilities issued by the server after a successful login
	QString m_sessionid;		//!< Session id obtained after a successful login
	QString m_clientid;		//!< Client Identifier
	QString m_forced_state;		//!< Forced state sent by the server
	QHash<QString, QString> m_callerids;	//!< List of caller Ids
	int m_version_server;		//!< Version issued by the server after a successful login
        QString m_xivover_server;	//!< Server's XIVO version
        QString m_fullname;		//!< Full Name

	// Status variables
	EngineState m_state;		//!< State of the engine (Logged/Not Logged)
	QString m_availstate;		//!< Availability state to send to the server

	// Internal management
	QHostAddress m_serveraddress;	//!< Resolved address of the login server
	QTcpSocket * m_sbsocket;	//!< TCP socket to connect to the server (SB mode)
	QTcpSocket * m_loginsocket;	//!< TCP socket to login to the server
	QTcpSocket * m_faxsocket;	//!< TCP socket to send fax data to the server
	QTcpSocket * m_connection;	//!< TCP socket set from TCP server listening for profiles
	QUdpSocket * m_udpsocket;      	//!< UDP socket used for keep alive
	QTcpServer * m_listenserver;	//!< TCP server listening for profiles
	ushort m_listenport;		//!< Port where we are listening for profiles
	int m_timer;			//!< timer id
	int m_ka_timerid;		//!< timer id for keep alive
	int m_try_timerid;		//!< timer id for try to reconnect
	int m_pendingkeepalivemsg;	//!< number of keepalivemsg sent without response
	QString m_pendingcommand;	//!< command to be sent to the server.
        QString m_numbertodial;		//!< Number dialed in

        QString m_faxid;
        QByteArray * m_faxdata;
        int m_faxsize;

        QString m_monitored_context;	//!< Context of the Monitored Phone (on SB, or one's own on XC)
        QString m_monitored_userid;	//!< UserId of the Monitored Phone (on SB, or one's own on XC)
        QString m_monitored_asterisk;	//!< Asterisk Id of the Monitored Phone (on SB, or one's own on XC)

        QSettings * m_settings;
};

#endif

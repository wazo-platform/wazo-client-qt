/* XIVO client
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

#ifndef __MAINWIDGET_H__
#define __MAINWIDGET_H__

#include <QDateTime>
#include <QHash>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QPushButton>
#include <QSettings>
#include <QSplitter>
#include <QSystemTrayIcon>
#include <QVBoxLayout>
#include <QWidget>

#include "baseengine.h"
#include "dialpanel.h"
#include "logwidget.h"
#include "directorypanel.h"
#include "videopanel.h"

class QAction;
class QActionGroup;
class QCloseEvent;
class QDateTime;
class QDockWidget;
class QEvent;
class QKeyEvent;
class QLabel;
class QScrollArea;
class QSystemTrayIcon;
class QTabWidget;
class QVBoxLayout;
class QWidget;
class QUrl;

class AgentsPanel;
class AgentdetailsPanel;
class BaseEngine;
class CallStackWidget;
class ConfWidget;
class DatetimePanel;
class DialPanel;
class DirectoryPanel;
class DisplayMessagesPanel;
class FaxPanel;
class IdentityDisplay;
class LeftPanel;
class LogWidget;
class ParkingPanel;
class Popup;
class QueuesPanel;
class QueuedetailsPanel;
class SearchPanel;
class ServicePanel;
class StatusPanel;
class SwitchBoardWindow;

/*! \brief Main window class
 */
class MainWidget : public QMainWindow
{
	Q_OBJECT
public:
	//! Constructor
	MainWidget(BaseEngine *,
                   const QString &,
                   const QString &,
                   const QString &,
                   const QString &,
                   QWidget * parent=0);
	//! Destructor
        virtual ~MainWidget();

	int tablimit() const;	//!< getter for m_tablimit
	void setTablimit(int);	//!< setter for m_tablimit
	void setForceTabs(bool);//!< setter for m_forcetabs
	void setAppearance(const QString &);	//!< dock options
public slots:
        void dispurl(const QUrl &url);
private slots:
	void showConfDialog();
	void showLogin();
	void hideLogin();
	void engineStopped();
	void engineStarted();
        void systrayActivated(QSystemTrayIcon::ActivationReason);
        void systrayMsgClicked();
	void showNewProfile(Popup *);	//!< display a Profile widget
        void checksAvailState();
	void about();
        void newParkEvent();
	void affTextChanged();
        void config_and_start();
        void logintextChanged(const QString &);
signals:
        void functionKeyPressed(int);
protected:
	void showEvent(QShowEvent *);	//!< Catch show events
	void hideEvent(QHideEvent *);	//!< Catch hide events
	void closeEvent(QCloseEvent *);
	void changeEvent(QEvent *);
	void keyPressEvent(QKeyEvent *);
        void addPanel(const QString &, const QString &, QWidget *);
        void removePanel(const QString &, QWidget *);
        // bool event(QEvent *);
private:
	void createActions();	//!< Create Actions (for menus)
	void createMenus();		//!< Create Menus
	void createSystrayIcon();	//!< Create the systray Icon and its menu
        void savePositions() const;

	BaseEngine * m_engine;	//!< Engine
	QSystemTrayIcon * m_systrayIcon;	//!< System Tray Icon
	QIcon m_icon;		//!< Icon Object
	QIcon m_icongrey;	//!< greyed Icon Object
	QWidget * m_wid;	//!< Main widget

	SwitchBoardWindow * m_sbwidget;	//!< Widget to display peers
	QTabWidget * m_cinfo_tabwidget;	//!< Area to display profiles
	QTabWidget * m_svc_tabwidget;	//!< Area to display messages, services and histories
        LeftPanel * m_leftpanel;
        QScrollArea * m_areaCalls;
        QScrollArea * m_areaPeers;
	CallStackWidget * m_calls;
	QLabel * m_messagetosendlabel;    //!< Label for message for switchboards
	QLineEdit * m_messagetosend;    //!< Message to send to the connected switchboards
        QVBoxLayout * m_mainlayout;

        ConfWidget * m_conf;
        QLabel * m_xivobg;

	DirectoryPanel * m_dirpanel;
        DisplayMessagesPanel * m_messages_widget;
        FaxPanel * m_faxwidget;
	LogWidget * m_historypanel;
        ParkingPanel * m_parkingpanel;
	SearchPanel * m_searchpanel;
	ServicePanel * m_featureswidget;
        StatusPanel * m_statuspanel;
        IdentityDisplay * m_infowidget;
        PlayerWidget * m_videopanel;
        QueuesPanel * m_queuespanel;
        QueuedetailsPanel * m_queuedetailspanel;
        AgentsPanel * m_agentspanel;
        AgentdetailsPanel * m_agentdetailspanel;
        DatetimePanel * m_datetimepanel;

	bool m_forcetabs;    //!< Flag to allow the display of "unallowed" tabs, useful to test server-side capabilities
	bool m_presence;
	int m_cinfo_index;

        QString m_appliname;
        QHash<QString, QString> m_dockoptions;
        QStringList m_docknames;
        QStringList m_gridnames;
        QStringList m_tabnames;

        bool m_withsystray;
        bool m_loginfirst;
        bool m_switchboard;
        bool m_normalmenus;


        QHash<QString, QDockWidget *> m_docks;
        QStringList m_display_capas;
        DialPanel * m_dialpanel;
	int m_tablimit;		//!< Maximum number of tabs in m_cinfo_tabwidget

	// actions :
	QAction * m_cfgact;		//!< Configuration Action
	QAction * m_quitact;		//!< Quit Action
	QAction * m_connectact;		//!< "Connect" Action
	QAction * m_disconnectact;	//!< "Disconnect" Action
	QAction * m_systraymin;		//!< "Go to systray" action
	QAction * m_systraymax;		//!< "Go to systray" action
	QActionGroup * m_availgrp;	//!< Availability action group

	QGridLayout * m_gridlayout;
        QLabel * m_lab1;
        QLabel * m_lab2;
        QLabel * m_lab3;
        QLabel * m_lab4;
        QLineEdit * m_qlab1;
        QLineEdit * m_qlab2;
        QLineEdit * m_qlab3;
        QLineEdit * m_qlab4;
        QPushButton * m_ack;
        QFrame * m_qhline;

	QMenu * m_avail;		//!< Availability submenu
	QAction * m_avact_avail;	//!< "Available" action
	QAction * m_avact_brb;		//!< "Be right back" action
	QAction * m_avact_dnd;		//!< "Do not disturb" action
	QAction * m_avact_otl;		//!< "out to lunch" action
	QAction * m_avact_away;		//!< "away" action
	QLabel * m_status;	//!< status indicator

        QDateTime m_launchDateTime;
        QSettings * m_settings;
};


/*! \brief Widget containing the CallStackWidget and a Title QLabel
 */
class LeftPanel : public QWidget
{
public:
	LeftPanel(QWidget *, QWidget * parent = 0);	//!< Constructor
	QLabel * titleLabel();	//!< getter for m_titleLabel
private:
	QLabel * m_titleLabel;	//!< Title label property
};


#endif

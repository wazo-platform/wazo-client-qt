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

#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>
#include <QSystemTrayIcon>
#include <QMainWindow>
#include <QLineEdit>

#include "baseengine.h"
#include "dialpanel.h"
#include "logwidget.h"
#include "directorypanel.h"

class QActionGroup;
class QDateTime;
class QKeyEvent;
class QSystemTrayIcon;
class QTabWidget;
class QWidget;
class QUrl;

class ConfWidget;
class FaxPanel;
class IdentityDisplay;
class LogWidget;
class SearchPanel;
class ServicePanel;

/*! \brief Main window class
 *
 * This widget contains buttons enabling the user
 * to popup the config window, to quit and to start
 * the login process.
 */
class MainWidget: public QMainWindow
{
	Q_OBJECT
public:
	//! Constructor
	MainWidget(BaseEngine *, QWidget * parent=0);
	//! Destructor
        ~MainWidget();

	int tablimit() const;	//!< getter for m_tablimit
	void setTablimit(int);	//!< setter for m_tablimit
	void setForceTabs(bool);//!< setter for m_forcetabs
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
	void about();
	void affTextChanged();
        void checksAvailState();
        void config_and_start();
        void logintextChanged(const QString &);
protected:
	void showEvent(QShowEvent *);	//!< Catch show events
	void hideEvent(QHideEvent *);	//!< Catch hide events
	void closeEvent(QCloseEvent *);
	void changeEvent(QEvent *);
	void keyPressEvent(QKeyEvent *);
        // bool event(QEvent *);
private:
	void createActions();	//!< Create Actions (for menus)
	void createMenus();		//!< Create Menus
	void createSystrayIcon();	//!< Create the systray Icon and its menu
        void savePositions() const;

	BaseEngine * m_engine;			//!< pointer to the BaseEngine used
	//QPushButton * m_btnstart;	//!< Start Button
	QSystemTrayIcon * m_systrayIcon;	//!< System Tray Icon
	//QIcon m_iconred;	//!< Icon object with red indicator
	//QIcon m_icongreen;	//!< Icon object with green indicator
	QIcon m_icon;		//!< Icon Object
	QIcon m_icongrey;	//!< greyed Icon Object

	QWidget * m_wid;	//!< Main widget
	QTabWidget * m_cinfo_tabwidget;	//!< Area to display profiles
	QTabWidget * m_main_tabwidget;	//!< Area to display function tabs
	QLabel * m_messagetosendlabel;    //!< Label for message for switchboards
	QLineEdit * m_messagetosend;    //!< Message to send to the connected switchboards
	DialPanel * m_dial;
	DirectoryPanel * m_directory;
        QLabel * m_xivobg;
	//        QLabel * m_xivobg2;
	LogWidget * m_history;
        ConfWidget * m_conf;
        IdentityDisplay * m_infowidget;
	SearchPanel * m_searchpanel;
        FaxPanel * m_faxwidget;
	ServicePanel * m_featureswidget;
	bool m_forcetabs;    //!< Flag to allow the display of "unallowed" tabs, useful to test server-side capabilities
	bool m_presence;
        QDateTime m_launchDateTime;

	int m_tablimit;		//!< Maximum number of tabs in m_cinfo_tabwidget
	int m_cinfo_index;
	// actions :
	QAction * m_cfgact;		//!< Configuration Action
	QAction * m_quitact;		//!< Quit Action
	QAction * m_systraymin;		//!< "Go to systray" action
	QAction * m_systraymax;		//!< "Go to systray" action
	QAction * m_connectact;		//!< "Connect" Action
	QAction * m_disconnectact;	//!< "Disconnect" Action
	QActionGroup * m_availgrp;	//!< group For Availability Actions

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
	QAction * m_avact_avail;	//!< Available Action
	QAction * m_avact_away;		//!< Away Action
	QAction * m_avact_brb;		//!< "Be Right Back" Action
	QAction * m_avact_otl;		//!< "Out To Lunch" Action
	QAction * m_avact_dnd;		//!< "Do not disturb" action

	QLabel * m_status;	//!< status indicator
        QSettings * m_settings;
};

#endif

/* XIVO switchboard
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
#include <QMainWindow>
#include <QSettings>
#include <QSplitter>
#include <QSystemTrayIcon>
#include <QWidget>

class QAction;
class QActionGroup;
class QCloseEvent;
class QDateTime;
class QDockWidget;
class QKeyEvent;
class QLabel;
class QScrollArea;
class QTabWidget;
class QVBoxLayout;

class BaseEngine;
class CallStackWidget;
class ConfWidget;
class DialPanel;
class DirectoryPanel;
class DisplayMessagesPanel;
class FaxPanel;
class IdentityDisplay;
class LeftPanel;
class LogWidget;
class ParkingPanel;
class Popup;
class SearchPanel;
class ServicePanel;
class StatusPanel;
class SwitchBoardWindow;

/*! \brief Main window splitted to display peers and calls
 */
class MainWidget : public QMainWindow
{
	Q_OBJECT
public:
	//! Constructor
	MainWidget(BaseEngine *, QWidget * parent=0);
	//! Destructor
	virtual ~MainWidget();

	int tablimit() const;	//!< getter for m_tablimit
	void setTablimit(int);	//!< setter for m_tablimit
private slots:
	void showConfDialog();
	void engineStopped();
	void engineStarted();
	//	void systrayActivated(QSystemTrayIcon::ActivationReason);
	//	void systrayMsgClicked();
	void showNewProfile(Popup *);	//!< display a Profile widget
        void checksAvailState();
	void about();
        void newParkEvent();
protected:
	void keyPressEvent(QKeyEvent *);
private:
        void createActions();
        void createMenus();
        void buildSplitters();
        void removeSplitters();

	QSplitter * m_splitter;	//!< Splitter to separate right/left panels
	QSplitter * m_leftSplitter;	//!< Vertical splitter on the left
	QSplitter * m_middleSplitter;	//!< vertical splitter in the middle
	QSplitter * m_rightSplitter;	//!< Vertical splitter on the right
	BaseEngine * m_engine;	//!< Engine
	SwitchBoardWindow * m_sbwidget;	//!< Widget to display peers
	QTabWidget * m_cinfo_tabwidget;	//!< Area to display profiles
	QTabWidget * m_svc_tabwidget;	//!< Area to display messages, services and histories
	ServicePanel * m_featureswidget;
        DisplayMessagesPanel * m_messages_widget;
	DirectoryPanel * m_dirpanel;
	LogWidget * m_logwidget;
        LeftPanel * m_leftpanel;
        FaxPanel * m_faxwidget;
        ParkingPanel * m_parkingpanel;
        QScrollArea * m_areaCalls;
        QScrollArea * m_areaPeers;
        SearchPanel * m_searchpanel;
	CallStackWidget * m_calls;
        ConfWidget * m_conf;
        QLabel * m_xivobg;
        QWidget * m_wid;
        QVBoxLayout * m_mainlayout;
        IdentityDisplay * m_infowidget;

        QHash<QString, QDockWidget *> m_docks;
        QStringList m_docknames;
        DialPanel * m_dialpanel;
        StatusPanel * m_statuspanel;
	int m_tablimit;		//!< Maximum number of tabs in m_cinfo_tabwidget

	QAction * m_cfgact;		//!< Configuration Action
	QAction * m_quitact;		//!< Quit Action
	QAction * m_connectact;		//!< "Start" Action
	QAction * m_disconnectact;	//!< "Stop" Action
	QActionGroup * m_availgrp;	//!< Availability action group

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


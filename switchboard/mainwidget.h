/* XIVO switchboard
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

/* $Id$ */
#ifndef __MAINWIDGET_H__
#define __MAINWIDGET_H__

#include <QWidget>
#include <QMainWindow>
#include <QSplitter>
#include <QSystemTrayIcon>
#include <QTabWidget>

class QAction;
class QActionGroup;
class QCloseEvent;
class QLabel;
class QScrollArea;
class QVBoxLayout;

class BaseEngine;
class CallStackWidget;
class ConfWidget;
class DialPanel;
class DirectoryPanel;
class DisplayMessagesPanel;
class IdentityDisplay;
class LeftPanel;
class LogWidget;
class ParkingPanel;
class Popup;
class SearchPanel;
class ServicePanel;
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

        DialPanel * m_dialpanel;
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


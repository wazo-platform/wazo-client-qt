/* XiVO Client
 * Copyright (C) 2007-2009, Proformatique
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

#ifndef __MAINWIDGET_H__
#define __MAINWIDGET_H__

#include <QDateTime>
#include <QHash>
#include <QSet>
#include <QMainWindow>
#include <QSettings>
#include <QSystemTrayIcon>
#include <QWidget>

#include "xlet.h"
#include "directorypanel.h"

class QAction;
class QActionGroup;
class QCheckBox;
class QClipboard;
class QCloseEvent;
class QComboBox;
class QDockWidget;
class QEvent;
class QGridLayout;
class QKeyEvent;
class QLabel;
class QLineEdit;
class QPushButton;
class QScrollArea;
class QSystemTrayIcon;
class QTabWidget;
class QWidget;
class QUrl;
class QStackedWidget;

class BaseEngine;
class XLetFactory;

/*! \brief Main window class
 */
class MainWidget : public QMainWindow
{
    Q_OBJECT
public:
    //! Constructor
    MainWidget(BaseEngine *,
               QWidget * parent=0);
    //! Destructor
    virtual ~MainWidget();

    void setForceTabs(bool);//!< setter for m_forcetabs
    void setAppearance(const QStringList &);        //!< dock options
    void clearAppearance();
public slots:
    void dispurl(const QUrl &url);
    void customerInfoPopup(const QString &,
                           const QHash<QString, QString> &,
                           const QString &);
    void setSystrayIcon(const QString &);
private slots:
#ifndef Q_WS_WIN
    void clipselection();
    void clipdata();
#endif
    void showConfDialog();
    void showLogin();
    void hideLogin();
    void engineStopped();
    void engineStarted();
    void systrayActivated(QSystemTrayIcon::ActivationReason);
    void systrayMsgClicked();
    void checksAvailState();
    void about();
    void showCredits();
    void config_and_start();
    void logintextChanged(const QString &);
    void loginKindChanged(int);
    void confUpdated();
    void updatePresence(const QVariant &);
    void showMessageBox(const QString &);
    void showWidgetOnTop(QWidget *);
signals:
    void functionKeyPressed(int);
    void pasteToDialPanel(const QString &);
protected:
    void showEvent(QShowEvent *);        //!< Catch show events
    void hideEvent(QHideEvent *);        //!< Catch hide events
    void closeEvent(QCloseEvent *);
    void changeEvent(QEvent *);
    void keyPressEvent(QKeyEvent *);
    void addPanel(const QString &, const QString &, QWidget *);
    void removePanel(const QString &, QWidget *);
private:
    void createActions();        //!< Create Actions (for menus)
    void createMenus();                //!< Create Menus
    void createSystrayIcon();        //!< Create the systray Icon and its menu
    void savePositions() const;
    void updateAppliName();
    void clearPresence();
        
    BaseEngine * m_engine;        //!< Engine
    QSystemTrayIcon * m_systrayIcon;        //!< System Tray Icon
    QIcon m_icon_transp;        //!< Icon Objects
    QIcon m_icon_red;
    QIcon m_icon_green;
    QIcon m_icon_black;
    QStackedWidget * m_central_widget; //!< central widget
    QWidget * m_wid;        //!< "Main" Widget
    QWidget * m_login_widget;   //!< Central Widget for login
    QGridLayout * m_login_layout;   //!< layout for login widget
        
    // Widgets for Xlets
    QTabWidget * m_tabwidget;        //!< Area to display messages, services and histories
    //QHash<QString, XLet *> m_xlet;
    QSet<XLet *> m_xletlist;

    QLabel * m_xivobg;

    bool m_forcetabs;    //!< Flag to allow the display of "unallowed" tabs, useful to test server-side capabilities
    bool m_presence;

    QString m_appliname;
    QHash<QString, QString> m_dockoptions;
    QStringList m_docknames;
    QStringList m_gridnames;
    QStringList m_tabnames;
    QStringList m_allnames;

    bool m_withsystray;

    QHash<QString, QDockWidget *> m_docks;

    // actions :
    QAction * m_cfgact;                //!< Configuration Action
    QAction * m_quitact;                //!< Quit Action
    QAction * m_connectact;                //!< "Connect" Action
    QAction * m_disconnectact;        //!< "Disconnect" Action
    QAction * m_systraymin;                //!< "Go to systray" action
    QAction * m_systraymax;                //!< "Go to systray" action
    QActionGroup * m_availgrp;        //!< Availability action group

    QGridLayout * m_gridlayout;
    QLabel * m_lab1;
    QLabel * m_lab2;
    QLabel * m_lab3;
    QLineEdit * m_qlab1;
    QLineEdit * m_qlab2;
    QLineEdit * m_qlab3;
    QPushButton * m_ack;
    QCheckBox * m_kpass;
    QComboBox * m_loginkind;

    QMenu * m_avail;                //!< Availability submenu
    QHash<QString, QAction *> m_avact;        //!< Actions
    QLabel * m_status;        //!< status indicator

    QMenu * m_filemenu;
    QMenu * m_helpmenu;

    QDateTime m_launchDateTime;
    QSettings * m_settings;

    QClipboard * m_clipboard;
    XLetFactory * m_xletfactory;
};

#endif

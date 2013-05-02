/* XiVO Client
 * Copyright (C) 2007-2013, Avencall
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

#ifndef __MAINWIDGET_H__
#define __MAINWIDGET_H__

#include <QtGui>
#include <QList>

#include <xletlib/functests.h>

#include "systray_manager.h"
#include "config_widget/config_widget.h"

class XLet;
class ConfigWidget;
class LoginWidget;

class MainWidget : public QMainWindow
{
    Q_OBJECT
    FUNCTESTED

    public:
        MainWidget(QSystemTrayIcon & qt_system_tray_icon,
                   SystrayManager & systray_manager);
        ~MainWidget();

        void setAppearance(const QVariantList &);  //!< dock options
        void clearAppearance();
    public slots:
        void customerInfoPopup(const QString &, const QHash<QString, QString> &, const QString &);
        void setSystrayIcon(const QString &);
        void resetState();
    private slots:
        void clipselection();
        void clipdata();
        void showConfDialog();
        void cleanConfDialog();
        void showLogin();
        void hideLogin();
        void engineStopped();
        void engineStarted();
        void connectionStateChanged();
        void systrayActivated(QSystemTrayIcon::ActivationReason);
        void systrayMsgClicked();
        void checksAvailState();
        void about();
        void showCredits();
        void confUpdated();
        void updatePresence();
        void showMessageBox(const QString &);
        void showWidgetOnTop(QWidget *);
        void updateUserStatus(const QString &);
        void setAvailability();  //!< set user status from menu
        void showWindow();
        void hideWindow();
        void minimizeWindow();
    protected:
        virtual void closeEvent(QCloseEvent *);
        virtual void resizeEvent(QResizeEvent *);

        void addPanel(const QString &, const QString &, QWidget *);
        void removePanel(const QString &, QWidget *);
        void syncPresence();
        void setEnabledMenus(const QString & state);
    private:
        void createActions();  //!< Create Actions (for menus)
        void createMenus();  //!< Create Menus
        void createSystrayIcon();  //!< Create the systray Icon and its menu
        void updateAppliName();
        void clearPresence();
        void setMenuAvailabilityEnabled(bool);
        QDockWidget* createDockXlet(const QString& name,
                                    const QString& title,
                                    QDockWidget::DockWidgetFeatures features,
                                    QWidget *widget);
        QSystemTrayIcon & m_systrayIcon;
        QIcon m_icon_transp;  //!< Icon Objects
        QIcon m_icon_red;
        QIcon m_icon_green;
        QIcon m_icon_black;
        QPixmap m_pixmap_disconnected;
        QPixmap m_pixmap_connected;
        QString m_appliname;
        bool m_withsystray;
        QVariantMap m_config;

        QLabel *m_status;  //!< status indicator
        QLabel *m_padlock; //!< padlock icon (SSL indicator)
        QLabel * m_config_profile; //!< profile indicator (status bar)
        QStackedWidget *m_centralWidget;  //!< central widget
        QDockWidget *m_resizingHelper;

        QWidget *m_wid;  //!< "Main" Widget
        LoginWidget *m_loginWidget;  //!< Central Widget for login
        ConfigWidget *m_configwindow;  //!< Config Widget

        QByteArray m_defaultState;


        // Widgets for Xlets
        QTabWidget *m_tabwidget;        //!< Area to display messages, services and histories
        QHash<QString, XLet *> m_xletlist;

        bool m_presence;

        QHash<QString, QString> m_dockoptions;
        QStringList m_docknames;
        QStringList m_gridnames;
        QStringList m_tabnames;
        QStringList m_allnames;


        QHash<QString, QList<QDockWidget *>* > m_docks;

        // actions :
        QAction *m_cfgact;  //!< Configuration Action
        QAction *m_quitact;  //!< Quit Action
        QAction *m_connectact;  //!< "Connect" Action
        QAction *m_disconnectact;  //!< "Disconnect" Action
        QAction *m_systraymin;  //!< "Go to systray" action
        QAction *m_systraymax;  //!< "Go to systray" action
        QActionGroup *m_availgrp;  //!< Availability action group

        QVBoxLayout *m_vL;

        QMenu *m_avail;  //!< Availability submenu
        QHash<QString, QAction *>m_avact;  //!< Actions

        QMenu *m_filemenu;
        QMenu *m_helpmenu;

        QDateTime m_launchDateTime;

        QClipboard * m_clipboard;

        SystrayManager & m_systray_manager;
};

#endif

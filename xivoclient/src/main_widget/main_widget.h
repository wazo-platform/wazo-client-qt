/* XiVO Client
 * Copyright (C) 2013, Avencall
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

#ifndef __MAINWINDOW_H__
#define __MAINWINDOW_H__

#include <QtGui>
#include <QList>
#include <QMainWindow>

#include <systray_manager.h>
#include <login_widget/login_widget.h>
#include <config_widget/config_widget.h>
#include <ui_main_widget.h>
#include <xletlib/functests.h>

#include "menu_availability.h"
#include "statusbar.h"


class MainWindow : public QMainWindow
{
    Q_OBJECT
    FUNCTESTED

    public:
        MainWindow(QWidget *parent = 0);
        ~MainWindow();

    private slots:
        void clipselection();
        void clipdata();
        void showMessageBox(const QString &);
        void showLogin();
        void hideLogin();
        void showConfDialog();
        void cleanConfDialog();
        void confUpdated();
        void engineStopped();
        void engineStarted();
        void connectionStateChanged();
        void showWindow();
        void hideWindow();
        void minimizeWindow();
        void systrayActivated(QSystemTrayIcon::ActivationReason);
        void systrayMsgClicked();

    private:
        void updateAppliName();
        void createSystrayIcon();

        Ui::MainWindow *ui;
        QWidget *m_main_widget;
        QSystemTrayIcon *m_systray_icon;

        QIcon m_icon_transp;
        QIcon m_icon_red;
        QIcon m_icon_green;
        QIcon m_icon_black;

        QStackedWidget *m_central_widget;
        ConfigWidget *m_config_widget;
        LoginWidget *m_login_widget;
        MenuAvailability *m_menu_availability;
        Statusbar *m_menu_statusbar;
        QString m_appliname;
        QClipboard * m_clipboard;

};

#endif

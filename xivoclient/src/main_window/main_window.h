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

#ifndef __MAIN_WINDOW_H__
#define __MAIN_WINDOW_H__

#include <QtGui>
#include <QList>
#include <QMainWindow>

#include <login_widget/login_widget.h>
#include <config_widget/config_widget.h>
#include <ui_main_window.h>
#include <xletlib/functests.h>

#include "menu_availability.h"
#include "statusbar.h"
#include "system_tray_icon.h"
#include "central_widget.h"


class CentralWidget;
class ConfigWidget;
class MenuAvailability;
class Statusbar;
class SystemTrayIcon;


class MainWindow : public QMainWindow
{
    Q_OBJECT
    FUNCTESTED

    public:
        MainWindow(QWidget *parent = 0);
        ~MainWindow();
        void initialize();

    public slots:
        void setAppIcon(const QString & def);
        void showWindow();
        void hideWindow();

    private slots:
        void clipselection();
        void clipdata();
        void showMessageBox(const QString &);
        void showConfDialog();
        void cleanConfDialog();
        void confUpdated();
        void setStatusNotLogged();
        void setStatusLogged();
        void minimizeWindow();

    private:
        void setTitle(const QString &);

        Ui::MainWindow *ui;

        CentralWidget *m_central_widget;
        SystemTrayIcon *m_systray_icon;
        ConfigWidget *m_config_widget;
        MenuAvailability *m_menu_availability;
        Statusbar *m_menu_statusbar;
        QClipboard * m_clipboard;

};

#endif

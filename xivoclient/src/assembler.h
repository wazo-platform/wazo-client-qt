/* XiVO Client
 * Copyright (C) 2007-2014 Avencall
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

#ifndef _ASSEMBLER_H_
#define _ASSEMBLER_H_

#include "main_window/main_window.h"
#include "main_window/central_widget.h"
#include "main_window/main_widget.h"
#include "main_window/menu_availability.h"
#include "main_window/statusbar.h"
#include "main_window/system_tray_icon.h"
#include "main_window/xlet_dispatcher.h"
#include "login_widget/login_widget.h"
#include "config_widget/config_widget.h"

class MainWindow;
class CentralWidget;
class MainWidget;
class ConfigWidget;
class MenuAvailability;
class Statusbar;
class SystemTrayIcon;
class LoginWidget;
class XletDispatcher;

class Assembler
{
    public:
        Assembler();
        virtual ~Assembler();

        MainWindow *mainWindow();
        ConfigWidget *configWidget();
        CentralWidget *centralWidget();
        LoginWidget *loginWidget();
        MainWidget *mainWidget();
        MenuAvailability *menuAvailability();
        Statusbar *statusbar();
        SystemTrayIcon *systemTrayIcon();
        XletDispatcher *xletDispatcher();

    private:
        MainWindow *m_main_window;
        ConfigWidget *m_config_widget;
        CentralWidget *m_central_widget;
        LoginWidget *m_login_widget;
        MainWidget *m_main_widget;
        MenuAvailability *m_menu_availabilty;
        Statusbar *m_statusbar;
        SystemTrayIcon *m_system_tray_icon;
        XletDispatcher *m_xlet_dispatcher;
};

extern Assembler *assembler;

#endif /* _ASSEMBLER_H_ */

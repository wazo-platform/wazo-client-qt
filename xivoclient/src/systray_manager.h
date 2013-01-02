/* XiVO Client
 * Copyright (C) 2007-2012, Avencall
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

#ifndef __SYSTRAY_MANAGER_H__
#define __SYSTRAY_MANAGER_H__

#include "application_status_icon.h"

class ApplicationStatusIconManager;
class QIcon;

template <class _QSystemTrayIcon>
class SystrayManager
{
    public:
        SystrayManager(const ApplicationStatusIconManager & application_status_icon_manager,
                       _QSystemTrayIcon & qt_system_tray_icon);
        void changeIcon(ApplicationStatusIcon new_icon_id);
        void showNotification(const QString & title, const QString & message);

    private:
        const ApplicationStatusIconManager & m_application_status_icon_manager;
        _QSystemTrayIcon & m_qt_system_tray_icon;
};

/* Template class need to be implemented in the same place that it is defined.
 * This is because the compiler can't know with which type the template will be
 * used, this will be detected on compilation.
 */
#include "systray_manager.cpp"

#endif

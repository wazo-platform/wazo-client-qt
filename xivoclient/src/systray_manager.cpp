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

#include <QDebug>
#include <QIcon>

#include "application_status_icon_manager.h"

// Template class : the header includes the implementation
// #include "systray_manager.h"

template <class _QSystemTrayIcon>
SystrayManagerTpl<_QSystemTrayIcon>::SystrayManagerTpl(const ApplicationStatusIconManager & application_status_icon_manager,
                                  _QSystemTrayIcon & qt_system_tray_icon)
    : m_application_status_icon_manager(application_status_icon_manager),
      m_qt_system_tray_icon(qt_system_tray_icon)
{
}

template <class _QSystemTrayIcon>
void SystrayManagerTpl<_QSystemTrayIcon>::changeIcon(ApplicationStatusIcon new_icon_id)
{
    QIcon new_icon = this->m_application_status_icon_manager.getApplicationStatusIcon(new_icon_id);
    this->m_qt_system_tray_icon.setIcon(new_icon);
}

template <class _QSystemTrayIcon>
void SystrayManagerTpl<_QSystemTrayIcon>::showNotification(const QString & title, const QString & message)
{
    this->m_qt_system_tray_icon.showMessage(title,
                                            message,
                                            QSystemTrayIcon::Information,
                                            5000);
}

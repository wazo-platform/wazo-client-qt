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

/*
 * Source: http://www.codinginlondon.com/2009/05/cheap-ioc-in-native-c.html
 * Licence: Unknown
 */

#ifndef __CONTEXT_H__
#define  __CONTEXT_H__

#include <typeinfo>

#include <QMap>
#include <QString>
#include <QDebug>

#include <QSystemTrayIcon>
#include "application_status_icon_manager.h"
#include "systray_manager.h"

/*
 * Some Qt classes have to be treated separately:
 * - declare them as pointers
 * - instanciate them with new
 * - make sure they have a Qt parent
 *
 * The main reason is that some Qt classes (e.g. QSystemTrayIcon) need the main
 * window when they are destroyed (that may be a Qt bug). Hence they need to be
 * destroyed through the parenting system of Qt, which uses "delete", which can
 * only be used on pointers gotten from new.
 */

class Context
{
    public:
    template<class T> static T& get()
    {
        static Context instance;  // Calls the constructor only the first time
        return *((T*)type_instance_map[typeid(T).name()]);
    }

    private:
    static QMap<QString, void* > type_instance_map;

    Context()
        : m_qt_system_tray_icon(new QSystemTrayIcon()),
          m_application_status_icon_manager(),
          m_systray_manager(m_application_status_icon_manager,
                            *m_qt_system_tray_icon)
    {
        registerInstance<SystrayManager>(m_systray_manager);
        registerInstance<ApplicationStatusIconManager>(m_application_status_icon_manager);
        registerInstance<QSystemTrayIcon>(*m_qt_system_tray_icon);
    }


    template<class T> static void registerInstance(const T& instance)
    {
        type_instance_map[typeid(T).name()] = (void*)&instance;
    }

    QSystemTrayIcon * m_qt_system_tray_icon;
    ApplicationStatusIconManager m_application_status_icon_manager;
    SystrayManager m_systray_manager;
};

#endif

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

#include <QString>
#include <QStringList>

#include "init_watcher.h"

InitWatcher::InitWatcher()
    :m_watching_started(false)
{
}

void InitWatcher::watchList(QString list_name, QStringList ids)
{
    if (ids.isEmpty()) {
        return;
    }
    m_stack.insert(list_name, ids);
    if (m_watching_started == false) {
        m_watching_started = true;
        emit watching();
    }
}

void InitWatcher::sawItem(const QString & list_name, const QString & item_id)
{
    m_stack[list_name].removeOne(item_id);
    if (m_stack[list_name].isEmpty()) {
        m_stack.remove(list_name);
    }

    if (m_stack.isEmpty() && m_watching_started) {
        m_watching_started = false;
        emit sawAll();
    }
}

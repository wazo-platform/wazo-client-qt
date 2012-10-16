/* XiVO Client
 * Copyright (C) 2007-2011, Avencall
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

#include "queuememberinfo.h"

QueueMemberInfo::QueueMemberInfo(const QString & ipbxid,
                                 const QString & id)
    : XInfo(ipbxid, id)
{
}

bool QueueMemberInfo::updateConfig(const QVariantMap &prop)
{
    bool haschanged = false;
    haschanged |= setIfChangeString(prop, "queue_name", & m_queue_name);
    haschanged |= setIfChangeString(prop, "interface", & m_interface);
    haschanged |= setIfChangeString(prop, "status", & m_status);
    haschanged |= setIfChangeString(prop, "paused", & m_paused);
    haschanged |= setIfChangeString(prop, "membership", & m_membership);
    haschanged |= setIfChangeString(prop, "penalty", & m_penalty);
    haschanged |= setIfChangeString(prop, "callstaken", & m_callstaken);
    haschanged |= setIfChangeString(prop, "lastcall", & m_lastcall);
    return haschanged;
}

bool QueueMemberInfo::updateStatus(const QVariantMap & prop)
{
    bool haschanged = false;
    haschanged |= setIfChangeString(prop, "status", & m_status);
    haschanged |= setIfChangeString(prop, "paused", & m_paused);
    haschanged |= setIfChangeString(prop, "membership", & m_membership);
    haschanged |= setIfChangeString(prop, "callstaken", & m_callstaken);
    haschanged |= setIfChangeString(prop, "penalty", & m_penalty);
    haschanged |= setIfChangeString(prop, "lastcall", & m_lastcall);
    return haschanged;
}

bool QueueMemberInfo::is_agent() const
{
    QStringList interface_split = m_interface.split("/");
    return interface_split[0] == "Agent";
}

QString QueueMemberInfo::agentNumber() const
{
    QStringList interface_split = m_interface.split("/");
    if (interface_split.size() < 2) {
        return QString("");
    } else {
        return QString(interface_split[1]);
    }
}

QString QueueMemberInfo::agent_or_phone() const
{
    return this->is_agent() ? QObject::tr("Agent") : QObject::tr("Phone");
}

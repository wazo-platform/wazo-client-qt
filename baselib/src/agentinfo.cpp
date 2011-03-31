/* XiVO Client
 * Copyright (C) 2007-2011, Proformatique
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

#include <QDebug>
#include "agentinfo.h"

AgentInfo::AgentInfo(const QString & ipbxid,
                     const QString & id)
    : XInfo(ipbxid, id)
{
}

bool AgentInfo::updateConfig(const QVariantMap & prop)
{
    bool haschanged = false;
    if (m_properties != prop) {
        m_properties = prop;
        haschanged = true;

        m_context = prop.value("context").toString();
        m_agentnumber = prop.value("number").toString();
        QString firstname = prop.value("firstname").toString();
        QString lastname = prop.value("lastname").toString();
        m_fullname = QString("%1 %2").arg(firstname).arg(lastname);
    }
    return haschanged;
}

bool AgentInfo::updateStatus(const QVariantMap & prop)
{
    bool haschanged = false;

    if (prop.contains("status") && (m_status != prop.value("status").toString())) {
        m_status = prop.value("status").toString();
        haschanged = true;
    }
    if (prop.contains("phonenumber") && (m_phonenumber != prop.value("phonenumber").toString())) {
        m_phonenumber = prop.value("phonenumber").toString();
        haschanged = true;
    }

    if (prop.contains("queues")) {
        m_xqueueids.clear();
        foreach (QString queueid, prop.value("queues").toStringList()) {
            QString xqueueid = QString("%1/%2").arg(m_ipbxid).arg(queueid); // to match against queue membership
            m_xqueueids.append(xqueueid);
        }
        haschanged = true;
    }

    if (prop.contains("groups")) {
        m_xgroupids.clear();
        foreach (QString groupid, prop.value("groups").toStringList()) {
            QString xgroupid = QString("%1/%2").arg(m_ipbxid).arg(groupid); // to match against group membership
            m_xgroupids.append(xgroupid);
        }
        haschanged = true;
    }

    return haschanged;
}

const QString & AgentInfo::context() const
{
    return m_context;
}

const QString & AgentInfo::agentNumber() const
{
    return m_agentnumber;
}

const QString & AgentInfo::fullname() const
{
    return m_fullname;
}

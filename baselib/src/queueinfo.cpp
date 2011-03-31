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
#include "queueinfo.h"

QueueInfo::QueueInfo(const QString & ipbxid,
                     const QString & id)
    : XInfo(ipbxid, id)
{
}

bool QueueInfo::updateConfig(const QVariantMap & prop)
{
    bool haschanged = false;
    if (prop.contains("context") && (m_context != prop.value("context").toString())) {
        m_context = prop.value("context").toString();
        haschanged = true;
    }
    if (prop.contains("name") && (m_name != prop.value("name").toString())) {
        m_name = prop.value("name").toString();
        haschanged = true;
    }
    if (prop.contains("number") && (m_number != prop.value("number").toString())) {
        m_number = prop.value("number").toString();
        haschanged = true;
    }
    return haschanged;
}

bool QueueInfo::updateStatus(const QVariantMap & prop)
{
    bool haschanged = false;
    if (prop.contains("agentmembers")) {
        m_agentids = prop.value("agentmembers").toStringList();
        m_xagentids.clear();
        m_agentmembers.clear();
        m_xagentmembers.clear();
        foreach (QString agentid, m_agentids) {
            QString xagentid = QString("%1/%2").arg(m_ipbxid).arg(agentid); // to match against agent membership
            QString agentmember = QString("qa:%1-%2").arg(m_id).arg(agentid); // for requests to server
            QString xagentmember = QString("%1/qa:%2-%3").arg(m_ipbxid).arg(m_id).arg(agentid); // for link to properties
            m_xagentids.append(xagentid);
            m_agentmembers.append(agentmember);
            m_xagentmembers.append(xagentmember);
        }
        haschanged = true;
    }
    if (prop.contains("phonemembers")) {
        m_phonemembers.clear();
        m_xphonemembers.clear();
        foreach (QString phoneid, prop.value("phonemembers").toStringList()) {
            QString phonemember = QString("qp:%1-%2").arg(m_id).arg(phoneid);
            QString xphonemember = QString("%1/qp:%2-%3").arg(m_ipbxid).arg(m_id).arg(phoneid);
            m_phonemembers.append(phonemember);
            m_xphonemembers.append(xphonemember);
        }
        haschanged = true;
    }
    if (m_properties != prop) {
        m_properties = prop;
        haschanged = true;
    }
    return haschanged;
}

// bool QueueInfo::hasAgentId(const QString & agentid)
// {
//     return m_agentids.contains(agentid);
// }

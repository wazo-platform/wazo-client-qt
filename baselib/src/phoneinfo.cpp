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
#include "phoneinfo.h"

PhoneInfo::PhoneInfo(const QString & ipbxid,
                     const QString & id)
    : XInfo(ipbxid, id)
{
    m_simultcalls = 0;
    m_initialized = false;
    m_enable_hint = false;
}


bool PhoneInfo::updateConfig(const QVariantMap & prop)
{
    bool haschanged = false;
    haschanged |= setIfChangeString(prop, "protocol", & m_protocol);
    haschanged |= setIfChangeString(prop, "context", & m_context);
    haschanged |= setIfChangeString(prop, "number", & m_number);
    haschanged |= setIfChangeString(prop, "iduserfeatures", & m_iduserfeatures);

    haschanged |= setIfChangeInt(prop, "simultcalls", & m_simultcalls);
    haschanged |= setIfChangeBool(prop, "initialized", & m_initialized);
    haschanged |= setIfChangeBool(prop, "enable_hint", & m_enable_hint);

    haschanged |= setIfChangeBool(prop, "enablerna", & m_enablerna);
    haschanged |= setIfChangeBool(prop, "enableunc", & m_enableunc);
    haschanged |= setIfChangeBool(prop, "enablebusy", & m_enablebusy);
    haschanged |= setIfChangeString(prop, "destrna", & m_destrna);
    haschanged |= setIfChangeString(prop, "destunc", & m_destunc);
    haschanged |= setIfChangeString(prop, "destbusy", & m_destbusy);

    haschanged |= setIfChangeBool(prop, "enableautomon", & m_enableautomon);
    haschanged |= setIfChangeBool(prop, "enablednd", & m_enablednd);
    haschanged |= setIfChangeBool(prop, "enablevoicemail", & m_enablevoicemail);
    haschanged |= setIfChangeBool(prop, "enablexfer", & m_enablexfer);
    haschanged |= setIfChangeBool(prop, "incallfilter", & m_incallfilter);
    haschanged |= setIfChangeBool(prop, "callrecord", & m_callrecord);

    return haschanged;
}

bool PhoneInfo::updateStatus(const QVariantMap & prop)
{
    bool haschanged = false;
    haschanged |= setIfChangeString(prop, "hintstatus", & m_hintstatus);
    if(prop.contains("channels")) {
        m_channels = prop.value("channels").toStringList();
        m_xchannels.clear();
        foreach (QString channel, m_channels) {
            QString xchannel = QString("%1/%2").arg(m_ipbxid).arg(channel);
            m_xchannels.append(xchannel);
        }
        haschanged = true;
    }
    return haschanged;
}

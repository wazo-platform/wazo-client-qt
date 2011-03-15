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
                     const QString & phoneid)
{
    m_ipbxid = ipbxid;
    m_phoneid = phoneid;
    m_simultcalls = 0;
    m_initialized = false;
    m_enable_hint = false;
}

bool PhoneInfo::updateConfig(const QVariantMap & prop)
{
    bool haschanged = true;
    if (prop.contains("protocol"))
        m_protocol = prop.value("protocol").toString();
    if (prop.contains("context"))
        m_context = prop.value("context").toString();
    if (prop.contains("number"))
        m_number = prop.value("number").toString();
    if (prop.contains("simultcalls"))
        m_simultcalls = prop.value("simultcalls").toInt();
    if (prop.contains("initialized"))
        m_initialized = prop.value("initialized").toBool();
    if (prop.contains("enable_hint"))
        m_enable_hint = prop.value("enable_hint").toBool();

    if (prop.contains("enablerna"))
        m_enablerna = prop.value("enablerna").toBool();
    if (prop.contains("enableunc"))
        m_enableunc = prop.value("enableunc").toBool();
    if (prop.contains("enablebusy"))
        m_enablebusy = prop.value("enablebusy").toBool();
    if (prop.contains("destrna"))
        m_destrna = prop.value("destrna").toString();
    if (prop.contains("destunc"))
        m_destunc = prop.value("destunc").toString();
    if (prop.contains("destbusy"))
        m_destbusy = prop.value("destbusy").toString();

    if (prop.contains("enableautomon"))
        m_enableautomon = prop.value("enableautomon").toBool();
    if (prop.contains("enablednd"))
        m_enablednd = prop.value("enablednd").toBool();
    if (prop.contains("enablevoicemail"))
        m_enablevoicemail = prop.value("enablevoicemail").toBool();
    if (prop.contains("enablexfer"))
        m_enablexfer = prop.value("enablexfer").toBool();
    if (prop.contains("incallfilter"))
        m_incallfilter = prop.value("incallfilter").toBool();
    if (prop.contains("callrecord"))
        m_callrecord = prop.value("callrecord").toBool();

    return haschanged;
}

bool PhoneInfo::updateStatus(const QVariantMap & prop)
{
    bool haschanged = true;
    if (prop.contains("hintstatus"))
        m_hintstatus = prop.value("hintstatus").toString();
    if(prop.contains("channels"))
        m_channels = prop.value("channels").toStringList();
    return haschanged;
}

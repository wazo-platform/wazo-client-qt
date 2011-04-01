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

#include "meetmeinfo.h"

MeetmeInfo::MeetmeInfo(const QString & ipbxid,
                       const QString & id)
    : XInfo(ipbxid, id)
{
}

const QString & MeetmeInfo::roomname() const
{
    return m_roomname;
}

const QString & MeetmeInfo::roomnumber() const
{
    return m_roomnumber;
}

const QString& MeetmeInfo::adminid() const
{
    return m_adminid;
}

const QStringList& MeetmeInfo::adminlist() const
{
    return m_adminlist;
}

const QVariantMap& MeetmeInfo::uniqueids() const
{
    return m_uniqueids;
}

const QString& MeetmeInfo::adminnum() const
{
    return m_adminnum;
}

bool MeetmeInfo::paused() const
{
    return m_paused;
}

bool MeetmeInfo::updateConfig(const QVariantMap & prop)
{
    bool haschanged = false;
    // fixed-by-config stuff
    haschanged |= setIfChangeString(prop, "context", & m_context);
    haschanged |= setIfChangeString(prop, "roomname", & m_roomname);
    haschanged |= setIfChangeString(prop, "roomnumber", & m_roomnumber);

    haschanged |= setIfChangeString(prop, "pin", & m_pin);
    haschanged |= setIfChangeString(prop, "pinadmin", & m_adminpin);
    haschanged |= setIfChangeBool(prop, "paused", & m_paused);

    // variable stuff
    haschanged = true;
    m_adminlist = prop.value("adminlist").toStringList();
    m_uniqueids = prop.value("uniqueids").toMap();
    return haschanged;
}

bool MeetmeInfo::updateStatus(const QVariantMap & prop)
{
    bool haschanged = true;
    QString action = prop.value("action").toString();
    QString uniqueid = prop.value("uniqueid").toString();

    haschanged |= setIfChangeString(prop, "adminid", & m_adminid);
    haschanged |= setIfChangeString(prop, "adminnum", & m_adminnum);
    m_adminlist = prop.value("adminlist").toStringList();
    if (prop.contains("details"))
        m_uniqueids[uniqueid] = prop.value("details").toMap();

    if (action == "leave") {
        m_uniqueids.remove(uniqueid);
    } else if(action == "changeroompausedstate") {
        m_paused = prop.value("paused").toBool();
    }
    return haschanged;
}

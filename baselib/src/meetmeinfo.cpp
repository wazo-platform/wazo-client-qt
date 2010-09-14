/* XiVO Client
 * Copyright (C) 2007-2010, Proformatique
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

const QString& MeetmeInfo::roomname() const
{
    return m_roomname;
}

const QString& MeetmeInfo::roomnumber() const
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

const QString& MeetmeInfo::astId() const
{
    return m_astid;
}

void MeetmeInfo::setProperties(const QString &astid, const QVariantMap &properties)
{
    // fixed-by-config stuff
    m_context = properties["context"].toString();
    m_roomname = properties["roomname"].toString();
    m_roomnumber = properties["roomnumber"].toString();
    m_pin = properties["pin"].toString();
    m_adminpin = properties["pinadmin"].toString();
    m_astid = astid;
    
    // variable stuff
    m_adminid = properties["adminid"].toString();
    m_adminnum = properties["adminnum"].toString();
    m_adminlist = properties["adminlist"].toStringList();
    m_uniqueids = properties["uniqueids"].toMap();
    m_paused = properties["paused"].toBool();
}

bool MeetmeInfo::update(const QVariantMap &map)
{
    QString action = map["action"].toString();
    QString uniqueid = map["uniqueid"].toString();
    
    if (!map["adminid"].toString().isNull()) {
        m_adminid = map["adminid"].toString();
    }
    m_adminnum = map["adminnum"].toString();
    m_adminlist = map["adminlist"].toStringList();
    if (map.contains("details"))
        m_uniqueids[uniqueid] = map["details"].toMap();

    if (action == "leave") {
        m_uniqueids.remove(uniqueid);
    } else if(action == "changeroompausedstate") {
        m_paused = map["paused"].toBool();
    }
    return true;
}

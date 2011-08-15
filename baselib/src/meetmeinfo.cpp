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

#include <QDebug>

MeetmeInfo::MeetmeInfo(const QString & ipbxid,
                       const QString & id)
    : XInfo(ipbxid, id)
{
}

const QString & MeetmeInfo::name() const
{
    return m_name;
}

const QString & MeetmeInfo::number() const
{
    return m_number;
}

const QVariantMap & MeetmeInfo::channels() const
{
    return m_channels;
}

bool MeetmeInfo::paused() const
{
    return m_paused;
}

bool MeetmeInfo::updateConfig(const QVariantMap & prop)
{
    qDebug() << Q_FUNC_INFO << prop;
    bool haschanged = false;
    haschanged |= setIfChangeString(prop, "context", & m_context);
    haschanged |= setIfChangeString(prop, "name", & m_name);
    haschanged |= setIfChangeString(prop, "confno", & m_number);
    haschanged |= setIfChangeString(prop, "admin_moderationmode", & m_admin_moderationmode);
    haschanged |= setIfChangeBool(prop, "pin_needed", & m_pin_needed);
    return haschanged;
}

bool MeetmeInfo::updateStatus(const QVariantMap & prop)
{
    qDebug() << Q_FUNC_INFO << prop;
    bool haschanged = false;
    haschanged |= setIfChangeBool(prop, "paused", & m_paused);
    haschanged |= setIfChangeString(prop, "pseudochan", & m_pseudochan);
    if (prop.contains("channels")) {
        m_channels = prop.value("channels").toMap();
        haschanged = true;
    }
    return haschanged;
}

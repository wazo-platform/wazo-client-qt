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

#include "channelinfo.h"

/*! \brief Constructor
 *
 * just set userid
 */
ChannelInfo::ChannelInfo(const QString & ipbxid,
                         const QString & id)
{
    m_ipbxid = ipbxid;
    m_channel = id;
    m_xchannel = QString("%1/%2").arg(m_ipbxid).arg(m_channel);
    m_isparked = false;
}

// bool ChannelInfo::updateConfig(const QVariantMap &)
// {
//     bool haschanged = true;
//     return haschanged;
// }

bool ChannelInfo::updateStatus(const QVariantMap & qvm)
{
    bool haschanged = true;
    if (qvm.contains("direction"))
        m_direction = qvm.value("direction").toString();
    if (qvm.contains("talkingto_kind"))
        m_talkingto_kind = qvm.value("talkingto_kind").toString();
    if (qvm.contains("talkingto_id"))
        m_talkingto_id = qvm.value("talkingto_id").toString();
    if (qvm.contains("commstatus"))
        m_commstatus = qvm.value("commstatus").toString();
    if (qvm.contains("peerdisplay"))
        m_peerdisplay = qvm.value("peerdisplay").toString();
    if (qvm.contains("timestamp"))
        m_timestamp = qvm.value("timestamp").toDouble();
    return haschanged;
}

const QString & ChannelInfo::talkingto_kind() const
{
    return m_talkingto_kind;
}

const QString & ChannelInfo::talkingto_id() const
{
    return m_talkingto_id;
}

const QString & ChannelInfo::channel() const
{
    return m_channel;
}

const QString & ChannelInfo::xchannel() const
{
    return m_xchannel;
}

const QString & ChannelInfo::direction() const
{
    return m_direction;
}

const QString & ChannelInfo::commstatus() const
{
    return m_commstatus;
}

double ChannelInfo::timestamp() const
{
    return m_timestamp;
}

const QString ChannelInfo::peerdisplay() const
{
    // go fetch information about 'talking to'
    return m_peerdisplay;
}

int ChannelInfo::linenumber() const
{
    return m_linenumber;
}

bool ChannelInfo::ismonitored() const
{
    return m_ismonitored;
}

bool ChannelInfo::isspied() const
{
    return m_isspied;
}

bool ChannelInfo::isholded() const
{
    return m_isholded;
}

bool ChannelInfo::isparked() const
{
    return m_isparked;
}

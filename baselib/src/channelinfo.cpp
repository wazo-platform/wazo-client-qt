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
}

// bool ChannelInfo::updateConfig(const QVariantMap &)
// {
//     bool haschanged = true;
//     return haschanged;
// }

bool ChannelInfo::updateStatus(const QVariantMap & qvm)
{
    bool haschanged = true;
    m_talkingto_kind = qvm.value("talkingto_kind").toString();
    m_talkingto_id = qvm.value("talkingto_id").toString();
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

const QString & ChannelInfo::status() const
{
    return m_status;
}

const QString ChannelInfo::peerdisplay() const
{
    // go fetch information about 'talking to'
    return "who";
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

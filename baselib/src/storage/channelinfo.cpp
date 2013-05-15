/* XiVO Client
 * Copyright (C) 2007-2013, Avencall
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

#include "baseengine.h"
#include "channelinfo.h"

ChannelInfo::ChannelInfo(const QString & ipbxid,
                         const QString & id)
    : XInfo(ipbxid, id),
      m_timestamp(0.0),
      m_linenumber(0),
      m_isholded(false)
{
}

bool ChannelInfo::updateStatus(const QVariantMap & prop)
{
    bool haschanged = false;
    haschanged |= setIfChangeString(prop, "direction", & m_direction);
    haschanged |= setIfChangeString(prop, "talkingto_kind", & m_talkingto_kind);
    haschanged |= setIfChangeString(prop, "talkingto_id", & m_talkingto_id);
    haschanged |= setIfChangeString(prop, "commstatus", & m_commstatus);
    haschanged |= setIfChangeString(prop, "state", & m_state);
    haschanged |= setIfChangeDouble(prop, "timestamp", & m_timestamp);
    haschanged |= setIfChangeBool(prop, "holded", & m_isholded);

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

int ChannelInfo::linenumber() const
{
    return m_linenumber;
}

bool ChannelInfo::isholded() const
{
    return m_isholded;
}

bool ChannelInfo::isTalking() const
{
    return m_state == "Up";
}

bool ChannelInfo::canBeTransferred() const
{
    bool talking = this->isTalking();
    bool holded = this->isholded();

    if (holded) {
        return false;
    }

    return talking;
}

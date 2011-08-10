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

#include "baseengine.h"
#include "channelinfo.h"

/*! \brief Constructor
 *
 * just set userid
 */
ChannelInfo::ChannelInfo(const QString & ipbxid,
                         const QString & id)
    : XInfo(ipbxid, id)
{
    m_isparked = false;
}

bool ChannelInfo::updateStatus(const QVariantMap & prop)
{
    bool haschanged = false;
    bool meetmechanged = false;
    haschanged |= setIfChangeString(prop, "direction", & m_direction);
    haschanged |= setIfChangeString(prop, "talkingto_kind", & m_talkingto_kind);
    haschanged |= setIfChangeString(prop, "talkingto_id", & m_talkingto_id);
    haschanged |= setIfChangeString(prop, "commstatus", & m_commstatus);
    haschanged |= setIfChangeString(prop, "thisdisplay", & m_thisdisplay);
    haschanged |= setIfChangeString(prop, "peerdisplay", & m_peerdisplay);
    haschanged |= setIfChangeDouble(prop, "timestamp", & m_timestamp);

    meetmechanged |= setIfChangeBool(prop, "meetme_isadmin", & m_meetme_isadmin);
    meetmechanged |= setIfChangeBool(prop, "meetme_isauthed", & m_meetme_isauthed);
    meetmechanged |= setIfChangeBool(prop, "meetme_ismuted", & m_meetme_ismuted);
    meetmechanged |= setIfChangeInt(prop, "meetme_usernum", & m_meetme_usernum);

    // If a meetme* status changed update the meetme tree
    if (meetmechanged) {
        foreach (const QString & xmeetmeid, b_engine->iterover("meetmes").keys()) {
            const MeetmeInfo * m = b_engine->meetme(xmeetmeid);
            if (m && m->xchannels().contains(xid())) {
                b_engine->addUpdateConfMemberInTree(b_engine->tree(), m->xid());
            }
        }
    }

    return haschanged || meetmechanged;
}

/*! \brief Returns a string representation of a ChannelInfo */
QString ChannelInfo::toString() const
{
    QString s;
    s += "This display(" + m_thisdisplay + ") ";
    s += "Peer display(" + m_peerdisplay + ") ";
    s += "Comm status(" + m_commstatus + ") ";
    s += "Talking to kind(" + m_talkingto_kind + ") ";
    s += "Talking to id(" + m_talkingto_id + ") ";
    s += "Parked(" + QString(m_isparked ? "true" : "false")  + ")";
    s += "Held(" + QString(m_isholded ? "true" : "false") + ")";
    return s;
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

const QString ChannelInfo::thisdisplay() const
{
    // go fetch information about 'relations ?'
    return m_thisdisplay;
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

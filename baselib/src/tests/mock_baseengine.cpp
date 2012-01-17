/* XiVO Client
 * Copyright (C) 2007-2011, Avencall
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

#include <QtTest/QtTest>

#include "mock_phoneinfo.h"
#include "mock_channelinfo.h"

#include "mock_baseengine.h"

MockBaseEngine * b_engine;

MockBaseEngine::MockBaseEngine()
{
}

void MockBaseEngine::setPhone(const QString & xphoneid, PhoneInfo *phone)
{
    this->m_phones[xphoneid] = phone;
}

const PhoneInfo * MockBaseEngine::phone(const QString &xphoneid) const
{
    return this->m_phones[xphoneid];
}

void MockBaseEngine::setUser(const QString & xuserid, UserInfo *user)
{
    this->m_users[xuserid] = user;
}

const UserInfo * MockBaseEngine::user(const QString &xuserid) const
{
    return this->m_users[xuserid];
}

void MockBaseEngine::setChannel (const QString &xchannelid, ChannelInfo *channel)
{
    this->m_channels[xchannelid] = channel;
}

const ChannelInfo * MockBaseEngine::channel(const QString &xchannelid) const
{
    return this->m_channels[xchannelid];
}

QHash<QString, XInfo *> MockBaseEngine::iterover(const QString &listname) const
{
    QHash<QString, XInfo *> ret;
    return ret;
}

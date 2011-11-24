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

const MockPhoneInfo * MockBaseEngine::phone(const QString &xphoneid) const
{
    QVariantMap config;
    MockPhoneInfo *ret = NULL;

    if (xphoneid == "asterisk/p1") {
        ret = new MockPhoneInfo("asterisk", "p1");
        config["identity"] = "SIP/sip1";
        ret->setConfig(config);
    } else if (xphoneid == "asterisk/p2") {
        ret = new MockPhoneInfo("asterisk", "p2");
        config["identity"] = "SIP/sip2";
        ret->setConfig(config);
    }
    return ret;
}

const UserInfo * MockBaseEngine::user(const QString &xphoneid) const
{
    return NULL;
}

const MockChannelInfo * MockBaseEngine::channel(const QString &xphoneid) const
{
    return NULL;
}

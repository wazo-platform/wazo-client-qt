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

#ifndef __BASEENGINE_H__
#define __BASEENGINE_H__

#include <QMap>
#include <QHash>

#include <userinfo.h>

#include "mock_phoneinfo.h"
#include "mock_channelinfo.h"

class QString;
class XInfo;

// Creates getter and setter for SomethingInfo
#define ADD_GET_SET(type,Type) \
    public: void set##Type (const QString & x##type##id, Type##Info * type) { \
        this->m_##type##s[x##type##id] = type; \
    } \
    const Type##Info * type (const QString & x##type##id) const { \
        return this->m_##type##s[x##type##id]; \
    } \
    private : QMap<QString, Type##Info *> m_##type##s;


class MockBaseEngine
{
    public:
        MockBaseEngine();
        QHash<QString, XInfo *> iterover(const QString & listname) const;
        ADD_GET_SET(phone,Phone)
        ADD_GET_SET(user,User)
        ADD_GET_SET(channel,Channel)
};

typedef MockBaseEngine BaseEngine;

extern MockBaseEngine * b_engine;

#endif

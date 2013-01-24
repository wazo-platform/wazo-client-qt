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

#include <QDebug>
#include <QPixmap>
#include <QObject>

#include <storage/userinfo.h>
#include <dao/phonedao.h>
#include <dao/userdao.h>
#include <xletlib/taintedpixmap.h>

#include "mobile_directory_entry.h"

MobileDirectoryEntry::MobileDirectoryEntry(const UserInfo &user)
    : m_user(user)
{
}

bool MobileDirectoryEntry::operator==(const MobileDirectoryEntry & other) const
{
    const QString & this_user = this->m_user.id();
    const QString & other_user = other.m_user.id();
    return this_user == other_user;
}

MobileDirectoryEntry & MobileDirectoryEntry::operator=(const MobileDirectoryEntry & other)
{
    if (this != &other) {
        this->MobileDirectoryEntry::~MobileDirectoryEntry();
        new (this) MobileDirectoryEntry(other);
    }
    return *this;
}

const QString &MobileDirectoryEntry::number() const
{
    return this->m_user.mobileNumber();
}

QString MobileDirectoryEntry::name() const
{
    return this->m_user.fullname();
}

QPixmap MobileDirectoryEntry::statusIcon() const
{
    return QPixmap(":/images/mobile_grey.png");
}

QString MobileDirectoryEntry::statusText() const
{
    return QObject::tr("Mobile phone");
}

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

#include <storage/phoneinfo.h>
#include <dao/phonedao.h>
#include <dao/userdao.h>
#include <xletlib/taintedpixmap.h>

#include "line_directory_entry.h"

LineDirectoryEntry::LineDirectoryEntry(const PhoneInfo &phone,
                                       const UserDAO &user_dao,
                                       const PhoneDAO &phone_dao)
    : m_phone(phone), m_user_dao(user_dao), m_phone_dao(phone_dao)
{
}

bool LineDirectoryEntry::operator==(const LineDirectoryEntry & other) const
{
    const QString & this_phone = this->m_phone.id();
    const QString & other_phone = other.m_phone.id();
    return this_phone == other_phone;
}

LineDirectoryEntry & LineDirectoryEntry::operator=(const LineDirectoryEntry & other)
{
    if (this != &other) {
        this->LineDirectoryEntry::~LineDirectoryEntry();
        new (this) LineDirectoryEntry(other);
    }
    return *this;
}

const QString &LineDirectoryEntry::number() const
{
    return this->m_phone.number();
}

QString LineDirectoryEntry::name() const
{
    return this->m_user_dao.findNameByPhone(&this->m_phone);
}

QPixmap LineDirectoryEntry::statusIcon() const
{
    QColor color = m_phone_dao.getStatusColor(&m_phone);
    QPixmap icon = TaintedPixmap(QString(":/images/phone-trans.png"), color).getPixmap();
    return icon;
}

QString LineDirectoryEntry::statusText() const
{
    QString text = m_phone_dao.getStatusName(&this->m_phone);
    return text;
}

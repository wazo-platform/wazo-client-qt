/*
 * Copyright 2007-2017 The Wazo Authors  (see the AUTHORS file)
 *
 * This program is free software: you can redistribute it and/or modify
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
 * This programm is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QDebug>
#include <QPixmap>
#include <QObject>

#include <xletlib/taintedpixmap.h>

#include "lookup_directory_entry.h"

LookupDirectoryEntry::LookupDirectoryEntry(const QVariant &lookup_result)
    : m_lookup_result(lookup_result.toMap()),
      m_number(lookup_result.toMap()["number"].toString())
{
}

bool LookupDirectoryEntry::hasSource(const QVariant &lookup_result) const
{
    return lookup_result.toMap() == m_lookup_result;
}

QStringList LookupDirectoryEntry::searchList() const
{
    QStringList list;
    QVariant value;
    foreach(value, this->m_lookup_result) {
        list.append(value.toString());
    }

    return list;
}

QString LookupDirectoryEntry::getField(const QString &field, enum ColumnType type) const
{
    switch(type) {
    case NUMBER:
        return this->number();
    case NAME:
        return this->name();
    case OTHER:
        return m_lookup_result.value(field).toString();
    default:
        return "";
    }
}

QString LookupDirectoryEntry::number() const
{
    return this->m_number;
}

QString LookupDirectoryEntry::name() const
{
    return this->m_lookup_result["name"].toString();
}

QPixmap LookupDirectoryEntry::statusIcon() const
{
    const QString &number_type = m_lookup_result["number_type"].toString();
    if (number_type == "mobile") {
        return QPixmap(":/images/mobile-grey.png");
    } else if(number_type == "office") {
        return TaintedPixmap(QString(":/images/phone-trans.png"), QColor("Black")).getPixmap();
    } else {
        return QPixmap(":/images/phonebook.png");
    }
}

QString LookupDirectoryEntry::statusText() const
{
    return QObject::tr("Remote search result");
}

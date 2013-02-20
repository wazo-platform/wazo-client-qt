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

#ifndef _DIRECTORY_ENTRY_H_
#define _DIRECTORY_ENTRY_H_

#include <xletlib/xletlib_export.h>

#include <QStringList>
#include <QHash>
#include <QVariantMap>

class QString;
class QPixmap;
class PhoneInfo;
class UserInfo;

enum ColumnType {
  STATUS_ICON,
  NAME,
  NUMBER,
  OTHER
};

class XLETLIB_EXPORT DirectoryEntry
{
    public:
        virtual QString getField(const QString &field, enum ColumnType type) const;
        virtual QString number() const = 0;
        virtual QString name() const = 0;
        virtual QPixmap statusIcon() const = 0;
        virtual QString statusText() const = 0;
        virtual bool hasSource(const PhoneInfo */*phone*/) const { return false; }
        virtual bool hasSource(const UserInfo */*user*/) const { return false; }
        virtual bool hasSource(const QVariant &/*lookup_result*/) const { return false; }
        virtual bool hasSource(const QString &/*current_search*/) const { return false; }
        virtual QStringList searchList() const;
        virtual void setExtraFields(const QVariantMap &fields);
        virtual ~DirectoryEntry() {}

    private:
        QHash<QString, QString> m_extra_fields;
};

#endif /* _LINE_DIRECTORY_ENTRY_H_ */

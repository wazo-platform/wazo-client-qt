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

#include <QColor>
#include "dao/phonedao.h"
#include "baseengine.h"
#include "phoneinfo.h"

QColor PhoneDAO::getStatusColor(const PhoneInfo *phone)
{
    QVariantMap config = PhoneDAO::getPhoneStatusConfig(phone);
    return QColor(config.value("color").toString());
}

QString PhoneDAO::getStatusName(const PhoneInfo *phone)
{
    QVariantMap config = PhoneDAO::getPhoneStatusConfig(phone);
    return config.value("longname").toString();
}

QVariantMap PhoneDAO::getPhoneStatusConfig(const PhoneInfo *phone)
{
    QVariantMap config;
    if (! phone) {
        return config;
    }

    const QString &status = phone->hintstatus();
    if (b_engine->getOptionsPhoneStatus().contains(status)) {
        config = b_engine->getOptionsPhoneStatus().value(status).toMap();
    }

    return config;
}

const PhoneInfo *PhoneDAO::findByIdentity(const QString &line_interface)
{
    foreach (const QString & phonexid, b_engine->iterover("phones").keys()) {
        const PhoneInfo * p = b_engine->phone(phonexid);
        if (p && p->identity() == line_interface) {
            return p;
        }
    }
    return NULL;
}

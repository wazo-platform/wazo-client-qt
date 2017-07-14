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

#include <QColor>
#include <QDebug>

#include <baseengine.h>
#include <storage/phoneinfo.h>

#include "dao/phonedaoimpl.h"

PhoneDAOImpl::~PhoneDAOImpl()
{
}

QColor PhoneDAOImpl::getStatusColor(const PhoneInfo *phone) const
{
    QVariantMap config = this->getPhoneStatusConfig(phone);
    return QColor(config.value("color").toString());
}

QString PhoneDAOImpl::getStatusName(const PhoneInfo *phone) const
{
    QVariantMap config = this->getPhoneStatusConfig(phone);
    return config.value("longname").toString();
}

QVariantMap PhoneDAOImpl::getPhoneStatusConfig(const PhoneInfo *phone) const
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

const PhoneInfo *PhoneDAOImpl::findByIdentity(const QString &line_interface) const
{
    foreach (const QString & phonexid, b_engine->iterover("phones").keys()) {
        const PhoneInfo * p = b_engine->phone(phonexid);
        if (p && p->identity() == line_interface) {
            return p;
        }
    }
    return NULL;
}

const PhoneInfo *PhoneDAOImpl::findByXId(const QString &phone_xid) const
{
    return b_engine->phone(phone_xid);
}

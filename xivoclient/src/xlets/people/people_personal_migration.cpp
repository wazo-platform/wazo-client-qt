/* XiVO Client
 * Copyright (C) 2015 Avencall
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

#include <QDir>
#include <QFileInfo>
#include <QSettings>

#include <baseengine.h>

#include "people_personal_migration.h"

QDir contacts_dir();
QString contacts_file_name();
QByteArray replace_headers(const QByteArray &headers);


bool PeoplePersonalMigration::needMigration() {
    return QFileInfo::exists(contacts_file_name());
}

QByteArray PeoplePersonalMigration::getOldContacts() {
    QByteArray result;
    QFile file(contacts_file_name());
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return result;
    }

    QByteArray headers = replace_headers(file.readLine());
    result = headers + file.readAll();
    return result;
}

void PeoplePersonalMigration::finishMigration() {
    QDir _contacts_dir = contacts_dir();
    foreach(QString old_contacts_file_name, _contacts_dir.entryList(QStringList("localdir*.csv"))) {
        _contacts_dir.remove(old_contacts_file_name);
    }
}

QDir contacts_dir() {
    QFileInfo config_file(b_engine->getSettings()->fileName());
    return QDir(config_file.canonicalPath());
}

QString contacts_file_name() {
    return contacts_dir().absoluteFilePath("localdir.csv");
}

QByteArray replace_headers(const QByteArray &headers) {
    QByteArray result(headers);
    result.replace("phonenumber", "number");
    result.replace("emailaddress", "email");
    result.replace("faxnumber", "fax");
    result.replace("mobilenumber", "mobile");
    return result;
}

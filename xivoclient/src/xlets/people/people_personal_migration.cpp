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

#include <QAbstractButton>
#include <QFileInfo>
#include <QMessageBox>
#include <QSettings>
#include <QStringList>

#include <baseengine.h>
#include <message_factory.h>

#include "people_personal_migration.h"


bool PeoplePersonalMigration::needMigration() {
    return QFileInfo::exists(PeoplePersonalMigration::contactsFileName());
}

QByteArray PeoplePersonalMigration::getOldContacts() {
    QByteArray result;
    QFile file(PeoplePersonalMigration::contactsFileName());
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return result;
    }

    QByteArray headers = PeoplePersonalMigration::replaceHeaders(file.readLine());
    QByteArray contacts = file.readAll();
    if (! contacts.trimmed().isEmpty()) {
        result = headers + contacts;
    }
    return result;
}

void PeoplePersonalMigration::finishMigration() {
    QDir _contacts_dir = PeoplePersonalMigration::contactsDir();
    foreach(QString old_contacts_file_name, _contacts_dir.entryList(QStringList("localdir*.csv"))) {
        _contacts_dir.remove(old_contacts_file_name);
    }
}

QString PeoplePersonalMigration::contactsFileName() {
    return contactsDir().absoluteFilePath("localdir.csv");
}

QDir PeoplePersonalMigration::contactsDir() {
    QFileInfo config_file(b_engine->getSettings()->fileName());
    return QDir(config_file.canonicalPath());
}

QByteArray PeoplePersonalMigration::replaceHeaders(const QByteArray &headers) {
    QByteArray result(headers);
    result.replace("phonenumber", "number");
    result.replace("emailaddress", "email");
    result.replace("faxnumber", "fax");
    result.replace("mobilenumber", "mobile");
    return result;
}

void PeoplePersonalMigration::noticeAndMigratePersonalContacts(QWidget *parent)
{
    QMessageBox *message = new QMessageBox(QMessageBox::Information,
                                           tr("Contacts Migration"),
                                           tr("Your local contacts will be migrated to the "
                                              "People Xlet. %1").arg("<img src=\":/images/tab-people.svg\"/>"),
                                           QMessageBox::NoButton,
                                           parent);
    message->setDetailedText(tr("Your local contacts are currently stored on your machine in:\n"
                                "%1\n"
                                "They will be migrated to the XiVO server in your personal database.")
                             .arg(PeoplePersonalMigration::contactsFileName()));
    message->setTextFormat(Qt::RichText);
    message->setAttribute(Qt::WA_DeleteOnClose);
    QObject::connect(message, &QMessageBox::buttonClicked,
                     PeoplePersonalMigration::migrateContacts);
    message->show();
}

void PeoplePersonalMigration::migrateContacts()
{
    QByteArray old_contacts = PeoplePersonalMigration::getOldContacts();
    if (! old_contacts.isEmpty()) {
        b_engine->sendJsonCommand(MessageFactory::importPersonalContactsCSV(old_contacts));
    } else {
        PeoplePersonalMigration::finishMigration();
    }
}

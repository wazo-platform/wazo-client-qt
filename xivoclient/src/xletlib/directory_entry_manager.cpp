/* XiVO Client
 * Copyright (C) 2007-2014 Avencall
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

#include <baseengine.h>
#include <dao/phonedao.h>
#include <dao/userdao.h>
#include <storage/phoneinfo.h>
#include <storage/userinfo.h>
#include <xletlib/line_directory_entry.h>
#include <xletlib/lookup_directory_entry.h>
#include <xletlib/directory_entry.h>
#include <xletlib/mobile_directory_entry.h>

#include "directory_entry_manager.h"

DirectoryEntryManager::DirectoryEntryManager(const PhoneDAO &phone_dao,
                                             const UserDAO &user_dao,
                                             QObject *parent)
    : QObject(parent), m_phone_dao(phone_dao), m_user_dao(user_dao)
{
    this->registerListener("directory_search_result");

    connect(b_engine, SIGNAL(updatePhoneConfig(const QString &)),
            this, SLOT(updatePhone(const QString &)));
    connect(b_engine, SIGNAL(updatePhoneStatus(const QString &)),
            this, SLOT(updatePhone(const QString &)));
    connect(b_engine, SIGNAL(removePhoneConfig(const QString &)),
            this, SLOT(removePhone(const QString &)));

    connect(b_engine, SIGNAL(updateUserConfig(const QString &)),
            this, SLOT(updateUser(const QString &)));
    connect(b_engine, SIGNAL(removeUserConfig(const QString &)),
            this, SLOT(removeUser(const QString &)));

    this->addEntry(&m_current_filter_directory_entry);
}

const DirectoryEntry & DirectoryEntryManager::getEntry(int entry_index) const
{
    const DirectoryEntry *entry = m_directory_entries.at(entry_index);
    return *entry;
}

int DirectoryEntryManager::entryCount() const
{
    return m_directory_entries.size();
}

template<class T>
int DirectoryEntryManager::findEntryBy(const T looked_up) const
{
    for (int i = 0; i < m_directory_entries.size(); i++) {
        const DirectoryEntry *entry = m_directory_entries[i];
        if (entry->hasSource(looked_up)) {
            return i;
        }
    }
    return -1;
}

template<class T>
int DirectoryEntryManager::hasEntry(const T looked_up) const
{
    return this->findEntryBy(looked_up) != -1;
}

void DirectoryEntryManager::updateSearch(const QString &current_search)
{
    m_current_filter_directory_entry.setSearchedText(current_search);
    int matching_entry_index = this->findEntryBy(current_search);
    this->updateEntryAt(matching_entry_index);
}

void DirectoryEntryManager::updatePhone(const QString &phone_xid)
{
    const PhoneInfo *phone = this->m_phone_dao.findByXId(phone_xid);
    if (phone == NULL) {
        qDebug() << Q_FUNC_INFO << "phone" << phone_xid << "is null";
        return;
    }

    int matching_entry_index = this->findEntryBy(phone);
    if (matching_entry_index == -1) {
        this->addEntry(new LineDirectoryEntry(*phone, m_user_dao, m_phone_dao));
    } else {
        this->updateEntryAt(matching_entry_index);
    }
}

void DirectoryEntryManager::updateUser(const QString &user_xid)
{
    const UserInfo *user = this->m_user_dao.findByXId(user_xid);
    if (user == NULL) {
        qDebug() << Q_FUNC_INFO << "user" << user_xid << "is null";
        return;
    }

    int matching_entry_index = this->findEntryBy(user);
    if (matching_entry_index == -1) {
        if (! user->hasMobile()) {
            return;
        }
        this->addEntry(new MobileDirectoryEntry(*user));
    } else if (user->hasMobile()) {
        this->updateEntryAt(matching_entry_index);
    } else {
        this->removeEntryAt(matching_entry_index);
    }
}

void DirectoryEntryManager::removePhone(const QString &phone_xid)
{
    const PhoneInfo *phone = this->m_phone_dao.findByXId(phone_xid);
    if (phone == NULL) {
        qDebug() << Q_FUNC_INFO << "phone" << phone_xid << "is null";
        return;
    }

    int matching_entry_index = this->findEntryBy(phone);
    if (matching_entry_index == -1) {
        qDebug() << Q_FUNC_INFO << "removed phone" << phone_xid << "not in cache";
    } else {
        this->removeEntryAt(matching_entry_index);
    }
}

void DirectoryEntryManager::removeUser(const QString &user_xid)
{
    const UserInfo *user = this->m_user_dao.findByXId(user_xid);
    if (user == NULL) {
        qDebug() << Q_FUNC_INFO << "user" << user_xid << "is null";
        return;
    }

    int matching_entry_index = this->findEntryBy(user);
    if (matching_entry_index != -1) {
        this->removeEntryAt(matching_entry_index);
    }
}

int DirectoryEntryManager::findEntryByNumber(const QString &number) const
{
    for (int i = 0; i < m_directory_entries.size(); i++) {
        DirectoryEntry *entry = m_directory_entries[i];
        if (! entry) {
            continue;
        }
        if (entry->name().isEmpty()) {
            continue;
        }
        if (entry->number() == number) {
            return i;
        }
    }
    return -1;
}

void DirectoryEntryManager::parseCommand(const QVariantMap &result)
{
    const QList<QVariant> &entries = result["results"].toList();
    foreach (const QVariant &entry, entries) {
        const QString &number = entry.toMap()["number"].toString();
        int matching_entry_index = this->findEntryByNumber(number);
        if (matching_entry_index != -1) {
            DirectoryEntry *matching_number_entry = m_directory_entries[matching_entry_index];
            matching_number_entry->setExtraFields(entry.toMap());
            this->updateEntryAt(matching_entry_index);
        } else if (! this->hasEntry(entry)) {
            this->addEntry(new LookupDirectoryEntry(entry));
        }
    }
}

void DirectoryEntryManager::addEntry(DirectoryEntry *entry)
{
    if (! entry) {
        qDebug() << Q_FUNC_INFO << "Tried to add a NULL entry";
        return;
    }
    m_directory_entries.append(entry);

    emit directoryEntryAdded(m_directory_entries.size() - 1);
}

void DirectoryEntryManager::updateEntryAt(int index)
{
    emit directoryEntryUpdated(index);
}

void DirectoryEntryManager::removeEntryAt(int index)
{
    const DirectoryEntry *entry = m_directory_entries.at(index);
    m_directory_entries.removeAt(index);
    delete entry;
    entry = NULL;

    emit directoryEntryDeleted(index);
}

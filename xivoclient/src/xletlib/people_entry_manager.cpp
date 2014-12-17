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

#include <baseengine.h>

#include "people_entry.h"
#include "people_entry_manager.h"

PeopleEntryManager::PeopleEntryManager(QObject *parent)
    : QObject(parent)
{
    this->registerListener("people_search_result");
}

void PeopleEntryManager::parseCommand(const QVariantMap &result)
{
    emit aboutToClearEntries();
    m_entries.clear();
    const QList<QVariant> &entries = result["results"].toList();
    foreach (const QVariant &entry, entries) {
        QVariantMap entry_map = entry.toMap();
        const QVariantList &values = entry_map["column_values"].toList();
        const QVariantMap &relations = entry_map["relations"].toMap();
        this->addEntry(PeopleEntry(values, relations));
    }
}

void PeopleEntryManager::addEntry(PeopleEntry entry)
{
    m_entries.append(entry);

    emit entryAdded(m_entries.size() - 1);
}

int PeopleEntryManager::entryCount() const
{
    return m_entries.size();
}

const PeopleEntry & PeopleEntryManager::getEntry(int entry_index) const
{
    const PeopleEntry &entry = m_entries.at(entry_index);
    return entry;
}

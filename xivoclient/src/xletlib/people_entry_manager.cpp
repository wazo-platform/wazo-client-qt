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
#include <message_factory.h>

PeopleEntryManager::PeopleEntryManager(QObject *parent)
    : QObject(parent)
{
    this->registerListener("people_search_result");
    this->registerListener("endpoint_status_update");
}

int PeopleEntryManager::getIndexFromEndpointId(const QPair<QString, int> &id) const
{
    const QString &xivo_uuid = id.first;
    int endpoint_id = id.second;

    for (int i = 0; i < m_entries.size(); ++i) {
        const PeopleEntry &entry = m_entries[i];
        const QVariantMap &endpoint = entry.relations()["endpoint"].toMap();
        if (endpoint["xivo_uuid"].toString() == xivo_uuid
            && endpoint["endpoint_id"].toInt() == endpoint_id) {
            return i;
        }
    }
    return -1;
}

void PeopleEntryManager::parseCommand(const QVariantMap &result)
{
    if (result["class"] == "endpoint_status_update") {
        QPair<QString, int> id(result["data"].toMap()["xivo_uuid"].toString(),
                               result["data"].toMap()["endpoint_id"].toInt());
        int new_status = result["data"].toMap()["status"].toInt();
        m_endpoint_status[id] = new_status;
        int index = this->getIndexFromEndpointId(id);
        emit entryAdded(index);
        return;
    }
    emit aboutToClearEntries();
    m_entries.clear();
    const QList<QVariant> &entries = result["results"].toList();
    QVariantList endpoint_ids;
    foreach (const QVariant &entry, entries) {
        QVariantMap entry_map = entry.toMap();
        const QVariantList &values = entry_map["column_values"].toList();
        const QVariantMap &relations = entry_map["relations"].toMap();
        int endpoint_id = relations["endpoint"].toMap()["id"].toInt();
        const QString &xivo_id = relations["endpoint"].toMap()["xivo_id"].toString();
        QVariantList pair;
        pair.append(xivo_id);
        pair.append(endpoint_id);
        endpoint_ids.push_back(pair);
        this->addEntry(PeopleEntry(values, relations));
    }
    b_engine->sendJsonCommand(MessageFactory::registerEndpointStatus(endpoint_ids));
}

bool PeopleEntryManager::hasEndpointStatus(QPair<QString, int> id) const
{
    return m_endpoint_status.contains(id);
}

int PeopleEntryManager::getEndpointStatus(QPair<QString, int> id) const
{
    return m_endpoint_status[id];
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

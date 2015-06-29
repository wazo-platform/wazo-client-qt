/* XiVO Client
 * Copyright (C) 2007-2015 Avencall
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
#include <message_factory.h>

#include "people_entry.h"
#include "people_entry_manager.h"

PeopleEntryManager::PeopleEntryManager(QObject *parent)
    : QObject(parent)
{
    this->registerListener("people_search_result");
    this->registerListener("people_favorites_result");
    this->registerListener("people_set_favorite_result");
    this->registerListener("agent_status_update");
    this->registerListener("endpoint_status_update");
    this->registerListener("user_status_update");
}

int PeopleEntryManager::getIndexFromAgentId(const RelationID &id) const
{
    for (int i = 0; i < m_entries.size(); ++i) {
        const PeopleEntry &entry = m_entries[i];
        if (entry.uniqueAgentId() == id) {
            return i;
        }
    }
    return -1;
}

int PeopleEntryManager::getIndexFromEndpointId(const RelationID &id) const
{
    for (int i = 0; i < m_entries.size(); ++i) {
        const PeopleEntry &entry = m_entries[i];
        if (entry.uniqueEndpointId() == id) {
            return i;
        }
    }
    return -1;
}

int PeopleEntryManager::getIndexFromFavoriteId(const RelationSourceID &id) const
{
    for (int i = 0; i < m_entries.size(); ++i) {
        const PeopleEntry &entry = m_entries[i];
        if (entry.uniqueSourceId() == id) {
            return i;
        }
    }
    return -1;
}

int PeopleEntryManager::getIndexFromUserId(const RelationID &id) const
{
    for (int i = 0; i < m_entries.size(); ++i) {
        const PeopleEntry &entry = m_entries[i];
        if (entry.uniqueUserId() == id) {
            return i;
        }
    }
    return -1;
}

void PeopleEntryManager::parseAgentStatusUpdate(const QVariantMap &result)
{
    RelationID id(result["data"].toMap()["xivo_uuid"].toString(),
                  result["data"].toMap()["agent_id"].toInt());
    QString new_status = result["data"].toMap()["status"].toString();
    m_agent_status[id] = new_status;
    int index = this->getIndexFromAgentId(id);
    if (index > -1) {
        emit entryUpdated(index);
    }
}

void PeopleEntryManager::parseEndpointStatusUpdate(const QVariantMap &result)
{
    RelationID id(result["data"].toMap()["xivo_uuid"].toString(),
                  result["data"].toMap()["endpoint_id"].toInt());
    int new_status = result["data"].toMap()["status"].toInt();
    m_endpoint_status[id] = new_status;
    int index = this->getIndexFromEndpointId(id);
    if (index > -1) {
        emit entryUpdated(index);
    }
}

void PeopleEntryManager::parseUserStatusUpdate(const QVariantMap &result)
{
    RelationID id(result["data"].toMap()["xivo_uuid"].toString(),
                  result["data"].toMap()["user_id"].toInt());
    const QString &new_status = result["data"].toMap()["status"].toString();
    m_user_status[id] = new_status;
    int index = this->getIndexFromUserId(id);
    if (index > -1 ) {
        emit entryUpdated(index);
    }
}

void PeopleEntryManager::parsePeopleSetFavoriteResult(const QVariantMap &result)
{
    RelationSourceID id(result["data"].toMap()["source"].toString(),
                        result["data"].toMap()["source_entry_id"].toString());
    bool new_status = result["data"].toMap()["status"].toBool();
    int index = this->getIndexFromFavoriteId(id);
    if (index > -1) {
        int column = m_column_type.indexOf("favorite");
        m_entries[index].setData(column, new_status);
        emit entryUpdated(index);
    }
}

void PeopleEntryManager::parseCommand(const QVariantMap &result)
{
    const QString &event = result["class"].toString();

    if (event == "agent_status_update") {
        this->parseAgentStatusUpdate(result);
    } else if (event == "endpoint_status_update") {
        this->parseEndpointStatusUpdate(result);
    } else if (event == "user_status_update") {
        this->parseUserStatusUpdate(result);
    } else if (event == "people_search_result") {
        this->parsePeopleSearchResult(result);
    } else if (event == "people_favorites_result") {
        this->parsePeopleSearchResult(result);
    } else if (event == "people_set_favorite_result") {
        this->parsePeopleSetFavoriteResult(result);
    }
}

void PeopleEntryManager::setColumnTypes(const QVariantList &column_types)
{
    m_column_type = column_types;
}

void PeopleEntryManager::parsePeopleSearchResult(const QVariantMap &result)
{
    this->clearEntries();
    const QList<QVariant> &entries = result["results"].toList();
    QVariantList endpoint_ids;
    QVariantList agent_ids;
    QVariantList user_ids;
    foreach (const QVariant &result, entries) {
        QVariantMap entry_map = result.toMap();
        const QVariantList &values = entry_map["column_values"].toList();
        const QString &source_name = entry_map["source"].toString();
        const QVariantMap &relations = entry_map["relations"].toMap();
        PeopleEntry entry(values, relations, source_name);
        const QString &xivo_id = entry.xivoUuid();
        QVariantList agent;
        agent.append(xivo_id);
        agent.append(entry.agentId());
        agent_ids.push_back(agent);
        QVariantList endpoint;
        endpoint.append(xivo_id);
        endpoint.append(entry.endpointId());
        endpoint_ids.push_back(endpoint);
        QVariantList user;
        user.append(xivo_id);
        user.append(entry.userId());
        user_ids.push_back(user);
        this->addEntry(entry);
    }
    b_engine->sendJsonCommand(MessageFactory::registerAgentStatus(agent_ids));
    b_engine->sendJsonCommand(MessageFactory::registerEndpointStatus(endpoint_ids));
    b_engine->sendJsonCommand(MessageFactory::registerUserStatus(user_ids));
}

void PeopleEntryManager::clearEntries()
{
    emit aboutToClearEntries();
    m_entries.clear();
}

bool PeopleEntryManager::hasAgentStatus(const RelationID &id) const
{
    return m_agent_status.contains(id);
}

bool PeopleEntryManager::hasEndpointStatus(const RelationID &id) const
{
    return m_endpoint_status.contains(id);
}

bool PeopleEntryManager::hasUserStatus(const RelationID &id) const
{
    return m_user_status.contains(id);
}

QString PeopleEntryManager::getAgentStatus(const RelationID &id) const
{
    return m_agent_status[id];
}

int PeopleEntryManager::getEndpointStatus(const RelationID &id) const
{
    return m_endpoint_status[id];
}

bool PeopleEntryManager::getFavoriteStatus(const RelationSourceID &id) const
{
    int index = this->getIndexFromFavoriteId(id);
    if (index > -1) {
        int column = m_column_type.indexOf("favorite");
        return m_entries[index].data(column).toBool();
    }
    return false;
}

QString PeopleEntryManager::getUserStatus(const RelationID &id) const
{
    return m_user_status[id];
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

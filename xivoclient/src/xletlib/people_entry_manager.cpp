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

#include "people_entry.h"
#include "people_entry_manager.h"
#include <message_factory.h>

PeopleEntryManager::PeopleEntryManager(QObject *parent)
    : QObject(parent)
{
    this->registerListener("people_search_result");
    this->registerListener("agent_status_update");
    this->registerListener("endpoint_status_update");
    this->registerListener("user_status_update");
}

int PeopleEntryManager::getIndexFromAgentId(const RelationID &id) const
{
    const QString &xivo_uuid = id.first;
    int agent_id = id.second;

    for (int i = 0; i < m_entries.size(); ++i) {
        const PeopleEntry &entry = m_entries[i];
        const QVariantMap &agent = entry.relations()["agent"].toMap();
        if (agent["xivo_uuid"].toString() == xivo_uuid
            && agent["agent_id"].toInt() == agent_id) {
            return i;
        }
    }
    return -1;
}

int PeopleEntryManager::getIndexFromEndpointId(const RelationID &id) const
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

int PeopleEntryManager::getIndexFromUserId(const RelationID &id) const
{
    const QString &xivo_uuid = id.first;
    int user_id = id.second;

    for (int i = 0; i < m_entries.size(); ++i) {
        const PeopleEntry &entry = m_entries[i];
        const QVariantMap &user = entry.relations()["user"].toMap();
        if (user["xivo_uuid"].toString() == xivo_uuid
            && user["user_id"].toInt() == user_id) {
            return i;
        }
    }
    return -1;
}

void PeopleEntryManager::parseAgentStatusUpdate(const QVariantMap &result)
{
    qDebug() << Q_FUNC_INFO << "Agent status update" << result;
    RelationID id(result["data"].toMap()["xivo_uuid"].toString(),
                  result["data"].toMap()["agent_id"].toInt());
    QString new_status = result["data"].toMap()["status"].toString();
    m_agent_status[id] = new_status;
    int index = this->getIndexFromAgentId(id);
    emit entryUpdated(index);
}

void PeopleEntryManager::parseEndpointStatusUpdate(const QVariantMap &result)
{
    RelationID id(result["data"].toMap()["xivo_uuid"].toString(),
                  result["data"].toMap()["endpoint_id"].toInt());
    int new_status = result["data"].toMap()["status"].toInt();
    m_endpoint_status[id] = new_status;
    int index = this->getIndexFromEndpointId(id);
    emit entryUpdated(index);
}

void PeopleEntryManager::parseUserStatusUpdate(const QVariantMap &result)
{
    RelationID id(result["data"].toMap()["xivo_uuid"].toString(),
                  result["data"].toMap()["user_id"].toInt());
    const QString &new_status = result["data"].toMap()["status"].toString();
    m_user_status[id] = new_status;
    int index = this->getIndexFromUserId(id);
    emit entryUpdated(index);
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
    }
}

void PeopleEntryManager::parsePeopleSearchResult(const QVariantMap &result)
{
    emit aboutToClearEntries();
    m_entries.clear();
    const QList<QVariant> &entries = result["results"].toList();
    QVariantList endpoint_ids;
    QVariantList agent_ids;
    QVariantList user_ids;
    foreach (const QVariant &entry, entries) {
        QVariantMap entry_map = entry.toMap();
        const QVariantList &values = entry_map["column_values"].toList();
        const QVariantMap &relations = entry_map["relations"].toMap();
        qDebug() << Q_FUNC_INFO << relations;
        int endpoint_id = relations["endpoint"].toMap()["id"].toInt();
        int agent_id = relations["agent"].toMap()["id"].toInt();
        int user_id = relations["user"].toMap()["id"].toInt();
        const QString &xivo_id = relations["endpoint"].toMap()["xivo_id"].toString();
        QVariantList agent;
        agent.append(xivo_id);
        agent.append(agent_id);
        agent_ids.push_back(agent);
        QVariantList endpoint;
        endpoint.append(xivo_id);
        endpoint.append(endpoint_id);
        endpoint_ids.push_back(endpoint);
        QVariantList user;
        user.append(xivo_id);
        user.append(user_id);
        user_ids.push_back(user);
        this->addEntry(PeopleEntry(values, relations));
    }
    b_engine->sendJsonCommand(MessageFactory::registerAgentStatus(agent_ids));
    b_engine->sendJsonCommand(MessageFactory::registerEndpointStatus(endpoint_ids));
    b_engine->sendJsonCommand(MessageFactory::registerUserStatus(user_ids));
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

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

#include <QObject>

#include "people_entry.h"

PeopleEntry::PeopleEntry(const QVariantList &data,
                         const QString &xivo_uuid,
                         const QString &source_name,
                         const QString &source_entry_id,
                         int agent_id,
                         int endpoint_id,
                         int user_id)
  : m_data(data),
    m_xivo_uuid(xivo_uuid),
    m_source_name(source_name),
    m_source_entry_id(source_entry_id),
    m_agent_id(agent_id),
    m_user_id(user_id),
    m_endpoint_id(endpoint_id)
{
}

PeopleEntry::PeopleEntry()
{
}

PeopleEntry::~PeopleEntry()
{
}

const QVariant PeopleEntry::data(int column) const
{
    return this->m_data.value(column);
}

void PeopleEntry::setData(int column, const QVariant &status)
{
    this->m_data.replace(column, status);
}

const QString &PeopleEntry::xivoUuid() const
{
    return this->m_xivo_uuid;
}

int PeopleEntry::agentId() const
{
    return m_agent_id;
}

int PeopleEntry::endpointId() const
{
    return m_endpoint_id;
}

int PeopleEntry::userId() const
{
    return m_user_id;
}

const QString &PeopleEntry::sourceName() const
{
    return m_source_name;
}

const QString &PeopleEntry::sourceEntryId() const
{
    return m_source_entry_id;
}

QPair<QString, int> PeopleEntry::uniqueAgentId() const
{
    return QPair<QString, int>(m_xivo_uuid, m_agent_id);
}

QPair<QString, int> PeopleEntry::uniqueEndpointId() const
{
    return QPair<QString, int>(m_xivo_uuid, m_endpoint_id);
}

QPair<QString, QString> PeopleEntry::uniqueSourceId() const
{
    return QPair<QString, QString>(m_source_name, m_source_entry_id);
}

QPair<QString, int> PeopleEntry::uniqueUserId() const
{
    return QPair<QString, int>(m_xivo_uuid, m_user_id);
}

const QString &PeopleEntry::agentStatus() const
{
    return m_agent_status;
}

void PeopleEntry::setAgentStatus(const QString &status)
{
    m_agent_status = status;
}

int PeopleEntry::endpointStatus() const
{
    return m_endpoint_status;
}

void PeopleEntry::setEndpointStatus(int status)
{
    m_endpoint_status = status;
}

const QString &PeopleEntry::userStatus() const
{
    return m_user_status;
}

void PeopleEntry::setUserStatus(const QString &status)
{
    m_user_status = status;
}

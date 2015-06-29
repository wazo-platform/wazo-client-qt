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
                         const QVariantMap &relations,
                         const QString &source_name)
  : m_data(data),
    m_xivo_uuid(relations["xivo_id"].toString()),
    m_source_name(source_name),
    m_source_entry_id(relations["source_entry_id"].toString()),
    m_agent_id(relations["agent_id"].toInt()),
    m_user_id(relations["user_id"].toInt()),
    m_endpoint_id(relations["endpoint_id"].toInt())
{
}

PeopleEntry::PeopleEntry(const PeopleEntry &other)
    : m_data(other.m_data),
      m_xivo_uuid(other.m_xivo_uuid),
      m_source_name(other.m_source_name),
      m_source_entry_id(other.m_source_entry_id),
      m_agent_id(other.m_agent_id),
      m_user_id(other.m_user_id),
      m_endpoint_id(other.m_endpoint_id)
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

PeopleEntry & PeopleEntry::operator=(const PeopleEntry &other)
{
    this->m_data = other.m_data;
    this->m_xivo_uuid = other.m_xivo_uuid;
    this->m_agent_id = other.m_agent_id;
    this->m_endpoint_id = other.m_endpoint_id;
    this->m_source_name = other.m_source_name;
    this->m_source_entry_id = other.m_source_entry_id;
    this->m_user_id = other.m_user_id;

    return *this;
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

QString PeopleEntry::sourceName() const
{
    return m_source_name;
}

QString PeopleEntry::sourceEntryId() const
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

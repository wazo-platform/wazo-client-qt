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

#include <QAction>
#include <QIcon>
#include <cassert>

#include <baseengine.h>
#include <message_factory.h>

#include "people_actions.h"
#include "people_entry_model.h"

PeopleEntryModel::PeopleEntryModel(QWidget *parent)
    : AbstractTableModel(parent)
{
    this->m_type_map["agent"] = AGENT;
    this->m_type_map["favorite"] = FAVORITE;
    this->m_type_map["mobile"] = MOBILE;
    this->m_type_map["name"] = NAME;
    this->m_type_map["number"] = NUMBER;
    this->m_type_map["status"] = STATUS_ICON;
}

void PeopleEntryModel::addField(const QString &name, const QString &type)
{
    ColumnType t = this->m_type_map.value(type, OTHER);
    m_fields.append(QPair<QString, enum ColumnType>(name.toUpper(), t));
}

void PeopleEntryModel::refreshEntry(int row_id)
{
    unsigned first_column_index = 0;
    unsigned last_column_index = this->columnCount() - 1;
    QModelIndex cell_changed_start = createIndex(row_id, first_column_index);
    QModelIndex cell_changed_end = createIndex(row_id, last_column_index);
    emit dataChanged(cell_changed_start, cell_changed_end);
}

void PeopleEntryModel::clearFields()
{
    this->beginResetModel();
    m_fields.clear();
    this->endResetModel();
}

void PeopleEntryModel::clearEntries()
{
    this->beginResetModel();
    m_people_entries.clear();
    this->endResetModel();
}

int PeopleEntryModel::rowCount(const QModelIndex&) const
{
    return m_people_entries.size();
}

int PeopleEntryModel::columnCount(const QModelIndex&) const
{
    return m_fields.size();
}

QVariant PeopleEntryModel::data(const QModelIndex &index, int role) const
{
    int row = index.row(), column = index.column();
    const PeopleEntry &entry = m_people_entries[row];
    ColumnType column_type = this->headerType(column);

    switch(role) {
    case Qt::DecorationRole:
        return this->dataDecoration(entry, column);
    case Qt::DisplayRole:
        if (column_type != AGENT && column_type != FAVORITE) {
            return entry.data(column);
        }
        break;
    case NUMBER_ROLE:
        if (column_type == NUMBER) {
            return QVariant::fromValue(new PeopleActions(m_fields, entry, column));
        }
        break;
    case INDICATOR_COLOR_ROLE:
        return this->dataIndicatorColor(entry, column);
    case UNIQUE_SOURCE_ID_ROLE:
        if (column_type == FAVORITE) {
            QVariantMap favorite_key;
            favorite_key["source"] = entry.sourceName();
            favorite_key["source_entry_id"] = entry.sourceEntryId();
            return favorite_key;
        }
    case SORT_FILTER_ROLE:
        this->dataSortFilter(entry, column);
    default:
        break;
    }
    return AbstractTableModel::data(index, role);
}

QVariant PeopleEntryModel::headerData(int column,
                                      Qt::Orientation orientation,
                                      int role) const
{
    if (orientation != Qt::Horizontal) {
        return QVariant();
    }

    switch(role) {
    case Qt::DisplayRole:
        return this->headerText(column);
    case Qt::UserRole:
        return this->headerType(column);
    default:
        return QVariant();
    }
}

QString PeopleEntryModel::headerText(int column) const
{
    return this->m_fields[column].first;
}

enum ColumnType PeopleEntryModel::headerType(int column) const
{
    return this->m_fields[column].second;
}

QVariant PeopleEntryModel::dataDecoration(const PeopleEntry &entry, int column) const
{
    ColumnType column_type = this->headerType(column);

    switch (column_type) {
    case AGENT:
    {
        const QString &agent_status = entry.agentStatus();
        if (agent_status == "logged_in") {
            return QIcon(":/images/agent-on.svg").pixmap(QSize(20, 20));
        } else if (agent_status == "logged_out") {
            return QIcon(":/images/agent-off.svg").pixmap(QSize(20, 20));
        }
    }
    break;
    case FAVORITE:
    {
        if (entry.sourceEntryId().isEmpty()) {
            break;
        }
        if (entry.data(column).toBool()) {
            return QIcon(":/images/star-filled.svg").pixmap(QSize(12, 12));
        } else {
            return QIcon(":/images/star-empty.svg").pixmap(QSize(12, 12));
        }
    }
    break;
    default:
        break;
    }
    return QVariant();
}

QVariant PeopleEntryModel::dataIndicatorColor(const PeopleEntry &entry, int column) const
{
    ColumnType column_type = this->headerType(column);

    switch (column_type) {
    case NAME: // user
    {
        if (entry.userId() == 0) {
            return QVariant();
        }
        QString user_status = entry.userStatus();
        const QVariantMap &status_map = b_engine->getOptionsUserStatus()[QString("%1").arg(user_status)].toMap();
        const QString &color = status_map["color"].toString();
        return QColor(color);
    }
    break;
    case NUMBER: // endpoint
    {
        if (entry.data(column).toString().isEmpty()) {
            return QVariant();
        }
        if (entry.endpointId() == 0) {
            return QVariant();
        }
        int endpoint_status = entry.endpointStatus();
        const QVariantMap &status_map = b_engine->getOptionsPhoneStatus()[QString("%1").arg(endpoint_status)].toMap();
        const QString &color = status_map["color"].toString();
        return QColor(color);
    }
    break;
    default:
        break;
    }
    return QVariant();
}

QVariant PeopleEntryModel::dataSortFilter(const PeopleEntry &entry, int column) const
{
    ColumnType column_type = this->headerType(column);

    switch (column_type) {
    case AGENT:
        return entry.agentStatus();
    case FAVORITE:
    {
        if (entry.sourceEntryId().isEmpty()) {
            break;
        }
        return entry.data(column);
    }
    default:
        break;
    }
    return QVariant();
}

int PeopleEntryModel::getNameColumnIndex() const
{
    for (int column = 0; column < this->columnCount(); ++column) {
        if (this->headerType(column) == NAME) {
            return column;
        }
    }
    return -1;
}

int PeopleEntryModel::getFavoriteColumnIndex() const
{
    for (int column = 0; column < this->columnCount(); ++column) {
        if (this->headerType(column) == FAVORITE) {
            return column;
        }
    }
    return -1;
}

bool PeopleEntryModel::favoriteStatus(const QVariantMap &unique_source_entry_id) const
{
    QPair<QString, QString> id(unique_source_entry_id["source"].toString(),
                               unique_source_entry_id["source_entry_id"].toString());

    for (int i = 0; i < m_people_entries.size(); ++i) {
        const PeopleEntry &entry = m_people_entries[i];
        if (entry.uniqueSourceId() == id) {
            int column = this->getFavoriteColumnIndex();
            if (column != -1) {
                return entry.data(column).toBool();
            }
        }
    }
    return false;
}

void PeopleEntryModel::setAgentStatusFromAgentId(const RelationID &id, const QString &status)
{
    for (int i = 0; i < m_people_entries.size(); ++i) {
        PeopleEntry &entry = m_people_entries[i];
        if (entry.uniqueAgentId() == id) {
            entry.setAgentStatus(status);
            this->refreshEntry(i);
        }
    }
}

void PeopleEntryModel::setEndpointStatusFromEndpointId(const RelationID &id, int status)
{
    for (int i = 0; i < m_people_entries.size(); ++i) {
        PeopleEntry &entry = m_people_entries[i];
        if (entry.uniqueEndpointId() == id) {
            entry.setEndpointStatus(status);
            this->refreshEntry(i);
        }
    }
}

void PeopleEntryModel::setUserStatusFromUserId(const RelationID &id, const QString &status)
{
    for (int i = 0; i < m_people_entries.size(); ++i) {
        PeopleEntry &entry = m_people_entries[i];
        if (entry.uniqueUserId() == id) {
            entry.setUserStatus(status);
            this->refreshEntry(i);
        }
    }
}

void PeopleEntryModel::setFavoriteStatusFromSourceId(const RelationSourceID &id, bool status)
{
    for (int i = 0; i < m_people_entries.size(); ++i) {
        PeopleEntry &entry = m_people_entries[i];
        if (entry.uniqueSourceId() == id) {
            int column = this->getFavoriteColumnIndex();
            if (column != -1) {
                entry.setData(column, status);
                this->refreshEntry(i);
            }
        }
    }
}

void PeopleEntryModel::parseAgentStatusUpdate(const QVariantMap &result)
{
    RelationID id(result["data"].toMap()["xivo_uuid"].toString(),
                  result["data"].toMap()["agent_id"].toInt());
    QString new_status = result["data"].toMap()["status"].toString();

    this->setAgentStatusFromAgentId(id, new_status);
}

void PeopleEntryModel::parseEndpointStatusUpdate(const QVariantMap &result)
{
    RelationID id(result["data"].toMap()["xivo_uuid"].toString(),
                  result["data"].toMap()["endpoint_id"].toInt());
    int new_status = result["data"].toMap()["status"].toInt();

    this->setEndpointStatusFromEndpointId(id, new_status);
}

void PeopleEntryModel::parseUserStatusUpdate(const QVariantMap &result)
{
    RelationID id(result["data"].toMap()["xivo_uuid"].toString(),
                  result["data"].toMap()["user_id"].toInt());
    const QString &new_status = result["data"].toMap()["status"].toString();

    this->setUserStatusFromUserId(id, new_status);
}

void PeopleEntryModel::parsePeopleFavoriteUpdate(const QVariantMap &result)
{
    QVariantMap data = result["data"].toMap();
    RelationSourceID id(data["source"].toString(), data["source_entry_id"].toString());
    bool new_status = data["favorite"].toBool();

    this->setFavoriteStatusFromSourceId(id, new_status);
}

void PeopleEntryModel::parsePeopleHeadersResult(const QVariantMap &result)
{
    const QVariantList &headers = result["column_headers"].toList();
    const QVariantList &types = result["column_types"].toList();
    if (headers.length() != types.length() || headers.length() == 0){
        return;
    }

    this->clearFields();

    int first_column = 0;
    int last_column = headers.length() - 1;
    this->beginInsertColumns(QModelIndex(), first_column, last_column);
    for (int i = 0; i < headers.length() ; i++) {
        const QString &name = headers[i].toString();
        const QString &type = types[i].toString();

        this->addField(name, type);
    }
    this->endInsertColumns();
}

void PeopleEntryModel::parsePeopleSearchResult(const QVariantMap &result)
{
    const QList<QVariant> &entries = result["results"].toList();
    QVariantList endpoint_ids;
    QVariantList agent_ids;
    QVariantList user_ids;

    this->beginResetModel();
    m_people_entries.clear();
    foreach (const QVariant &result, entries) {
        QVariantMap entry_map = result.toMap();
        const QVariantMap &relations = entry_map["relations"].toMap();

        PeopleEntry entry(entry_map["column_values"].toList(),
                          relations["xivo_id"].toString(),
                          entry_map["source"].toString(),
                          relations["source_entry_id"].toString(),
                          relations["agent_id"].toInt(),
                          relations["endpoint_id"].toInt(),
                          relations["user_id"].toInt()
                         );
        const QString &xivo_id = entry.xivoUuid();
        if (entry.agentId() != 0) {
            QVariantList agent;
            agent.append(xivo_id);
            agent.append(entry.agentId());
            agent_ids.push_back(agent);
        }
        if (entry.endpointId() != 0) {
            QVariantList endpoint;
            endpoint.append(xivo_id);
            endpoint.append(entry.endpointId());
            endpoint_ids.push_back(endpoint);
        }
        if (entry.userId() != 0) {
            QVariantList user;
            user.append(xivo_id);
            user.append(entry.userId());
            user_ids.push_back(user);
        }

        m_people_entries.append(entry);
    }
    this->endResetModel();

    if (!agent_ids.empty()) {
        b_engine->sendJsonCommand(MessageFactory::registerAgentStatus(agent_ids));
    }
    if (!endpoint_ids.empty()) {
        b_engine->sendJsonCommand(MessageFactory::registerEndpointStatus(endpoint_ids));
    }
    if (!user_ids.empty()) {
        b_engine->sendJsonCommand(MessageFactory::registerUserStatus(user_ids));
    }
}

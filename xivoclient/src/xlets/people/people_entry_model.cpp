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

#include <QString>
#include <QPixmap>
#include <cassert>

#include <baseengine.h>
#include <xletlib/people_entry_manager.h>
#include <xletlib/people_entry.h>

#include "people_entry_model.h"

PeopleEntryModel::PeopleEntryModel(const PeopleEntryManager & people_entry_manager,
                                         QObject *parent)
    : QAbstractTableModel(parent),
      m_people_entry_manager(people_entry_manager)
{
    connect(b_engine, SIGNAL(clearingCache()),
            this, SLOT(clearCache()));
    connect(&m_people_entry_manager, SIGNAL(entryAdded(int)),
            this, SLOT(addPeopleEntry(int)));
    connect(&m_people_entry_manager, SIGNAL(entryUpdated(int)),
            this, SLOT(updatePeopleEntry(int)));
    connect(&m_people_entry_manager, SIGNAL(aboutToClearEntries()),
            this, SLOT(clearCache()));

    this->registerListener("people_headers_result");
}

void PeopleEntryModel::addField(const QString &name, const QString &type)
{
    enum ColumnType t;
    if (type == "name") {
        t = NAME;
    } else if (type == "number") {
        t = NUMBER;
    } else if (type == "status") {
        t = STATUS_ICON;
    } else if (type == "agent") {
        t = AGENT;
    } else {
        t = OTHER;
    }
    m_fields.append(QPair<QString, enum ColumnType>(name, t));
    int inserted_column = m_fields.size() - 1;
    this->beginInsertColumns(QModelIndex(), inserted_column, inserted_column);
    this->endInsertColumns();
}

void PeopleEntryModel::addPeopleEntry(int entry_index) {
    int inserted_row = entry_index;
    beginInsertRows(QModelIndex(), inserted_row, inserted_row);
    endInsertRows();
    this->refreshEntry(inserted_row);
}

void PeopleEntryModel::updatePeopleEntry(int entry_index) {
    this->refreshEntry(entry_index);
}

void PeopleEntryModel::deletePeopleEntry(int entry_index) {
    this->removeRow(entry_index);
}

void PeopleEntryModel::clearCache()
{
    this->removeRows(0, this->rowCount(), QModelIndex());
}

void PeopleEntryModel::refreshEntry(int row_id)
{
    unsigned first_column_index = 0;
    unsigned last_column_index = this->columnCount() - 1;
    QModelIndex cell_changed_start = createIndex(row_id, first_column_index);
    QModelIndex cell_changed_end = createIndex(row_id, last_column_index);
    emit dataChanged(cell_changed_start, cell_changed_end);
}

int PeopleEntryModel::rowCount(const QModelIndex&) const
{
    return m_people_entry_manager.entryCount();
}

int PeopleEntryModel::columnCount() const
{
    return m_fields.size();
}

int PeopleEntryModel::columnCount(const QModelIndex&) const
{
    return this->columnCount();
}

QVariant PeopleEntryModel::data(const QModelIndex &index, int role) const
{
    int row = index.row(), column = index.column();
    const PeopleEntry & entry = m_people_entry_manager.getEntry(row);

    switch(role) {
    case Qt::TextAlignmentRole:
        return Qt::AlignCenter;
    case  Qt::DisplayRole:
        return this->dataDisplay(entry, column);
    case Qt::BackgroundRole:
      return this->dataBackground(entry, column);
    case Qt::UserRole:
        return this->dataUser(entry, column);
    default:
        return QVariant();
    }
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


QVariant PeopleEntryModel::dataDisplay(const PeopleEntry & entry, int column) const
{
    return entry.data(column);
}


QVariant PeopleEntryModel::dataBackground(const PeopleEntry & entry, int column) const
{
    ColumnType column_type = m_fields[column].second;

    switch (column_type) {
    case NAME: // user
    {
        QPair<QString, int> user_key = entry.uniqueUserId();

        if (!m_people_entry_manager.hasUserStatus(user_key)) {
            return QVariant();
        }
        QString user_status = m_people_entry_manager.getUserStatus(user_key);
        const QVariantMap &status_map = b_engine->getOptionsUserStatus()[QString("%1").arg(user_status)].toMap();
        const QString &color = status_map["color"].toString();
        return QColor(color);
    }
    break;
    case NUMBER: // endpoint
    {
        QPair<QString, int> endpoint_key = entry.uniqueEndpointId();

        if (!m_people_entry_manager.hasEndpointStatus(endpoint_key)) {
            return QVariant();
        }
        int endpoint_status = m_people_entry_manager.getEndpointStatus(endpoint_key);
        const QVariantMap &status_map = b_engine->getOptionsPhoneStatus()[QString("%1").arg(endpoint_status)].toMap();
        const QString &color = status_map["color"].toString();
        return QColor(color);
    }
    break;
    case AGENT: // agent
    {
        QPair<QString, int> agent_key = entry.uniqueAgentId();

        if (!m_people_entry_manager.hasAgentStatus(agent_key)) {
            return QVariant();
        }
        if (m_people_entry_manager.getAgentStatus(agent_key) == "logged_in") {
            return QColor("green");
        } else {
            return QVariant();
        }
        break;
    }
    default:
        return QVariant();
        break;
    }
    return QVariant();
}

QVariant PeopleEntryModel::dataUser(const PeopleEntry & entry, int column) const
{
    ColumnType column_type = m_fields[column].second;
    QPair<QString, int> agent_key = entry.uniqueAgentId();

    switch (column_type) {
    case AGENT:
        return m_people_entry_manager.getAgentStatus(agent_key);
        break;
    default:
        return QVariant();
        break;
    }
    return QVariant();
}


bool PeopleEntryModel::removeRows(int row, int count, const QModelIndex & index)
{
    if (count > 0) {
        beginRemoveRows(index, row, row + count - 1);
        endRemoveRows();
    }
    return true;
}

void PeopleEntryModel::parseCommand(const QVariantMap &command)
{
    const QVariantList &headers = command["column_headers"].toList();
    const QVariantList &types = command["column_types"].toList();
    assert(headers.length() == types.length());

    for (int i = 0; i < headers.length() ; i++) {
        const QString &name = headers[i].toString();
        const QString &type = types[i].toString();

        this->addField(name, type);
    }
}

int PeopleEntryModel::getNumberColumnIndex() const
{
    for (int i = 0; i < m_fields.size(); ++i) {
        const QPair<QString, enum ColumnType> &field = m_fields[i];
        if (field.second == NUMBER) {
            return i;
        }
    }
    return -1;
}

int PeopleEntryModel::getNameColumnIndex() const
{
    for (int i = 0; i < m_fields.size(); ++i) {
        const QPair<QString, enum ColumnType> &field = m_fields[i];
        if (field.second == NAME) {
            return i;
        }
    }
    return -1;
}

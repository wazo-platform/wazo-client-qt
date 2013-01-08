/* XiVO Client
 * Copyright (C) 2007-2013, Avencall
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
#include <queueinfo.h>
#include <QTimer>

#include <id_converter.h>

#include "queue_entries_model.h"

QueueEntriesModel::QueueEntriesModel(QObject *parent)
    : QAbstractTableModel(parent)
{
    this->fillHeaders();

    connect(b_engine, SIGNAL(queueEntryUpdate(const QString &, const QVariantList &)),
            this, SLOT(queueEntryUpdate(const QString &, const QVariantList &)));

    QTimer * timer_display = new QTimer(this);
    connect(timer_display, SIGNAL(timeout()),
            this, SLOT(increaseTime()));
    timer_display->start(1000);
}

void QueueEntriesModel::fillHeaders()
{
    m_headers[ID] = tr("ID");
    m_headers[POSITION] = tr("Position");
    m_headers[NAME] = tr("Name");
    m_headers[NUMBER] = tr("Number");
    m_headers[TIME] = tr("Time");
    m_headers[UNIQUE_ID] = "Unique ID";
}

QueueEntriesModel::~QueueEntriesModel()
{
}

int QueueEntriesModel::rowCount(const QModelIndex & /*index*/) const
{
    return m_entries.size();
}

int QueueEntriesModel::columnCount(const QModelIndex & /*index*/) const
{
    return NB_COL;
}


bool QueueEntriesModel::removeRows(int start_row_index, int row_count, const QModelIndex & index)
{
    if (row_count <= 0) {
        return false;
    }

    bool ret = true;
    beginRemoveRows(index, start_row_index, start_row_index + row_count - 1);
    for (int row = start_row_index; row < start_row_index + row_count; row ++) {
        ret = ret && start_row_index < m_entries.size();
        m_entries.removeAt(start_row_index);
    }
    endRemoveRows();
    return ret;
}

void QueueEntriesModel::queueEntryUpdate(const QString & queue_id,
                                         const QVariantList & entry_list)
{
    if (queue_id != IdConverter::xidToId(m_queue_id)) {
        return;
    }

    if (this->rowCount(QModelIndex()) > 0) {
        this->removeRows(0, this->rowCount(QModelIndex()), QModelIndex());
    }
    if (entry_list.size() > 0) {
        beginInsertRows(QModelIndex(), 0, entry_list.size() - 1);
        m_entries = entry_list;
        endInsertRows();
    }
}

void QueueEntriesModel::changeWatchedQueue(const QString & queue_id)
{
    this->subscribeQueueEntry(queue_id);
}

QVariant QueueEntriesModel::data(const QModelIndex & index, int role) const
{
    int row = index.row(), column = index.column();

    switch(role) {
    case Qt::TextAlignmentRole:
        return Qt::AlignCenter;
    case  Qt::DisplayRole:
        return this->dataDisplay(row, column);
    default:
        return QVariant();
    }
}

QVariant QueueEntriesModel::dataDisplay(int row, int column) const
{
    QVariantMap entry = this->m_entries[row].toMap();
    switch(column) {
    case ID:
        return row;
    case POSITION:
        return entry.value("position");
    case NAME:
        return entry.value("name");
    case NUMBER:
        return entry.value("number");
    case TIME:
        return b_engine->timeElapsed(entry.value("join_time").toDouble());
    case UNIQUE_ID:
        return entry.value("uniqueid");
    default:
        return QVariant();
    }
}

QVariant QueueEntriesModel::headerData(int column_index,
                                       Qt::Orientation orientation,
                                       int role) const
{
    if (orientation != Qt::Horizontal) {
        return QVariant();
    }

    switch(role) {
    case Qt::DisplayRole :
        return m_headers[column_index];
    default :
        return QVariant();
    }
}

void QueueEntriesModel::subscribeQueueEntry(const QString & queue_id)
{
    const QueueInfo *queue = b_engine->queue(queue_id);
    if (queue == NULL) {
        return;
    }
    this->m_queue_id = queue_id;

    QVariantMap subscribe_command;
    subscribe_command["class"] = "subscribe";
    subscribe_command["message"] = "queueentryupdate";
    subscribe_command["queueid"] = queue->id();

    b_engine->sendJsonCommand(subscribe_command);
}

void QueueEntriesModel::increaseTime()
{
    this->refreshColumn(TIME);
}

void QueueEntriesModel::refreshColumn(int column_index)
{
    unsigned first_row_index = 0;
    unsigned last_row_index = m_entries.size() - 1;
    QModelIndex cell_changed_start = createIndex(first_row_index, column_index);
    QModelIndex cell_changed_end = createIndex(last_row_index, column_index);
    emit dataChanged(cell_changed_start, cell_changed_end);
}

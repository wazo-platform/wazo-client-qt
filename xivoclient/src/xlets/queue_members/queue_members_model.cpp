/* XiVO Client
 * Copyright (C) 2007-2012, Avencall
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

#include <QBrush>

#include <baseengine.h>

#include "queue_members_model.h"

QueueMembersModel::QueueMembersModel(QObject *parent)
    : QAbstractTableModel(parent)
{
    this->fillHeaders();
    connect(b_engine, SIGNAL(updateQueueMemberConfig(const QString &)),
            this, SLOT(updateQueueMemberConfig(const QString &)));
    connect(b_engine, SIGNAL(removeQueueMemberConfig(const QString &)),
            this, SLOT(removeQueueMemberConfig(const QString &)));
}

void QueueMembersModel::fillHeaders()
{
    m_headers[ID].label = "ID";
    m_headers[ID].tooltip = "ID";
    m_headers[NUMBER].label = tr("Number");
    m_headers[NUMBER].tooltip = tr("Phone number of the queue");
    m_headers[FIRSTNAME].label = tr("Firstname");
    m_headers[FIRSTNAME].tooltip = tr("Queue member's firstname");
    m_headers[LASTNAME].label = tr("Lastname");
    m_headers[LASTNAME].tooltip = tr("Queue member's lastname");
    m_headers[STATUS].label = tr("Status");
    m_headers[STATUS].tooltip = tr("Queue member login status");
    m_headers[PAUSED].label = tr("Paused");
    m_headers[PAUSED].tooltip = tr("Queue member pause status");
    m_headers[ANSWERED_CALLS].label = tr("Answered calls");
    m_headers[ANSWERED_CALLS].tooltip = tr("Number of answered calls");
    m_headers[LAST_CALL_DATE].label = tr("Last call");
    m_headers[LAST_CALL_DATE].tooltip = tr("Hangup date of the last answered call");
    m_headers[PENALTY].label = tr("Penalty");
    m_headers[PENALTY].tooltip = tr("Queue member's penalty");
}

void QueueMembersModel::updateQueueMemberConfig(const QString &xid)
{
    if (! m_row2id.contains(xid)) {
        int insertedRow = m_row2id.size();
        beginInsertRows(QModelIndex(), insertedRow, insertedRow);
        m_row2id.append(xid);
        endInsertRows();
    } else {
        // QModelIndex cellChanged = createIndex(m_row2id.indexOf(xid), NAME);
        // // sends signal to proxy/view that the data should be refreshed
        // emit dataChanged(cellChanged, cellChanged);
    }
}

void QueueMembersModel::removeQueueMemberConfig(const QString &xid)
{
    if (m_row2id.contains(xid)) {
        int removedRow = m_row2id.indexOf(xid);
        removeRow(removedRow);
    }
}

bool QueueMembersModel::removeRows(int row, int count, const QModelIndex &)
{
    bool ret = true;
    if (count > 0) {
        beginRemoveRows(QModelIndex(), row, row + count - 1);
        for (int i = 0 ; i < count ; i ++) {
            ret = ret && row < m_row2id.size();
            m_row2id.removeAt(row);
        }
        endRemoveRows();
    }
    return ret;
}

int QueueMembersModel::rowCount(const QModelIndex&) const
{
    return m_row2id.size();
}

int QueueMembersModel::columnCount(const QModelIndex&) const
{
    return NB_COL;
}

QVariant QueueMembersModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::TextAlignmentRole) {
        return Qt::AlignCenter;
    }

    QString not_available = tr("N/A");
    if (role == Qt::DisplayRole) {
    return QString("Bidon");
    //     switch (index.column()) {
    //         case ID :
    //         case NUMBER :
    //         case FIRSTNAME :
    //         case LASTNAME :
    //         default :
    //             return not_available;
    //     }
    }

    return QVariant();
}

QVariant QueueMembersModel::headerData(int index,
                                 Qt::Orientation orientation,
                                 int role) const
{
    if (orientation != Qt::Horizontal) {
        return QVariant();
    }

    switch(role) {
        case Qt::DisplayRole :
            return m_headers[index].label;
        case Qt::ToolTipRole :
            return m_headers[index].tooltip;
        default :
            return QVariant();
    }
}

void QueueMembersModel::refreshQueueMemberRow(const QString &queue_member_id)
{
    unsigned last_column_index = NB_COL - 1;
    unsigned queue_member_row_id = m_row2id.indexOf(queue_member_id);
    QModelIndex cell_changed_start = createIndex(queue_member_row_id, ID);
    QModelIndex cell_changed_end = createIndex(queue_member_row_id, last_column_index);
    emit dataChanged(cell_changed_start, cell_changed_end);
}

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

#include <QTimer>

#include <baseengine.h>

#include "conference_enum.h"
#include "conference_room_model.h"

ConferenceRoomModel::ConferenceRoomModel(QWidget *parent)
    : AbstractTableModel(parent)
{
    QTimer * join_time_timer = new QTimer(this);
    connect(join_time_timer, SIGNAL(timeout()),
            this, SLOT(updateJoinTime()));
    join_time_timer->start(1000);
}

const QString & ConferenceRoomModel::roomNumber() const
{
    return m_room_number;
}

void ConferenceRoomModel::setConfRoom(const QString &room_number, const QVariantMap &members)
{

    beginResetModel();

    m_room_number = room_number;
    m_confroom_item.clear();
    foreach(QVariant item, members) {
        QVariantMap confroom_item = item.toMap();
        ConferenceRoomItem entry;
        entry.extension = confroom_item.value("number").toString();
        entry.join_order = confroom_item.value("join_order").toInt();
        entry.is_me = b_engine->isMeetmeMember(m_room_number, entry.join_order);
        entry.join_time = confroom_item.value("join_time").toInt();
        entry.muted = confroom_item.value("muted").toBool();
        entry.name = confroom_item.value("name").toString();
        m_confroom_item.append(entry);
    }

    endResetModel();
}

int ConferenceRoomModel::rowCount(const QModelIndex &) const
{
    return m_confroom_item.size();
}

int ConferenceRoomModel::columnCount(const QModelIndex&) const
{
    return ConferenceRoom::NB_COL;
}

QList<int> ConferenceRoomModel::columnDisplayBold() const
{
    return QList<int>() << ConferenceRoom::COL_NAME;
}

QList<int> ConferenceRoomModel::columnDisplaySmaller() const
{
    return QList<int>() << ConferenceRoom::COL_SINCE;
}

bool ConferenceRoomModel::isExtensionMuted(const QString &extension) const
{
    foreach(ConferenceRoomItem item, m_confroom_item) {
        if (item.extension == extension) {
            return item.muted;
        }
    }
    return false;
}

int ConferenceRoomModel::joinOrder(const QString &extension) const
{
    foreach(ConferenceRoomItem item, m_confroom_item) {
        if (item.extension == extension) {
            return item.join_order;
        }
    }
    return -1;
}

void ConferenceRoomModel::updateJoinTime()
{
    QModelIndex first = createIndex(0, ConferenceRoom::COL_SINCE);
    QModelIndex last = createIndex(this->rowCount() - 1, ConferenceRoom::COL_SINCE);
    emit dataChanged(first, last);
}

QVariant ConferenceRoomModel::data(const QModelIndex & index, int role) const
{
    int row = index.row();
    int col = index.column();

    switch (role) {
    case Qt::TextAlignmentRole:
        return Qt::AlignVCenter;
    case Qt::DecorationRole:
        if (col == ConferenceRoom::COL_ACTION_MUTE && m_confroom_item[row].is_me) {
            return QPixmap(":images/conference/mute.png").scaledToHeight(16, Qt::SmoothTransformation);
        }
        break;
    case Qt::ToolTipRole:
        if (col == ConferenceRoom::COL_ACTION_MUTE) {
            return tr("Mute/UnMute");
        }
        break;
    case Qt::UserRole:
        if (col == ConferenceRoom::COL_ACTION_MUTE) {
            return m_confroom_item[row].is_me;
        }
        break;
    case Qt::DisplayRole:
        switch (col) {
        case ConferenceRoom::COL_NUMBER:
            return m_confroom_item[row].extension;
        case ConferenceRoom::COL_NAME:
            return m_confroom_item[row].name;
        case ConferenceRoom::COL_SINCE:
            if (m_confroom_item[row].join_time == -1) {
                return tr("Unknown");
            } else {
                return QDateTime::fromTime_t(QDateTime::currentDateTime().toTime_t()
                                             - m_confroom_item[row].join_time
                                             - b_engine->timeDeltaServerClient()).toUTC().toString("hh:mm:ss");
            }
        default:
            break;
        }
    default:
        break;
    }
    return AbstractTableModel::data(index, role);
}

QVariant ConferenceRoomModel::headerData(int section,
                                         Qt::Orientation orientation,
                                         int role) const
{
    if (role != Qt::DisplayRole ||
        orientation != Qt::Horizontal) {
        return QVariant();
    }

    switch (section) {
    case ConferenceRoom::COL_NUMBER:
        return tr("Number");
    case ConferenceRoom::COL_NAME:
        return tr("Name");
    case ConferenceRoom::COL_SINCE:
        return tr("Since");
    default:
        return QVariant();
    }
}

Qt::ItemFlags ConferenceRoomModel::flags(const QModelIndex &index) const
{
    int col = index.column();
    int row = index.row();

    if (col != ConferenceRoom::COL_ACTION_MUTE) {
        return QAbstractItemModel::flags(index);
    }

    bool isMuted = m_confroom_item[row].muted;
    bool is_me = m_confroom_item[row].is_me;

    if (col == ConferenceRoom::COL_ACTION_MUTE && is_me && isMuted) {
        return Qt::ItemIsEnabled;
    }
    return Qt::NoItemFlags;
}

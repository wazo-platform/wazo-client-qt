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

#include <QTimer>

#include "baseengine.h"
#include "conference_room_model.h"

static QVariant COL_TITLE[ConferenceRoomModel::NB_COL];

ConferenceRoomModel::ConferenceRoomModel(QWidget *parent)
    : QAbstractTableModel(parent)
{
    COL_TITLE[ID] = tr("ID");
    COL_TITLE[NUMBER] = tr("Number");
    COL_TITLE[NAME] = tr("Name");
    COL_TITLE[SINCE] = tr("Since");

    QTimer * join_time_timer = new QTimer(this);
    connect(join_time_timer, SIGNAL(timeout()),
            this, SLOT(updateJoinTime()));
    join_time_timer->start(1000);
}

void ConferenceRoomModel::setRoomNumber(QString &room_number)
{
    m_room_number = room_number;
}

void ConferenceRoomModel::updateConfRoom(const QVariantMap &members)
{
    beginResetModel();
    m_members = members;
    extractRow2IdMap();
    endResetModel();
}

void ConferenceRoomModel::extractRow2IdMap()
{
    m_row2number = m_members.keys();
}

void ConferenceRoomModel::sort(int column, Qt::SortOrder order)
{
    struct {
        static bool ascending(const QPair<QString, QString> &a,
                              const QPair<QString, QString> &b) {
            return QString::localeAwareCompare(a.second, b.second) < 0 ?
                                               true : false;
        }
        static bool descending(const QPair<QString, QString> &a,
                               const QPair<QString, QString> &b) {
            return QString::localeAwareCompare(a.second, b.second) < 0 ?
                                               false : true;
        }
    } sFun;

    QList<QPair<QString, QString> > toSort;

    int count = rowCount(QModelIndex());
    beginResetModel();
    for (int i = 0; i < count; i++) {
        toSort.append(QPair<QString, QString>(index(i, ID).data().toString(),
                                              index(i, column).data().toString()));
    }

    qSort(toSort.begin(), toSort.end(), (order == Qt::AscendingOrder) ?
                                         sFun.ascending :
                                         sFun.descending);

    for (int i = 0; i < count; i++) {
        m_row2number.insert(i, QString(toSort[i].first));
    }
    endResetModel();
}

int ConferenceRoomModel::rowCount(const QModelIndex &) const
{
    return m_members.size();
}

int ConferenceRoomModel::columnCount(const QModelIndex&) const
{
    return NB_COL;
}

bool ConferenceRoomModel::isRowMuted(int row) const
{
    const QVariantMap &member = m_members[m_row2number[row]].toMap();
    return member["muted"].toString() == "Yes";
}

int ConferenceRoomModel::userNumberFromRow(int row) const
{
    const QString &number = m_row2number[row];
    return number.toInt();
}

void ConferenceRoomModel::updateJoinTime()
{
    QModelIndex first = createIndex(0, SINCE);
    QModelIndex last = createIndex(m_members.size() - 1, SINCE);

    emit dataChanged(first, last);
}

QVariant ConferenceRoomModel::data(const QModelIndex & index, int role) const
{
    int row = index.row();
    int col = index.column();
    const QString &number = m_row2number[row];
    const QVariantMap &member = m_members[number].toMap();
    int join_sequence = member["join_order"].toInt();
    bool isMe = b_engine->isMeetmeMember(m_room_number, join_sequence);

    if (role != Qt::DisplayRole) {
        if (role == Qt::TextAlignmentRole) {
            return Qt::AlignVCenter;
        } else if (role == Qt::DecorationRole) {
            if (col == ACTION_MUTE && isMe) {
                return QPixmap(":images/conference/mute.png").scaledToHeight(16, Qt::SmoothTransformation);
            }
        } else if (role == Qt::ToolTipRole) {
            if (col == ACTION_MUTE) {
                return tr("Mute/UnMute");
            }
        }
        return QVariant();
    }

    int started_since = member["join_time"].toInt();

    switch (col) {
    case ID:
        return member["join_order"].toInt();
    case NUMBER:
        return member["number"].toString();
    case NAME:
        return member["name"].toString();
    case SINCE:
        if (started_since == -1)
            return tr("Unknown");
        else if (started_since == 0)
            return tr("Not started");
        return QDateTime::fromTime_t(
            QDateTime::currentDateTime().toTime_t()
            - started_since
            - b_engine->timeDeltaServerClient()).toUTC().toString("hh:mm:ss");
    default:
        break;
    }
    return QVariant();
}

QVariant ConferenceRoomModel::headerData(int section,
                                   Qt::Orientation orientation,
                                   int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) {
        return COL_TITLE[section];
    }

    return QVariant();
}

Qt::ItemFlags ConferenceRoomModel::flags(const QModelIndex &index) const
{
    int col = index.column();
    if (col != ACTION_MUTE) return QAbstractItemModel::flags(index);

    int row = index.row();
    const QString &number = m_row2number[row];
    const QVariantMap &member = m_members[number].toMap();
    bool isMuted = member["muted"] == "Yes";
    bool isMe = b_engine->isMeetmeMember(m_room_number, number.toInt());

    if (isMe && col == ACTION_MUTE && isMuted) {
        return Qt::ItemIsEnabled;
    }
    return Qt::NoItemFlags;
}


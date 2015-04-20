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

#include "conference_list_model.h"

ConferenceListModel::ConferenceListModel(QWidget *parent)
    : QAbstractTableModel(parent)
{
    COL_TITLE[NUMBER] = tr("Number");
    COL_TITLE[NAME] = tr("Name");
    COL_TITLE[PIN_REQUIRED] = tr("PIN code");
    COL_TITLE[MEMBER_COUNT] = tr("Member count");
    COL_TITLE[STARTED_SINCE] = tr("Started since");

    QTimer * timer_display = new QTimer(this);
    connect(timer_display, SIGNAL(timeout()),
            this, SLOT(updateConfTime()));
    timer_display->start(1000);
}

void ConferenceListModel::updateConfList(const QVariantMap &configs)
{
    beginResetModel();
    m_room_configs = configs;
    refreshRow2Number();
    endResetModel();
}

void ConferenceListModel::refreshRow2Number()
{
    m_row2number = m_room_configs.keys();
}

int ConferenceListModel::rowCount(const QModelIndex&) const
{
    return m_row2number.size();
}

int ConferenceListModel::columnCount(const QModelIndex&) const
{
    return NB_COL;
}

QVariant ConferenceListModel::data(const QModelIndex &index, int role) const
{
    if (role != Qt::DisplayRole) {
        if (role == Qt::TextAlignmentRole)
            return Qt::AlignVCenter;
        return QVariant();
    }

    /* Rows here are not the same than the lines displayed by the view,
     * as there is a proxy model between the view and this model,
     * that maps the displayed lines to the stored lines
     */
    int row = index.row(), col = index.column();
    if (m_row2number.size() <= row) {
        return QVariant();
    }

    const QString &room_number = m_row2number[row];
    const QVariantMap &room_config = m_room_configs[room_number].toMap();

    switch (col) {
    case NUMBER:
        return room_config["number"].toString();
    case NAME:
        return room_config["name"].toString();
    case PIN_REQUIRED:
        return room_config["pin_required"].toString();
    case MEMBER_COUNT:
        return room_config["member_count"].toString();
    case STARTED_SINCE:
        return startedSince(room_config["start_time"].toDouble());
    default:
        break;
    }

    return QVariant();
}

QString ConferenceListModel::startedSince(double time) const
{
    if (time == 0)
        return tr("Not started");
    else if (time == -1)
        return tr("Unknown");

    uint now = QDateTime::currentDateTime().toTime_t();
    uint started_since = now - uint(time) - b_engine->timeDeltaServerClient();

    return QDateTime::fromTime_t(started_since).toUTC().toString("hh:mm:ss");
}

QVariant ConferenceListModel::headerData(int section,
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

void ConferenceListModel::updateConfTime()
{
    QModelIndex cellChanged1 = createIndex(0, STARTED_SINCE);
    QModelIndex cellChanged2 = createIndex(this->rowCount() - 1, STARTED_SINCE);
    emit dataChanged(cellChanged1, cellChanged2);
}

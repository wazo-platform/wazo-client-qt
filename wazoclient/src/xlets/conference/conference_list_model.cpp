/*
 * Copyright 2007-2017 The Wazo Authors  (see the AUTHORS file)
 *
 * This program is free software: you can redistribute it and/or modify
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
 * This programm is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QTimer>

#include "conference_list_model.h"


ConferenceListModel::ConferenceListModel(QWidget *parent)
  : AbstractTableModel(parent),
    m_headers(QList<QString>() << tr("Name").toUpper()
                               << tr("Number").toUpper()
                               << tr("PIN code").toUpper()
                               << tr("Member count").toUpper()
                               << tr("Started since").toUpper())
{
    QTimer * timer_display = new QTimer(this);
    connect(timer_display, SIGNAL(timeout()),
            this, SLOT(updateConfTime()));
    timer_display->start(1000);
}

void ConferenceListModel::updateConfList(const QVariantMap &configs)
{
    beginResetModel();

    m_conflist_item.clear();
    foreach(QVariant item, configs) {
        QVariantMap conflist_item = item.toMap();
        ConferenceListItem entry(conflist_item.value("name").toString(),
                                 conflist_item.value("number").toString(),
                                 conflist_item.value("start_time").toDouble(),
                                 conflist_item.value("member_count").toInt(),
                                 conflist_item.value("pin_required").toBool()
                                );
        m_conflist_item.append(entry);
    }

    endResetModel();
}

int ConferenceListModel::rowCount(const QModelIndex&) const
{
    return m_conflist_item.size();
}

int ConferenceListModel::columnCount(const QModelIndex&) const
{
    return ConferenceList::NB_COL;
}

QList<int> ConferenceListModel::columnDisplayBold() const
{
    return QList<int>() << ConferenceList::COL_NAME;
}

QList<int> ConferenceListModel::columnDisplaySmaller() const
{
    return QList<int>() << ConferenceList::COL_STARTED_SINCE
                        << ConferenceList::COL_MEMBER_COUNT
                        << ConferenceList::COL_PIN_REQUIRED;
}

QVariant ConferenceListModel::data(const QModelIndex &index, int role) const
{
    if (role != Qt::DisplayRole) {
        if (role == Qt::TextAlignmentRole)
            return Qt::AlignVCenter;
        return AbstractTableModel::data(index, role);
    }

    int row = index.row(), col = index.column();

    switch (col) {
    case ConferenceList::COL_NAME:
        return m_conflist_item[row].name;
    case ConferenceList::COL_NUMBER:
        return m_conflist_item[row].extension;
    case ConferenceList::COL_PIN_REQUIRED:
        if (m_conflist_item[row].pin_required) {
            return tr("Yes");
        } else {
            return tr("No");
        }
    case ConferenceList::COL_MEMBER_COUNT:
        return m_conflist_item[row].member_count;
    case ConferenceList::COL_STARTED_SINCE:
        return this->startedSince(m_conflist_item[row].start_time);
    default:
        break;
    }

    return AbstractTableModel::data(index, role);
}

QVariant ConferenceListModel::headerData(int section,
                                         Qt::Orientation orientation,
                                         int role) const
{
    if (role != Qt::DisplayRole ||
        orientation != Qt::Horizontal) {
        return QVariant();
    }

    return m_headers.value(section);
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

void ConferenceListModel::updateConfTime()
{
    QModelIndex cellChanged1 = createIndex(0, ConferenceList::COL_STARTED_SINCE);
    QModelIndex cellChanged2 = createIndex(this->rowCount() - 1, ConferenceList::COL_STARTED_SINCE);
    emit dataChanged(cellChanged1, cellChanged2);
}

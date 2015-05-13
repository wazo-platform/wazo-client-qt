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

#include <QDateTime>
#include <QIcon>
#include <QString>

#include "history_model.h"

QSize HistoryModel::icon_size = QSize(12, 12);


HistoryModel::HistoryModel(QWidget * parent)
  : AbstractTableModel(parent),
    m_headers(QList<QString>() << tr("Name").toUpper()
                               << tr("Number").toUpper()
                               << tr("Date").toUpper()
                               << tr("Duration").toUpper())
{
}

void HistoryModel::initializeHistory(const QVariantMap &p)
{
    const QVariantList &history_items = p.value("history").toList();
    m_history_item.clear();

    beginResetModel();
    foreach (const QVariant &item, history_items) {
        QVariantMap history_item = item.toMap();
        if (history_item.value("fullname").toString().isEmpty()) {
            history_item.insert("fullname", QString("-"));
        }
        HistoryItem call_log(history_item.value("calldate").toDateTime(),
            history_item.value("extension").toString(),
            history_item.value("fullname").toString(),
            history_item.value("duration").toInt(),
            history_item.value("mode").toInt());
        m_history_item.append(call_log);
    }
    endResetModel();
}

void HistoryModel::updateHistory(const QVariantMap &p)
{
    this->initializeHistory(p);
}

int HistoryModel::rowCount(const QModelIndex&) const
{
    return m_history_item.count();
}

int HistoryModel::columnCount(const QModelIndex&) const
{
    return NB_COLS;
}

QList<int> HistoryModel::columnDisplayBold() const
{
    return QList<int>() << COL_NAME;
}

QList<int> HistoryModel::columnDisplaySmaller() const
{
    return QList<int>() << COL_DATE << COL_DURATION;
}

QVariant HistoryModel::data(const QModelIndex &a, int role) const
{
    int row, column; row = a.row(); column = a.column();
    const HistoryItem &item = m_history_item.at(row);

    if (role == Qt::DisplayRole) {
        switch (column) {
        case COL_NAME:
            return item.name;
        case COL_EXTEN:
            return item.extension;
        case COL_DATE:
            return item.datetime.toString(QString("dd/MM/yyyy HH:mm:ss"));
        case COL_DURATION:
            return this->prettyPrintDuration(item.duration, item.mode);
        default:
            break;
        }
    } else if (role == Qt::UserRole) {
        switch (column) {
        case COL_NAME:
            return item.mode;
        case COL_DATE:
            return item.datetime;
        case COL_DURATION:
            return item.duration;
        default:
            break;
        }
    } else if (role == Qt::DecorationRole && column == COL_NAME) {
        switch (item.mode) {
        case OUTCALL:
            return QIcon(":/images/history/sent-call.svg").pixmap(icon_size);
        case INCALL:
            return QIcon(":/images/history/received-call.svg").pixmap(icon_size);
        case MISSEDCALL:
            return QIcon(":/images/history/missed-call.svg").pixmap(icon_size);
        default:
            break;
        }
    }
    return AbstractTableModel::data(a,role);
}

QVariant HistoryModel::headerData(int section,
                                  Qt::Orientation orientation,
                                  int role = Qt::DisplayRole) const
{
    if (role != Qt::DisplayRole ||
        orientation != Qt::Horizontal) {
        return QVariant();
    }

    return m_headers.value(section);
}

QString HistoryModel::prettyPrintDuration(int duration, int mode) const
{
    if (mode == MISSEDCALL) {
        return "-";
    }

    int sec =   ( duration % 60);
    int min =   ( duration - sec ) / 60 % 60;
    int hou = ( ( duration - sec - min * 60 ) / 60 ) / 60;
    if (hou)
        return tr("%1 hr %2 min %3 s").arg(hou).arg(min).arg(sec);
    else if (min)
        return tr("%1 min %2 s").arg(min).arg(sec);
    else
        return tr("%1 s").arg(sec);
}

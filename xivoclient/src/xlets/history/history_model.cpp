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

#include <QIcon>
#include <QList>

#include <baseengine.h>

#include "history_model.h"

QSize HistoryModel::icon_size = QSize(12, 12);

HistoryModel::HistoryModel(QWidget * parent)
    : AbstractTableModel(parent)
{
    registerListener("history");
}

void HistoryModel::parseCommand(const QVariantMap &map) {
    updateHistory(map);
}

void HistoryModel::initializeHistory(const QVariantMap &p)
{
    QVariantList history_items = p.value("history").toList();
    m_all_history.clear();

    beginResetModel();
    foreach (QVariant item, history_items) {
        QVariantMap history_item = item.toMap();
        m_all_history.append(history_item);
    }
    endResetModel();
}

void HistoryModel::updateHistory(const QVariantMap &p)
{
    this->initializeHistory(p);
}

int HistoryModel::rowCount(const QModelIndex&) const
{
    return m_all_history.count();
}

int HistoryModel::columnCount(const QModelIndex&) const
{
    return NB_COLS;
}

QVariant HistoryModel::data(const QModelIndex &a, int role) const
{
    int row, column; row = a.row(); column = a.column();
    QVariantList histlist = m_all_history;

    if (role == Qt::DisplayRole) {
        if (((histlist.count()) &&
             ((histlist).value(row).toMap().count()))) {
            if (column == COL_NAME) {
                return histlist.value(row).toMap().value("fullname");
            } else if (column == COL_EXTEN) {
                return histlist.value(row).toMap().value("extension");
            } else if (column == COL_DATE) {
                QString qsd = histlist.value(row).toMap().value("calldate").toString();
                QDateTime qdt = QDateTime::fromString(qsd, Qt::ISODate);
                return qdt.toString(QString("dd/MM/yyyy HH:mm:ss"));
            } else if (column == COL_DURATION) {
                int duration = histlist.value(row).toMap().value("duration").toInt();
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
        }
    } else if (role == Qt::UserRole) {
        if (column == COL_NAME) {
            return histlist.value(row).toMap().value("mode");
        } else if (column == COL_DATE) {
            QString call_datetime = histlist.value(row).toMap().value("calldate").toString();
            return QDateTime::fromString(call_datetime, Qt::ISODate);
        } else if (column == COL_DURATION) {
            return histlist.value(row).toMap().value("duration").toInt();
        }
    } else if (role == Qt::DecorationRole && column == COL_NAME) {
        int mode = histlist.value(row).toMap().value("mode").toInt();
        if (mode == OUTCALL) {
            return QIcon(":/images/history/sent-call.svg").pixmap(icon_size);
        } else if (mode == INCALL) {
            return QIcon(":/images/history/received-call.svg").pixmap(icon_size);
        } else if (mode == MISSEDCALL) {
            return QIcon(":/images/history/missed-call.svg").pixmap(icon_size);
        }
    } else if (role == Qt::FontRole) {
        if (column == COL_DATE || column == COL_DURATION) {
            QFont font;
            font.setPixelSize(13);
            return font;
        }
    }
    return AbstractTableModel::data(a,role);
}

QVariant HistoryModel::headerData(int section,
                                    Qt::Orientation orientation,
                                    int role = Qt::DisplayRole) const
{
    if ((role == Qt::DisplayRole) &&
        (orientation == Qt::Horizontal)) {
        if (section == COL_NAME) {
            return QVariant(tr("Name").toUpper());
        } else if (section == COL_EXTEN) {
            return QVariant(tr("Number").toUpper());
        } else if (section == COL_DATE) {
            return QVariant(tr("Date").toUpper());
        } else if (section == COL_DURATION) {
            return QVariant(tr("Duration").toUpper());
        }
    }

    return QVariant();
}

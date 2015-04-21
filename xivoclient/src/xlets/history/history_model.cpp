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

#include <QList>

#include <baseengine.h>

#include "history_model.h"

HistoryModel::HistoryModel(int initialMode, QWidget * parent)
    : QAbstractTableModel(parent), m_sorted(false), m_sorted_column(0), m_sort_order(Qt::AscendingOrder)
{
    registerListener("history");
    m_mode = (HistoryMode) initialMode;
    m_history << QVariant() << QVariant() << QVariant();
    connect(b_engine, SIGNAL(settingsChanged()),
            this, SLOT(requestHistory()));
}

void HistoryModel::parseCommand(const QVariantMap &map) {
    updateHistory(map);
}

void HistoryModel::sort(int column, Qt::SortOrder order)
{
    m_sorted = true;
    m_sorted_column = column;
    m_sort_order = order;

    QList<QVariant> tosort = m_history[m_mode].toList();

    beginResetModel();
    if (order == Qt::AscendingOrder) {
        if (column == 0) {
            qSort(tosort.begin(), tosort.end(), ascendingOrderByNumber);
        } else if (column == 1) {
            qSort(tosort.begin(), tosort.end(), ascendingOrderByDate);
        } else if (column == 2) {
            qSort(tosort.begin(), tosort.end(), ascendingOrderByDuration);
        }
    } else {
        if (column == 0) {
            qSort(tosort.begin(), tosort.end(), descendingOrderByNumber);
        } else if (column == 1) {
            qSort(tosort.begin(), tosort.end(), descendingOrderByDate);
        } else if (column == 2) {
            qSort(tosort.begin(), tosort.end(), descendingOrderByDuration);
        }
    }

    m_history[m_mode] = tosort;
    endResetModel();
}

int HistoryModel::rowCount(const QModelIndex&) const
{
    int nbrow = 0;

    if (((m_history[m_mode].toList().count())))
        nbrow = (m_history[m_mode].toList()).count();

    return nbrow;
}

int HistoryModel::columnCount(const QModelIndex&) const
{
    if ((m_mode == OUTCALLS) || (m_mode == INCALLS))
        return 3;
    else if (m_mode == MISSEDCALLS)
        return 2;

    return 0;
}

QVariant HistoryModel::data(const QModelIndex &a, int role) const
{
    int row, column; row = a.row(); column = a.column();

    if (role == Qt::DisplayRole) {
        QVariantList histlist = m_history[m_mode].toList();
        if (((histlist.count()) &&
             ((histlist).value(row).toMap().count()))) {
            if (column == 0) {
                return histlist.value(row).toMap().value("fullname");
            } else if (column == 1) {
                QString qsd = histlist.value(row).toMap().value("calldate").toString();
                QDateTime qdt = QDateTime::fromString(qsd, Qt::ISODate);
                QString qsf = qdt.toString(Qt::DefaultLocaleLongDate); // Qt::DefaultLocaleShortDate
                return qsf;
            } else if (column == 2) {
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
    }

    return QVariant();
}

/*! \brief parse history command response
 */
void HistoryModel::updateHistory(const QVariantMap &p)
{
    int mode = p.value("mode").toInt();
    QVariantList h = p.value("history").toList();
    beginResetModel();
    if (mode == m_mode)
        m_history[m_mode] = h;
    if (m_sorted) {
        sort(m_sorted_column, m_sort_order);
    }
    endResetModel();
}

Qt::ItemFlags HistoryModel::flags(const QModelIndex &) const
{
    return Qt::NoItemFlags;
}

/*! \brief ask history for an extension */
void HistoryModel::requestHistory(HistoryMode mode, QString xuserid)
{
    if (mode == DEFAULT) {
        mode = m_mode;
    }
    if (xuserid.isEmpty()) {
        xuserid = b_engine->getFullId();
    }
    if (mode == OUTCALLS || mode == INCALLS || mode == MISSEDCALLS) {
        QVariantMap command;
        command["class"] = "history";
        command["xuserid"] = xuserid;
        command["size"] = QString::number(b_engine->getConfig("historysize").toUInt());
        command["mode"] = QString::number(mode);
        b_engine->sendJsonCommand(command);
    }
}

void HistoryModel::changeMode(bool active)
{
    if (active) {
        m_mode = (HistoryMode)sender()->property("mode").toInt();
        requestHistory(m_mode);
	beginResetModel();
        emit headerDataChanged(Qt::Horizontal, 0, 3);
	endResetModel();
    }
}

QVariant HistoryModel::headerData(int section,
                                    Qt::Orientation orientation,
                                    int role = Qt::DisplayRole) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) {
        if (section == 0)
            return QVariant(tr("Number"));
        else if (section == 1)
            return QVariant(tr("Date"));

        if ((section == 2) && ((m_mode == OUTCALLS) || (m_mode == INCALLS)))
            return QVariant(tr("Duration"));
    }

    return QVariant();
}


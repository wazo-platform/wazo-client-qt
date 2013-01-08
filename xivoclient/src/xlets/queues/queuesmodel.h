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

#ifndef __QUEUESMODEL_H__
#define __QUEUESMODEL_H__

#include <QAbstractTableModel>
#include <QStringList>

/*! \brief Queues model.
 *
 * Infos come mainly from BaseEngine.
 *
 * m_row2id maps row numbers to queue ids. It is an ordered QStringList.
 * 
 * Exceptions are (see QueueDataStruct):
 * - max waiting times, one for each queue, computed on each updateStatus
 * - waiting calls, cached from BaseEngine (used in the computation of the max
 *   waiting time
 * - stats, fetched from the server and stored here (plugin-only info, should
 *   not be in BaseEngine
 */
class QueuesModel : public QAbstractTableModel
{
    Q_OBJECT

    // Methods
    public:
        QueuesModel(QObject *parent = NULL);

        int rowCount(const QModelIndex &) const;
        int columnCount(const QModelIndex &) const;

        bool removeRows(int, int, const QModelIndex &);

        QVariant data(const QModelIndex&, int role = Qt::DisplayRole) const;
        QVariant headerData(int,
                            Qt::Orientation,
                            int) const;

        void eatQueuesStats(const QVariantMap &);

    public slots:
        void updateQueueConfig(const QString &);
        void removeQueueConfig(const QString &);
        void increaseWaitTime();
        void updateQueueNames();

    private:
        QVariant formatTime(const QVariant &) const;
        QVariant formatPercent(const QVariant &) const;
        void refreshQueueDisplay(const QString &queue_xid);
    // Attributes
    public:
        enum Columns {
            ID, NUMBER, NAME, WAITING_CALLS, EWT, CURRENT_MAX_WAIT,
            TALKING_AGENTS, LOGGEDAGENTS, AVAILABLE_AGENTS,
            RECEIVED, ANSWERED, ABANDONED, MEAN_WAIT, TOTAL_MAX_WAIT, EFFICIENCY, QOS, NB_COL
        };

    signals:
        void askForQueueStats();

    private:

        struct HeaderStruct {
            QString label;
            QString tooltip;
        };

        struct QueueDataStruct {
            QMap<QString, QString> stats;
        };

        HeaderStruct m_headers[NB_COL];
        QStringList m_row2id;
        QMap<QString, QueueDataStruct> m_queues_data;
};

#endif

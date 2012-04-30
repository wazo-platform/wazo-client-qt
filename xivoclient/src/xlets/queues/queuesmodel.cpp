/* XiVO Client
 * Copyright (C) 2007-2011, Avencall
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

#include "queuesmodel.h"

static QStringList statsOfDurationType;
static QStringList statsInPercent;
static QStringList statsToRequest;

QueuesModel::QueuesModel(QObject *parent)
    : QAbstractTableModel(parent)
{
    m_headers[ID].label = "ID";
    m_headers[ID].tooltip = "ID";
    m_headers[NUMBER].label = tr("Number");
    m_headers[NUMBER].tooltip = tr("Phone number of the queue");
    m_headers[NAME].label = tr("Queues");
    m_headers[NAME].tooltip = tr("Queue names");
    m_headers[WAITING_CALLS].label = tr("Waiting calls");
    m_headers[WAITING_CALLS].tooltip = tr("Number of waiting calls");
    m_headers[CURRENT_MAX_WAIT].label = tr("Longest wait");
    m_headers[CURRENT_MAX_WAIT].tooltip = tr("Longest waiting call");
    m_headers[LOGGEDAGENTS].label = tr("Logged");
    m_headers[LOGGEDAGENTS].tooltip = tr("number of logged agents");
    m_headers[RECEIVED].label = tr("Received");
    m_headers[RECEIVED].tooltip = tr("Number of received calls");
    m_headers[ANSWERED].label = tr("Answered");
    m_headers[ANSWERED].tooltip = tr("Number of answered calls");
    m_headers[ABANDONNED].label = tr("Abandonned");
    m_headers[ABANDONNED].tooltip = tr("Number of abandonned calls");
    m_headers[TOTAL_MAX_WAIT].label = tr("Max Waiting Time");
    m_headers[TOTAL_MAX_WAIT].tooltip = tr("Maximum waiting time before getting an agent");
    m_headers[EFFICIENCY].label = tr("Efficiency");
    m_headers[EFFICIENCY].tooltip = tr("Ratio (Answered) / (Received)");
    m_headers[QOS].label = tr("QOS");
    m_headers[QOS].tooltip = tr("Ratio (Calls answered in less than X sec / "
                                 "Number of calls answered)");

    connect(b_engine, SIGNAL(updateQueueConfig(const QString &)),
            this, SLOT(updateQueueConfig(const QString &)));
    connect(b_engine, SIGNAL(removeQueueConfig(const QString &)),
            this, SLOT(removeQueueConfig(const QString &)));
    connect(b_engine, SIGNAL(updateQueueStatus(const QString &)),
            this, SLOT(updateQueueStatus(const QString &)));

    // In case the option "show queue numbers" is toggled
    connect(b_engine, SIGNAL(settingsChanged()),
            this, SLOT(updateQueueNames()));

    statsOfDurationType << "Xivo-Holdtime-max";
    statsToRequest << "Xivo-Holdtime-max" << "Xivo-Qos"
                   << "Xivo-Join" << "Xivo-Lost"
                   << "Xivo-Rate" << "Xivo-Link";
    statsInPercent << "Xivo-Rate" << "Xivo-Qos";
}

void QueuesModel::updateQueueConfig(const QString &xid)
{
    if (! m_row2id.contains(xid)) {
        int insertedRow = m_row2id.size();
        beginInsertRows(QModelIndex(), insertedRow, insertedRow);
        m_row2id.append(xid);
        m_queues_data[xid].max_wait = 0;
        m_queues_data[xid].waiting_calls = 0;
        endInsertRows();

        /* Ask for stats once now, to avoid waiting the first update (default
         * is 30s)
         */
        emit askForQueueStats();
    } else {
        QModelIndex cellChanged = createIndex(m_row2id.indexOf(xid), NAME);
        // sends signal to proxy/view that the data should be refreshed
        emit dataChanged(cellChanged, cellChanged);
    }
}

void QueuesModel::removeQueueConfig(const QString &xid)
{
    if (m_row2id.contains(xid)) {
        int removedRow = m_row2id.indexOf(xid);
        removeRow(removedRow); // calls removeRows
    }
}

void QueuesModel::updateQueueStatus(const QString &xid)
{
    if (!m_row2id.contains(xid)) {
        return;
    }

    const QueueInfo * queueinfo = b_engine->queue(xid);
    if (queueinfo == NULL) return;
    
    // Waiting calls
    {
        m_queues_data[xid].waiting_calls = queueinfo->xincalls().count();
        QModelIndex cellChanged = createIndex(m_row2id.indexOf(xid), WAITING_CALLS);
        // sends signal to proxy/view that the data should be refreshed
        emit dataChanged(cellChanged, cellChanged);
    }
    
    // Current max wait
    {
        uint oldest = 0;
        bool first_item = true;
        uint current_entrytime;

        foreach (QString xchannel, queueinfo->xincalls()) {
            const ChannelInfo * channelinfo = b_engine->channels().value(xchannel);
            if (channelinfo == NULL) continue;
            
            current_entrytime = uint(channelinfo->timestamp());
            if (first_item) {
                oldest = current_entrytime;
                first_item = false;
            }
            oldest = (oldest < current_entrytime) ? oldest : current_entrytime ;
        }

        if (oldest == 0) { // There are no waiting calls
            m_queues_data[xid].max_wait = 0;
        } else {
            m_queues_data[xid].max_wait = b_engine->timeServer() - oldest;
        }
        
        QModelIndex cellChanged = createIndex(m_row2id.indexOf(xid), CURRENT_MAX_WAIT);
        // sends signal to proxy/view that the data should be refreshed
        emit dataChanged(cellChanged, cellChanged);
    }
}

/*! \brief Increase max waiting times of one second and tells the view to
 * refresh
 */
void QueuesModel::increaseWaitTime()
{
    foreach(QString xqueueid, m_queues_data.keys()) {
        // Do not update if no one is waiting in the queue
        if (m_queues_data[xqueueid].waiting_calls > 0) {
            m_queues_data[xqueueid].max_wait ++;
        }
    }
    
    QModelIndex cellChanged1 = createIndex(0, CURRENT_MAX_WAIT);
    QModelIndex cellChanged2 = createIndex(m_row2id.size() - 1, CURRENT_MAX_WAIT);
    // sends signal to proxy/view that the data should be refreshed
    emit dataChanged(cellChanged1, cellChanged2);
}

/*! \brief Tells the view that the queue names should be refreshed
 */
void QueuesModel::updateQueueNames()
{
    QModelIndex cellChanged1 = createIndex(0, NAME);
    QModelIndex cellChanged2 = createIndex(m_row2id.size() - 1, NAME);
    // sends signal to proxy/view that the data should be refreshed
    emit dataChanged(cellChanged1, cellChanged2);
}

bool QueuesModel::removeRows(int row, int count, const QModelIndex &)
{
    bool ret = true;
    if (count > 0) {
        beginRemoveRows(QModelIndex(), row, row + count - 1);
        for (int i = 0 ; i < count ; i ++) {
            ret = ret && row < m_row2id.size();
            m_row2id.removeAt(row);
        }
        // sends signal to proxy/view that the data should be refreshed
        endRemoveRows();
    }
    return ret;
}

int QueuesModel::rowCount(const QModelIndex&) const
{
    return m_row2id.size();
}

int QueuesModel::columnCount(const QModelIndex&) const
{
    return NB_COL;
}

QVariant QueuesModel::formatPercent(const QVariant &data) const
{
    QString value = data.toString();
    if (value.isEmpty()) {
        return "-";
    } else {
        bool data_is_int;
        data.toInt(&data_is_int);
        if (!data_is_int) {
            return data;
        } else {
            return QString("%0 %").arg(value);
        }
    }
}

QVariant QueuesModel::formatTime(const QVariant &data) const
{
    QString value = data.toString();
    if (value.isEmpty()) {
        return "-";
    } else {
        bool data_is_int;
        unsigned nsecs = data.toInt(&data_is_int);
        if (!data_is_int) {
            return data;
        } else {
            QTime ret;
            ret = ret.addSecs(nsecs);
            return ret.toString("mm':'ss");
        }
    }
}

QVariant QueuesModel::data(const QModelIndex &index, int role) const
{
    // Alignment
    if (role == Qt::TextAlignmentRole) {
        return Qt::AlignCenter;
    }

    /* Rows here are not the same than the lines displayed by the view,
     * as there is a proxy model between the view and this model,
     * that maps the displayed lines to the stored lines, see ConfList
     * constructor
     */
    int row = index.row(), col = index.column();
    QString xqueueid;

    if (m_row2id.size() > row) {
        xqueueid = m_row2id[row];
    }

    const QueueInfo * queueinfo = b_engine->queue(xqueueid);
    if (queueinfo == NULL) return QVariant();

    if (! m_queues_data.contains(xqueueid)) return QVariant();
    QueueDataStruct queue_data = m_queues_data[xqueueid];

    // Background color
    if (role == Qt::BackgroundRole) {
        unsigned greenlevel = 0, orangelevel = 0, value = 0;
        switch (col) {
            case WAITING_CALLS :
                greenlevel = b_engine->getConfig("guioptions.queuelevels").toMap().value("green").toUInt() - 1;
                orangelevel = b_engine->getConfig("guioptions.queuelevels").toMap().value("orange").toUInt() - 1;
                value = queue_data.waiting_calls;
                break ;
            case CURRENT_MAX_WAIT :
                greenlevel = b_engine->getConfig("guioptions.queuelevels_wait").toMap().value("green").toUInt() - 1;
                orangelevel = b_engine->getConfig("guioptions.queuelevels_wait").toMap().value("orange").toUInt() - 1;
                value = queue_data.max_wait;
                break ;
            default :
                return QVariant();
        }

        if (value <= 0) {
            return QVariant();
        } else if (value <= greenlevel) {
            return QBrush(QColor(Qt::green));
        } else if (value <= orangelevel) {
            return QBrush(QColor("orange"));
        } else {
            return QBrush(QColor(Qt::red));
        }
    }

    // Data
    QString not_available = tr("N/A");
    if (role == Qt::DisplayRole) {
        switch (col) {
            case ID :
                return queueinfo->xid();
            case NUMBER :
                return queueinfo->queueNumber();
            case NAME :
                return queueinfo->queueDisplayName();
            case WAITING_CALLS :
                return QString::number(queue_data.waiting_calls);
            case CURRENT_MAX_WAIT :
                return formatTime(queue_data.max_wait);
            case LOGGEDAGENTS:
                return queue_data.stats.value("Xivo-LoggedAgents", not_available);
            case RECEIVED :
                return queue_data.stats.value("Xivo-Join", not_available);
            case ANSWERED :
                return queue_data.stats.value("Xivo-Link", not_available);
            case ABANDONNED :
                return queue_data.stats.value("Xivo-Lost", not_available);
            case TOTAL_MAX_WAIT :
                return formatTime(queue_data.stats.value("Xivo-Holdtime-max", not_available));
            case EFFICIENCY :
                return formatPercent(queue_data.stats.value("Xivo-Rate", not_available));
            case QOS :
                return formatPercent(queue_data.stats.value("Xivo-Qos", not_available));
            default :
                return not_available;
        }
    }
    return QVariant();
}

QVariant QueuesModel::headerData(int section,
                                 Qt::Orientation orientation,
                                 int role) const
{
    if (orientation != Qt::Horizontal) {
        return QVariant();
    }

    switch(role) {
        case Qt::DisplayRole :
            return m_headers[section].label;
        case Qt::ToolTipRole :
            return m_headers[section].tooltip;
        case Qt::BackgroundRole :
            if (section >= RECEIVED) {
                return QBrush(Qt::darkGray);
            } else {
                return QVariant();
            }
        case Qt::ForegroundRole :
            if (section >= RECEIVED) {
                return QBrush(Qt::white);
            } else {
                return QVariant();
            }
        default :
            return QVariant();
    }
}

/*! \brief Decode queue stats from the server
 *
 * Converts queues stats from server format to model format
 */
void QueuesModel::eatQueuesStats(const QVariantMap &p)
{
    foreach (QString queueid, p.value("stats").toMap().keys()) {
        QString xqueueid = QString("%0/%1").arg(b_engine->ipbxid()).arg(queueid);
        QVariantMap qvm = p.value("stats").toMap().value(queueid).toMap();
        foreach (QString stat_name, qvm.keys()) {
            QString field;
            if (statsInPercent.contains(stat_name)) {
                field = formatPercent(qvm.value(stat_name)).toString();
            }
            field = qvm.value(stat_name).toString();
            m_queues_data[xqueueid].stats[stat_name] = field;
        }
        
        QModelIndex cellChanged1 = createIndex(m_row2id.indexOf(xqueueid), RECEIVED);
        QModelIndex cellChanged2 = createIndex(m_row2id.indexOf(xqueueid), QOS);
        emit dataChanged(cellChanged1, cellChanged2);
    }
}

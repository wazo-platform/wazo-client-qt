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
#include <queuememberdao.h>

#include "agentsmodel.h"

QString AgentsModel::not_available = tr("N/A");

AgentsModel::AgentsModel(QObject *parent)
    : QAbstractTableModel(parent)
{
    m_headers[ID] = "ID";
    m_headers[NUMBER] = tr("Number");
    m_headers[FIRSTNAME] = tr("First name");
    m_headers[LASTNAME] = tr("Last name");
    m_headers[SPY_STATUS] = tr("Listen");
    m_headers[ONLINE] = tr("On Line");
    m_headers[LOGGED_STATUS] = tr("Logged");
    m_headers[JOINED_QUEUES] = tr("Joined\nqueues");
    m_headers[PAUSED_STATUS] = tr("Paused");
    m_headers[PAUSED_QUEUES] = tr("Paused\nqueues");

    connect(b_engine, SIGNAL(updateAgentConfig(const QString &)),
            this, SLOT(updateAgentConfig(const QString &)));
    connect(b_engine, SIGNAL(updateAgentStatus(const QString &)),
            this, SLOT(updateAgentStatus(const QString &)));
    connect(b_engine, SIGNAL(statusListen(const QString &, const QString &, const QString &)),
            this, SLOT(updateAgentListenStatus(const QString &, const QString &, const QString &)));
}

int AgentsModel::rowCount(const QModelIndex&) const
{
    return m_row2id.size();
}

int AgentsModel::columnCount(const QModelIndex&) const
{
    return NB_COL;
}

bool AgentsModel::removeRows(int /*row*/, int /*count*/, const QModelIndex & /*index*/)
{
    return true;
}

void AgentsModel::updateAgentConfig(const QString &agent_id)
{
    if (! m_row2id.contains(agent_id)) {
        int insertedRow = m_row2id.size();
        beginInsertRows(QModelIndex(), insertedRow, insertedRow);
        m_row2id.append(agent_id);
        endInsertRows();

    } else {
        this->refreshAgentRow(agent_id);
    }
}

void AgentsModel::removeAgentConfig(const QString &agent_id)
{
    if (m_row2id.contains(agent_id)) {
        int removedRow = m_row2id.indexOf(agent_id);
        removeRow(removedRow);
    }
}

void AgentsModel::updateAgentStatus(const QString &agent_id)
{
    if (!m_row2id.contains(agent_id)) {
        return;
    }

    const AgentInfo * agentinfo = b_engine->agent(agent_id);
    if (agentinfo == NULL) return;

    this->refreshAgentRow(agent_id);
}

void AgentsModel::refreshAgentRow(const QString & agent_id)
{
    unsigned first_column_index = 0;
    unsigned last_column_index = NB_COL - 1;
    unsigned agent_row_id = m_row2id.indexOf(agent_id);
    QModelIndex cell_changed_start = createIndex(agent_row_id, first_column_index);
    QModelIndex cell_changed_end = createIndex(agent_row_id, last_column_index);
    emit dataChanged(cell_changed_start, cell_changed_end);
}

void AgentsModel::updateAgentListenStatus(const QString & /*ipbxid*/, const QString & /*agent_id*/, const QString & /*status*/)
{
}

QVariant AgentsModel::headerData(int section,
                                 Qt::Orientation orientation,
                                 int role) const
{
    if (orientation != Qt::Horizontal) {
        return QVariant();
    }

    switch(role) {
        case Qt::DisplayRole :
            return m_headers[section];
        default :
            return QVariant();
    }
}

QVariant AgentsModel::data(const QModelIndex &index, int role) const
{
    int row = index.row(), column = index.column();

    switch(role) {
    case Qt::TextAlignmentRole:
        return Qt::AlignCenter;
    case  Qt::DisplayRole:
        return this->dataDisplay(row, column);
    default:
        return QVariant();
    }
}

QVariant AgentsModel::dataDisplay(int row, int column) const
{
    QString agent_id;

    if (m_row2id.size() > row) {
        agent_id = m_row2id[row];
    }

    const AgentInfo * agent = b_engine->agent(agent_id);
    if (agent == NULL) return QVariant();

    switch (column) {
    case ID :
        return agent_id;
    case NUMBER :
        return agent->agentNumber();
    case FIRSTNAME :
        return agent->firstname();
    case LASTNAME :
        return agent->lastname();
    case SPY_STATUS:
        return tr("Listen");
    case ONLINE:
        return "Online";
    case LOGGED_STATUS:
        return this->dataDisplayLogged(agent->logged());
    case JOINED_QUEUES :
        return agent->joinedQueueCount();
    case PAUSED_STATUS:
        return this->dataDisplayPaused(agent->pausedStatus());
    case PAUSED_QUEUES :
        return agent->pausedQueueCount();
    default :
        return this->not_available;
    }

    return QVariant();
}


QString AgentsModel::dataDisplayLogged(bool logged_status) const
{
    if (logged_status) {
        return tr("Logged");
    } else {
        return tr("Unlogged");
    }
}

QString AgentsModel::dataDisplayPaused(enum AgentPauseStatus pause_status) const
{
    switch (pause_status) {
    case UNPAUSED:
        return tr("Unpaused");
    case PAUSED:
        return tr("Paused");
    case PARTIALLY_PAUSED:
        return tr("Partially Paused");
    default:
        return this->not_available;
    }
}

int AgentsModel::getNumberOfJoinedQueues(const QString &agent_id)
{
    return (QueueMemberDAO::queueListFromAgentId(agent_id).size());
}

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

#include <QBrush>

#include <baseengine.h>
#include <dao/queuememberdao.h>

#include "agents_model.h"

QString AgentsModel::not_available = QObject::tr("N/A");

AgentsModel::AgentsModel(QObject *parent)
    : QAbstractTableModel(parent)
{
    m_headers[ID] = "ID";
    m_headers[NUMBER] = tr("Number");
    m_headers[FIRSTNAME] = tr("First name");
    m_headers[LASTNAME] = tr("Last name");
    m_headers[LISTEN] = tr("Listen");
    m_headers[AVAILABILITY] = tr("Status since");
    m_headers[LOGGED_STATUS] = tr("Logged");
    m_headers[JOINED_QUEUES] = tr("Joined\nqueues");
    m_headers[PAUSED_STATUS] = tr("Paused");
    m_headers[PAUSED_QUEUES] = tr("Paused\nqueues");

    connect(b_engine, SIGNAL(updateAgentConfig(const QString &)),
            this, SLOT(updateAgentConfig(const QString &)));
    connect(b_engine, SIGNAL(removeAgentConfig(const QString &)),
            this, SLOT(removeAgentConfig(const QString &)));
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

bool AgentsModel::removeRows(int row, int count, const QModelIndex & index)
{
    bool ret = true;
    if (count > 0) {
        beginRemoveRows(index, row, row + count - 1);
        for (int i = 0 ; i < count ; i ++) {
            ret = ret && row < m_row2id.size();
            m_row2id.removeAt(row);
        }
        endRemoveRows();
    }
    return ret;
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

void AgentsModel::refreshColumn(int column_index)
{
    unsigned first_row_index = 0;
    unsigned last_row_index = m_row2id.size() - 1;
    QModelIndex cell_changed_start = createIndex(first_row_index, column_index);
    QModelIndex cell_changed_end = createIndex(last_row_index, column_index);
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
    case Qt::BackgroundRole:
        return this->dataBackground(row, column);
    case Qt::ToolTipRole:
        return this->dataTooltip(row, column);
    case Qt::UserRole:
        return this->dataUser(row, column);
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
    case LISTEN:
        return tr("Listen");
    case AVAILABILITY:
        return this->dataDisplayAvailability(agent);
    case STATUS_LABEL:
        return this->dataDisplayStatusLabel(agent);
    case STATUS_SINCE:
        return this->dataDisplayStatusSince(agent);
    case LOGGED_STATUS:
        return this->dataDisplayLogged(agent->logged());
    case JOINED_QUEUES :
        return agent->joinedQueueCount();
    case JOINED_QUEUE_LIST :
        return this->dataDisplayQueueList(agent_id);
    case PAUSED_STATUS:
        return this->dataDisplayPaused(agent->pausedStatus());
    case PAUSED_QUEUES :
        return agent->pausedQueueCount();
    default :
        return this->not_available;
    }
}

QVariant AgentsModel::dataTooltip(int row, int column) const
{
    QString agent_id;

    if (m_row2id.size() > row) {
        agent_id = m_row2id[row];
    }

    const AgentInfo * agent = b_engine->agent(agent_id);
    if (agent == NULL) return QVariant();

    switch (column) {
    case AVAILABILITY:
        return this->dataTooltipAvailability(agent->availability());
    case JOINED_QUEUES:
        return this->dataTooltipJoinedQueues(agent);
    case PAUSED_QUEUES:
        return this->dataTooltipPausedQueues(agent);
    default :
        return QVariant();
    }
}

QString AgentsModel::dataTooltipPausedQueues(const AgentInfo *agent) const
{
    if (agent == NULL) {
        return QString();
    }
    QStringList paused_queues = agent->pausedQueueNames();
    return paused_queues.join("\n");
}


QString AgentsModel::dataTooltipJoinedQueues(const AgentInfo *agent) const
{
    if (agent == NULL) {
        return QString();
    }
    QStringList joined_queues = agent->joinedQueueNames();
    return joined_queues.join("\n");
}

QString AgentsModel::dataTooltipAvailability(enum AgentInfo::AgentAvailability availability) const
{
    switch (availability) {
    case AgentInfo::AVAILABLE:
        return tr("Agent ready to receive a call");
    case AgentInfo::UNAVAILABLE:
        return tr("Agent processing a call or paused");
    case AgentInfo::ON_CALL_NONACD_INCOMING_INTERNAL:
        return tr("Agent receiving an internal call out of queue");
    case AgentInfo::ON_CALL_NONACD_INCOMING_EXTERNAL:
        return tr("Agent receiving an external call out of queue");
    case AgentInfo::ON_CALL_NONACD_OUTGOING_INTERNAL:
        return tr("Agent emiting an internal call");
    case AgentInfo::ON_CALL_NONACD_OUTGOING_EXTERNAL:
        return tr("Agent emiting an external call");
    default:
        return QString();
    }
}

QVariant AgentsModel::dataBackground(int row, int column) const
{
    QString agent_id;

    if (m_row2id.size() > row) {
        agent_id = m_row2id[row];
    }

    const AgentInfo * agent = b_engine->agent(agent_id);
    if (agent == NULL) return QVariant();

    switch (column) {
    case AVAILABILITY:
        return this->dataBackgroundAvailability(agent);
    case LOGGED_STATUS:
        return this->dataBackgroundLogged(agent->logged());
    case PAUSED_STATUS:
        return this->dataBackgroundPaused(agent->pausedStatus());
    default :
        return QVariant();
    }
}

QString AgentsModel::dataDisplayAvailability(const AgentInfo * agent) const
{
    QString availability = this->convertAgentAvailabilityToString(agent->availability());
    QString time_since = agent->availabilitySince();
    if (agent->availability() != AgentInfo::LOGGED_OUT) {
        return QString("%1 (%2)").arg(availability).arg(time_since);
    } else {
        return availability;
    }
}

QString AgentsModel::dataDisplayStatusLabel(const AgentInfo * agent) const
{
    return this->convertAgentAvailabilityToString(agent->availability());
}

QString AgentsModel::dataDisplayStatusSince(const AgentInfo * agent) const
{
    return agent->availabilitySince();
}

QString AgentsModel::convertAgentAvailabilityToString(AgentInfo::AgentAvailability availability) const
{
    switch (availability) {
    case AgentInfo::LOGGED_OUT:
        return "-";
    case AgentInfo::AVAILABLE:
        return tr("Not in use");
    case AgentInfo::UNAVAILABLE:
        return tr("In use");
    case AgentInfo::ON_CALL_NONACD_INCOMING_INTERNAL:
        return tr("Int. Incoming");
    case AgentInfo::ON_CALL_NONACD_INCOMING_EXTERNAL:
        return tr("Ext. Incoming");
    case AgentInfo::ON_CALL_NONACD_OUTGOING_INTERNAL:
        return tr("Int. Outgoing");
    case AgentInfo::ON_CALL_NONACD_OUTGOING_EXTERNAL:
        return tr("Ext. Outgoing");
    default:
        return QString();
    }
}

QString AgentsModel::convertAgentAvailabilityToObjectName(AgentInfo::AgentAvailability availability) const
{
    switch (availability) {
    case AgentInfo::AVAILABLE:
        return "AgentAvailable";
    case AgentInfo::UNAVAILABLE:
        return "AgentInUse";
    case AgentInfo::ON_CALL_NONACD_INCOMING_INTERNAL:
        return "AgentOnCallNonACDIncomingInternal";
    case AgentInfo::ON_CALL_NONACD_INCOMING_EXTERNAL:
        return "AgentOnCallNonACDIncomingExternal";
    case AgentInfo::ON_CALL_NONACD_OUTGOING_INTERNAL:
        return "AgentOnCallNonACDOutgoingInternal";
    case AgentInfo::ON_CALL_NONACD_OUTGOING_EXTERNAL:
        return "AgentOnCallNonACDOutgoingExternal";
    case AgentInfo::LOGGED_OUT:
    default:
        return "AgentStatus";
    }
}

QVariant AgentsModel::dataBackgroundAvailability(const AgentInfo * agent) const
{
    switch (agent->availability()) {
    case AgentInfo::AVAILABLE:
        return Qt::green;
    case AgentInfo::UNAVAILABLE:
    case AgentInfo::ON_CALL_NONACD_INCOMING_INTERNAL:
    case AgentInfo::ON_CALL_NONACD_INCOMING_EXTERNAL:
    case AgentInfo::ON_CALL_NONACD_OUTGOING_INTERNAL:
    case AgentInfo::ON_CALL_NONACD_OUTGOING_EXTERNAL:
        return Qt::red;
    default:
        return QVariant();
    }
}

QString AgentsModel::dataDisplayLogged(bool logged_status) const
{
    if (logged_status) {
        return tr("Logged");
    } else {
        return tr("Unlogged");
    }
}

QVariant AgentsModel::dataBackgroundLogged(bool logged_status) const
{
    if (logged_status) {
        return Qt::green;
    } else {
        return Qt::red;
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

QVariant AgentsModel::dataBackgroundPaused(enum AgentPauseStatus pause_status) const
{
    switch (pause_status) {
    case UNPAUSED:
        return Qt::green;
    case PAUSED:
        return Qt::red;
    case PARTIALLY_PAUSED:
        return Qt::yellow;
    default:
        return QVariant();
    }
}

void AgentsModel::increaseAvailability()
{
    this->refreshColumn(AVAILABILITY);
    this->refreshColumn(STATUS_SINCE);
}

QStringList AgentsModel::dataDisplayQueueList(QString agent_id) const
{
   return QueueMemberDAO::queueListFromAgentId(agent_id);
}

QVariant AgentsModel::dataUser(int row, int column) const
{
    QString agent_id;

    if (m_row2id.size() > row) {
        agent_id = m_row2id[row];
    }

    const AgentInfo * agent = b_engine->agent(agent_id);
    if (agent == NULL) return QVariant();

    switch(column) {
    case LOGGED_STATUS:
        return agent->logged();
    case AVAILABILITY:
    case STATUS_LABEL:
        return this->convertAgentAvailabilityToObjectName(agent->availability());
    default:
        return QVariant();
    }
}

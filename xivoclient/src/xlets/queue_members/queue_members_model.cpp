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

#include <QBrush>

#include <baseengine.h>
#include <storage/agentinfo.h>
#include <storage/phoneinfo.h>
#include <storage/queuememberinfo.h>
#include <storage/userinfo.h>
#include <dao/queuememberdao.h>
#include <dao/phonedao.h>
#include <dao/phonedaoimpl.h>

#include "queue_members_model.h"

QString QueueMembersModel::not_available = QObject::tr("N/A");

QueueMembersModel::QueueMembersModel(QObject *parent)
    : QAbstractTableModel(parent)
{
    this->fillHeaders();
    connect(b_engine, SIGNAL(updateQueueMemberConfig(const QString &)),
            this, SLOT(updateQueueMemberConfig(const QString &)));
    connect(b_engine, SIGNAL(removeQueueMemberConfig(const QString &)),
            this, SLOT(removeQueueMemberConfig(const QString &)));
    connect(b_engine, SIGNAL(updateAgentConfig(const QString &)),
            this, SLOT(updateAgentConfig(const QString &)));
}

void QueueMembersModel::fillHeaders()
{
    m_headers[ID].label = tr("ID");
    m_headers[ID].tooltip = tr("ID");
    m_headers[NUMBER].label = tr("Number");
    m_headers[NUMBER].tooltip = tr("Phone number of the queue");
    m_headers[FIRSTNAME].label = tr("Firstname");
    m_headers[FIRSTNAME].tooltip = tr("Queue member's firstname");
    m_headers[LASTNAME].label = tr("Lastname");
    m_headers[LASTNAME].tooltip = tr("Queue member's lastname");
    m_headers[LOGGED].label = tr("Logged");
    m_headers[LOGGED].tooltip = tr("Queue member's login status");
    m_headers[PAUSED].label = tr("Paused");
    m_headers[PAUSED].tooltip = tr("Queue member's pause status");
    m_headers[ANSWERED_CALLS].label = tr("Answered calls");
    m_headers[ANSWERED_CALLS].tooltip = tr("Number of answered calls");
    m_headers[LAST_CALL_DATE].label = tr("Last call");
    m_headers[LAST_CALL_DATE].tooltip = tr("Hangup date of the last answered call");
    m_headers[PENALTY].label = tr("Penalty");
    m_headers[PENALTY].tooltip = tr("Queue member's penalty");
}

void QueueMembersModel::updateQueueMemberConfig(const QString &queue_member_id)
{
    if (! m_row2id.contains(queue_member_id)) {
        int insertedRow = m_row2id.size();
        beginInsertRows(QModelIndex(), insertedRow, insertedRow);
        m_row2id.append(queue_member_id);
        endInsertRows();
    } else {
        this->refreshQueueMemberRow(queue_member_id);
    }
}

void QueueMembersModel::removeQueueMemberConfig(const QString &xid)
{
    if (m_row2id.contains(xid)) {
        int removedRow = m_row2id.indexOf(xid);
        removeRow(removedRow);
    }
}

void QueueMembersModel::updateAgentConfig(const QString & agent_id)
{
    QStringList queue_member_ids = QueueMemberDAO::queueMembersFromAgentId(agent_id);
    foreach (QString queue_member_id, queue_member_ids) {
        if (m_row2id.contains(queue_member_id)) {
            this->refreshQueueMemberRow(queue_member_id);
        }
    }
}

bool QueueMembersModel::removeRows(int row, int count, const QModelIndex &)
{
    bool ret = true;
    if (count > 0) {
        beginRemoveRows(QModelIndex(), row, row + count - 1);
        for (int i = 0 ; i < count ; i ++) {
            ret = ret && row < m_row2id.size();
            m_row2id.removeAt(row);
        }
        endRemoveRows();
    }
    return ret;
}

int QueueMembersModel::rowCount(const QModelIndex&) const
{
    return m_row2id.size();
}

int QueueMembersModel::columnCount(const QModelIndex&) const
{
    return NB_COL;
}

QVariant QueueMembersModel::data(const QModelIndex &index, int role) const
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
    default:
        return QVariant();
    }
}

QVariant QueueMembersModel::dataDisplay(int row, int column) const
{
    QString queue_member_id;

    if (m_row2id.size() > row) {
        queue_member_id = m_row2id[row];
    }

    const QueueMemberInfo * queue_member = b_engine->queuemember(queue_member_id);
    if (queue_member == NULL) return QVariant();

    if (queue_member->is_agent())
      return this->agentDataDisplay(row, column, queue_member);
    else
      return this->phoneDataDisplay(column, queue_member);
}

QVariant QueueMembersModel::phoneDataDisplay(int column, const QueueMemberInfo * queue_member) const
{
    PhoneDAO *phone_dao = new PhoneDAOImpl();
    if (! phone_dao) {
        return QVariant();
    }
    const PhoneInfo * phone = phone_dao->findByIdentity(queue_member->interface());
    delete phone_dao;
    if (phone == NULL) return QVariant();
    const UserInfo * user = b_engine->user(phone->xid_user_features());
    if (user == NULL) return QVariant();

    switch(column) {
    case ID:
        return queue_member->xid();
    case NUMBER:
        return phone->number();
    case FIRSTNAME:
        return user->firstname();
    case LASTNAME:
        return user->lastname();
    case ANSWERED_CALLS:
        return queue_member->callstaken();
    case LAST_CALL_DATE:
        return queue_member->lastcall();
    case PENALTY:
        return queue_member->penalty();
    case LOGGED:
        return tr("Always");
    case PAUSED:
        return tr("Never");
    default:
        return this->not_available;
    }
}

QVariant QueueMembersModel::agentDataDisplay(int row, int column, const QueueMemberInfo * queue_member) const
{
    QString agent_id = QueueMemberDAO::agentIdFromAgentNumber(queue_member->agentNumber());
    const AgentInfo * agent = b_engine->agent(agent_id);
    if (agent == NULL) return QVariant();

    QueueAgentStatus agent_status = this->getAgentStatus(row);

    switch (column) {
    case ID :
        return queue_member->xid();
    case NUMBER :
        return queue_member->agentNumber();
    case FIRSTNAME :
        return agent->firstname();
    case LASTNAME :
        return agent->lastname();
    case LOGGED :
        return agent_status.display_status_logged();
    case PAUSED :
        return agent_status.display_status_paused();
    case ANSWERED_CALLS:
        return queue_member->callstaken();
    case LAST_CALL_DATE:
        return queue_member->lastcall();
    case PENALTY:
        return queue_member->penalty();
    default :
        return this->not_available;
    }
}

QVariant QueueMembersModel::dataBackground(int row, int column) const
{
    QString queue_member_id;

    if (m_row2id.size() > row) {
        queue_member_id = m_row2id[row];
    }

    const QueueMemberInfo * queue_member = b_engine->queuemember(queue_member_id);
    if (queue_member == NULL) return QVariant();

    if (queue_member->is_agent())
      return this->agentDataBackground(row, column);
    else
      return this->phoneDataBackground();
}

QVariant QueueMembersModel::agentDataBackground(int row, int column) const
{
    QueueAgentStatus agent_status = this->getAgentStatus(row);
    QColor agent_status_color = agent_status.display_status_color();

    switch (column) {
    case LOGGED:
        return agent_status_color;
    default:
        return QVariant();
    }
}

QVariant QueueMembersModel::phoneDataBackground() const
{
    return QVariant();
}

QVariant QueueMembersModel::dataTooltip(int row, int column) const
{
    QString queue_member_id;
    if (m_row2id.size() > row) {
        queue_member_id = m_row2id[row];
    }
    const QueueMemberInfo * queue_member = b_engine->queuemember(queue_member_id);
    if (queue_member == NULL)
        return QVariant();

    QueueAgentStatus agent_status = this->getAgentStatus(row);
    QString agent_status_tooltip = agent_status.display_status_membership();
    switch (column) {
    case NUMBER:
        return queue_member->agent_or_phone();
    case LOGGED:
        return agent_status_tooltip;
    default:
        return QVariant();
    }

}

QueueAgentStatus QueueMembersModel::getAgentStatus(int row) const
{
    QString queue_member_id;
    if (m_row2id.size() > row) {
        queue_member_id = m_row2id[row];
    }

    const QueueMemberInfo * queue_member = b_engine->queuemember(queue_member_id);
    if (queue_member == NULL) {
        return QueueAgentStatus();
    }

    return QueueMemberDAO::getAgentStatus(queue_member);
}

QVariant QueueMembersModel::headerData(int index,
                                 Qt::Orientation orientation,
                                 int role) const
{
    if (orientation != Qt::Horizontal) {
        return QVariant();
    }

    switch(role) {
    case Qt::DisplayRole :
        return m_headers[index].label;
    case Qt::ToolTipRole :
        return m_headers[index].tooltip;
    default :
        return QVariant();
    }
}

void QueueMembersModel::refreshQueueMemberRow(const QString &queue_member_id)
{
    unsigned first_column_index = 0;
    unsigned last_column_index = NB_COL - 1;
    unsigned queue_member_row_id = m_row2id.indexOf(queue_member_id);
    QModelIndex cell_changed_start = createIndex(queue_member_row_id, first_column_index);
    QModelIndex cell_changed_end = createIndex(queue_member_row_id, last_column_index);
    emit dataChanged(cell_changed_start, cell_changed_end);
}

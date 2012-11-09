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

AgentsModel::AgentsModel(QObject *parent)
    : QAbstractTableModel(parent)
{
  m_headers[ID] = "ID";
  m_headers[NUMBER] = tr("Number");
  m_headers[LASTNAME] = tr("Last name");
  m_headers[FIRSTNAME] = tr("First name");
  m_headers[SPY_BUTTON] = "";
  m_headers[SPY_STATUS] = tr("Listen");
  m_headers[ONLINE] = tr("On Line");
  m_headers[PRESENCE] = tr("Presence");
  m_headers[LOGGED_STATUS] = tr("Logged");
  m_headers[JOINED_QUEUES] = tr("Joined\nqueues");
  m_headers[PAUSE_STATUS] = tr("Paused");
  m_headers[PAUSE_BUTTON] = "";
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

bool AgentsModel::removeRows(int row, int count, const QModelIndex &)
{
    return true;
}

void AgentsModel::updateAgentConfig(const QString &xid)
{
    if (! m_row2id.contains(xid)) {
        int insertedRow = m_row2id.size();
        beginInsertRows(QModelIndex(), insertedRow, insertedRow);
        m_row2id.append(xid);
        endInsertRows();

    } else {
        QModelIndex firstCell = createIndex(m_row2id.indexOf(xid), ID);
        QModelIndex lastCell = createIndex(m_row2id.indexOf(xid), PAUSED_QUEUES);
        // sends signal to proxy/view that the data should be refreshed
        emit dataChanged(firstCell, lastCell);
    }
}

void AgentsModel::removeAgentConfig(const QString &xid)
{
    if (m_row2id.contains(xid)) {
        int removedRow = m_row2id.indexOf(xid);
        removeRow(removedRow); // calls removeRows
    }
}

void AgentsModel::updateAgentStatus(const QString &xid)
{
  if (!m_row2id.contains(xid)) {
      return;
  }

  const AgentInfo * agentinfo = b_engine->agent(xid);
  if (agentinfo == NULL) return;

  QModelIndex firstCell = createIndex(m_row2id.indexOf(xid), ID);
  QModelIndex lastCell = createIndex(m_row2id.indexOf(xid), PAUSED_QUEUES);
  // sends signal to proxy/view that the data should be refreshed
  emit dataChanged(firstCell, lastCell);
}

void AgentsModel::updateAgentListenStatus(const QString & ipbxid, const QString & xagentid, const QString & status)
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
    QString xagentid;

    if (m_row2id.size() > row) {
        xagentid = m_row2id[row];
    }

    const AgentInfo * agentinfo = b_engine->agent(xagentid);
    if (agentinfo == NULL) return QVariant();


    QStringList joined_queues;
    QStringList paused_queues;

    foreach (QString xqueueid, agentinfo->xqueueids()) {
        const QueueInfo * queueinfo = b_engine->queue(xqueueid);
        if (queueinfo == NULL)
            continue;
        QString xqueuemember = queueinfo->reference("agents", xagentid);
        const QueueMemberInfo * qmi = b_engine->queuemembers().value(xqueuemember);
        if (qmi == NULL)
            continue;
        joined_queues << queueinfo->queueName();
        if (qmi->paused() == "1")
            paused_queues << queueinfo->queueName();
    }

    int njoined = joined_queues.size();
    int npaused = paused_queues.size();



    // Data

    QString not_available = tr("N/A");
    if (role == Qt::DisplayRole) {
        switch (col) {
            case ID :
                return xagentid;
            case NUMBER :
                return agentinfo->agentNumber();
            case LASTNAME :
                return agentinfo->lastname();
            case FIRSTNAME :
                return agentinfo->firstname();
            case JOINED_QUEUES :
                return njoined;
            case PAUSED_QUEUES :
                return npaused;
            case LOGGED_STATUS:
                return agentinfo->status();
            default :
                return not_available;
        }
    }

    return QVariant();
}

int AgentsModel::getNumberOfJoinedQueues(const QString &agent_xid)
{
    return (QueueMemberDAO::queueListFromAgentId(agent_xid).size());
}

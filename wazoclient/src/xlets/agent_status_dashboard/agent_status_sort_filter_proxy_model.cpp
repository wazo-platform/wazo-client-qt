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

#include <QDebug>
#include <xletlib/agents_model.h>

#include "agent_status_sort_filter_proxy_model.h"

AgentStatusSortFilterProxyModel::AgentStatusSortFilterProxyModel(const QString &queue_id, QObject *parent)
    : AbstractSortFilterProxyModel(parent)
{
    m_queue_id = queue_id;
}

AgentStatusSortFilterProxyModel::~AgentStatusSortFilterProxyModel()
{
}

bool AgentStatusSortFilterProxyModel::filterAcceptsRow(int source_row, const QModelIndex & source_parent) const
{
    return this->agentInQueue(source_row, source_parent)
        && this->agentLogged(source_row, source_parent);
}

bool AgentStatusSortFilterProxyModel::agentInQueue(int source_row, const QModelIndex & source_parent) const
{
    QModelIndex agent_model_index = this->sourceModel()->index(source_row, AgentsModel::JOINED_QUEUE_LIST, source_parent);
    QVariantList queue_list = agent_model_index.data().toList();
    return queue_list.contains(this->m_queue_id);
}

bool AgentStatusSortFilterProxyModel::agentLogged(int source_row, const QModelIndex & source_parent) const
{
    QModelIndex agent_model_index = this->sourceModel()->index(source_row, AgentsModel::LOGGED_STATUS, source_parent);
    bool agent_logged = agent_model_index.data(Qt::UserRole).toBool();
    return agent_logged;
}

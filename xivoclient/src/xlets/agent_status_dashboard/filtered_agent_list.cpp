/* Copyright (C) 2007-2013, Avencall
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

#include <QListView>

#include <baseengine.h>
#include <xletlib/agents_model.h>
#include <storage/queueinfo.h>

#include "agent_status_sort_filter_proxy_model.h"
#include "filtered_agent_list.h"

FilteredAgentList::FilteredAgentList(QString queue_id, AgentsModel * model, AgentStatusDelegate * delegate)
{
    this->m_queue_id = queue_id;

    this->m_sort_filter_proxy_model = new AgentStatusSortFilterProxyModel(this->m_queue_id, this);
    this->m_sort_filter_proxy_model->setSourceModel(model);

    QListView * agent_list_view = new QListView(this);
    agent_list_view->setObjectName("AgentListView");
    agent_list_view->setModel(this->m_sort_filter_proxy_model);
    agent_list_view->setModelColumn(AgentsModel::AVAILABILITY);
    agent_list_view->setItemDelegate((QAbstractItemDelegate*) delegate);
    agent_list_view->setViewMode(QListView::IconMode);
    agent_list_view->setSpacing(3);
    agent_list_view->setResizeMode(QListView::Adjust);
}

FilteredAgentList::~FilteredAgentList()
{
}

QString FilteredAgentList::getQueueName()
{
    const QueueInfo * queue = b_engine->queue(this->m_queue_id);
    if (queue == NULL) {
        return "N/A - Yet Unknown";
    }
    return queue->queueDisplayName();
}

QListView * FilteredAgentList::getView()
{
    return this->findChild<QListView *>("AgentListView");
}

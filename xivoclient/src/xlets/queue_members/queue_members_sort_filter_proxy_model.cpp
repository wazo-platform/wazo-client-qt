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

#include <QDebug>

#include <baseengine.h>
#include <dao/queuememberdao.h>
#include <storage/queuememberinfo.h>

#include "queue_members_sort_filter_proxy_model.h"
#include "queue_members_model.h"

QueueMembersSortFilterProxyModel::QueueMembersSortFilterProxyModel(QObject *parent)
    : AbstractSortFilterProxyModel(parent), m_current_queue_id("")
{
    connect(b_engine, SIGNAL(changeWatchedQueueSignal(const QString &)),
            this, SLOT(changeWatchedQueue(const QString &)));
    connect(b_engine, SIGNAL(settingsChanged()),
            this, SLOT(settingsChanged()));
}

void QueueMembersSortFilterProxyModel::changeWatchedQueue(const QString & queue_id)
{
    m_current_queue_id = queue_id;
    this->invalidateFilter();
}

void QueueMembersSortFilterProxyModel::settingsChanged()
{
    this->invalidateFilter();
}

bool QueueMembersSortFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    if (m_current_queue_id == "")
        return false;
    QModelIndex queue_member_id_index = sourceModel()->index(sourceRow,
                                                             QueueMembersModel::ID,
                                                             sourceParent);
    QString queue_member_id = sourceModel()->data(queue_member_id_index).toString();
    const QueueMemberInfo * queue_member = b_engine->queuemember(queue_member_id);
    if (queue_member == NULL) {
        qDebug() << Q_FUNC_INFO << queue_member_id;
        return false;
    }

    if (! this->isMemberOfThisQueue(queue_member)) return false;

    if (queue_member->is_agent() && this->hideUnloggedAgents()) {
        if (! this->isLogged(queue_member)) {
            return false;
        }
    }
    return true;
}

bool QueueMembersSortFilterProxyModel::isLogged(const QueueMemberInfo *queue_member) const
{
    QueueAgentStatus agent_status = QueueMemberDAO::getAgentStatus(queue_member);
    return agent_status.is_logged();
}

bool QueueMembersSortFilterProxyModel::hideUnloggedAgents() const
{
    return b_engine->getConfig("guioptions.queue_members_hide_unlogged_agents").toBool();
}

bool QueueMembersSortFilterProxyModel::isMemberOfThisQueue(const QueueMemberInfo * queue_member) const
{
    QString queue_name = queue_member->queueName();
    QString queue_id = QueueMemberDAO::queueIdFromQueueName(queue_name);

    return m_current_queue_id == queue_id;
}

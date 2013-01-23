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

#include <QDebug>
#include <QStringList>
#include <QSet>

#include <baseengine.h>
#include <storage/queue_agent_status.h>
#include <storage/queuememberinfo.h>
#include <storage/agentinfo.h>
#include <storage/queueinfo.h>

#include "queuememberdao.h"

QStringList QueueMemberDAO::queueListFromAgentId(const QString & agent_id)
{
    QStringList ret;
    QString agent_number = agentNumberFromAgentId(agent_id);
    foreach (QString queuemember_xid_config, b_engine->iterover("queuemembers").keys()) {
        const QueueMemberInfo *queuememberinfo_config = b_engine->queuemember(queuemember_xid_config);
        if (queuememberinfo_config != NULL) {
            if (queuememberinfo_config->agentNumber() == agent_number) {
                QString queue_name = queuememberinfo_config->queueName();
                QString queue_xid = queueIdFromQueueName(queue_name);
                ret << queue_xid;
            }
        }
    }
    return ret;
}

QString QueueMemberDAO::queueIdFromQueueName(const QString & queue_name)
{
    foreach(const QString &queue_xid, b_engine->iterover("queues").keys()) {
        const QueueInfo *queueinfo = b_engine->queue(queue_xid);
        if (queueinfo != NULL) {
            if (queueinfo->queueName() == queue_name) {
                return queue_xid;
            }
        }
    }
    return "";
}

QString QueueMemberDAO::agentIdFromAgentNumber(const QString & agent_number)
{
    foreach(const QString &agent_id, b_engine->iterover("agents").keys()) {
        const AgentInfo *agentinfo = b_engine->agent(agent_id);
        if (agentinfo != NULL) {
            if (agentinfo->agentNumber() == agent_number) {
                return agent_id;
            }
        }
    }
    return "";
}

QString QueueMemberDAO::agentNumberFromAgentId(const QString & agent_id)
{
    const AgentInfo * agentinfo = b_engine->agent(agent_id);
    if (agentinfo == NULL) {
        return QString();
    }
    return agentinfo->agentNumber();
}

QString QueueMemberDAO::queueMemberId(const QString & agent_id,
                                      const QString & queue_xid)
{
    const AgentInfo * agentinfo = b_engine->agent(agent_id);
    const QueueInfo * queueinfo = b_engine->queue(queue_xid);
    if (agentinfo != NULL && queueinfo != NULL) {
        QString agent_number = agentinfo->agentNumber();
        QString queue_name = queueinfo->queueName();
        foreach (const QString &queuemember_xid, b_engine->iterover("queuemembers").keys()) {
            const QueueMemberInfo * queuememberinfo = b_engine->queuemember(queuemember_xid);
            if (queuememberinfo->queueName() == queue_name
                && queuememberinfo->agentNumber() == agent_number) {
                return queuemember_xid;
            }
        }
    }
    return "";
}

QStringList QueueMemberDAO::queueMembersFromAgentId(const QString & agent_id)
{
    QStringList ret;
    QString agent_number = agentNumberFromAgentId(agent_id);
    foreach (const QString & queue_member_id, b_engine->iterover("queuemembers").keys()) {
        const QueueMemberInfo * queue_member = b_engine->queuemember(queue_member_id);
        if (queue_member->agentNumber() == agent_number) {
            ret << queue_member_id;
        }
    }
    return ret;
}

QueueAgentStatus QueueMemberDAO::getAgentStatus(const QueueMemberInfo *queue_member)
{
    QueueAgentStatus agent_status;
    QString membership = queue_member->membership();
    QString status = queue_member->status();
    QString paused = queue_member->paused();

    agent_status.update(membership, status, paused);

    return agent_status;
}

int QueueMemberDAO::nbAgentsFromQueue(const QueueInfo * queue)
{
    int nb_of_agents = 0;

    QHash<QString, XInfo *> queue_members = b_engine->iterover("queuemembers");
    foreach (XInfo * info, queue_members.values()) {
        QueueMemberInfo * queue_member = static_cast<QueueMemberInfo *>(info);
        if ((queue_member->queueName() == queue->queueName()) && (queue_member->is_agent())) {
            ++nb_of_agents;
        }
    }

    return nb_of_agents;
}

int QueueMemberDAO::nbNonAgentsFromQueue(const QueueInfo * queue)
{
    int nb_of_non_agents = 0;

    QHash<QString, XInfo *> queue_members = b_engine->iterover("queuemembers");
    foreach (XInfo * info, queue_members.values()) {
        QueueMemberInfo * queue_member = static_cast<QueueMemberInfo *>(info);
        if ((queue_member->queueName() == queue->queueName()) && (!queue_member->is_agent())) {
            ++nb_of_non_agents;
        }
    }

    return nb_of_non_agents;
}

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

#include "baseengine.h"
#include "queuememberdao.h"

QStringList uniquify(QStringList list)
{
    QStringList ret = list.toSet().toList();
    return ret;
}

QStringList QueueMemberDAO::queueListFromAgentId(const QString & agent_xid)
{
    QStringList ret;
    const AgentInfo * agentinfo = b_engine->agent(agent_xid);
    if (agentinfo == NULL) {
        return QStringList();
    }
    QString agent_number = agentinfo->agentNumber();
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
    foreach (QString queue_xid, agentinfo->xqueueids()) {
        const QueueInfo * queueinfo = b_engine->queue(queue_xid);
        QString queuemember_xid_status = queueinfo->reference("agents", agent_xid);
        const QueueMemberInfo * queuememberinfo_status = b_engine->queuemembers().value(queuemember_xid_status);
        if (queuememberinfo_status != NULL) {
            if (queuememberinfo_status->membership() != "static") {
                ret << queue_xid;
            }
        }
    }
    ret = uniquify(ret);
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
    foreach(const QString &agent_xid, b_engine->iterover("agents").keys()) {
        const AgentInfo *agentinfo = b_engine->agent(agent_xid);
        if (agentinfo != NULL) {
            if (agentinfo->agentNumber() == agent_number) {
                return agent_xid;
            }
        }
    }
    return "";
}

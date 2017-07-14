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

#include "baseengine.h"
#include "queueinfo.h"
#include <dao/queuememberdao.h>
#include <dao/queuedao.h>

#include "agentinfo.h"
#include "queuememberinfo.h"

AgentInfo::AgentInfo(const QString & ipbxid,
                     const QString & id)
    : XInfo(ipbxid, id),
      m_availability_since(0.0)
{
}

bool AgentInfo::updateConfig(const QVariantMap & prop)
{
    bool haschanged = false;
    haschanged |= setIfChangeString(prop, "context", & m_context);
    haschanged |= setIfChangeString(prop, "number", & m_agentnumber);
    haschanged |= setIfChangeString(prop, "firstname", & m_firstname);
    haschanged |= setIfChangeString(prop, "lastname", & m_lastname);

    m_fullname = QString("%1 %2").arg(m_firstname).arg(m_lastname);
    return haschanged;
}

bool AgentInfo::updateStatus(const QVariantMap & prop)
{
    bool haschanged = false;
    haschanged |= setIfChangeString(prop, "availability", & m_availability);
    haschanged |= setIfChangeDouble(prop, "availability_since", & m_availability_since);

    if (prop.contains("queues")) {
        m_queue_ids.clear();
        foreach (QString queueid, prop.value("queues").toStringList()) {
            QString xqueueid = QString("%1/%2").arg(m_ipbxid).arg(queueid);
            m_queue_ids.append(xqueueid);
        }
        haschanged = true;
    }

    return haschanged;
}

const QString & AgentInfo::context() const
{
    return m_context;
}

const QString & AgentInfo::agentNumber() const
{
    return m_agentnumber;
}

const QString & AgentInfo::fullname() const
{
    return m_fullname;
}

const QString & AgentInfo::firstname() const
{
    return m_firstname;
}

const QString & AgentInfo::lastname() const
{
    return m_lastname;
}

const QVariantMap & AgentInfo::properties() const
{
    return m_properties;
}

const QStringList & AgentInfo::queue_ids() const
{
    return m_queue_ids;
}

enum AgentInfo::AgentAvailability AgentInfo::availability() const
{
    if (m_availability == "available") {
        return AVAILABLE;
    } else if (m_availability == "unavailable") {
        return UNAVAILABLE;
    } else if (m_availability == "on_call_nonacd_incoming_internal") {
        return ON_CALL_NONACD_INCOMING_INTERNAL;
    } else if (m_availability == "on_call_nonacd_incoming_external") {
        return ON_CALL_NONACD_INCOMING_EXTERNAL;
    } else if (m_availability == "on_call_nonacd_outgoing_internal") {
        return ON_CALL_NONACD_OUTGOING_INTERNAL;
    } else if (m_availability == "on_call_nonacd_outgoing_external") {
        return ON_CALL_NONACD_OUTGOING_EXTERNAL;
    } else {
        return LOGGED_OUT;
    }
}

QString AgentInfo::availabilitySince() const
{
    QString time_since = b_engine->timeElapsed(m_availability_since);
    return time_since;
}

bool AgentInfo::logged() const
{
    return this->availability() != LOGGED_OUT;
}

bool AgentInfo::paused() const
{
    QStringList queue_members = QueueMemberDAO::queueMembersFromAgentId(this->xid());
    foreach (const QString & queue_member_id, queue_members) {
        const QueueMemberInfo * queue_member = b_engine->queuemember(queue_member_id);
        if (queue_member != NULL && queue_member->paused() == "1") {
            return true;
        }
    }
    return false;
}

int AgentInfo::joinedQueueCount() const
{
    int joined_queues = 0;

    QStringList queue_members = QueueMemberDAO::queueMembersFromAgentId(this->xid());
    joined_queues = queue_members.size();
    return joined_queues;
}

QStringList AgentInfo::joinedQueueNames() const
{
    QStringList queue_ids = QueueMemberDAO::queueListFromAgentId(this->xid());
    QStringList queue_names;
    foreach (const QString &queue_id, queue_ids) {
        const QueueInfo * queue = b_engine->queue(queue_id);
        if (queue != NULL) {
            queue_names << queue->queueDisplayName();
        }
    }
    return queue_names;
}

QStringList AgentInfo::pausedQueueNames() const
{
    QStringList queue_names;
    QStringList queue_members = QueueMemberDAO::queueMembersFromAgentId(this->xid());
    foreach (const QString & queue_member_id, queue_members) {
        const QueueMemberInfo * queue_member = b_engine->queuemember(queue_member_id);
        if (queue_member != NULL && queue_member->paused() == "1") {
            QString queue_name = queue_member->queueName();
            QString display_name = QueueDAO::queueDisplayNameFromQueueName(queue_name);
            queue_names << display_name;
        }
    }
    return queue_names;
}


int AgentInfo::pausedQueueCount() const
{
    int paused_queues = 0;

    QStringList queue_members = QueueMemberDAO::queueMembersFromAgentId(this->xid());
    foreach (const QString & queue_member_id, queue_members) {
        const QueueMemberInfo * queue_member = b_engine->queuemember(queue_member_id);
        if (queue_member != NULL && queue_member->paused() == "1") {
            ++paused_queues;
        }
    }
    return paused_queues;
}

enum AgentPauseStatus AgentInfo::pausedStatus() const
{
    int joined = this->joinedQueueCount();
    int paused = this->pausedQueueCount();
    if (joined == 0) {
        return UNPAUSED;
    } else if (paused == 0) {
        return UNPAUSED;
    } else if (joined == paused) {
        return PAUSED;
    } else {
        return PARTIALLY_PAUSED;
    }
}

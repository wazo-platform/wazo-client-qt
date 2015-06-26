/* XiVO Client
 * Copyright (C) 2007-2015 Avencall
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

#include "message_factory.h"

QVariantMap MessageFactory::subscribeCurrentCalls()
{
    QVariantMap message = MessageFactory::baseMessage("subscribe");
    message["message"] = "current_calls";

    return message;
}

QVariantMap MessageFactory::answer(const QString &unique_id)
{
    QVariantMap message = MessageFactory::baseMessage("answer");
    message["unique_id"] = unique_id;
    return message;
}

QVariantMap MessageFactory::callFormResult(const QVariant& info)
{
    QVariantMap command = MessageFactory::baseMessage("call_form_result");
    command["infos"] = info;
    return command;
}

QVariantMap MessageFactory::dial(const QString &destination)
{
    QVariantMap command = MessageFactory::ipbxcommand("dial");
    command["destination"] = destination;

    return command;
}

QVariantMap MessageFactory::hangup()
{
    return MessageFactory::baseMessage("hangup");
}

QVariantMap MessageFactory::holdSwitchboard(const QString &queue_name)
{
    QVariantMap command = MessageFactory::baseMessage("hold_switchboard");
    command["queue_name"] = queue_name;

    return command;
}

QVariantMap MessageFactory::resumeSwitchboard(const QString &unique_id)
{
    QVariantMap message = MessageFactory::baseMessage("resume_switchboard");
    message["unique_id"] = unique_id;

    return message;
}

QVariantMap MessageFactory::attendedTransfer(const QString &number)
{
    QVariantMap message = MessageFactory::baseMessage("attended_transfer");
    message["number"] = number;

    return message;
}

QVariantMap MessageFactory::directTransfer(const QString &number)
{
    QVariantMap message = MessageFactory::baseMessage("direct_transfer");
    message["number"] = number;

    return message;
}

QVariantMap MessageFactory::completeTransfer()
{
    return MessageFactory::baseMessage("complete_transfer");
}

QVariantMap MessageFactory::cancelTransfer()
{
    return MessageFactory::baseMessage("cancel_transfer");
}


QVariantMap MessageFactory::directorySearch(const QString &pattern)
{
    QVariantMap message = MessageFactory::baseMessage("directory");
    message["pattern"] = pattern;

    return message;
}

QVariantMap MessageFactory::switchboardDirectorySearch(const QString &pattern)
{
    QVariantMap message = MessageFactory::baseMessage("switchboard_directory_search");
    message["pattern"] = pattern;

    return message;
}

QVariantMap MessageFactory::peopleSearch(const QString &pattern)
{
    QVariantMap message = MessageFactory::baseMessage("people_search");
    message["pattern"] = pattern;

    return message;
}

QVariantMap MessageFactory::registerAgentStatus(const QVariantList &ids)
{
    QVariantMap message = MessageFactory::baseMessage("register_agent_status_update");

    message["agent_ids"] = ids;

    return message;
}

QVariantMap MessageFactory::registerEndpointStatus(const QVariantList &ids)
{
    QVariantMap message = MessageFactory::baseMessage("register_endpoint_status_update");

    message["endpoint_ids"] = ids;

    return message;
}

QVariantMap MessageFactory::registerUserStatus(const QVariantList &ids)
{
    QVariantMap message = MessageFactory::baseMessage("register_user_status_update");

    message["user_ids"] = ids;

    return message;
}

QVariantMap MessageFactory::getSwitchboardDirectoryHeaders()
{
    QVariantMap message = MessageFactory::baseMessage("get_switchboard_directory_headers");

    return message;
}

QVariantMap MessageFactory::getPeopleHeaders()
{
    QVariantMap message = MessageFactory::baseMessage("people_headers");

    return message;
}

QVariantMap MessageFactory::pauseAgentInQueue(const QString &agent_id, const QString &queue_id)
{
    QVariantMap message = MessageFactory::ipbxcommand("queuepause");
    message["member"] = QString("agent:%0").arg(agent_id);
    message["queue"] = QString("queue:%0").arg(queue_id);

    return message;
}

QVariantMap MessageFactory::unpauseAgentInQueue(const QString &agent_id, const QString &queue_id)
{
    QVariantMap message = MessageFactory::ipbxcommand("queueunpause");
    message["member"] = QString("agent:%0").arg(agent_id);
    message["queue"] = QString("queue:%0").arg(queue_id);

    return message;
}

QVariantMap MessageFactory::pauseAgentInAllQueues(const QString &agent_id, const QString &ipbxid)
{
    QVariantMap message = MessageFactory::ipbxcommand("queuepause");
    message["member"] = QString("agent:%0").arg(agent_id);
    message["queue"] = QString("queue:%0/all").arg(ipbxid);

    return message;
}

QVariantMap MessageFactory::unpauseAgentInAllQueues(const QString &agent_id, const QString &ipbxid)
{
    QVariantMap message = MessageFactory::ipbxcommand("queueunpause");
    message["member"] = QString("agent:%0").arg(agent_id);
    message["queue"] = QString("queue:%0/all").arg(ipbxid);

    return message;
}

QVariantMap MessageFactory::setUnconditionalForward(bool enabled, QString destination)
{
    QVariantMap command, value;
    command["class"] = "featuresput";
    command["function"] = "fwd";
    value["enableunc"] = enabled;
    value["destunc"] = destination;
    command["value"] = value;

    return command;
}

QVariantMap MessageFactory::setForwardNoAnswer(bool enabled, QString destination)
{
    QVariantMap command, value;
    command["class"] = "featuresput";
    command["function"] = "fwd";
    value["enablerna"] = enabled;
    value["destrna"] = destination;
    command["value"] = value;

    return command;
}

QVariantMap MessageFactory::setForwardBusy(bool enabled, QString destination)
{
    QVariantMap command, value;
    command["class"] = "featuresput";
    command["function"] = "fwd";
    value["enablebusy"] = enabled;
    value["destbusy"] = destination;
    command["value"] = value;

    return command;
}

QVariantMap MessageFactory::setVoicemail(bool enabled)
{
    QVariantMap command = MessageFactory::baseMessage("featuresput");
    command["function"] = "enablevoicemail";
    command["value"] = enabled;

    return command;
}

QVariantMap MessageFactory::setCallFiltering(bool enabled)
{
    QVariantMap command = MessageFactory::baseMessage("featuresput");
    command["function"] = "incallfilter";
    command["value"] = enabled;

    return command;
}

QVariantMap MessageFactory::setDoNotDisturb(bool enabled)
{
    QVariantMap command = MessageFactory::baseMessage("featuresput");
    command["function"] = "enablednd";
    command["value"] = enabled;

    return command;
}

QVariantMap MessageFactory::setFavoriteStatus(const QString &source_desc, const QString &source_id, bool enabled)
{
    QVariantMap command = MessageFactory::baseMessage("people_set_favorite");
    command["directory"] = source_desc;
    command["contact_id"] = source_id;
    command["status"] = enabled;

    return command;
}

QVariantMap MessageFactory::baseMessage(const QString &class_name)
{
    QVariantMap message;
    message["class"] = class_name;
    return message;
}

QVariantMap MessageFactory::ipbxcommand(const QString &action)
{
    QVariantMap message = MessageFactory::baseMessage("ipbxcommand");
    message["command"] = action;
    return message;
}

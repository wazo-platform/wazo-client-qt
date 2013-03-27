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

#ifdef FUNCTESTS

#include "xlets/agents/agents.h"
#include "xletlib/agents_model.h"
#include "remote_control.h"

QAbstractItemModel* RemoteControl::getAgentListModel()
{

    qDebug() << "xlet list" << m_exec_obj.win->m_xletlist;
    XletAgents* agents_xlet = static_cast<XletAgents*>(m_exec_obj.win->m_xletlist.value("agents"));

    if (agents_xlet == NULL) {
        this->assert(false, "agent list xlet was not found");
    }

    QAbstractItemModel* agents_model = agents_xlet->findChild<QAbstractItemModel*>("agents_model");

    if (agents_model == NULL) {
        this->assert(false, "agent model was not found");
    }

    return agents_model;
}

QString RemoteControl::getStatusForAgent(QString agent_number)
{
    QAbstractItemModel* agents_model = this->getAgentListModel();

    int row = this->findRowWithItem(agents_model, AgentsModel::NUMBER, agent_number);

    if (row == -1) {
        this->assert(false, QString("agent %1 was not found in agent list").arg(agent_number));
    }

    QString status = this->getValueInModel(agents_model, row, AgentsModel::AVAILABILITY);
    return status;
}

void RemoteControl::then_the_agent_list_xlet_shows_agent_as_in_use(const QVariantList &args)
{
    QString agent_number = args[0].toString();

    QString status = getStatusForAgent(agent_number);

    this->assert(status.startsWith(tr("In use")), QString("agent status for %1 is not 'In use' (got '%2')")
            .arg(agent_number, status));
}


void RemoteControl::then_the_agent_list_xlet_shows_agent_as_not_in_use(const QVariantList &args)
{
    QString agent_number = args[0].toString();

    QString status = getStatusForAgent(agent_number);

    this->assert(status.startsWith(tr("Not in use")), QString("agent status for %1 is not 'Not in use' (got '%2')")
            .arg(agent_number, status));
}

void RemoteControl::then_the_agent_list_xlet_shows_agent_as_unlogged(const QVariantList &args)
{
    QString agent_number = args[0].toString();

    QString status = this->getStatusForAgent(agent_number);
    this->assert(status == "-", QString("agent status for %1 is not '-' (got '%2')")
            .arg(agent_number, status));

}

#endif

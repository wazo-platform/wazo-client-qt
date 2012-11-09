/* Copyright (C) 2007-2012, Avencall
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

#include <QVBoxLayout>

#include <baseengine.h>

#include "agents.h"
#include "agentstatusdelegate.h"
#include "agentsview.h"

Q_EXPORT_PLUGIN2(xletagentsplugin, XLetAgentsPlugin);

XLet* XLetAgentsPlugin::newXLetInstance(QWidget *parent)
{
    b_engine->registerTranslation(":/obj/agents_%1");
    return new XletAgents(parent);
}

XletAgents::XletAgents(QWidget *parent)
    : XLet(parent)
{
    setTitle(tr("Agents' List (plain)"));

    QVBoxLayout *xletLayout = new QVBoxLayout();
    setLayout(xletLayout);
    xletLayout->setSpacing(0);

    m_model = new AgentsModel(this);

    AgentsView *view = new AgentsView(this);
    view->setModel(m_model);
    view->hideColumn(AgentsModel::ID);
    view->sortByColumn(AgentsModel::LASTNAME, Qt::AscendingOrder);
    view->setItemDelegateForColumn(AgentsModel::LOGGED_STATUS, new AgentStatusDelegate());

    xletLayout->addWidget(view);

}


// if (action == "changeagent") {
//     b_engine->changeWatchedAgent(xagentid, true);
// }

// else if (action == "loginoff") {
//     QString status = agentinfo->status();
//     ipbxcommand["agentids"] = xagentid;
//     if (status == "AGENT_IDLE")
//         ipbxcommand["command"] = "agentlogout";
//     else if (status == "AGENT_ONCALL")
//         ipbxcommand["command"] = "agentlogout";
//     else if (status == "AGENT_LOGGEDOFF")
//         ipbxcommand["command"] = "agentlogin";
//     else
//         b_engine->logClient("warning", "XletAgents::agentClicked",
//                             QString("agentid %1 action %2 status %3")
//                             .arg(xagentid).arg(action).arg(status));
// } else if (action == "unpause") {
//     ipbxcommand["command"] = "queuepause";
//     ipbxcommand["member"] = QString("agent:%0").arg(xagentid);
//     ipbxcommand["queue"] = QString("queue:%1/all").arg(ipbxid);
// } else if (action == "pause") {
//     ipbxcommand["command"] = "queueunpause";
//     ipbxcommand["member"] = QString("agent:%0").arg(xagentid);
//     ipbxcommand["queue"] = QString("queue:%1/all").arg(ipbxid);
// } else if (action.endsWith("listen")) {
//     ipbxcommand["command"] = action;
//     ipbxcommand["subcommand"] = action == "listen" ? "start" : "stop";
//     ipbxcommand["destination"] = xagentid;
// }
// emit ipbxCommand(ipbxcommand);

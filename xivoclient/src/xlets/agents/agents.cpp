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

/* $Revision$
 * $Date$
 */

#include "agents.h"
#include "agentstatusdelegate.h"

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
    // Model
    m_model = new AgentsModel(this);

    AgentsView *view = new AgentsView(this);
    view->setModel(m_model);
    view->hideColumn(AgentsModel::ID);
    view->sortByColumn(AgentsModel::LASTNAME, Qt::AscendingOrder);
    view->setItemDelegateForColumn(AgentsModel::LOGGED_STATUS, new AgentStatusDelegate());

    // Layout
    xletLayout->addWidget(view);

}

/*! \brief set font
 */
void XletAgents::setGuiOptions()
{
    QVariantMap optionsMap = b_engine->getConfig();
    if (optionsMap.contains("guioptions.xlet.agents.fontname") &&
        optionsMap.contains("guioptions.xlet.agents.fontsize"))
            m_gui_font = QFont(b_engine->getConfig("guioptions.xlet.agents.fontname").toString(),
                               b_engine->getConfig("guioptions.xlet.agents.fontsize").toInt());
    if (optionsMap.contains("guioptions.xlet.agents.iconsize"))
        m_gui_buttonsize = b_engine->getConfig("guioptions.xlet.agents.iconsize").toInt();

    m_title_agent->setFont(m_gui_font);
    m_title_listen->setFont(m_gui_font);
    m_title_online->setFont(m_gui_font);
    m_title_presence->setFont(m_gui_font);
    m_title_logged->setFont(m_gui_font);
    m_title_njoined->setFont(m_gui_font);
    m_title_paused->setFont(m_gui_font);
    m_title_npaused->setFont(m_gui_font);
}

void XletAgents::updateAgentConfig(const QString & xagentid)
{
    const AgentInfo * agentinfo = b_engine->agent(xagentid);
    if (agentinfo == NULL)
        return;

    bool newagentid = false;

    if (! m_agent_labels.contains(xagentid)) {
        newAgentLine(xagentid);
        newagentid = true;
    }
    updateAgentLineAdmin(xagentid);
    updateAgentLineEvent(xagentid);
    if (newagentid)
        displayLine(xagentid, m_agent_labels.size());

    updateAgentDisplay(xagentid);
}

void XletAgents::updateAgentStatus(const QString & xagentid)
{
    updateAgentDisplay(xagentid);
}

void XletAgents::newAgentLine(const QString & xagentid)
{
    m_agent_labels[xagentid] = new QLabel(this);
    m_agent_more[xagentid] = new QPushButton(this);
    connect(m_agent_more[xagentid], SIGNAL(clicked()),
             this, SLOT(agentClicked()));
    m_agent_listen[xagentid] = new QPushButton(this);
    connect(m_agent_listen[xagentid], SIGNAL(clicked()),
             this, SLOT(agentClicked()));
    m_agent_logged_action[xagentid] = new QPushButton(this);
    connect(m_agent_logged_action[xagentid], SIGNAL(clicked()),
             this, SLOT(agentClicked()));
    m_agent_paused_action[xagentid] = new QPushButton(this);
    connect(m_agent_paused_action[xagentid], SIGNAL(clicked()),
             this, SLOT(agentClicked()));

    m_agent_busy[xagentid] = new QLabel(this);
    m_agent_presence[xagentid] = new QLabel(this);
    m_agent_logged_status[xagentid] = new QLabel(this);
    m_agent_joined_number[xagentid] = new QLabel(this);
    // m_agent_joined_status[xagentid] = new QLabel(this);
    m_agent_paused_number[xagentid] = new QLabel(this);
    m_agent_paused_status[xagentid] = new QLabel(this);
}

// update according to admin-defined parameters
void XletAgents::updateAgentLineAdmin(const QString & xagentid)
{
    const AgentInfo * agentinfo = b_engine->agent(xagentid);
    if (agentinfo == NULL)
        return;

    m_agent_labels[xagentid]->setText(QString("%1 (%2)")
                                      .arg(agentinfo->fullname())
                                      .arg(agentinfo->agentNumber()));
    m_agent_labels[xagentid]->setToolTip(tr("Server: %1\n"
                                            "Context: %2")
                                         .arg(agentinfo->ipbxid())
                                         .arg(agentinfo->context()));
    m_agent_more[xagentid]->setProperty("xagentid", xagentid);
    m_agent_more[xagentid]->setProperty("action", "changeagent");
    m_agent_listen[xagentid]->setProperty("xagentid", xagentid);
    m_agent_busy[xagentid]->setProperty("xagentid", xagentid);
    m_agent_logged_action[xagentid]->setProperty("xagentid", xagentid);
    m_agent_paused_action[xagentid]->setProperty("xagentid", xagentid);
}

// update according to misc parameters
void XletAgents::updateAgentLineEvent(const QString & xagentid)
{
    m_agent_listen[xagentid]->setProperty("action", "listen");
    m_agent_logged_action[xagentid]->setProperty("action", "loginoff");
    m_agent_paused_action[xagentid]->setProperty("action", "pause");
}

void XletAgents::displayLine(const QString & xagentid, int linenum)
{
    QFrame * qvline1 = new QFrame(this);
    qvline1->setFrameShape(QFrame::VLine);
    qvline1->setLineWidth(1);
    QFrame * qvline2 = new QFrame(this);
    qvline2->setFrameShape(QFrame::VLine);
    qvline2->setLineWidth(1);
    QFrame * qvline3 = new QFrame(this);
    qvline3->setFrameShape(QFrame::VLine);
    qvline3->setLineWidth(1);

    QPixmap square(m_gui_buttonsize, m_gui_buttonsize);
    square.fill(Qt::gray);
    m_agent_presence[xagentid]->setPixmap(square);

    m_agent_more[xagentid]->setIconSize(QSize(m_gui_buttonsize, m_gui_buttonsize));
    m_agent_more[xagentid]->setIcon(QIcon(":/images/add.png"));
    m_agent_listen[xagentid]->setIconSize(QSize(m_gui_buttonsize, m_gui_buttonsize));
    m_agent_listen[xagentid]->setIcon(QIcon(":/images/player_play.png"));
    m_agent_logged_action[xagentid]->setIconSize(QSize(m_gui_buttonsize, m_gui_buttonsize));
    m_agent_paused_action[xagentid]->setIconSize(QSize(m_gui_buttonsize, m_gui_buttonsize));

    int colnum = 0;
    m_gridlayout->addWidget(m_agent_labels[xagentid], linenum, colnum++, Qt::AlignLeft);
    m_gridlayout->addWidget(m_agent_more[xagentid], linenum, colnum++, Qt::AlignCenter);
    m_gridlayout->addWidget(m_agent_listen[xagentid], linenum, colnum++, Qt::AlignCenter);
    m_gridlayout->addWidget(m_agent_busy[xagentid], linenum, colnum++, Qt::AlignCenter);
    m_gridlayout->addWidget(qvline1, linenum, colnum++, Qt::AlignHCenter);
    m_gridlayout->addWidget(m_agent_presence[xagentid], linenum, colnum++, Qt::AlignCenter);
    m_gridlayout->addWidget(qvline2, linenum, colnum++, Qt::AlignHCenter);
    m_gridlayout->addWidget(m_agent_logged_status[xagentid], linenum, colnum++, Qt::AlignCenter);
    m_gridlayout->addWidget(m_agent_logged_action[xagentid], linenum, colnum++, Qt::AlignCenter);
    ++colnum;
    m_gridlayout->addWidget(m_agent_joined_number[xagentid], linenum, colnum++, Qt::AlignCenter);
    m_gridlayout->addWidget(qvline3, linenum, colnum++, Qt::AlignHCenter);
    m_gridlayout->addWidget(m_agent_paused_status[xagentid], linenum, colnum++, Qt::AlignCenter);
    m_gridlayout->addWidget(m_agent_paused_action[xagentid], linenum, colnum++, Qt::AlignCenter);
    m_gridlayout->addWidget(m_agent_paused_number[xagentid], linenum, colnum++, Qt::AlignCenter);
}

void XletAgents::updateAgentDisplay(const QString & xagentid)
{
    const AgentInfo * agentinfo = b_engine->agent(xagentid);
    if (agentinfo == NULL)
        return;
    if (! m_agent_busy.contains(xagentid))
        return;

    QString context_agent = agentinfo->context();
    QString agstatus = agentinfo->status();
    QString phonenum = agentinfo->phonenumber();

    QVariantMap slink; // "Xivo-Agent-Status-Link"
    bool link = false;
    if (! slink.isEmpty()) {
        QString linkmode = slink.value("linkmode").toString();
        if ((linkmode == "phonelink") || (linkmode == "agentlink"))
            link = true;
        else
            b_engine->logClient("warning", "XletAgents::updateAgentDisplay",
                                QString("agentid %1 linkmode <%2>")
                                .arg(xagentid).arg(linkmode));
    }

    QPixmap square(m_gui_buttonsize, m_gui_buttonsize);
    if (link) {
        square.fill(Qt::green);
    } else {
        square.fill(Qt::gray);
    }

    m_agent_busy[xagentid]->setPixmap(square);

    QString tooltip;
    if (agstatus == "AGENT_IDLE") {
        square.fill(Qt::green);
        m_agent_logged_action[xagentid]->setIcon(QIcon(":/images/cancel.png"));
        tooltip = tr("Agent logged on %1").arg(phonenum);
    } else if (agstatus == "AGENT_ONCALL") {
        // square.fill(Qt::darkGreen);
        square.fill(Qt::green);
        m_agent_logged_action[xagentid]->setIcon(QIcon(":/images/cancel.png"));
        // tooltip = tr("Agent busy");
        tooltip = tr("Agent logged on %1").arg(phonenum);
    } else if (agstatus == "AGENT_LOGGEDOFF") {
        square.fill(Qt::red);
        m_agent_logged_action[xagentid]->setIcon(QIcon(":/images/button_ok.png"));
        tooltip = tr("Agent NOT logged");
    } else {
        square.fill(Qt::gray);
        m_agent_logged_action[xagentid]->setIcon(QIcon(square));
        tooltip = tr("Unknown %1").arg(agstatus);
        b_engine->logClient("warning", "XletAgents::updateAgentDisplay",
                            QString("agentid %1 agstatus <%2>")
                            .arg(xagentid).arg(agstatus));
    }
    m_agent_logged_status[xagentid]->setPixmap(square);
    m_agent_logged_status[xagentid]->setToolTip(tooltip);

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
    QString sep = ", ";

    m_agent_joined_number[xagentid]->setText(QString::number(njoined));
    m_agent_paused_number[xagentid]->setText(QString::number(npaused));

    if (njoined)
        m_agent_joined_number[xagentid]->setToolTip(tr("Joined queues : %1")
                                                   .arg(joined_queues.join(sep)));
    else
        m_agent_joined_number[xagentid]->setToolTip("");

    if (njoined == 0) {
        m_agent_paused_status[xagentid]->hide();
        m_agent_paused_action[xagentid]->hide();
        m_agent_paused_number[xagentid]->hide();
    } else {
        QString tooltip;
        if (npaused == 0) {
            m_agent_paused_status[xagentid]->setText(tr("No"));
            m_agent_paused_action[xagentid]->setIcon(QIcon(":/images/cancel.png"));
            m_agent_paused_action[xagentid]->setProperty("action", "unpause");
            tooltip = "";
        } else if (npaused == njoined) {
            m_agent_paused_status[xagentid]->setText(tr("Yes"));
            m_agent_paused_action[xagentid]->setIcon(QIcon(":/images/button_ok.png"));
            m_agent_paused_action[xagentid]->setProperty("action", "pause");
            tooltip = tr("Paused queues : %1").arg(paused_queues.join(sep));
        } else {
            m_agent_paused_status[xagentid]->setText(tr("Partial"));
            m_agent_paused_action[xagentid]->setIcon(QIcon(":/images/button_ok.png"));
            m_agent_paused_action[xagentid]->setProperty("action", "pause");
            tooltip = tr("Paused queues : %1").arg(paused_queues.join(sep));
        }
        m_agent_paused_status[xagentid]->setToolTip(tooltip);
        m_agent_paused_number[xagentid]->setToolTip(tooltip);
        m_agent_paused_status[xagentid]->show();
        m_agent_paused_action[xagentid]->show();
        m_agent_paused_number[xagentid]->show();
    }
}

/*! \brief process actions
 */
void XletAgents::agentClicked()
{
    QString xagentid = sender()->property("xagentid").toString();
    const AgentInfo * agentinfo = b_engine->agent(xagentid);
    if (agentinfo == NULL)
        return;
    QString action  = sender()->property("action").toString();

    QString ipbxid = agentinfo->ipbxid();
    QVariantMap ipbxcommand;

    if (action == "changeagent") {
        b_engine->changeWatchedAgent(xagentid, true);
    }

    else if (action == "loginoff") {
        QString status = agentinfo->status();
        ipbxcommand["agentids"] = xagentid;
        if (status == "AGENT_IDLE")
            ipbxcommand["command"] = "agentlogout";
        else if (status == "AGENT_ONCALL")
            ipbxcommand["command"] = "agentlogout";
        else if (status == "AGENT_LOGGEDOFF")
            ipbxcommand["command"] = "agentlogin";
        else
            b_engine->logClient("warning", "XletAgents::agentClicked",
                                QString("agentid %1 action %2 status %3")
                                .arg(xagentid).arg(action).arg(status));
    } else if (action == "unpause") {
        ipbxcommand["command"] = "queuepause";
        ipbxcommand["member"] = QString("agent:%0").arg(xagentid);
        ipbxcommand["queue"] = QString("queue:%1/all").arg(ipbxid);
    } else if (action == "pause") {
        ipbxcommand["command"] = "queueunpause";
        ipbxcommand["member"] = QString("agent:%0").arg(xagentid);
        ipbxcommand["queue"] = QString("queue:%1/all").arg(ipbxid);
    } else if (action.endsWith("listen")) {
        ipbxcommand["command"] = action;
        ipbxcommand["subcommand"] = action == "listen" ? "start" : "stop";
        ipbxcommand["destination"] = xagentid;
    }
    emit ipbxCommand(ipbxcommand);
}

/*! \brief update Listen/Stop Listen buttons
 */
void XletAgents::statusListen(const QString & xagentid, const QString & status)
{
    if (! m_agent_listen.contains(xagentid))
        return;

    if (status == "started") {
        m_agent_listen[xagentid]->setProperty("action", "stoplisten");
        m_agent_listen[xagentid]->setStyleSheet("QPushButton {background: #fbb638}");
    } else if (status == "stopped") {
        m_agent_listen[xagentid]->setProperty("action", "listen");
        m_agent_listen[xagentid]->setStyleSheet("");
    }
}

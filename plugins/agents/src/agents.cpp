/* XiVO Client
 * Copyright (C) 2007-2011, Proformatique
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

#include <agents.h>

Q_EXPORT_PLUGIN2(xletagentsplugin, XLetAgentsPlugin);

XLet* XLetAgentsPlugin::newXLetInstance(QWidget *parent)
{
    b_engine->registerTranslation(":/agents_%1");
    return new XletAgents(parent);
}

XletAgents::XletAgents(QWidget *parent)
    : XLet(parent)
{
    setTitle(tr("Agents' List (plain)"));
    m_gui_buttonsize = 10;

    m_gridlayout = new QGridLayout(this);
    m_title1 = new QLabel(tr("Agent"), this);
    m_title2 = new QLabel(tr("Record"), this);
    m_title3 = new QLabel(tr("Listen"), this);
    m_title4 = new QLabel(tr("On Line"), this);
    m_title5 = new QLabel(tr("Presence"), this);
    m_title_logged  = new QLabel(tr("Logged"), this);
    m_title_njoined = new QLabel(tr("Joined\nqueues"), this);
    m_title_paused  = new QLabel(tr("Paused"), this);
    m_title_npaused = new QLabel(tr("Paused\nqueues"), this);

    m_gridlayout->addWidget(m_title1, 0, 0, 1, 2, Qt::AlignLeft);
    m_gridlayout->addWidget(m_title2, 0, 2, 1, 1, Qt::AlignCenter);
    m_gridlayout->addWidget(m_title3, 0, 3, 1, 1, Qt::AlignCenter);
    m_gridlayout->addWidget(m_title4, 0, 4, 1, 1, Qt::AlignCenter);
    m_gridlayout->addWidget(m_title5, 0, 6, 1, 1, Qt::AlignCenter);
    m_gridlayout->addWidget(m_title_logged, 0, 8, 1, 2, Qt::AlignCenter);
    m_gridlayout->addWidget(m_title_njoined, 0, 10, 1, 1, Qt::AlignCenter);
    m_gridlayout->addWidget(m_title_paused, 0, 12, 1, 2, Qt::AlignCenter);
    m_gridlayout->addWidget(m_title_npaused, 0, 14, 1, 1, Qt::AlignCenter);
    m_gridlayout->setColumnStretch(15, 1);
    m_gridlayout->setRowStretch(100, 1);
    m_gridlayout->setVerticalSpacing(0);

    setGuiOptions(b_engine->getGuiOptions("merged_gui"));
    // connect signals/slots with engine
    connect(b_engine, SIGNAL(newAgentList(const QStringList &)),
            this, SLOT(newAgentList(const QStringList &)));
    connect(b_engine, SIGNAL(newQueueList(const QStringList &)),
            this, SLOT(newQueueList(const QStringList &)));
    connect(this, SIGNAL(changeWatchedAgent(const QString &, bool)),
            b_engine, SLOT(changeWatchedAgentSlot(const QString &, bool)));
    connect(b_engine, SIGNAL(updateAgentPresence(const QString &, const QString &, const QVariant &)),
            this, SLOT(updateAgentPresence(const QString &, const QString &, const QVariant &)));
    connect(b_engine, SIGNAL(statusRecord(const QString &, const QString &, const QString &)),
            this, SLOT(statusRecord(const QString &, const QString &, const QString &)));
    connect(b_engine, SIGNAL(statusListen(const QString &, const QString &, const QString &)),
            this, SLOT(statusListen(const QString &, const QString &, const QString &)));

}

/*! \brief set font
 */
void XletAgents::setGuiOptions(const QVariantMap & optionsMap)
{
    if (optionsMap.contains("xlet.agents.fontname") && optionsMap.contains("xlet.agents.fontsize"))
        m_gui_font = QFont(optionsMap.value("xlet.agents.fontname").toString(),
                           optionsMap.value("xlet.agents.fontsize").toInt());
    if (optionsMap.contains("xlet.agents.iconsize"))
        m_gui_buttonsize = optionsMap.value("xlet.agents.iconsize").toInt();

    m_title1->setFont(m_gui_font);
    m_title2->setFont(m_gui_font);
    m_title3->setFont(m_gui_font);
    m_title4->setFont(m_gui_font);
    m_title5->setFont(m_gui_font);
    m_title_logged->setFont(m_gui_font);
    m_title_njoined->setFont(m_gui_font);
    m_title_paused->setFont(m_gui_font);
    m_title_npaused->setFont(m_gui_font);
}

/*! \brief set agent presence status
 */
void XletAgents::updateAgentPresence(const QString & astid, const QString & agent_number, const QVariant & presencestatus)
{
    // qDebug() << Q_FUNC_INFO << astid << agent_number << presencestatus;
    QString agentid = QString("agent:%1/%2").arg(astid).arg(agent_number);
    if (b_engine->agents().contains(agentid))
        if (m_agent_presence.contains(agentid)) {
            QPixmap square(m_gui_buttonsize, m_gui_buttonsize);
            square.fill(QColor(presencestatus.toMap().value("color").toString()));
            m_agent_presence[agentid]->setPixmap(square);
            m_agent_presence[agentid]->setToolTip(presencestatus.toMap().value("longname").toString());
        }
}

/*! \brief slot when one or more queues have been updated
 */
void XletAgents::newQueueList(const QStringList &)
{
    // qDebug() << Q_FUNC_INFO;
}

/*! \brief slot when one or more agents have been updated
 */
void XletAgents::newAgentList(const QStringList & list)
{
    //qDebug() << Q_FUNC_INFO << list;
    QHashIterator<QString, AgentInfo *> iter = QHashIterator<QString, AgentInfo *>(b_engine->agents());
    // qDebug() << Q_FUNC_INFO << b_engine->agents();


    while (iter.hasNext()) {
        iter.next();
        AgentInfo * ainfo = iter.value();
        QString agentid = iter.key();
        if (!list.contains(agentid))
            continue;

        bool newagentid = false;

        if (! m_agent_labels.contains(agentid)) {
            newAgentLine(agentid);
            newagentid = true;
        }
        updateAgentLineAdmin(agentid, ainfo->fullname(), ainfo->agentNumber());
        updateAgentLineEvent(agentid);
        if (newagentid)
            displayLine(agentid, m_agent_labels.size());
        updateAgentStatus(agentid, ainfo->properties());
    }
}

void XletAgents::newAgentLine(const QString & agentid)
{
    m_agent_labels[agentid] = new QLabel(this);
    m_agent_more[agentid] = new QPushButton(this);
    connect(m_agent_more[agentid], SIGNAL(clicked()),
             this, SLOT(agentClicked()));
    m_agent_record[agentid] = new QPushButton(this);
    connect(m_agent_record[agentid], SIGNAL(clicked()),
             this, SLOT(agentClicked()));
    m_agent_listen[agentid] = new QPushButton(this);
    connect(m_agent_listen[agentid], SIGNAL(clicked()),
             this, SLOT(agentClicked()));
    m_agent_logged_action[agentid] = new QPushButton(this);
    connect(m_agent_logged_action[agentid], SIGNAL(clicked()),
             this, SLOT(agentClicked()));
    m_agent_paused_action[agentid] = new QPushButton(this);
    connect(m_agent_paused_action[agentid], SIGNAL(clicked()),
             this, SLOT(agentClicked()));

    m_agent_busy[agentid] = new QLabel(this);
    m_agent_presence[agentid] = new QLabel(this);
    m_agent_logged_status[agentid] = new QLabel(this);
    m_agent_joined_number[agentid] = new QLabel(this);
    // m_agent_joined_status[agentid] = new QLabel(this);
    m_agent_paused_number[agentid] = new QLabel(this);
    m_agent_paused_status[agentid] = new QLabel(this);
}

// update according to admin-defined parameters
void XletAgents::updateAgentLineAdmin(const QString & agentid, const QString & agfullname, const QString & agentnumber)
{
    AgentInfo * ainfo = b_engine->agents()[agentid];
    m_agent_labels[agentid]->setText(QString("%1 (%2)").arg(agfullname).arg(agentnumber));
    m_agent_labels[agentid]->setToolTip(tr("Server: %1\nContext: %2").arg(ainfo->ipbxid()).arg(ainfo->context()));
    m_agent_more[agentid]->setProperty("agentid", agentid);
    m_agent_more[agentid]->setProperty("action", "changeagent");
    m_agent_record[agentid]->setProperty("agentid", agentid);
    m_agent_listen[agentid]->setProperty("agentid", agentid);
    m_agent_busy[agentid]->setProperty("agentid", agentid);
    m_agent_logged_action[agentid]->setProperty("agentid", agentid);
    m_agent_paused_action[agentid]->setProperty("agentid", agentid);
}

// update according to misc parameters
void XletAgents::updateAgentLineEvent(const QString & agentid)
{
    m_agent_record[agentid]->setProperty("action", "record");
    m_agent_listen[agentid]->setProperty("action", "listen");
    m_agent_logged_action[agentid]->setProperty("action", "loginoff");
    m_agent_paused_action[agentid]->setProperty("action", "pause");
}

void XletAgents::displayLine(const QString & agentid, int linenum)
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
    m_agent_presence[agentid]->setPixmap(square);

    m_agent_more[agentid]->setIconSize(QSize(m_gui_buttonsize, m_gui_buttonsize));
    m_agent_more[agentid]->setIcon(QIcon(":/images/add.png"));
    m_agent_record[agentid]->setIconSize(QSize(m_gui_buttonsize, m_gui_buttonsize));
    m_agent_record[agentid]->setIcon(QIcon(":/images/player_stop.png"));
    m_agent_listen[agentid]->setIconSize(QSize(m_gui_buttonsize, m_gui_buttonsize));
    m_agent_listen[agentid]->setIcon(QIcon(":/images/player_play.png"));
    m_agent_logged_action[agentid]->setIconSize(QSize(m_gui_buttonsize, m_gui_buttonsize));
    m_agent_paused_action[agentid]->setIconSize(QSize(m_gui_buttonsize, m_gui_buttonsize));

    int colnum = 0;
    m_gridlayout->addWidget(m_agent_labels[agentid], linenum, colnum++, Qt::AlignLeft);
    m_gridlayout->addWidget(m_agent_more[agentid], linenum, colnum++, Qt::AlignCenter);
    m_gridlayout->addWidget(m_agent_record[agentid], linenum, colnum++, Qt::AlignCenter);
    m_gridlayout->addWidget(m_agent_listen[agentid], linenum, colnum++, Qt::AlignCenter);
    m_gridlayout->addWidget(m_agent_busy[agentid], linenum, colnum++, Qt::AlignCenter);
    m_gridlayout->addWidget(qvline1, linenum, colnum++, Qt::AlignHCenter);
    m_gridlayout->addWidget(m_agent_presence[agentid], linenum, colnum++, Qt::AlignCenter);
    m_gridlayout->addWidget(qvline2, linenum, colnum++, Qt::AlignHCenter);
    m_gridlayout->addWidget(m_agent_logged_status[agentid], linenum, colnum++, Qt::AlignCenter);
    m_gridlayout->addWidget(m_agent_logged_action[agentid], linenum, colnum++, Qt::AlignCenter);
    m_gridlayout->addWidget(m_agent_joined_number[agentid], linenum, colnum++, Qt::AlignRight);
    m_gridlayout->addWidget(qvline3, linenum, colnum++, Qt::AlignHCenter);
    m_gridlayout->addWidget(m_agent_paused_status[agentid], linenum, colnum++, Qt::AlignCenter);
    m_gridlayout->addWidget(m_agent_paused_action[agentid], linenum, colnum++, Qt::AlignCenter);
    m_gridlayout->addWidget(m_agent_paused_number[agentid], linenum, colnum++, Qt::AlignRight);
}

void XletAgents::updateAgentStatus(const QString & agentid, const QVariantMap & properties)
{
    AgentInfo * ainfo = b_engine->agents()[agentid];
    if (ainfo == NULL)
        return;
    QString context_agent = ainfo->context();
    QVariantMap agqjoined = properties.value("queues_by_agent").toMap();
    QVariantMap aggjoined = properties.value("groups_by_agent").toMap();
    QString agstatus = properties.value("agentstats").toMap().value("status").toString();
    QString phonenum = properties.value("agentstats").toMap().value("agent_phone_number").toString();

    QVariantMap slink = properties.value("agentstats").toMap().value("Xivo-Agent-Status-Link").toMap();
    bool link = false;
    if (! slink.isEmpty()) {
        QString linkmode = slink.value("linkmode").toString();
        if ((linkmode == "phonelink") || (linkmode == "agentlink"))
            link = true;
        else
            b_engine->logClientWarning("XletAgents::updateAgentStatus",
                                       QString("agentid %1 linkmode %2")
                                       .arg(agentid).arg(linkmode));
    }

    QPixmap square(m_gui_buttonsize, m_gui_buttonsize);
    QStringList ttips;
    if (link) {
        square.fill(Qt::green);
        QHashIterator<QString, PhoneInfo *> iter = QHashIterator<QString, PhoneInfo *>(b_engine->phones());
        while (iter.hasNext()) {
            iter.next();
            if ((iter.value()->number() == phonenum) &&
                (iter.value()->ipbxid() == ainfo->ipbxid())) {
                foreach (const QString channel, iter.value()->channels()) {
                    const ChannelInfo * channelinfo = b_engine->channels().value(channel);
                    if(channelinfo == NULL)
                        continue;
                    ttips << tr("online with %1").arg(channelinfo->peerdisplay());
                }
            }
        }
    } else
        square.fill(Qt::gray);
    m_agent_busy[agentid]->setPixmap(square);
    m_agent_busy[agentid]->setToolTip(ttips.join("\n"));

    QString tooltip;
    if (agstatus == "AGENT_IDLE") {
        square.fill(Qt::green);
        m_agent_logged_action[agentid]->setIcon(QIcon(":/images/cancel.png"));
        tooltip = tr("Agent logged on %1").arg(phonenum);
    } else if (agstatus == "AGENT_ONCALL") {
        // square.fill(Qt::darkGreen);
        square.fill(Qt::green);
        m_agent_logged_action[agentid]->setIcon(QIcon(":/images/cancel.png"));
        // tooltip = tr("Agent busy");
        tooltip = tr("Agent logged on %1").arg(phonenum);
    } else if (agstatus == "AGENT_LOGGEDOFF") {
        square.fill(Qt::red);
        m_agent_logged_action[agentid]->setIcon(QIcon(":/images/button_ok.png"));
        tooltip = tr("Agent NOT logged");
    } else {
        square.fill(Qt::gray);
        m_agent_logged_action[agentid]->setIcon(QIcon(square));
        tooltip = tr("Unknown %1").arg(agstatus);
        b_engine->logClientWarning("XletAgents::updateAgentStatus",
                                   QString("agentid %1 agstatus %2")
                                   .arg(agentid).arg(agstatus));
    }
    m_agent_logged_status[agentid]->setPixmap(square);
    m_agent_logged_status[agentid]->setToolTip(tooltip);

    QStringList joined_queues;
    QStringList paused_queues;
    foreach (QString qname, agqjoined.keys()) {
        QString queueid = QString("queue:%1/%2").arg(ainfo->ipbxid()).arg(qname);
        QueueInfo * qinfo = b_engine->queues()[queueid];
        if (qinfo != NULL) {
            QVariant qv = agqjoined.value(qname);
            if (qv.toMap().contains("Status")) {
                QString pstatus = qv.toMap().value("Paused").toString();
                QString sstatus = qv.toMap().value("Status").toString();
                if ((sstatus == "1") || (sstatus == "3") || (sstatus == "4") || (sstatus == "5"))
                    joined_queues << qname;
                if (pstatus == "1")
                    paused_queues << qname;
                }
            // }
        }
    }

    int njoined = joined_queues.size();
    int npaused = paused_queues.size();
    QString sep = ", ";

    m_agent_joined_number[agentid]->setText(QString::number(njoined));
    m_agent_paused_number[agentid]->setText(QString::number(npaused));

    if (njoined)
        m_agent_joined_number[agentid]->setToolTip(tr("Joined queues : %1").arg(joined_queues.join(sep)));
    else
        m_agent_joined_number[agentid]->setToolTip("");

    if (njoined == 0) {
        m_agent_paused_status[agentid]->hide();
        m_agent_paused_action[agentid]->hide();
        m_agent_paused_number[agentid]->hide();
    } else {
        QString tooltip = tr("Paused queues : %1").arg(paused_queues.join(sep));
        if (npaused == 0) {
            m_agent_paused_status[agentid]->setText(tr("No"));
            m_agent_paused_action[agentid]->setIcon(QIcon(":/images/cancel.png"));
            m_agent_paused_action[agentid]->setProperty("action", "pause");
            m_agent_paused_status[agentid]->setToolTip("");
            m_agent_paused_number[agentid]->setToolTip("");
        } else if (npaused == njoined) {
            m_agent_paused_status[agentid]->setText(tr("Yes"));
            m_agent_paused_action[agentid]->setIcon(QIcon(":/images/button_ok.png"));
            m_agent_paused_action[agentid]->setProperty("action", "unpause");
            m_agent_paused_status[agentid]->setToolTip(tooltip);
            m_agent_paused_number[agentid]->setToolTip(tooltip);
        } else {
            m_agent_paused_status[agentid]->setText(tr("Partial"));
            m_agent_paused_action[agentid]->setIcon(QIcon(":/images/button_ok.png"));
            m_agent_paused_action[agentid]->setProperty("action", "unpause");
            m_agent_paused_status[agentid]->setToolTip(tooltip);
            m_agent_paused_number[agentid]->setToolTip(tooltip);
        }
        m_agent_paused_status[agentid]->show();
        m_agent_paused_action[agentid]->show();
        m_agent_paused_number[agentid]->show();
    }
}

/*! \brief process actions
 */
void XletAgents::agentClicked()
{
    QString agentid = sender()->property("agentid").toString();
    QString action  = sender()->property("action").toString();

    if (! b_engine->agents().keys().contains(agentid))
        return;

    AgentInfo * ainfo = b_engine->agents()[agentid];
    QString astid = ainfo->ipbxid();
    QString agentnumber = ainfo->agentNumber();
    QVariantMap ipbxcommand;

    if (action == "changeagent") {
        emit changeWatchedAgent(agentid, true);
    }

    else if (action == "loginoff") {
        QString status = ainfo->properties().value("agentstats").toMap().value("status").toString();
        ipbxcommand["agentids"] = agentid;
        if (status == "AGENT_IDLE")
            ipbxcommand["command"] = "agentlogout";
        else if (status == "AGENT_ONCALL")
            ipbxcommand["command"] = "agentlogout";
        else if (status == "AGENT_LOGGEDOFF")
            ipbxcommand["command"] = "agentlogin";
        else
            b_engine->logClientWarning("XletAgents::agentClicked",
                                       QString("agentid %1 action %2 status %3")
                                       .arg(agentid).arg(action).arg(status));
    } else if (action == "unpause") {
        ipbxcommand["command"] = "agentunpausequeue";
        ipbxcommand["agentids"] = agentid;
        ipbxcommand["queueids"] = QString("queue:%1/special:all").arg(astid);
    } else if (action == "pause") {
        ipbxcommand["command"] = "agentpausequeue";
        ipbxcommand["agentids"] = agentid;
        ipbxcommand["queueids"] = QString("queue:%1/special:all").arg(astid);
    } else if (action == "listen") {
        ipbxcommand["command"] = "listen";
        ipbxcommand["source"] = "user:special:me";
        ipbxcommand["destination"] = agentid;
    } else if (action == "stoplisten") {
        ipbxcommand["command"] = "stoplisten";
        ipbxcommand["source"] = "user:special:me";
        ipbxcommand["destination"] = agentid;
    } else if (action == "record") {
        ipbxcommand["command"] = "record";
        ipbxcommand["source"] = "user:special:me";
        ipbxcommand["destination"] = agentid;
    } else if (action == "stoprecord") {
        ipbxcommand["command"] = "stoprecord";
        ipbxcommand["source"] = "user:special:me";
        ipbxcommand["destination"] = agentid;
    }
    emit ipbxCommand(ipbxcommand);
}

/*! \brief update Record/Stop Record buttons
 */
void XletAgents::statusRecord(const QString & astid, const QString & agentid, const QString & status)
{
    // qDebug() << Q_FUNC_INFO << astid << agentid << status;
    QString gagentid = QString("agent:%1/%2").arg(astid).arg(agentid);
    if (! m_agent_record.contains(gagentid))
        return;

    if (status == "started") {
        m_agent_record[gagentid]->setProperty("action", "stoprecord");
        m_agent_record[gagentid]->setStyleSheet("QPushButton {background: #fbb638}");
    } else if (status == "stopped") {
        m_agent_record[gagentid]->setProperty("action", "record");
        m_agent_record[gagentid]->setStyleSheet("");
    }
}

/*! \brief update Listen/Stop Listen buttons
 */
void XletAgents::statusListen(const QString & astid, const QString & agentid, const QString & status)
{
    // qDebug() << Q_FUNC_INFO << astid << agentid << status;
    QString gagentid = QString("agent:%1/%2").arg(astid).arg(agentid);
    if (! m_agent_listen.contains(gagentid))
        return;

    if (status == "started") {
        m_agent_listen[gagentid]->setProperty("action", "stoplisten");
        m_agent_listen[gagentid]->setStyleSheet("QPushButton {background: #fbb638}");
    } else if (status == "stopped") {
        m_agent_listen[gagentid]->setProperty("action", "listen");
        m_agent_listen[gagentid]->setStyleSheet("");
    }
}

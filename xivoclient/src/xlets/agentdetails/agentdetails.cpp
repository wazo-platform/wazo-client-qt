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

#include <dao/queuememberdao.h>

#include "agentdetails.h"

Q_EXPORT_PLUGIN2(xletagentdetailsplugin, XLetAgentDetailsPlugin);

XLet* XLetAgentDetailsPlugin::newXLetInstance(QWidget *parent)
{
    b_engine->registerTranslation(":/obj/agentdetails_%1");
    return new XletAgentDetails(parent);
}

XletAgentDetails::XletAgentDetails(QWidget *parent)
    : XLet(parent)
{
    setTitle(tr("Agent Details"));
    m_linenum = 0;
    m_gridlayout = new QGridLayout(this);

    m_agentstatus = new QLabel(this);
    m_agentlegend_qname = new QLabel(tr("Queues"), this);
    m_agentlegend_joined = new QLabel(tr("Joined"), this);
    m_agentlegend_paused = new QLabel(tr("Paused"), this);
    m_agentlegend_njoined = new QLabel("0", this);
    m_agentlegend_npaused = new QLabel("0", this);

    m_actionlegends["agentlogin"] = new QLabel(tr("Login"), this);

    foreach (QString function, m_actionlegends.keys())
        m_action[function] = new QPushButton(this);
    m_action["agentlogin"]->setIconSize(QSize(10, 10));
    m_action["agentlogin"]->setIcon(QIcon(":/images/button_ok.png"));

    m_gridlayout->setRowStretch( 100, 1 );
    m_gridlayout->addWidget(m_agentstatus, m_linenum, 0, 1, 9);
    m_linenum ++;

    int colnum = 0;
    foreach (QString function, m_actionlegends.keys()) {
        m_gridlayout->addWidget(m_actionlegends[function], m_linenum, 2 + 3 * colnum, 1, 1, Qt::AlignCenter);
        m_gridlayout->addWidget(m_action[function], m_linenum, 3 + 3 * colnum, 1, 2, Qt::AlignCenter);
        colnum ++;
    }
    m_gridlayout->setColumnStretch( 8, 1 );
    m_linenum ++;

    m_gridlayout->addWidget(m_agentlegend_qname, m_linenum, 0, Qt::AlignLeft);
    m_gridlayout->addWidget(m_agentlegend_joined, m_linenum, 2, 1, 3, Qt::AlignCenter);
    m_gridlayout->addWidget(m_agentlegend_paused, m_linenum, 5, 1, 3, Qt::AlignCenter);
    m_linenum ++;
    m_gridlayout->addWidget(m_agentlegend_njoined, m_linenum, 2, 1, 3, Qt::AlignCenter);
    m_gridlayout->addWidget(m_agentlegend_npaused, m_linenum, 5, 1, 3, Qt::AlignCenter);
    m_linenum ++;

    m_gridlayout->setVerticalSpacing(0);
    m_agentlegend_qname->hide();
    m_agentlegend_joined->hide();
    m_agentlegend_paused->hide();
    m_agentlegend_njoined->hide();
    m_agentlegend_npaused->hide();

    foreach (QString function, m_actionlegends.keys()) {
        m_actionlegends[function]->hide();
        m_action[function]->hide();
        m_action[function]->setProperty("function", function);
        connect( m_action[function], SIGNAL(clicked()),
                 this, SLOT(actionClicked()));
    }

    // connect signal/slots with engine
    connect(b_engine, SIGNAL(newAgentList(const QStringList &)),
            this, SLOT(newAgentList(const QStringList &)));
    connect(b_engine, SIGNAL(updateAgentConfig(const QString &)),
            this, SLOT(updateAgentConfig(const QString &)));
    connect(b_engine, SIGNAL(updateAgentStatus(const QString &)),
            this, SLOT(updateAgentStatus(const QString &)));
    connect(b_engine, SIGNAL(updateQueueConfig(const QString &)),
            this, SLOT(updatePanel()));
    connect(b_engine, SIGNAL(updateQueueStatus(const QString &)),
            this, SLOT(updatePanel()));
    connect(b_engine, SIGNAL(updateQueueMemberConfig(const QString &)),
            this, SLOT(updatePanel()));
    connect(b_engine, SIGNAL(removeQueueMemberConfig(const QString &)),
            this, SLOT(updatePanel()));

    connect(b_engine, SIGNAL(changeWatchedAgentSignal(const QString &)),
            this, SLOT(monitorThisAgent(const QString &)));

}

void XletAgentDetails::updateAgentConfig(const QString & xagentid)
{
    if (xagentid == m_monitored_agentid)
        updatePanel();
}

void XletAgentDetails::updateAgentStatus(const QString & xagentid)
{
    if (xagentid == m_monitored_agentid)
        updatePanel();
}

void XletAgentDetails::monitorThisAgent(const QString & agentid)
{
    if (b_engine->hasAgent(agentid)) {
        m_monitored_agentid = agentid;
        clearPanel();
        updatePanel();
    }
}

void XletAgentDetails::clearPanel()
{
    // qDebug() << Q_FUNC_INFO;
    foreach (QString q, m_queue_labels.keys())
        delete m_queue_labels[q];
    foreach (QString q, m_queue_more.keys())
        delete m_queue_more[q];
    foreach (QString q, m_queue_join_status.keys())
        delete m_queue_join_status[q];
    foreach (QString q, m_queue_join_action.keys())
        delete m_queue_join_action[q];
    foreach (QString q, m_queue_pause_status.keys())
        delete m_queue_pause_status[q];
    foreach (QString q, m_queue_pause_action.keys())
        delete m_queue_pause_action[q];

    m_queue_labels.clear();
    m_queue_more.clear();
    m_queue_join_status.clear();
    m_queue_join_action.clear();
    m_queue_pause_status.clear();
    m_queue_pause_action.clear();
}

void XletAgentDetails::updatePanel()
{
    const AgentInfo * agentinfo = b_engine->agent(m_monitored_agentid);
    if (agentinfo == NULL)
        return;

    QStringList agent_descriptions;
    agent_descriptions << QString("<b>%1</b> (%2)").arg(agentinfo->fullname()).arg(agentinfo->agentNumber());
    if (! b_engine->getConfig("guioptions.xlet.agentdetails.hideastid").toBool())
        agent_descriptions << tr("on <b>%1</b>").arg(agentinfo->ipbxid());
    if (! b_engine->getConfig("guioptions.xlet.agentdetails.hidecontext").toBool())
        agent_descriptions << QString("(%1)").arg(agentinfo->context());
    QString lstatus = agentinfo->status();
    QString phonenum = b_engine->getConfig()["agentphonenumber"].toString();

    if (lstatus == "AGENT_LOGGEDOFF") {
        agent_descriptions << tr("logged off <b>%1</b>").arg(phonenum);
        m_action["agentlogin"]->setProperty("function", "agentlogin");
        m_action["agentlogin"]->setIcon(QIcon(":/images/button_ok.png"));
        m_actionlegends["agentlogin"]->setText(tr("Login"));
    } else if (lstatus == "AGENT_IDLE") {
        agent_descriptions << tr("logged on phone number <b>%1</b>").arg(phonenum);
        m_action["agentlogin"]->setProperty("function", "agentlogout");
        m_action["agentlogin"]->setIcon(QIcon(":/images/cancel.png"));
        m_actionlegends["agentlogin"]->setText(tr("Logout"));
    } else if (lstatus == "AGENT_ONCALL") {
        // QString talkingto = agentstats.toMap().value("talkingto").toString();
        // agent_status = tr("logged (busy with %1) on phone number <b>%2</b>").arg(talkingto).arg(phonenum);
        agent_descriptions << tr("logged on phone number <b>%1</b>").arg(phonenum);
        m_action["agentlogin"]->setProperty("function", "agentlogout");
        m_action["agentlogin"]->setIcon(QIcon(":/images/cancel.png"));
        m_actionlegends["agentlogin"]->setText(tr("Logout"));
    }

    m_agentstatus->setText(agent_descriptions.join(" "));

    m_agentlegend_qname->show();
    m_agentlegend_joined->show();
    m_agentlegend_paused->show();
    m_agentlegend_njoined->show();
    m_agentlegend_npaused->show();
    foreach (QString function, m_actionlegends.keys()) {
        m_actionlegends[function]->show();
        m_action[function]->show();
    }

    QStringList xqueueids;
    QVariantMap properties = agentinfo->properties();
    QVariant agentstats = properties["agentstats"];
    m_agentlegend_njoined->setText(agentstats.toMap().value("Xivo-NQJoined").toString());
    m_agentlegend_npaused->setText(agentstats.toMap().value("Xivo-NQPaused").toString());

    QHashIterator<QString, XInfo *> iter = QHashIterator<QString, XInfo *>(b_engine->iterover("queues"));
    while (iter.hasNext()) {
        iter.next();
        QString xqueueid = iter.key();
        QueueInfo * queueinfo = (QueueInfo *) iter.value();
        xqueueids << xqueueid;
        bool isnewqueue = false;
        if (! m_queue_labels.contains(xqueueid))
            isnewqueue = true;

        if (isnewqueue) {
            m_queue_labels[xqueueid] = new QLabel(this);
            m_queue_more[xqueueid] = new QPushButton(this);
            m_queue_join_status[xqueueid] = new QLabel(this);
            m_queue_join_action[xqueueid] = new QPushButton(this);
            m_queue_pause_status[xqueueid] = new QLabel(this);
            m_queue_pause_action[xqueueid] = new QPushButton(this);
            m_queue_join_status[xqueueid]->setProperty("Status", "undefined");
            m_queue_pause_status[xqueueid]->setProperty("Paused", "undefined");

            m_queue_join_status[xqueueid]->hide();
            m_queue_join_action[xqueueid]->hide();
            m_queue_pause_status[xqueueid]->hide();
            m_queue_pause_action[xqueueid]->hide();
        }

        setQueueLookProps(xqueueid);
        setQueueProps(xqueueid);
        if (queueinfo->ipbxid() == agentinfo->ipbxid()) {
            QString refmember = QueueMemberDAO::queueMemberId(agentinfo->xid(), queueinfo->xid());
            setQueueAgentProps(xqueueid, refmember);
        }

        if (isnewqueue)
            setQueueAgentSignals(xqueueid);
    }

    xqueueids.sort();
    int i = 0;
    foreach (QString xqueueid, xqueueids) {
        fillQueue(i, xqueueid);
        i ++;
    }
}

void XletAgentDetails::setQueueLookProps(const QString & xqueueid)
{
    m_queue_more[xqueueid]->setIconSize(QSize(10, 10));
    m_queue_join_action[xqueueid]->setIconSize(QSize(8, 8));
    m_queue_pause_action[xqueueid]->setIconSize(QSize(8, 8));
    m_queue_more[xqueueid]->setIcon(QIcon(":/images/add.png"));
}

void XletAgentDetails::setQueueProps(const QString & xqueueid)
{
    const QueueInfo * queueinfo = b_engine->queue(xqueueid);
    if (queueinfo == NULL)
        return;

    m_queue_labels[xqueueid]->setText(getQueueLabelText(xqueueid));
    QStringList tooltips;
    if (! b_engine->getConfig("guioptions.xlet.agentdetails.hideastid").toBool())
        tooltips << tr("Server: %1").arg(queueinfo->ipbxid());
    if (! b_engine->getConfig("guioptions.xlet.agentdetails.hidecontext").toBool())
        tooltips << tr("Context: %1").arg(queueinfo->context());
    m_queue_labels[xqueueid]->setToolTip(tooltips.join("\n"));
}

QString XletAgentDetails::getQueueLabelText(const QString & queue_xid)
{
    const QueueInfo * queueinfo = b_engine->queue(queue_xid);
    if (queueinfo == NULL) {
        return QString();
    }

    bool show_number = b_engine->getConfig("guioptions.queue_displaynu").toBool();
    QString display_name = queueinfo->queueDisplayName();
    if (show_number)
        display_name += QString(" (%1)").arg(queueinfo->queueNumber());
    return display_name;
}

void XletAgentDetails::setQueueAgentSignals(const QString & xqueueid)
{
    m_queue_more[xqueueid]->setProperty("xqueueid", xqueueid);
    m_queue_more[xqueueid]->setProperty("action", "changequeue");

    connect( m_queue_more[xqueueid], SIGNAL(clicked()),
             this, SLOT(queueClicked()));
    if (! b_engine->getConfig("guioptions.xlet.agentdetails.noqueueaction").toBool()) {
        connect( m_queue_join_action[xqueueid], SIGNAL(clicked()),
                 this, SLOT(queueClicked()));
        connect( m_queue_pause_action[xqueueid], SIGNAL(clicked()),
                 this, SLOT(queueClicked()));
    }
}

void XletAgentDetails::setQueueAgentProps(const QString & xqueueid, const QString & xqueuemember)
{
    const QueueMemberInfo * qmi = b_engine->queuemember(xqueuemember);
    if ((qmi == NULL) && (! xqueuemember.isEmpty()))
        return;
    m_queue_join_action[xqueueid]->setProperty("xqueueid", xqueueid);
    m_queue_join_action[xqueueid]->setProperty("action", "leavejoin");
    m_queue_pause_action[xqueueid]->setProperty("xqueueid", xqueueid);
    m_queue_pause_action[xqueueid]->setProperty("action", "pause");
    m_queue_join_status[xqueueid]->show();
    m_queue_pause_status[xqueueid]->show();

    QString oldstatus = m_queue_join_status[xqueueid]->property("Status").toString();
    QString oldpaused = m_queue_pause_status[xqueueid]->property("Paused").toString();

    QString status = "";
    QString paused = "";
    QString membership = "";

    if (qmi != NULL) {
        status = qmi->status();
        paused = qmi->paused();
        membership = qmi->membership();
    }

    QueueAgentStatus * qas = new QueueAgentStatus();
    qas->update(membership, status, paused);

    QString joinicon = qas->display_action_join();
    if (joinicon.isEmpty()) {
        m_queue_join_action[xqueueid]->hide();
    } else {
        m_queue_join_action[xqueueid]->setIcon(QIcon(joinicon));
        m_queue_join_action[xqueueid]->show();
    }

    if (status != oldstatus) {
        QPixmap square(12, 12);
        square.fill(qas->display_status_color());
        m_queue_join_status[xqueueid]->setPixmap(square);
        m_queue_join_status[xqueueid]->setToolTip(QString("%1\n%2\n%3")
                                                 .arg(qas->display_status_queue())
                                                 .arg(qas->display_status_logged())
                                                 .arg(qas->display_status_membership()));
        m_queue_join_status[xqueueid]->setProperty("Status", status);
    }

    if (paused != oldpaused) {
        QString pauseicon = qas->display_action_pause();
        if (pauseicon.isEmpty()) {
            m_queue_pause_action[xqueueid]->hide();
        } else {
            m_queue_pause_action[xqueueid]->setIcon(QIcon(pauseicon));
            m_queue_pause_action[xqueueid]->show();
        }
        m_queue_pause_status[xqueueid]->setToolTip(qas->display_status_paused());
        QPixmap square(12, 12);
        square.fill(qas->display_status_paused_color());
        m_queue_pause_status[xqueueid]->setPixmap(square);
        m_queue_pause_status[xqueueid]->setProperty("Paused", paused);
    }

    delete qas;
}

void XletAgentDetails::fillQueue(int ii, const QString & xqueueid)
{
    m_gridlayout->addWidget( m_queue_labels[xqueueid], ii + m_linenum, 0, Qt::AlignLeft );
    m_gridlayout->addWidget( m_queue_more[xqueueid], ii + m_linenum, 1, Qt::AlignCenter );
    m_gridlayout->addWidget( m_queue_join_status[xqueueid], ii + m_linenum, 2, Qt::AlignCenter );
    m_gridlayout->addWidget( m_queue_join_action[xqueueid], ii + m_linenum, 3, Qt::AlignCenter );
    m_gridlayout->addWidget( m_queue_pause_status[xqueueid], ii + m_linenum, 5, Qt::AlignCenter );
    m_gridlayout->addWidget( m_queue_pause_action[xqueueid], ii + m_linenum, 6, Qt::AlignCenter );
}

/*! \brief execute action on queue
 *
 * supports actions "changequeue", "leavejoin", "pause"
 */
void XletAgentDetails::queueClicked()
{
    QString xqueueid = sender()->property("xqueueid").toString();
    const QueueInfo * queueinfo = b_engine->queue(xqueueid);
    if (queueinfo == NULL)
        return;
    QString action  = sender()->property("action").toString();
    QString queuename = queueinfo->queueName();

    QString xqueuemember = QueueMemberDAO::queueMemberId(m_monitored_agentid, xqueueid);
    const QueueMemberInfo * qmi = b_engine->queuemember(xqueuemember);

    QVariantMap ipbxcommand;
    ipbxcommand["member"] = QString("agent:%0").arg(m_monitored_agentid);
    ipbxcommand["queue"] = QString("queue:%0").arg(xqueueid);

    if (action == "changequeue")
        b_engine->changeWatchedQueue(xqueueid);
    else if (action == "leavejoin") {
        QString smstatus, pmstatus;
        if (qmi != NULL) {
            smstatus = qmi->status();
            pmstatus = qmi->paused();
        }
        if ((smstatus == "1") || (smstatus == "3") || (smstatus == "4") || (smstatus == "5")) {
            ipbxcommand["command"] = "queueremove";
        } else if (smstatus == "") {
            ipbxcommand["command"] = "queueadd";
        }
    } else if (action == "pause") {
        if (qmi == NULL)
            return;
        QString smstatus = qmi->status();
        QString pmstatus = qmi->paused();
        if (pmstatus == "0") {
            ipbxcommand["command"] = "queuepause";
        } else if (pmstatus == "1") {
            ipbxcommand["command"] = "queueunpause";
        }
    }

    emit ipbxCommand(ipbxcommand);
}

/*! \brief left click actions (login, logout) */
void XletAgentDetails::actionClicked()
{
    QString function = sender()->property("function").toString();
    QVariantMap ipbxcommand;
    if (function == "agentlogin") {
        ipbxcommand["command"] = "agentlogin";
        ipbxcommand["agentids"] = m_monitored_agentid;
        emit ipbxCommand(ipbxcommand);
    } else if (function == "agentlogout") {
        ipbxcommand["command"] = "agentlogout";
        ipbxcommand["agentids"] = m_monitored_agentid;
        emit ipbxCommand(ipbxcommand);
    }
}

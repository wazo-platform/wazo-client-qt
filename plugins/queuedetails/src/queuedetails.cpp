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

#include "queuedetails.h"

Q_EXPORT_PLUGIN2(xletqueuedetailsplugin, XLetQueueDetailsPlugin);

XLet* XLetQueueDetailsPlugin::newXLetInstance(QWidget *parent)
{
    b_engine->registerTranslation(":/queuedetails_%1");
    return new XletQueueDetails(parent);
}


XletQueueDetails::XletQueueDetails(QWidget *parent)
    : XLet(parent)
{
    setTitle(tr("Agents of a Queue"));
    m_gridlayout = new QGridLayout(this);

    m_queuedescription = new QLabel(this);
    m_queuelegend_agentid = new QLabel(tr("Agent"), this);
    m_queuelegend_status = new QLabel(tr("Status"), this);
    m_queuelegend_paused = new QLabel(tr("Paused ?"), this);
    m_queuelegend_callstaken = new QLabel(tr("Calls\nTaken"), this);
    m_queuelegend_lastcall = new QLabel(tr("Last\nCall"), this);
    m_queuelegend_penalty = new QLabel(tr("Penalty"), this);
    m_gridlayout->setRowStretch(100, 1);
    m_gridlayout->addWidget(m_queuedescription, 0, 0);
    m_gridlayout->addWidget(m_queuelegend_agentid, 1, 0);
    m_gridlayout->addWidget(m_queuelegend_status, 1, 2);
    m_gridlayout->addWidget(m_queuelegend_paused, 1, 3);
    m_gridlayout->addWidget(m_queuelegend_callstaken, 1, 4);
    m_gridlayout->addWidget(m_queuelegend_lastcall, 1, 5);
    m_gridlayout->addWidget(m_queuelegend_penalty, 1, 6);
    m_gridlayout->setColumnStretch(7, 1);
    m_gridlayout->setVerticalSpacing(0);
    m_queuelegend_agentid->hide();
    m_queuelegend_status->hide();
    m_queuelegend_paused->hide();
    m_queuelegend_callstaken->hide();
    m_queuelegend_lastcall->hide();
    m_queuelegend_penalty->hide();

    // connect signals/slots to engine
    connect(b_engine, SIGNAL(updateAgentConfig(const QString &)),
            this, SLOT(updateAgentConfig(const QString &)));
    connect(b_engine, SIGNAL(updateAgentStatus(const QString &)),
            this, SLOT(updateAgentStatus(const QString &)));
    connect(b_engine, SIGNAL(updateQueueConfig(const QString &)),
            this, SLOT(updateQueueConfig(const QString &)));
    connect(b_engine, SIGNAL(updateQueueStatus(const QString &)),
            this, SLOT(updateQueueStatus(const QString &)));

    connect(b_engine, SIGNAL(changeWatchedQueueSignal(const QString &)),
            this, SLOT(monitorThisQueue(const QString &)));
}

void XletQueueDetails::updateAgentConfig(const QString & xagentid)
{
    qDebug() << Q_FUNC_INFO << xagentid;
}

void XletQueueDetails::updateAgentStatus(const QString & xagentid)
{
    qDebug() << Q_FUNC_INFO << xagentid;
}

void XletQueueDetails::updateQueueConfig(const QString & xqueueid)
{
    qDebug() << Q_FUNC_INFO << xqueueid << m_monitored_queueid;
}

void XletQueueDetails::updateQueueStatus(const QString & xqueueid)
{
    qDebug() << Q_FUNC_INFO << xqueueid << m_monitored_queueid;
}

void XletQueueDetails::monitorThisQueue(const QString & queueid)
{
    qDebug() << Q_FUNC_INFO << queueid;
    if (b_engine->hasQueue(queueid)) {
        m_monitored_queueid = queueid;
        clearPanel();
        updatePanel();
    }
}

void XletQueueDetails::clearPanel()
{
    // qDebug() << Q_FUNC_INFO;
    foreach(QString q, m_agent_labels.keys())
        delete m_agent_labels[q];
    foreach(QString q, m_agent_more.keys())
        delete m_agent_more[q];
    foreach(QString q, m_agent_join_status.keys())
        delete m_agent_join_status[q];
    foreach(QString q, m_agent_pause_status.keys())
        delete m_agent_pause_status[q];
    foreach(QString q, m_agent_callstaken.keys())
        delete m_agent_callstaken[q];
    foreach(QString q, m_agent_lastcall.keys())
        delete m_agent_lastcall[q];
    foreach(QString q, m_agent_penalty.keys())
        delete m_agent_penalty[q];

    m_agent_labels.clear();
    m_agent_more.clear();
    m_agent_join_status.clear();
    m_agent_pause_status.clear();
    m_agent_callstaken.clear();
    m_agent_lastcall.clear();
    m_agent_penalty.clear();
}

void XletQueueDetails::updatePanel()
{
    const QueueInfo * qinfo = b_engine->queue(m_monitored_queueid);
    if (qinfo == NULL)
        return;
    m_queuedescription->setText(tr("<b>%1</b> (%2) on <b>%3</b> (%4)")
                                .arg(qinfo->queueName())
                                .arg(qinfo->queueNumber())
                                .arg(qinfo->ipbxid())
                                .arg(qinfo->context()));

    if (! b_engine->iterover("agents").isEmpty()) {
        m_queuelegend_agentid->show();
        m_queuelegend_status->show();
        m_queuelegend_paused->show();
        m_queuelegend_callstaken->show();
        m_queuelegend_lastcall->show();
        m_queuelegend_penalty->show();
    }

    qDebug() << Q_FUNC_INFO << m_monitored_queueid << qinfo->xagentids() << qinfo->xphoneids();

    int i = 0;
    QHashIterator<QString, XInfo *> iter = QHashIterator<QString, XInfo *>(b_engine->iterover("agents"));
    while (iter.hasNext()) {
        iter.next();
        QString xagentid = iter.key();
        AgentInfo * ainfo = (AgentInfo *) iter.value();

        bool isnewagent = false;
        if (! m_agent_more.contains(xagentid))
            isnewagent = true;

        if (isnewagent) {
            m_agent_labels[xagentid] = new QLabel(this);
            m_agent_more[xagentid] = new QPushButton(this);
            m_agent_join_status[xagentid] = new QLabel(this);
            m_agent_pause_status[xagentid] = new QLabel(this);
            m_agent_callstaken[xagentid] = new QLabel(this);
            m_agent_lastcall[xagentid] = new QLabel(this);
            m_agent_penalty[xagentid] = new QLabel(this);

            m_agent_join_status[xagentid]->setProperty("Status", "undefined");
            m_agent_pause_status[xagentid]->setProperty("Paused", "undefined");

            fillAgent(i, xagentid);
        }

        setAgentLookProps(xagentid);
        setAgentProps(xagentid, ainfo);
        if(qinfo->ipbxid() == ainfo->ipbxid()) {
            QString refmember = qinfo->reference("agents", xagentid);
            setAgentQueueProps(xagentid, refmember);
        }

        if(isnewagent) {
            setAgentQueueSignals(xagentid);
        }

        i++;
    }
}

void XletQueueDetails::setAgentLookProps(const QString & xagentid)
{
    m_agent_more[xagentid]->setIconSize(QSize(10, 10));
    m_agent_more[xagentid]->setIcon(QIcon(":/images/add.png"));
}

void XletQueueDetails::setAgentProps(const QString & xagentid, const AgentInfo * ainfo)
{
    m_agent_labels[xagentid]->setText(QString("%1 (%2)")
                                      .arg(ainfo->fullname())
                                      .arg(ainfo->agentNumber()));
    m_agent_labels[xagentid]->setToolTip(tr("Server: %1\n"
                                            "Context: %2")
                                         .arg(ainfo->ipbxid())
                                         .arg(ainfo->context()));
}

void XletQueueDetails::setAgentQueueSignals(const QString & xagentid)
{
    if (! m_agent_more.contains(xagentid))
        return;
    m_agent_more[xagentid]->setProperty("agentid", xagentid);
    connect(m_agent_more[xagentid], SIGNAL(clicked()),
            this, SLOT(agentClicked()));
}

void XletQueueDetails::setAgentQueueProps(const QString & xagentid, const QString & xqueuemember)
{
    const QueueMemberInfo * qmi = b_engine->queuemembers().value(xqueuemember);
    if ((qmi == NULL) && (! xqueuemember.isEmpty()))
        return;
    // note : right now, we handle agents that do not belong to queue according to xqueuemember = ""
    // this is not really great, so please feel free to improve it one day
    QString oldstatus = m_agent_join_status[xagentid]->property("Status").toString();
    QString oldpaused = m_agent_pause_status[xagentid]->property("Paused").toString();

    QString status = "";
    QString paused = "";
    QString membership = "";
    QString callstaken = "";
    QString penalty = "";
    int lastcall = 0;

    if (qmi != NULL) {
        status = qmi->status();
        paused = qmi->paused();
        membership = qmi->membership();
        callstaken = qmi->callstaken();
        penalty = qmi->penalty();
        lastcall = qmi->lastcall();
    }

    QueueAgentStatus * qas = new QueueAgentStatus();
    qas->update(membership, status, paused);

    if (status != oldstatus) {
        QPixmap square(12, 12);
        square.fill(qas->display_status_color());
        m_agent_join_status[xagentid]->setPixmap(square);
        m_agent_join_status[xagentid]->setToolTip(QString("%1\n%2\n%3")
                                                  .arg(qas->display_status_queue())
                                                  .arg(qas->display_status_logged())
                                                  .arg(qas->display_status_membership()));
        m_agent_join_status[xagentid]->setProperty("Status", status);
    }

    if (paused != oldpaused) {
        m_agent_pause_status[xagentid]->setText(qas->display_status_paused());
        m_agent_pause_status[xagentid]->setProperty("Paused", paused);
    }

    delete qas;

    m_agent_callstaken[xagentid]->setText(callstaken);
    m_agent_penalty[xagentid]->setText(penalty);

    QString slastcall = "-";
    QDateTime dtlastcall;
    if(lastcall > 0) {
        dtlastcall.setTime_t(lastcall);
        slastcall = dtlastcall.toString("hh:mm:ss");
    }
    m_agent_lastcall[xagentid]->setText(slastcall);
}

void XletQueueDetails::fillAgent(int ii, const QString & xagentid)
{
    int m_linenum = 3;
    int colnum = 0;
    m_gridlayout->addWidget(m_agent_labels[xagentid], ii + m_linenum, colnum++, Qt::AlignLeft);
    m_gridlayout->addWidget(m_agent_more[xagentid], ii + m_linenum, colnum++, Qt::AlignCenter);
    m_gridlayout->addWidget(m_agent_join_status[xagentid], ii + m_linenum, colnum++, Qt::AlignLeft);
    m_gridlayout->addWidget(m_agent_pause_status[xagentid], ii + m_linenum, colnum++, Qt::AlignLeft);
    m_gridlayout->addWidget(m_agent_callstaken[xagentid], ii + m_linenum, colnum++, Qt::AlignRight);
    m_gridlayout->addWidget(m_agent_lastcall[xagentid], ii + m_linenum, colnum++, Qt::AlignRight);
    m_gridlayout->addWidget(m_agent_penalty[xagentid], ii + m_linenum, colnum++, Qt::AlignRight);
}

void XletQueueDetails::agentClicked()
{
    // qDebug() << Q_FUNC_INFO << sender()->property("agentid");
    QString agentid = sender()->property("agentid").toString();
    b_engine->changeWatchedAgent(agentid, true);
}

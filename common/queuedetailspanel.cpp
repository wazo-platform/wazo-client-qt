/* XiVO Client
 * Copyright (C) 2007-2010, Proformatique
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

#include <QDebug>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QVariant>

#include "baseengine.h"
#include "queuedetailspanel.h"
#include "queue_agent_status.h"
#include "userinfo.h"
#include "queueinfo.h"
#include "agentinfo.h"

/*! \brief Constructor
 */
QueuedetailsPanel::QueuedetailsPanel(QWidget * parent)
    : XLet(parent)
{
    setTitle( tr("Agents of a Queue") );
    m_gridlayout = new QGridLayout(this);
    
    m_queuedescription = new QLabel(this);
    m_queuelegend_agentid = new QLabel(tr("Agent"), this);
    m_queuelegend_status = new QLabel(tr("Status"), this);
    m_queuelegend_paused = new QLabel(tr("Paused ?"), this);
    m_queuelegend_callstaken = new QLabel(tr("Calls\nTaken"), this);
    m_queuelegend_lastcall = new QLabel(tr("Last\nCall"), this);
    m_queuelegend_penalty = new QLabel(tr("Penalty"), this);
    m_gridlayout->setRowStretch( 100, 1 );
    m_gridlayout->addWidget(m_queuedescription, 0, 0);
    m_gridlayout->addWidget(m_queuelegend_agentid, 1, 0);
    m_gridlayout->addWidget(m_queuelegend_status, 1, 2);
    m_gridlayout->addWidget(m_queuelegend_paused, 1, 3);
    m_gridlayout->addWidget(m_queuelegend_callstaken, 1, 4);
    m_gridlayout->addWidget(m_queuelegend_lastcall, 1, 5);
    m_gridlayout->addWidget(m_queuelegend_penalty, 1, 6);
    m_gridlayout->setColumnStretch( 7, 1 );
    m_gridlayout->setVerticalSpacing(0);
    m_queuelegend_agentid->hide();
    m_queuelegend_status->hide();
    m_queuelegend_paused->hide();
    m_queuelegend_callstaken->hide();
    m_queuelegend_lastcall->hide();
    m_queuelegend_penalty->hide();
    
    // connect signals/slots to engine
    connect(b_engine, SIGNAL(newAgentList(const QStringList &)),
            this, SLOT(newAgentList(const QStringList &)) );
    connect(b_engine, SIGNAL(newQueueList(const QStringList &)),
            this, SLOT(newQueueList(const QStringList &)) );
    
    connect(b_engine, SIGNAL(changeWatchedQueueSignal(const QString &)),
            this, SLOT(monitorThisQueue(const QString &)) );
    connect(this, SIGNAL(changeWatchedAgent(const QString &, bool)),
            b_engine, SLOT(changeWatchedAgentSlot(const QString &, bool)) );
}

/*! \brief 
 */
void QueuedetailsPanel::newQueueList(const QStringList & qsl)
{
    // qDebug() << "QueuedetailsPanel::newQueueList()" << qsl;
    if(qsl.contains(m_monitored_queueid) && b_engine->queues().contains(m_monitored_queueid))
        updatePanel();
}

/*! \brief 
 */
void QueuedetailsPanel::newAgentList(const QStringList &)
{
    // qDebug() << "QueuedetailsPanel::newAgentList()" << qsl;
    if(b_engine->queues().contains(m_monitored_queueid))
        updatePanel();
}

/*! \brief 
 */
void QueuedetailsPanel::monitorThisQueue(const QString & queueid)
{
    // qDebug() << "QueuedetailsPanel::monitorThisQueue" << queueid;
    if(b_engine->queues().contains(queueid)) {
        m_monitored_queueid = queueid;
        clearPanel();
        updatePanel();
    }
}

/*! \brief 
 */
void QueuedetailsPanel::clearPanel()
{
    // qDebug() << "QueuedetailsPanel::update()";
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

/*! \brief 
 */
void QueuedetailsPanel::updatePanel()
{
    QueueInfo * qinfo = b_engine->queues()[m_monitored_queueid];
    m_queuedescription->setText(tr("<b>%1</b> (%2) on <b>%3</b> (%4)")
                                .arg(qinfo->queueName())
                                .arg(qinfo->queueNumber())
                                .arg(qinfo->astid())
                                .arg(qinfo->context()));
    QVariantMap properties = qinfo->properties();
    QVariant queuestats = properties["queuestats"];
    QVariantMap agentstats = properties["agents_in_queue"].toMap();
    
    if(! b_engine->agents().isEmpty()) {
        m_queuelegend_agentid->show();
        m_queuelegend_status->show();
        m_queuelegend_paused->show();
        m_queuelegend_callstaken->show();
        m_queuelegend_lastcall->show();
        m_queuelegend_penalty->show();
    }
    
    int i = 0;
    QHashIterator<QString, AgentInfo *> iter = QHashIterator<QString, AgentInfo *>(b_engine->agents());
    while( iter.hasNext() )
        {
            iter.next();
            AgentInfo * ainfo = iter.value();
            QString agentid = iter.key();
            
            bool isnewagent = false;
            if(! m_agent_more.contains(agentid))
                isnewagent = true;
            
            if(isnewagent) {
                m_agent_labels[agentid] = new QLabel(this);
                m_agent_more[agentid] = new QPushButton(this);
                m_agent_join_status[agentid] = new QLabel(this);
                m_agent_pause_status[agentid] = new QLabel(this);
                m_agent_callstaken[agentid] = new QLabel(this);
                m_agent_lastcall[agentid] = new QLabel(this);
                m_agent_penalty[agentid] = new QLabel(this);
                
                m_agent_join_status[agentid]->setProperty("Status", "undefined");
                m_agent_pause_status[agentid]->setProperty("Paused", "undefined");
                
                fillAgent(i, agentid);
            }
            
            setAgentLookProps(agentid);
            setAgentProps(agentid, ainfo);
            QString agentname = "Agent/" + ainfo->agentNumber();
            if(qinfo->astid() == ainfo->astid()) {
                setAgentQueueProps(agentid, agentstats[agentname]);
            }
            
            if(isnewagent)
                setAgentQueueSignals(agentid);
            i ++;
        }
}

/*! \brief 
 */
void QueuedetailsPanel::setAgentLookProps(const QString & agentid)
{
    m_agent_more[agentid]->setIconSize(QSize(10, 10));
    m_agent_more[agentid]->setIcon(QIcon(":/images/add.png"));
}

/*! \brief 
 */
void QueuedetailsPanel::setAgentProps(const QString & agentid, const AgentInfo * ainfo)
{
    m_agent_labels[agentid]->setText(QString("%1 (%2)").arg(ainfo->fullname()).arg(ainfo->agentNumber()));
    m_agent_labels[agentid]->setToolTip(tr("Server: %1\nContext: %2").arg(ainfo->astid()).arg(ainfo->context()));
    // qDebug() << "QueuedetailsPanel::setAgentProps" << agentid << ainfo->properties()["agentstats"].toMap()["loggedintime"].toInt();
}

/*! \brief 
 */
void QueuedetailsPanel::setAgentQueueSignals(const QString & agentid)
{
    m_agent_more[agentid]->setProperty("agentid", agentid);
    connect( m_agent_more[agentid], SIGNAL(clicked()),
             this, SLOT(agentClicked()));
}

/*! \brief 
 */
void QueuedetailsPanel::setAgentQueueProps(const QString & agentid, const QVariant & qv)
{
    QString oldsstatus = m_agent_join_status[agentid]->property("Status").toString();
    QString oldpstatus = m_agent_pause_status[agentid]->property("Paused").toString();
    
    QString pstatus = qv.toMap()["Paused"].toString();
    QString sstatus = qv.toMap()["Status"].toString();
    QString dynstatus = qv.toMap()["Membership"].toString();
    
    QueueAgentStatus * qas = new QueueAgentStatus();
    qas->update(dynstatus, sstatus, pstatus);
    
    if(sstatus != oldsstatus) {
        QPixmap square(12, 12);
        square.fill(qas->display_status_color());
        m_agent_join_status[agentid]->setPixmap(square);
        m_agent_join_status[agentid]->setToolTip(QString("%1\n%2\n%3")
                                                 .arg(qas->display_status_queue())
                                                 .arg(qas->display_status_logged())
                                                 .arg(qas->display_status_membership()));
        m_agent_join_status[agentid]->setProperty("Status", sstatus);
    }
    
    if(pstatus != oldpstatus) {
        m_agent_pause_status[agentid]->setText(qas->display_status_paused());
        m_agent_pause_status[agentid]->setProperty("Paused", pstatus);
    }
    
    if(qv.toMap().contains("CallsTaken"))
        m_agent_callstaken[agentid]->setText(qv.toMap()["CallsTaken"].toString());
    else
        m_agent_callstaken[agentid]->setText("0");
    
    delete qas;
    
    QString slastcall = "-";
    if(qv.toMap().contains("LastCall")) {
        QDateTime lastcall;
        int epoch = qv.toMap()["LastCall"].toInt();
        if(epoch > 0) {
            lastcall.setTime_t(epoch);
            slastcall = lastcall.toString("hh:mm:ss");
        }
    }
    m_agent_lastcall[agentid]->setText(slastcall);
    
    if(qv.toMap().contains("Penalty"))
        m_agent_penalty[agentid]->setText(qv.toMap()["Penalty"].toString());
    else
        m_agent_penalty[agentid]->setText("0");
}

/*! \brief 
 */
void QueuedetailsPanel::fillAgent(int ii, const QString & agentid)
{
    int m_linenum = 3;
    int colnum = 0;
    m_gridlayout->addWidget( m_agent_labels[agentid], ii + m_linenum, colnum++, Qt::AlignLeft );
    m_gridlayout->addWidget( m_agent_more[agentid], ii + m_linenum, colnum++, Qt::AlignCenter );
    m_gridlayout->addWidget( m_agent_join_status[agentid], ii + m_linenum, colnum++, Qt::AlignLeft );
    m_gridlayout->addWidget( m_agent_pause_status[agentid], ii + m_linenum, colnum++, Qt::AlignLeft );
    m_gridlayout->addWidget( m_agent_callstaken[agentid], ii + m_linenum, colnum++, Qt::AlignRight );
    m_gridlayout->addWidget( m_agent_lastcall[agentid], ii + m_linenum, colnum++, Qt::AlignRight );
    m_gridlayout->addWidget( m_agent_penalty[agentid], ii + m_linenum, colnum++, Qt::AlignRight );
}

/*! \brief 
 */
void QueuedetailsPanel::update()
{
    // qDebug() << "QueuedetailsPanel::update()";
    // UserInfo * ui = b_engine->findUserFromPhone(m_monitored_astid, agent_channel);
}

/*! \brief emit changeWatchedAgent signal
 */
void QueuedetailsPanel::agentClicked()
{
    // qDebug() << "QueuedetailsPanel::agentClicked()" << sender()->property("agentid");
    QString agentid = sender()->property("agentid").toString();
    emit changeWatchedAgent(agentid, true);
}

/*! \brief 
 */
void QueuedetailsPanel::timerEvent(QTimerEvent *)
{
    // qDebug() << "QueuedetailsPanel::timerEvent()";
}

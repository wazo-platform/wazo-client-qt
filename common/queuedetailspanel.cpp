/* XIVO CTI clients
 * Copyright (C) 2007-2009  Proformatique
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License version 2 for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Linking the Licensed Program statically or dynamically with other
 * modules is making a combined work based on the Licensed Program. Thus,
 * the terms and conditions of the GNU General Public License version 2
 * cover the whole combination.
 *
 * In addition, as a special exception, the copyright holders of the
 * Licensed Program give you permission to combine the Licensed Program
 * with free software programs or libraries that are released under the
 * GNU Library General Public License version 2.0 or GNU Lesser General
 * Public License version 2.1 or any later version of the GNU Lesser
 * General Public License, and with code included in the standard release
 * of OpenSSL under a version of the OpenSSL license (with original SSLeay
 * license) which is identical to the one that was published in year 2003,
 * or modified versions of such code, with unchanged license. You may copy
 * and distribute such a system following the terms of the GNU GPL
 * version 2 for the Licensed Program and the licenses of the other code
 * concerned, provided that you include the source code of that other code
 * when and as the GNU GPL version 2 requires distribution of source code.
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
#include "userinfo.h"
#include "queueinfo.h"
#include "agentinfo.h"

/*! \brief Constructor
 */
QueuedetailsPanel::QueuedetailsPanel(BaseEngine * engine,
                                     const QVariant &, // options
                                     QWidget * parent)
    : QWidget(parent), m_engine(engine)
{
    m_gridlayout = new QGridLayout(this);
    
    m_queuedescription = new QLabel(this);
    m_queuelegend_agentid = new QLabel(tr("Agent"), this);
    m_queuelegend_status = new QLabel(tr("Status"), this);
    m_queuelegend_paused = new QLabel(tr("Paused ?"), this);
    m_queuelegend_callstaken = new QLabel(tr("Calls Taken"), this);
    m_queuelegend_lastcall = new QLabel(tr("Last Call"), this);
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
    // startTimer(1000);
}

/*! \brief destructor
 */
QueuedetailsPanel::~QueuedetailsPanel()
{
    // qDebug() << "QueuedetailsPanel::~QueuedetailsPanel()";
}

/*! \brief 
 */
void QueuedetailsPanel::newQueueList()
{
    // qDebug() << "QueuedetailsPanel::newQueueList()";
    if(m_engine->queues().contains(m_monitored_queueid))
        updatePanel();
}

/*! \brief 
 */
void QueuedetailsPanel::newAgentList()
{
    // qDebug() << "QueuedetailsPanel::newAgentList()";
    if(m_engine->queues().contains(m_monitored_queueid))
        updatePanel();
}

/*! \brief 
 */
void QueuedetailsPanel::monitorThisQueue(const QString & queueid)
{
    // qDebug() << "QueuedetailsPanel::monitorThisQueue" << queueid;
    if(m_engine->queues().contains(queueid)) {
        m_monitored_queueid = queueid;
        m_monitored_astid = m_engine->queues()[queueid]->astid();
        m_monitored_context = m_engine->queues()[queueid]->context();
        m_monitored_queuename = m_engine->queues()[queueid]->queuename();
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
    QueueInfo * qinfo = m_engine->queues()[m_monitored_queueid];
    m_queuedescription->setText(tr("<b>%1</b> on <b>%2</b> (%3)").arg(qinfo->queuename()).arg(qinfo->astid()).arg(qinfo->context()));
    QVariantMap properties = qinfo->properties();
    QVariant queuestats = properties["queuestats"];
    QVariantMap agentstats = properties["agents_in_queue"].toMap();
    
    m_queuelegend_agentid->show();
    m_queuelegend_status->show();
    m_queuelegend_paused->show();
    m_queuelegend_callstaken->show();
    m_queuelegend_lastcall->show();
    m_queuelegend_penalty->show();
    
    int i = 0;
    QHashIterator<QString, AgentInfo *> iter = QHashIterator<QString, AgentInfo *>(m_engine->agents());
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
            QString agentname = "Agent/" + ainfo->agentnumber();
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
    m_agent_labels[agentid]->setText(QString("%1 (%2)").arg(ainfo->fullname()).arg(ainfo->agentnumber()));
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
    QString pstatus = qv.toMap()["Paused"].toString();
    QString sstatus = qv.toMap()["Status"].toString();
    QString dynstatus = qv.toMap()["Membership"].toString();
    // LastCall, Penalty
    
    QString oldsstatus = m_agent_join_status[agentid]->property("Status").toString();
    QString oldpstatus = m_agent_pause_status[agentid]->property("Paused").toString();
    
    QString displaysstatus;
    QString displaypstatus;
    
    if(sstatus != oldsstatus) {
        if (sstatus == "") {
            displaysstatus = tr("Not in Queue");
        } else if (sstatus == "1") {
            if(dynstatus == "dynamic") {
                displaysstatus = tr("In Queue");
            } else if(dynstatus == "static") {
                displaysstatus = tr("In Queue (stat)");
            } else {
                displaysstatus = tr("In Queue (%1)").arg(dynstatus);
            }
        } else if (sstatus == "3") {
            if(dynstatus == "dynamic") {
                displaysstatus = tr("Called/Busy");
            } else if(dynstatus == "static") {
                displaysstatus = tr("Called/Busy (stat)");
            } else {
                displaysstatus = tr("Called/Busy (%1)").arg(dynstatus);
            }
        } else if (sstatus == "4") {
            if(dynstatus == "dynamic") {
                displaysstatus = tr("Invalid");
            } else if(dynstatus == "static") {
                displaysstatus = tr("Invalid (stat)");
            } else {
                displaysstatus = tr("Invalid (%1)").arg(dynstatus);
            }
        } else if (sstatus == "5") {
            if(dynstatus == "dynamic") {
                displaysstatus = tr("In Queue NOT logged");
            } else if(dynstatus == "static") {
                displaysstatus = tr("In Queue (stat) NOT logged");
            } else {
                displaysstatus = tr("In Queue (%1) NOT logged").arg(dynstatus);
            }
        } else {
            displaysstatus = tr("Unknown %1").arg(sstatus);
        }
        m_agent_join_status[agentid]->setText(displaysstatus);
        m_agent_join_status[agentid]->setProperty("Status", sstatus);
    }
    
    if(pstatus != oldpstatus) {
        if(pstatus == "0") {
            displaypstatus = tr("Not paused");
        } else if(pstatus == "1") {
            displaypstatus = tr("Paused");
        } else if(pstatus == "") {
            displaypstatus = tr("Not relevant");
        } else {
            displaypstatus = tr("Unknown %1").arg(pstatus);
        }
        m_agent_pause_status[agentid]->setText(displaypstatus);
        m_agent_pause_status[agentid]->setProperty("Paused", pstatus);
    }
    
    if(qv.toMap().contains("CallsTaken"))
        m_agent_callstaken[agentid]->setText(qv.toMap()["CallsTaken"].toString());
    else
        m_agent_callstaken[agentid]->setText("0");
    
    QString slastcall = "-";
    if(qv.toMap().contains("LastCall")) {
        QDateTime lastcall;
        int epoch = qv.toMap()["LastCall"].toInt();
        if(epoch > 0) {
            lastcall.setTime_t(epoch);
            slastcall = lastcall.toString();
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
    // UserInfo * ui = m_engine->findUserFromPhone(m_monitored_astid, agent_channel);
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

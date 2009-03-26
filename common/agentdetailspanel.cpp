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
#include <QContextMenuEvent>
#include <QFileDialog>
#include <QGridLayout>
#include <QLabel>
#include <QMenu>
#include <QPushButton>
#include <QScrollArea>
#include <QVariant>

#include "baseengine.h"
#include "agentdetailspanel.h"
#include "agentinfo.h"
#include "queueinfo.h"
#include "userinfo.h"

const QColor Orange = QColor(255, 128, 0);

/*! \brief Constructor
 */
AgentdetailsPanel::AgentdetailsPanel(BaseEngine * engine,
                                     const QVariant & options,
                                     QWidget * parent)
    : QWidget(parent), m_engine(engine)
{
    m_linenum = 0;
    m_gridlayout = new QGridLayout(this);
    
    m_monitored_agentnumber = "";
    m_agentstatus = new QLabel(this);
    m_agentlegend_qname = new QLabel(tr("Queues"), this);
    m_agentlegend_joined = new QLabel(tr("Joined"), this);
    m_agentlegend_paused = new QLabel(tr("Paused"), this);
    m_agentlegend_njoined = new QLabel("0", this);
    m_agentlegend_npaused = new QLabel("0", this);
    
    m_actionlegends["record"] = new QLabel(tr("Record"), this);
    m_actionlegends["alogin"] = new QLabel(tr("Login"), this);
    
    foreach (QString function, m_actionlegends.keys())
        m_action[function] = new QPushButton(this);
    m_action["record"]->setIconSize(QSize(10, 10));
    m_action["record"]->setIcon(QIcon(":/images/player_stop.png"));
    m_action["alogin"]->setIconSize(QSize(10, 10));
    m_action["alogin"]->setIcon(QIcon(":/images/button_ok.png"));
    
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
    setGuiOptions(options);
}

/*! \brief Destructor
 */
AgentdetailsPanel::~AgentdetailsPanel()
{
    // qDebug() << "AgentdetailsPanel::~AgentdetailsPanel()";
}

/*! \brief set options
 */
void AgentdetailsPanel::setGuiOptions(const QVariant & options)
{
    m_options = options;
}

/*! \brief 
 */
void AgentdetailsPanel::newQueueList(const QStringList &)
{
    // qDebug() << "AgentdetailsPanel::newQueueList()";
}

/*! \brief 
 */
void AgentdetailsPanel::newAgentList(const QStringList &)
{
    // qDebug() << "AgentdetailsPanel::newAgentList()" << m_monitored_agentid;
    if(m_engine->agents().contains(m_monitored_agentid))
        updatePanel();
}

/*! \brief 
 */
void AgentdetailsPanel::monitorThisAgent(const QString & agentid)
{
    if(m_engine->agents().contains(agentid)) {
        m_monitored_agentid = agentid;
        m_monitored_astid = m_engine->agents()[agentid]->astid();
        m_monitored_context = m_engine->agents()[agentid]->context();
        m_monitored_agentnumber = m_engine->agents()[agentid]->agentnumber();
        clearPanel();
        updatePanel();
    }
}

/*! \brief 
 */
void AgentdetailsPanel::clearPanel()
{
    // qDebug() << "AgentdetailsPanel::clearPanel()";
    foreach(QString q, m_queue_labels.keys())
        delete m_queue_labels[q];
    foreach(QString q, m_queue_more.keys())
        delete m_queue_more[q];
    foreach(QString q, m_queue_join_status.keys())
        delete m_queue_join_status[q];
    foreach(QString q, m_queue_join_action.keys())
        delete m_queue_join_action[q];
    foreach(QString q, m_queue_pause_status.keys())
        delete m_queue_pause_status[q];
    foreach(QString q, m_queue_pause_action.keys())
        delete m_queue_pause_action[q];
    
    m_queue_labels.clear();
    m_queue_more.clear();
    m_queue_join_status.clear();
    m_queue_join_action.clear();
    m_queue_pause_status.clear();
    m_queue_pause_action.clear();
}

/*! \brief 
 */
void AgentdetailsPanel::updatePanel()
{
    AgentInfo * ainfo = m_engine->agents()[m_monitored_agentid];
    QStringList agent_descriptions;
    agent_descriptions << QString("<b>%1</b> (%2)").arg(ainfo->agentnumber()).arg(ainfo->fullname());
    if(! m_options.toMap()["hideastid"].toBool())
        agent_descriptions << tr("on <b>%1</b>").arg(ainfo->astid());
    if(! m_options.toMap()["hidecontext"].toBool())
        agent_descriptions << QString("(%1)").arg(ainfo->context());
    QVariantMap properties = ainfo->properties();
    // qDebug() << "AgentdetailsPanel::updatePanel()" << ainfo->astid() << ainfo->agentnumber();
    QVariant agentstats = properties["agentstats"];
    QString lstatus = agentstats.toMap()["status"].toString();
    QString phonenum = agentstats.toMap()["agent_phone_number"].toString();
    QVariantMap queuesstats = properties["queues_by_agent"].toMap();
    
    if(lstatus == "AGENT_LOGGEDOFF") {
        agent_descriptions << tr("logged off <b>%1</b>").arg(phonenum);
        m_action["alogin"]->setProperty("function", "alogin");
        m_action["alogin"]->setIcon(QIcon(":/images/button_ok.png"));
        m_actionlegends["alogin"]->setText(tr("Login"));
    } else if(lstatus == "AGENT_IDLE") {
        agent_descriptions << tr("logged on phone number <b>%1</b>").arg(phonenum);
        m_action["alogin"]->setProperty("function", "alogout");
        m_action["alogin"]->setIcon(QIcon(":/images/cancel.png"));
        m_actionlegends["alogin"]->setText(tr("Logout"));
    } else if(lstatus == "AGENT_ONCALL") {
        // QString talkingto = agentstats.toMap()["talkingto"].toString();
        // agent_status = tr("logged (busy with %1) on phone number <b>%2</b>").arg(talkingto).arg(phonenum);
        agent_descriptions << tr("logged on phone number <b>%1</b>").arg(phonenum);
        m_action["alogin"]->setProperty("function", "alogout");
        m_action["alogin"]->setIcon(QIcon(":/images/cancel.png"));
        m_actionlegends["alogin"]->setText(tr("Logout"));
    } else
        qDebug() << "AgentdetailsPanel::newAgent() unknown status" << m_monitored_agentid << lstatus;
    
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
    
    QStringList queueids;
    m_agentlegend_njoined->setText(agentstats.toMap()["Xivo-NQJoined"].toString());
    m_agentlegend_npaused->setText(agentstats.toMap()["Xivo-NQPaused"].toString());
    
    QHashIterator<QString, QueueInfo *> iter = QHashIterator<QString, QueueInfo *>(m_engine->queues());
    while( iter.hasNext() ) {
        iter.next();
        QueueInfo * qinfo = iter.value();
        // newQueue(qinfo->astid(), qinfo->queuename(), qinfo->properties());
        QString queueid = iter.key();
        queueids << queueid;
        bool isnewqueue = false;
        if(! m_queue_labels.contains(queueid))
            isnewqueue = true;
        
        if(isnewqueue) {
            m_queue_labels[queueid] = new QLabel(this);
            m_queue_more[queueid] = new QPushButton(this);
            m_queue_join_status[queueid] = new QLabel(this);
            m_queue_join_action[queueid] = new QPushButton(this);
            m_queue_pause_status[queueid] = new QLabel(this);
            m_queue_pause_action[queueid] = new QPushButton(this);
            m_queue_join_status[queueid]->setProperty("Status", "undefined");
            m_queue_pause_status[queueid]->setProperty("Paused", "undefined");
            
            m_queue_join_status[queueid]->hide();
            m_queue_join_action[queueid]->hide();
            m_queue_pause_status[queueid]->hide();
            m_queue_pause_action[queueid]->hide();
        }
        
        setQueueLookProps(queueid);
        
        QString queuename = qinfo->queuename();
        // if(queuesstats.contains(queuename))
        setQueueProps(queueid, qinfo);
        // qDebug() << ainfo->astid() << ainfo->agentnumber() << queuename << qinfo->astid();
        if(qinfo->astid() == ainfo->astid())
                setQueueAgentProps(queueid, queuesstats[queuename]);
        
        if(isnewqueue)
            setQueueAgentSignals(queueid);
    }
    
    queueids.sort();
    int i = 0;
    foreach(QString queueid, queueids) {
        fillQueue(i, queueid);
        i ++;
    }
}

/*! \brief 
 */
void AgentdetailsPanel::setQueueLookProps(const QString & queueid)
{
    m_queue_more[queueid]->setIconSize(QSize(10, 10));
    m_queue_join_action[queueid]->setIconSize(QSize(8, 8));
    m_queue_pause_action[queueid]->setIconSize(QSize(8, 8));
    m_queue_more[queueid]->setIcon(QIcon(":/images/add.png"));
}

/*! \brief 
 */
void AgentdetailsPanel::setQueueProps(const QString & queueid, const QueueInfo * qinfo)
{
    m_queue_labels[queueid]->setText(qinfo->queuename());
    QStringList tooltips;
    if(! m_options.toMap()["hideastid"].toBool())
        tooltips << tr("Server: %1").arg(qinfo->astid());
    if(! m_options.toMap()["hidecontext"].toBool())
        tooltips << tr("Context: %1").arg(qinfo->context());
    m_queue_labels[queueid]->setToolTip(tooltips.join("\n"));
}

/*! \brief 
 */
void AgentdetailsPanel::setQueueAgentSignals(const QString & queueid)
{
    m_queue_more[queueid]->setProperty("queueid", queueid);
    m_queue_more[queueid]->setProperty("action", "changequeue");
    
    connect( m_queue_more[queueid], SIGNAL(clicked()),
             this, SLOT(queueClicked()));
    if(! m_options.toMap()["noqueueaction"].toBool()) {
        connect( m_queue_join_action[queueid], SIGNAL(clicked()),
                 this, SLOT(queueClicked()));
        connect( m_queue_pause_action[queueid], SIGNAL(clicked()),
                 this, SLOT(queueClicked()));
    }
}

/*! \brief 
 */
void AgentdetailsPanel::setQueueAgentProps(const QString & queueid, const QVariant & qv)
{
    m_queue_join_action[queueid]->setProperty("queueid", queueid);
    m_queue_join_action[queueid]->setProperty("action", "leavejoin");
    
    m_queue_pause_action[queueid]->setProperty("queueid", queueid);
    m_queue_pause_action[queueid]->setProperty("action", "pause");
    
    m_queue_join_status[queueid]->show();
    m_queue_pause_status[queueid]->show();
    
    QString pstatus = qv.toMap()["Paused"].toString();
    QString sstatus = qv.toMap()["Status"].toString();
    QString dynstatus = qv.toMap()["Membership"].toString();
    // CallsTaken, LastCall, Penalty
    
    QString oldsstatus = m_queue_join_status[queueid]->property("Status").toString();
    QString oldpstatus = m_queue_pause_status[queueid]->property("Paused").toString();
    
    QPixmap * p_square = new QPixmap(12, 12);
    if(sstatus != oldsstatus) {
        if (sstatus == "") {
            p_square->fill(Qt::gray);
            m_queue_join_status[queueid]->setToolTip(tr("Agent not in Queue"));
            m_queue_join_action[queueid]->setIcon(QIcon(":/images/button_ok.png"));
            m_queue_join_action[queueid]->show();
        } else if (sstatus == "1") {
            if(dynstatus == "dynamic") {
                p_square->fill(Qt::green);
                m_queue_join_status[queueid]->setToolTip(tr("Agent in Queue"));
                m_queue_join_action[queueid]->setIcon(QIcon(":/images/cancel.png"));
                m_queue_join_action[queueid]->show();
            } else if(dynstatus == "static") {
                p_square->fill(Qt::darkGreen);
                m_queue_join_status[queueid]->setToolTip(tr("Agent in Queue (statically)"));
                m_queue_join_action[queueid]->hide();
            } else {
                p_square->fill(Qt::black);
                m_queue_join_status[queueid]->setToolTip(tr("Agent in Queue (%1)").arg(dynstatus));
                m_queue_join_action[queueid]->hide();
            }
        } else if (sstatus == "3") {
            if(dynstatus == "dynamic") {
                p_square->fill(Qt::yellow);
                m_queue_join_status[queueid]->setToolTip(tr("Called/Busy"));
                m_queue_join_action[queueid]->setIcon(QIcon(":/images/cancel.png"));
                m_queue_join_action[queueid]->show();
            } else if(dynstatus == "static") {
                p_square->fill(Qt::darkYellow);
                m_queue_join_status[queueid]->setToolTip(tr("Called/Busy (statically)"));
                m_queue_join_action[queueid]->hide();
            } else {
                p_square->fill(Qt::black);
                m_queue_join_status[queueid]->setToolTip(tr("Called/Busy (%1)").arg(dynstatus));
                m_queue_join_action[queueid]->hide();
            }
        } else if (sstatus == "4") {
            if(dynstatus == "dynamic") {
                p_square->fill(Qt::red);
                m_queue_join_status[queueid]->setToolTip(tr("Agent in Queue but Invalid"));
                m_queue_join_action[queueid]->setIcon(QIcon(":/images/cancel.png"));
                m_queue_join_action[queueid]->show();
            } else if(dynstatus == "static") {
                p_square->fill(Qt::darkRed);
                m_queue_join_status[queueid]->setToolTip(tr("Agent in Queue but Invalid (statically)"));
                m_queue_join_action[queueid]->hide();
            } else {
                p_square->fill(Qt::black);
                m_queue_join_status[queueid]->setToolTip(tr("Agent in Queue but Invalid (%1)").arg(dynstatus));
                m_queue_join_action[queueid]->hide();
            }
        } else if (sstatus == "5") {
            if(dynstatus == "dynamic") {
                p_square->fill(Qt::blue);
                m_queue_join_status[queueid]->setToolTip(tr("Agent in Queue but NOT logged"));
                m_queue_join_action[queueid]->setIcon(QIcon(":/images/cancel.png"));
                m_queue_join_action[queueid]->show();
            } else if(dynstatus == "static") {
                p_square->fill(Qt::darkBlue);
                m_queue_join_status[queueid]->setToolTip(tr("Agent in Queue (statically) but NOT logged"));
                m_queue_join_action[queueid]->hide();
            } else {
                p_square->fill(Qt::black);
                m_queue_join_status[queueid]->setToolTip(tr("Agent in Queue (%1) but NOT logged").arg(dynstatus));
                m_queue_join_action[queueid]->hide();
            }
        } else {
            p_square->fill(Qt::black);
            m_queue_join_status[queueid]->setToolTip(tr("Unknown %1").arg(sstatus));
            m_queue_join_action[queueid]->hide();
        }
        m_queue_join_status[queueid]->setPixmap(* p_square);
        m_queue_join_status[queueid]->setProperty("Status", sstatus);
    }
    
    if(pstatus != oldpstatus) {
        if(pstatus == "0") {
            p_square->fill(Qt::green);
            m_queue_pause_status[queueid]->setToolTip(tr("Not paused"));
            m_queue_pause_action[queueid]->setIcon(QIcon(":/images/cancel.png"));
            m_queue_pause_action[queueid]->show();
        } else if(pstatus == "1") {
            p_square->fill(Orange);
            m_queue_pause_status[queueid]->setToolTip(tr("Paused"));
            m_queue_pause_action[queueid]->setIcon(QIcon(":/images/button_ok.png"));
            m_queue_pause_action[queueid]->show();
        } else if(pstatus == "") {
            p_square->fill(Qt::gray);
            m_queue_pause_status[queueid]->setToolTip(tr("Not relevant"));
            m_queue_pause_action[queueid]->hide();
        } else {
            p_square->fill(Qt::black);
            m_queue_pause_status[queueid]->setToolTip(tr("Unknown %1").arg(pstatus));
            m_queue_pause_action[queueid]->hide();
        }
        m_queue_pause_status[queueid]->setPixmap(* p_square);
        m_queue_pause_status[queueid]->setProperty("Paused", pstatus);
    }
}

/*! \brief 
 */
void AgentdetailsPanel::fillQueue(int ii, const QString & queueid)
{
    m_gridlayout->addWidget( m_queue_labels[queueid], ii + m_linenum, 0, Qt::AlignLeft );
    m_gridlayout->addWidget( m_queue_more[queueid], ii + m_linenum, 1, Qt::AlignCenter );
    m_gridlayout->addWidget( m_queue_join_status[queueid], ii + m_linenum, 2, Qt::AlignCenter );
    m_gridlayout->addWidget( m_queue_join_action[queueid], ii + m_linenum, 3, Qt::AlignCenter );
    m_gridlayout->addWidget( m_queue_pause_status[queueid], ii + m_linenum, 5, Qt::AlignCenter );
    m_gridlayout->addWidget( m_queue_pause_action[queueid], ii + m_linenum, 6, Qt::AlignCenter );
}

/*! \brief execute action on queue
 *
 * supports actions "changequeue", "leavejoin", "pause"
 */
void AgentdetailsPanel::queueClicked()
{
    // qDebug() << "AgentdetailsPanel::queueClicked()" << sender()->property("queueid");
    QString queueid = sender()->property("queueid").toString();
    QString action  = sender()->property("action").toString();
    
    QString astid = m_engine->queues()[queueid]->astid();
    QString queuename = m_engine->queues()[queueid]->queuename();
    QVariant mstatus = m_engine->agents()[m_monitored_agentid]->properties()["queues_by_agent"].toMap()[queuename];
    QString smstatus = mstatus.toMap()["Status"].toString();
    QString pmstatus = mstatus.toMap()["Paused"].toString();
    
    if(action == "changequeue")
        changeWatchedQueue(QString("queue:%1/%2").arg(astid).arg(queuename));
    else if(action == "leavejoin") {
        if((smstatus == "1") || (smstatus == "3") || (smstatus == "4") || (smstatus == "5"))
            emit agentAction(QString("leave %1 %2 %3").arg(queuename).arg(m_monitored_astid).arg(m_monitored_agentnumber));
        else if(smstatus == "")
            emit agentAction(QString("join %1 %2 %3 unpause").arg(queuename).arg(m_monitored_astid).arg(m_monitored_agentnumber));
        else
            qDebug() << "AgentdetailsPanel::queueClicked()" << queuename << m_monitored_astid << m_monitored_agentnumber << smstatus << pmstatus;
        // join the queue in the previously recorded paused status (to manage on the server side)
    } else if(action == "pause") {
        if(pmstatus == "0")
            emit agentAction(QString("pause %1 %2 %3").arg(queuename).arg(m_monitored_astid).arg(m_monitored_agentnumber));
        else if(pmstatus == "1")
            emit agentAction(QString("unpause %1 %2 %3").arg(queuename).arg(m_monitored_astid).arg(m_monitored_agentnumber));
        else
            qDebug() << "AgentdetailsPanel::queueClicked()" << queuename << m_monitored_astid << m_monitored_agentnumber << smstatus << pmstatus;
    } else
        qDebug() << "AgentdetailsPanel::queueClicked() : unknown action" << action;
}

/*! \brief left click actions (record, stoprecord, login, logout) */
void AgentdetailsPanel::actionClicked()
{
    // qDebug() << "AgentdetailsPanel::actionClicked()" << sender()->property("function").toString() << m_monitored_astid << m_monitored_agentnumber;
    QString function = sender()->property("function").toString();
    if(function == "record")
        emit agentAction(QString("record %1 %2").arg(m_monitored_astid).arg(m_monitored_agentnumber));
    else if(function == "stoprecord")
        emit agentAction(QString("stoprecord %1 %2").arg(m_monitored_astid).arg(m_monitored_agentnumber));
    else if(function == "alogin")
        emit agentAction(QString("login %1 %2").arg(m_monitored_astid).arg(m_monitored_agentnumber));
    else if(function == "alogout")
        emit agentAction(QString("logout %1 %2").arg(m_monitored_astid).arg(m_monitored_agentnumber));
}

/*! \brief triggerred on right click */
void AgentdetailsPanel::contextMenuEvent(QContextMenuEvent * event)
{
    // qDebug() << "AgentdetailsPanel::contextMenuEvent()" << event;
    m_eventpoint = event->globalPos();
    emit agentAction(QString("getfilelist %1 %2").arg(m_monitored_astid).arg(m_monitored_agentnumber));
}

/*! \brief display file list */
void AgentdetailsPanel::serverFileList(const QStringList & qsl)
{
    // qDebug() << "AgentdetailsPanel::serverFileList()" << qsl;
    QMenu contextMenu(this);
    foreach (QString filename, qsl) {
        QAction * action = new QAction(filename, this);
        action->setProperty("filename", filename);
        connect( action, SIGNAL(triggered()),
                 this, SLOT(getFile()) );
        contextMenu.addAction(action);
    }
    contextMenu.exec( m_eventpoint );
}

/*! \brief update Record/Stop Record buttons
 */
void AgentdetailsPanel::statusRecord(const QString & astid, const QString & agentid, const QString & status)
{
    // qDebug() << "AgentdetailsPanel::statusRecord()" << agentnum << m_monitored_agentnumber << status;
    QString gagentid = QString("agent:%1/%2").arg(astid).arg(agentid);
    if(gagentid == m_monitored_agentid) {
        if(status == "started") {
            m_action["record"]->setProperty("function", "stoprecord");
            m_action["record"]->setStyleSheet("QPushButton {background: #fbb638}");
        } else if(status == "stopped") {
            m_action["record"]->setProperty("function", "record");
            m_action["record"]->setStyleSheet("");
        }
    }
}

/*! \brief update Listen/Stop Listen buttons
 */
void AgentdetailsPanel::statusListen(const QString & astid, const QString & agentid, const QString & status)
{
    qDebug() << "AgentdetailsPanel::statusListen()" << astid << agentid << status;
}

/*! \brief retrieve a sound file
 */
void AgentdetailsPanel::getFile()
{
    // qDebug() << "AgentdetailsPanel::getFile()";
    QString filename = sender()->property("filename").toString();
    emit agentAction(QString("getfile %1 %2 %3").arg(m_monitored_astid).arg(m_monitored_agentnumber).arg(filename));
}

/*! \brief to save sound files
 *
 * open a QFileDialog and emit setFileName()
 */
void AgentdetailsPanel::saveToFile()
{
    // qDebug() << "AgentdetailsPanel::saveToFile()";
    QString selectedFilter;
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("Save Sound File"),
                                                    "",
                                                    tr("All Files (*)"),
                                                    &selectedFilter);
    if (!fileName.isEmpty())
        setFileName(fileName);
}

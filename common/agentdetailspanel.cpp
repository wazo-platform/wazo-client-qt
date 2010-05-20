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
#include <QContextMenuEvent>
#include <QFileDialog>
#include <QGridLayout>
#include <QLabel>
#include <QMenu>
#include <QPushButton>
#include <QVariant>

#include "xlet.h"
#include "baseengine.h"
#include "agentdetailspanel.h"
#include "queue_agent_status.h"
#include "agentinfo.h"
#include "queueinfo.h"
#include "userinfo.h"

/*! \brief Constructor
 */
AgentdetailsPanel::AgentdetailsPanel(BaseEngine * engine,
                                     QWidget * parent)
    : XLet(engine, parent)
{
    setTitle( tr("Agent Details") );
    m_linenum = 0;
    m_gridlayout = new QGridLayout(this);
    
    m_agentstatus = new QLabel(this);
    m_agentlegend_qname = new QLabel(tr("Queues"), this);
    m_agentlegend_joined = new QLabel(tr("Joined"), this);
    m_agentlegend_paused = new QLabel(tr("Paused"), this);
    m_agentlegend_njoined = new QLabel("0", this);
    m_agentlegend_npaused = new QLabel("0", this);
    
    m_actionlegends["record"] = new QLabel(tr("Record"), this);
    m_actionlegends["agentlogin"] = new QLabel(tr("Login"), this);
    
    foreach (QString function, m_actionlegends.keys())
        m_action[function] = new QPushButton(this);
    m_action["record"]->setIconSize(QSize(10, 10));
    m_action["record"]->setIcon(QIcon(":/images/player_stop.png"));
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
    setGuiOptions(m_engine->getGuiOptions("merged_gui"));
    
    // connect signal/slots with engine
    connect( m_engine, SIGNAL(newAgentList(const QStringList &)),
             this, SLOT(newAgentList(const QStringList &)) );
    connect( m_engine, SIGNAL(newQueueList(const QStringList &)),
             this, SLOT(newQueueList(const QStringList &)) );
    
    connect( m_engine, SIGNAL(changeWatchedAgentSignal(const QString &)),
             this, SLOT(monitorThisAgent(const QString &)) );
    connect( this, SIGNAL(changeWatchedQueue(const QString &)),
             m_engine, SLOT(changeWatchedQueueSlot(const QString &)) );
    
    connect( m_engine, SIGNAL(serverFileList(const QStringList &)),
             this, SLOT(serverFileList(const QStringList &)) );
    connect( m_engine, SIGNAL(fileReceived()),
             this, SLOT(saveToFile()) );
    connect( this, SIGNAL(setFileName(const QString &)),
             m_engine, SLOT(saveToFile(const QString &)) );
    connect( m_engine, SIGNAL(statusRecord(const QString &, const QString &, const QString &)),
             this, SLOT(statusRecord(const QString &, const QString &, const QString &)) );
    connect( m_engine, SIGNAL(statusListen(const QString &, const QString &, const QString &)),
             this, SLOT(statusListen(const QString &, const QString &, const QString &)) );
}

/*! \brief Destructor
 */
AgentdetailsPanel::~AgentdetailsPanel()
{
    // qDebug() << "AgentdetailsPanel::~AgentdetailsPanel()";
}

/*! \brief set options
 */
void AgentdetailsPanel::setGuiOptions(const QVariantMap & optionsMap)
{
    m_optionsMap = optionsMap;
}

/*! \brief 
 */
void AgentdetailsPanel::newQueueList(const QStringList &)
{
    // qDebug() << "AgentdetailsPanel::newQueueList()";
}

/*! \brief 
 */
void AgentdetailsPanel::newAgentList(const QStringList & qsl)
{
    // qDebug() << "AgentdetailsPanel::newAgentList()" << qsl;
    if(qsl.contains(m_monitored_agentid) && m_engine->agents().contains(m_monitored_agentid))
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
    if(! m_optionsMap["hideastid"].toBool())
        agent_descriptions << tr("on <b>%1</b>").arg(ainfo->astid());
    if(! m_optionsMap["hidecontext"].toBool())
        agent_descriptions << QString("(%1)").arg(ainfo->context());
    QVariantMap properties = ainfo->properties();
    // qDebug() << "AgentdetailsPanel::updatePanel()" << ainfo->astid() << ainfo->agentnumber();
    QVariant agentstats = properties["agentstats"];
    QString lstatus = agentstats.toMap()["status"].toString();
    QString phonenum = agentstats.toMap()["agent_phone_number"].toString();
    QVariantMap queuesstats = properties["queues_by_agent"].toMap();
    
    if(lstatus == "AGENT_LOGGEDOFF") {
        agent_descriptions << tr("logged off <b>%1</b>").arg(phonenum);
        m_action["agentlogin"]->setProperty("function", "agentlogin");
        m_action["agentlogin"]->setIcon(QIcon(":/images/button_ok.png"));
        m_actionlegends["agentlogin"]->setText(tr("Login"));
    } else if(lstatus == "AGENT_IDLE") {
        agent_descriptions << tr("logged on phone number <b>%1</b>").arg(phonenum);
        m_action["agentlogin"]->setProperty("function", "agentlogout");
        m_action["agentlogin"]->setIcon(QIcon(":/images/cancel.png"));
        m_actionlegends["agentlogin"]->setText(tr("Logout"));
    } else if(lstatus == "AGENT_ONCALL") {
        // QString talkingto = agentstats.toMap()["talkingto"].toString();
        // agent_status = tr("logged (busy with %1) on phone number <b>%2</b>").arg(talkingto).arg(phonenum);
        agent_descriptions << tr("logged on phone number <b>%1</b>").arg(phonenum);
        m_action["agentlogin"]->setProperty("function", "agentlogout");
        m_action["agentlogin"]->setIcon(QIcon(":/images/cancel.png"));
        m_actionlegends["agentlogin"]->setText(tr("Logout"));
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
        
        QString qid = qinfo->id();
        setQueueProps(queueid, qinfo);
        if(qinfo->astid() == ainfo->astid())
            setQueueAgentProps(queueid, queuesstats[qid]);
        
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
    if(! m_optionsMap["hideastid"].toBool())
        tooltips << tr("Server: %1").arg(qinfo->astid());
    if(! m_optionsMap["hidecontext"].toBool())
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
    if(! m_optionsMap["noqueueaction"].toBool()) {
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
    
    QString oldsstatus = m_queue_join_status[queueid]->property("Status").toString();
    QString oldpstatus = m_queue_pause_status[queueid]->property("Paused").toString();
    
    QString pstatus = qv.toMap()["Paused"].toString();
    QString sstatus = qv.toMap()["Status"].toString();
    QString dynstatus = qv.toMap()["Membership"].toString();
    // CallsTaken, LastCall, Penalty
    
    QueueAgentStatus * qas = new QueueAgentStatus();
    qas->update(dynstatus, sstatus, pstatus);
    
    QString joinicon = qas->display_action_join();
    if (joinicon.isEmpty())
        m_queue_join_action[queueid]->hide();
    else {
        m_queue_join_action[queueid]->setIcon(QIcon(joinicon));
        m_queue_join_action[queueid]->show();
    }
    
    if(sstatus != oldsstatus) {
        QPixmap square(12, 12);
        square.fill(qas->display_status_color());
        m_queue_join_status[queueid]->setPixmap(square);
        m_queue_join_status[queueid]->setToolTip(QString("%1\n%2\n%3")
                                                 .arg(qas->display_status_queue())
                                                 .arg(qas->display_status_logged())
                                                 .arg(qas->display_status_membership()));
        m_queue_join_status[queueid]->setProperty("Status", sstatus);
    }
    
    if(pstatus != oldpstatus) {
        QString pauseicon = qas->display_action_pause();
        if (pauseicon.isEmpty())
            m_queue_pause_action[queueid]->hide();
        else {
            m_queue_pause_action[queueid]->setIcon(QIcon(pauseicon));
            m_queue_pause_action[queueid]->show();
        }
        m_queue_pause_status[queueid]->setToolTip(qas->display_status_paused());
        QPixmap square(12, 12);
        square.fill(qas->display_status_paused_color());
        m_queue_pause_status[queueid]->setPixmap(square);
        m_queue_pause_status[queueid]->setProperty("Paused", pstatus);
    }
    
    delete qas;
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
    QString qid = m_engine->queues()[queueid]->id();
    QString queuename = m_engine->queues()[queueid]->queuename();
    QVariant mstatus = m_engine->agents()[m_monitored_agentid]->properties()["queues_by_agent"].toMap()[qid];
    QString smstatus = mstatus.toMap()["Status"].toString();
    QString pmstatus = mstatus.toMap()["Paused"].toString();
    
    QVariantMap ipbxcommand;
    ipbxcommand["agentids"] = m_monitored_agentid;
    ipbxcommand["queueids"] = queueid;
    
    if(action == "changequeue")
        changeWatchedQueue(queueid);
    else if(action == "leavejoin") {
        if((smstatus == "1") || (smstatus == "3") || (smstatus == "4") || (smstatus == "5")) {
            ipbxcommand["command"] = "agentleavequeue";
        } else if(smstatus == "") {
            ipbxcommand["command"] = "agentjoinqueue";
        } else
            qDebug() << "AgentdetailsPanel::queueClicked()" << queuename << m_monitored_agentid << smstatus << pmstatus;
        // join the queue in the previously recorded paused status (to manage on the server side)
    } else if(action == "pause") {
        if(pmstatus == "0") {
            ipbxcommand["command"] = "agentpausequeue";
        } else if(pmstatus == "1") {
            ipbxcommand["command"] = "agentunpausequeue";
        } else
            qDebug() << "AgentdetailsPanel::queueClicked()" << queuename << m_monitored_agentid << smstatus << pmstatus;
    } else
        qDebug() << "AgentdetailsPanel::queueClicked() : unknown action" << action;
    
    emit ipbxCommand(ipbxcommand);
}

/*! \brief left click actions (record, stoprecord, login, logout) */
void AgentdetailsPanel::actionClicked()
{
    // qDebug() << "AgentdetailsPanel::actionClicked()" << sender()->property("function").toString() << m_monitored_agentid;
    QString function = sender()->property("function").toString();
    QVariantMap ipbxcommand;
    if(function == "record") {
        ipbxcommand["command"] = "record";
        ipbxcommand["target"] = m_monitored_agentid;
        emit ipbxCommand(ipbxcommand);
    } else if(function == "stoprecord") {
        ipbxcommand["command"] = "stoprecord";
        ipbxcommand["target"] = m_monitored_agentid;
        emit ipbxCommand(ipbxcommand);
    } else if(function == "agentlogin") {
        ipbxcommand["command"] = "agentlogin";
        ipbxcommand["agentids"] = m_monitored_agentid;
        emit ipbxCommand(ipbxcommand);
    } else if(function == "agentlogout") {
        ipbxcommand["command"] = "agentlogout";
        ipbxcommand["agentids"] = m_monitored_agentid;
        emit ipbxCommand(ipbxcommand);
    }
}

/*! \brief triggerred on right click */
void AgentdetailsPanel::contextMenuEvent(QContextMenuEvent * event)
{
    // qDebug() << "AgentdetailsPanel::contextMenuEvent()" << event;
    m_eventpoint = event->globalPos();
    QVariantMap ipbxcommand;
    ipbxcommand["command"] = "getfilelist";
    ipbxcommand["agentid"] = m_monitored_agentid;
    emit ipbxCommand(ipbxcommand);
}

/*! \brief display file list */
void AgentdetailsPanel::serverFileList(const QStringList & qsl)
{
    // qDebug() << "AgentdetailsPanel::serverFileList()" << qsl;
    QMenu contextMenu(this);
    QStringList qsl_sorted = qsl;
    qsl_sorted.sort();
    if(qsl.size() > 0) {
        QAction * actiontitle = new QAction(tr("Record files for this agent"), this);
        contextMenu.addAction(actiontitle);
        contextMenu.addSeparator();
        foreach (QString filename, qsl_sorted) {
            QAction * action = new QAction(filename, this);
            action->setProperty("filename", filename);
            connect( action, SIGNAL(triggered()),
                     this, SLOT(getFile()) );
            contextMenu.addAction(action);
            delete action;
        }
        delete actiontitle;
    } else {
        QAction * action = new QAction(tr("No Record for this agent"), this);
        contextMenu.addAction(action);
        delete action;
    }
    contextMenu.exec( m_eventpoint );
}

/*! \brief update Record/Stop Record buttons
 */
void AgentdetailsPanel::statusRecord(const QString & astid, const QString & agentid, const QString & status)
{
    // qDebug() << "AgentdetailsPanel::statusRecord()" << agentnum << m_monitored_agentid << status;
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
    QVariantMap ipbxcommand;
    ipbxcommand["command"] = "getfile";
    ipbxcommand["agentid"] = m_monitored_agentid;
    ipbxcommand["filename"] = filename;
    emit ipbxCommand(ipbxcommand);
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

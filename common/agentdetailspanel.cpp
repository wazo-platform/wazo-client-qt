/* XIVO CTI clients
 * Copyright (C) 2007, 2008  Proformatique
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

#include "agentdetailspanel.h"

/*! \brief Constructor
 */
AgentdetailsPanel::AgentdetailsPanel(QWidget * parent)
        : QWidget(parent)
{
        m_linenum = 0;
	m_gridlayout = new QGridLayout(this);

        m_agent = "";
        m_agentname = new QLabel("", this);
        m_agentstatus = new QLabel("", this);
        m_agentlegend_qname = new QLabel(tr("Queue Name"), this);
        m_agentlegend_joined = new QLabel(tr("Joined"), this);
        m_agentlegend_paused = new QLabel(tr("UnPaused"), this);
        
        m_actionlegends["record"] = tr("Record");
        m_actionlegends["listen"] = tr("Listen");
        m_actionlegends["spy"] = tr("Spy");
        m_actionlegends["getfile"] = tr("Get File");
        
        foreach(QString function, m_actionlegends.keys())
                m_action[function] = new QPushButton(m_actionlegends[function]);
        m_gridlayout->setColumnStretch( 8, 1 );
        m_gridlayout->setRowStretch( 100, 1 );
        m_gridlayout->addWidget(m_agentname, m_linenum, 0);
        m_gridlayout->addWidget(m_agentstatus, m_linenum, 1, 1, 7);
        m_linenum ++;
        foreach(QString function, m_actionlegends.keys())
                m_gridlayout->addWidget(m_action[function], m_linenum ++, 0, Qt::AlignCenter);
        
        m_gridlayout->addWidget(m_agentlegend_qname, m_linenum, 0, Qt::AlignCenter);
        m_gridlayout->addWidget(m_agentlegend_joined, m_linenum, 1, 1, 3, Qt::AlignCenter);
        m_gridlayout->addWidget(m_agentlegend_paused, m_linenum, 4, 1, 3, Qt::AlignCenter);
        m_linenum ++;
        
        m_gridlayout->setVerticalSpacing(0);
        m_agentlegend_qname->hide();
        m_agentlegend_joined->hide();
        m_agentlegend_paused->hide();
        
        foreach(QString function, m_actionlegends.keys()) {
                m_action[function]->hide();
                m_action[function]->setProperty("function", function);
                connect( m_action[function], SIGNAL(clicked()),
                         this, SLOT(actionClicked()));
        }
}

AgentdetailsPanel::~AgentdetailsPanel()
{
        // qDebug() << "AgentdetailsPanel::~AgentdetailsPanel()";
}

void AgentdetailsPanel::updatePeerAgent(const QString &,
                                        const QString & what,
                                        const QStringList & params)
{
        if(what != "agentstatus")
                return;
        // qDebug() << "AgentdetailsPanel::updatePeerAgent()" << params << m_astid << m_agent;
        QString command = params[0];
        QString astid = params[1];
        QString agname = params[2];

        if(command == "agentlogin") {
                QString phonenum = params[3];
                if((m_agent == agname) && (m_astid == astid))
                        m_agentstatus->setText(tr("logged on phone number") + " <b>" + phonenum + "</b>");
        } else if(command == "agentlogout") {
                if((m_agent == agname) && (m_astid == astid))
                        m_agentstatus->setText(tr("logged off"));
        } else if(command == "joinqueue") {
                if((m_agent == agname) && (m_astid == astid)) {
                        QString qname = params[3];
                        if(m_queuelabels.contains(qname)) {
                                QPixmap * square = new QPixmap(12, 12);
                                square->fill(Qt::green);
                                m_queue_join_status[qname]->setPixmap(* square);
                                m_queue_join_status[qname]->setProperty("joined", "y");
                                m_queue_join_action[qname]->setIcon(QIcon(":/images/cancel.png"));

                                QString pstatus = params[4];
                                if(pstatus == "1") {
                                        square->fill(Qt::gray);
                                        m_queue_pause_status[qname]->setPixmap(* square);
                                        m_queue_pause_status[qname]->setProperty("paused", "y");
                                        m_queue_pause_action[qname]->setIcon(QIcon(":/images/button_ok.png"));
                                } else {
                                        square->fill(Qt::green);
                                        m_queue_pause_status[qname]->setPixmap(* square);
                                        m_queue_pause_status[qname]->setProperty("paused", "n");
                                        m_queue_pause_action[qname]->setIcon(QIcon(":/images/cancel.png"));
                                }
                                m_queue_pause_status[qname]->show();
                                m_queue_pause_action[qname]->show();
                        }
                }
        } else if(command == "leavequeue") {
                if((m_agent == agname) && (m_astid == astid)) {
                        QString qname = params[3];
                        if(m_queuelabels.contains(qname)) {
                                QPixmap * square = new QPixmap(12, 12);
                                square->fill(Qt::gray);
                                m_queue_join_status[qname]->setPixmap(* square);
                                m_queue_join_status[qname]->setProperty("joined", "n");
                                m_queue_join_action[qname]->setIcon(QIcon(":/images/button_ok.png"));

                                m_queue_pause_status[qname]->hide();
                                m_queue_pause_action[qname]->hide();
                        }
                }
        } else if(command == "paused") {
                if((m_agent == agname) && (m_astid == astid)) {
                        QString qname = params[3];
                        if(m_queuelabels.contains(qname)) {
                                QPixmap * square = new QPixmap(12, 12);
                                square->fill(Qt::gray);
                                m_queue_pause_status[qname]->setPixmap(* square);
                                m_queue_pause_status[qname]->setProperty("paused", "y");
                                m_queue_pause_action[qname]->setIcon(QIcon(":/images/button_ok.png"));
                        }
                }
        } else if(command == "unpaused") {
                if((m_agent == agname) && (m_astid == astid)) {
                        QString qname = params[3];
                        if(m_queuelabels.contains(qname)) {
                                QPixmap * square = new QPixmap(12, 12);
                                square->fill(Qt::green);
                                m_queue_pause_status[qname]->setPixmap(* square);
                                m_queue_pause_status[qname]->setProperty("paused", "n");
                                m_queue_pause_action[qname]->setIcon(QIcon(":/images/cancel.png"));
                        }
                }
        } else if(command == "queuememberstatus") {
                if((m_agent == agname) && (m_astid == astid)) {
                        QString qname = params[3];
                        if(m_queuelabels.contains(qname)) {
                                QString jstatus = params[4];
                                QString pstatus = params[5];

                                QPixmap * square = new QPixmap(12, 12);
                                if (jstatus == "1") {
                                        square->fill(Qt::green);
                                        m_queue_join_status[qname]->setPixmap(* square);
                                        m_queue_join_status[qname]->setProperty("joined", "y");
                                        m_queue_join_action[qname]->setIcon(QIcon(":/images/cancel.png"));
                                        
                                        if(pstatus == "1") {
                                                square->fill(Qt::gray);
                                                m_queue_pause_status[qname]->setPixmap(* square);
                                                m_queue_pause_status[qname]->setProperty("paused", "y");
                                                m_queue_pause_action[qname]->setIcon(QIcon(":/images/button_ok.png"));
                                        } else {
                                                square->fill(Qt::green);
                                                m_queue_pause_status[qname]->setPixmap(* square);
                                                m_queue_pause_status[qname]->setProperty("paused", "n");
                                                m_queue_pause_action[qname]->setIcon(QIcon(":/images/cancel.png"));
                                        }
                                        m_queue_pause_status[qname]->show();
                                        m_queue_pause_action[qname]->show();
                                } else if (jstatus == "5") {
                                        square->fill(Qt::gray);
                                        m_queue_join_status[qname]->setPixmap(* square);
                                        m_queue_join_status[qname]->setProperty("joined", "n");
                                        m_queue_join_action[qname]->setIcon(QIcon(":/images/button_ok.png"));
                                        
                                        m_queue_pause_status[qname]->hide();
                                        m_queue_pause_action[qname]->hide();
                                }
                        }
                }
        } else {
                // queuememberstatus
                qDebug() << "AgentdetailsPanel::updatePeerAgent()" << params;
        }
}

void AgentdetailsPanel::newAgent(const QStringList & agentstatus)
{
        // qDebug() << "AgentdetailsPanel::newAgent()" << agentstatus;
        m_astid = agentstatus[0];
        m_agent = agentstatus[1];
        QString longname = agentstatus[3];
        m_agentname->setText("<b>" + m_agent + "</b> (" + longname + ") " + tr("on") + " <b>" + m_astid + "</b>");

        QHashIterator<QString, QPushButton *> i(m_queuelabels);
        while (i.hasNext()) {
                i.next();
                delete i.value();
        }
        QHashIterator<QString, QLabel *> j(m_queue_join_status);
        while (j.hasNext()) {
                j.next();
                delete j.value();
        }
        QHashIterator<QString, QPushButton *> k(m_queue_join_action);
        while (k.hasNext()) {
                k.next();
                delete k.value();
        }
        QHashIterator<QString, QLabel *> l(m_queue_pause_status);
        while (l.hasNext()) {
                l.next();
                delete l.value();
        }
        QHashIterator<QString, QPushButton *> m(m_queue_pause_action);
        while (m.hasNext()) {
                m.next();
                delete m.value();
        }
        m_queuelabels.clear();
        m_queue_join_status.clear();
        m_queue_join_action.clear();
        m_queue_pause_status.clear();
        m_queue_pause_action.clear();
        // m_queuestatus.clear();

        if(agentstatus.size() == 6) {
                QString lstatus = agentstatus[2];
                QString phonenum = agentstatus[4];
                QString queues = agentstatus[5];
                
                if(lstatus == "0")
                        m_agentstatus->setText(tr("logged off") + " <b>" + phonenum + "</b>");
                else
                        m_agentstatus->setText(tr("logged on phone number") + " <b>" + phonenum + "</b>");
                
                m_agentlegend_qname->show();
                m_agentlegend_joined->show();
                m_agentlegend_paused->show();
                foreach(QString function, m_actionlegends.keys())
                        m_action[function]->show();
                
                QStringList queuesstats;
                if(queues.size() > 0)
                        queuesstats = queues.split(",");
                queuesstats.sort();
                
                int queuewidth = 0;
                for(int i = 0 ; i < queuesstats.size(); i++) {
                        QString queueid = queuesstats[i].split("-")[0];
                        QString pstatus = "";
                        QString sstatus = "";
                        if(queuesstats[i].split("-").size() > 2) {
                                pstatus = queuesstats[i].split("-")[1];
                                sstatus = queuesstats[i].split("-")[2];
                        }
                        m_queuelabels[queueid] = new QPushButton(queueid, this);
                        m_queuelabels[queueid]->setProperty("astid", m_astid);
                        m_queuelabels[queueid]->setProperty("queueid", queueid);
                        m_queuelabels[queueid]->setProperty("action", "changequeue");
                        connect( m_queuelabels[queueid], SIGNAL(clicked()),
                                 this, SLOT(queueClicked()));
                        if(m_queuelabels[queueid]->sizeHint().width() > queuewidth)
                                queuewidth = m_queuelabels[queueid]->sizeHint().width();
                        
                        m_queue_join_status[queueid] = new QLabel(this);
                        m_queue_join_action[queueid] = new QPushButton(this);
                        m_queue_join_action[queueid]->setProperty("astid", m_astid);
                        m_queue_join_action[queueid]->setProperty("queueid", queueid);
                        m_queue_join_action[queueid]->setProperty("agentid", m_agent);
                        m_queue_join_action[queueid]->setProperty("action", "leavejoin");
                        connect( m_queue_join_action[queueid], SIGNAL(clicked()),
                                 this, SLOT(queueClicked()));

                        m_queue_pause_status[queueid] = new QLabel(this);
                        m_queue_pause_action[queueid] = new QPushButton(this);
                        m_queue_pause_action[queueid]->setProperty("astid", m_astid);
                        m_queue_pause_action[queueid]->setProperty("queueid", queueid);
                        m_queue_pause_action[queueid]->setProperty("agentid", m_agent);
                        m_queue_pause_action[queueid]->setProperty("action", "pause");
                        connect( m_queue_pause_action[queueid], SIGNAL(clicked()),
                                 this, SLOT(queueClicked()));

                        m_queue_join_action[queueid]->setIconSize(QSize(8, 8));
                        m_queue_pause_action[queueid]->setIconSize(QSize(8, 8));

                        if (pstatus == "") {
                                QPixmap * square = new QPixmap(12, 12);
                                square->fill(Qt::gray);
                                m_queue_join_status[queueid]->setPixmap(* square);
                                m_queue_join_status[queueid]->setProperty("joined", "n");
                                m_queue_join_action[queueid]->setIcon(QIcon(":/images/button_ok.png"));
                                m_queue_pause_status[queueid]->hide();
                                m_queue_pause_action[queueid]->hide();
                        } else {
                                QPixmap * square = new QPixmap(12, 12);
                                square->fill(Qt::green);
                                m_queue_join_status[queueid]->setPixmap(* square);
                                m_queue_join_status[queueid]->setProperty("joined", "y");
                                m_queue_join_action[queueid]->setIcon(QIcon(":/images/cancel.png"));
                                if(pstatus == "0") {
                                        square->fill(Qt::green);
                                        m_queue_pause_status[queueid]->setPixmap(* square);
                                        m_queue_pause_status[queueid]->setProperty("paused", "n");
                                        m_queue_pause_action[queueid]->setIcon(QIcon(":/images/cancel.png"));
                                } else {
                                        square->fill(Qt::gray);
                                        m_queue_pause_status[queueid]->setPixmap(* square);
                                        m_queue_pause_status[queueid]->setProperty("paused", "y");
                                        m_queue_pause_action[queueid]->setIcon(QIcon(":/images/button_ok.png"));
                                }
                                m_queue_pause_status[queueid]->show();
                                m_queue_pause_action[queueid]->show();
                        }
                        m_gridlayout->addWidget( m_queuelabels[queueid], i + m_linenum, 0, Qt::AlignCenter );
                        m_gridlayout->addWidget( m_queue_join_status[queueid], i + m_linenum, 1, Qt::AlignCenter );
                        m_gridlayout->addWidget( m_queue_join_action[queueid], i + m_linenum, 2, Qt::AlignCenter );
                        m_gridlayout->addWidget( m_queue_pause_status[queueid], i + m_linenum, 4, Qt::AlignCenter );
                        m_gridlayout->addWidget( m_queue_pause_action[queueid], i + m_linenum, 5, Qt::AlignCenter );
                }
                
                foreach(QString queueid, m_queuelabels.keys())
                        m_queuelabels[queueid]->setMinimumWidth(queuewidth);
        }
}

void AgentdetailsPanel::queueClicked()
{
        // qDebug() << "AgentdetailsPanel::queueClicked()" << sender()->property("queueid");
        QString astid   = sender()->property("astid").toString();
        QString queueid = sender()->property("queueid").toString();
        QString action  = sender()->property("action").toString();

        if(action == "changequeue")
                changeWatchedQueue(astid + " " + queueid);
        else if(action == "leavejoin") {
                QString agentid = sender()->property("agentid").toString();
                QString jprop = m_queue_join_status[queueid]->property("joined").toString();
                if(jprop == "y")
                        agentAction("leave " + queueid + " " + astid + " " + agentid);
                else {
                        QString pprop = m_queue_pause_status[queueid]->property("paused").toString();
                        // join the queue in the previously recorded paused status
                        if(pprop == "y")
                                agentAction("join " + queueid + " " + astid + " " + agentid + " pause");
                        else
                                agentAction("join " + queueid + " " + astid + " " + agentid + " unpause");
                }
        } else if(action == "pause") {
                QString agentid = sender()->property("agentid").toString();
                QString pprop = m_queue_pause_status[queueid]->property("paused").toString();
                if(pprop == "y")
                        agentAction("unpause " + queueid + " " + astid + " " + agentid);
                else
                        agentAction("pause " + queueid + " " + astid + " " + agentid);
        } else
                qDebug() << "AgentdetailsPanel::queueClicked() : unknown action" << action;
}

void AgentdetailsPanel::actionClicked()
{
        // qDebug() << "AgentdetailsPanel::actionClicked()" << sender()->property("function").toString() << m_astid << m_agent;
        QString function = sender()->property("function").toString();
        if(function == "record")
                agentAction("record " + m_astid + " " + m_agent);
        else if(function == "listen")
                agentAction("listen " + m_astid + " " + m_agent);
        else if(function == "spy")
                agentAction("spy " + m_astid + " " + m_agent);
        else if(function == "getfile")
                agentAction("getfile " + m_astid + " " + m_agent);
        else if(function == "getfilelist")
                agentAction("getfilelist " + m_astid + " " + m_agent);
}

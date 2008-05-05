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
	m_gridlayout = new QGridLayout(this);

        m_agent = "";
        m_agentname = new QLabel("", this);
        m_agentstatus = new QLabel("", this);
        m_agentlegend_joined = new QLabel(tr("Joined"), this);
        m_agentlegend_paused = new QLabel(tr("Unpaused"), this);
        m_gridlayout->setColumnStretch( 8, 1 );
        m_gridlayout->setRowStretch( 100, 1 );
        m_gridlayout->addWidget(m_agentname, 0, 0);
        m_gridlayout->addWidget(m_agentstatus, 0, 1, 1, 7);
        m_gridlayout->addWidget(m_agentlegend_joined, 1, 2, 1, 3);
        m_gridlayout->addWidget(m_agentlegend_paused, 1, 5, 1, 3);
        m_agentlegend_joined->hide();
        m_agentlegend_paused->hide();
}

AgentdetailsPanel::~AgentdetailsPanel()
{
        // qDebug() << "AgentdetailsPanel::~AgentdetailsPanel()";
}

void AgentdetailsPanel::updatePeerAgent(const QString &, const QString & agentstatus)
{
        // qDebug() << "AgentdetailsPanel::updatePeerAgent()" << agentstatus;
        QStringList params = agentstatus.split("/");
        QString command = params[0];
        if(command == "agentlogin") {
                QString agname = params[2];
                QString phonenum = params[3];
                if(m_agent == agname)
                        m_agentstatus->setText("logged on phone number <b>" + phonenum + "</b>");
        } else if(command == "agentlogout") {
                QString agname = params[2];
                if(m_agent == agname)
                        m_agentstatus->setText("logged off");
        } else if(command == "joinqueue") {
                QString agname = params[2];
                if(m_agent == agname) {
                        QString qname = params[3];
                        if(m_queuelabels.contains(qname)) {
                                QPixmap * square = new QPixmap(12, 12);
                                square->fill(Qt::green);
                                m_queue_join_status[qname]->setPixmap(* square);
                                m_queue_join_status[qname]->setProperty("joined", "y");
                                m_queue_join_action[qname]->setIcon(QIcon(":/images/cancel.png"));

                                square->fill(Qt::gray);
                                m_queue_pause_status[qname]->setPixmap(* square);
                                m_queue_pause_action[qname]->setIcon(QIcon(":/images/button_ok.png"));

                                m_queue_pause_status[qname]->show();
                                m_queue_pause_action[qname]->show();
                        }
                }
        } else if(command == "leavequeue") {
                QString agname = params[2];
                if(m_agent == agname) {
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
        }
}

void AgentdetailsPanel::newAgent(const QStringList & agentstatus)
{
        //  qDebug() << "AgentdetailsPanel::newAgent()" << agentstatus;
        m_agent = agentstatus[1];
        m_agentname->setText("<b>" + agentstatus[1] + "</b>");

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


        if(agentstatus.size() == 5) {
                if(agentstatus[2] == "0")
                        m_agentstatus->setText("logged off");
                else
                        m_agentstatus->setText("logged on phone number <b>" + agentstatus[2] + "</b>");

                m_agentlegend_joined->show();
                m_agentlegend_paused->show();

                QStringList queuesstats;
                if(agentstatus[3].size() > 0)
                        queuesstats << agentstatus[3].split(",");
                if(agentstatus[4].size() > 0)
                        queuesstats << agentstatus[4].split(",");
                queuesstats.sort();

                for(int i = 0 ; i < queuesstats.size(); i++) {
                        QString queue = queuesstats[i].split(":")[0];
                        m_queuelabels[queue] = new QPushButton(queue, this);
                        m_queuelabels[queue]->setProperty("queueid", queue);
                        m_queuelabels[queue]->setProperty("action", "changequeue");
                        connect( m_queuelabels[queue], SIGNAL(clicked()),
                                 this, SLOT(queueClicked()));

                        m_queue_join_status[queue] = new QLabel(this);
                        m_queue_join_action[queue] = new QPushButton(this);
                        m_queue_join_action[queue]->setProperty("queueid", queue);
                        m_queue_join_action[queue]->setProperty("agentid", m_agent);
                        m_queue_join_action[queue]->setProperty("action", "leavejoin");
                        connect( m_queue_join_action[queue], SIGNAL(clicked()),
                                 this, SLOT(queueClicked()));

                        m_queue_pause_status[queue] = new QLabel(this);
                        m_queue_pause_action[queue] = new QPushButton(this);

                        m_queue_join_action[queue]->setIconSize(QSize(8, 8));
                        m_queue_pause_action[queue]->setIconSize(QSize(8, 8));

                        if(agentstatus[4].split(",").contains(queue)) {
                                QPixmap * square = new QPixmap(12, 12);
                                square->fill(Qt::gray);
                                m_queue_join_status[queue]->setPixmap(* square);
                                m_queue_join_status[queue]->setProperty("joined", "n");
                                m_queue_join_action[queue]->setIcon(QIcon(":/images/button_ok.png"));
                                m_queue_pause_status[queue]->hide();
                                m_queue_pause_action[queue]->hide();
                        } else if(agentstatus[3].split(",").contains(queuesstats[i])) {
                                QPixmap * square = new QPixmap(12, 12);
                                square->fill(Qt::green);
                                m_queue_join_status[queue]->setPixmap(* square);
                                m_queue_join_status[queue]->setProperty("joined", "y");
                                m_queue_join_action[queue]->setIcon(QIcon(":/images/cancel.png"));
                                if(queuesstats[i].split(":")[1] == "0") {
                                        square->fill(Qt::green);
                                        m_queue_pause_status[queue]->setPixmap(* square);
                                        m_queue_pause_action[queue]->setIcon(QIcon(":/images/cancel.png"));
                                } else {
                                        square->fill(Qt::gray);
                                        m_queue_pause_status[queue]->setPixmap(* square);
                                        m_queue_pause_action[queue]->setIcon(QIcon(":/images/button_ok.png"));
                                }
                                m_queue_pause_status[queue]->show();
                                m_queue_pause_action[queue]->show();
                        }
                        m_gridlayout->addWidget( m_queuelabels[queue], i + 2, 1, Qt::AlignCenter );
                        m_gridlayout->addWidget( m_queue_join_status[queue], i + 2, 2, Qt::AlignCenter );
                        m_gridlayout->addWidget( m_queue_join_action[queue], i + 2, 3, Qt::AlignCenter );
                        m_gridlayout->addWidget( m_queue_pause_status[queue], i + 2, 5, Qt::AlignCenter );
                        m_gridlayout->addWidget( m_queue_pause_action[queue], i + 2, 6, Qt::AlignCenter );
                }
        }
}

void AgentdetailsPanel::queueClicked()
{
        // qDebug() << "AgentdetailsPanel::queueClicked()" << this->sender()->property("queueid");
        QString queueid = this->sender()->property("queueid").toString();
        QString action = this->sender()->property("action").toString();
        if(action == "changequeue")
                changeWatchedQueue(queueid);
        else {
                QString prop = m_queue_join_status[queueid]->property("joined").toString();
                if(prop == "y")
                        agentAction("leave " + queueid + " xivo " + m_agent);
                else
                        agentAction("join " + queueid + " xivo " + m_agent);
        }
}

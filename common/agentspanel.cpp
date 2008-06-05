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
#include <QFrame>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QVariant>

#include "agentspanel.h"
#include "userinfo.h"

/*! \brief Constructor
 */
AgentsPanel::AgentsPanel(QWidget * parent)
        : QWidget(parent)
{
	m_gridlayout = new QGridLayout(this);

        m_title1 = new QLabel(tr("Agent"), this);
        m_title2 = new QLabel(tr("On Line"), this);
        m_title3 = new QLabel(tr("Present"), this);
        m_title4 = new QLabel(tr("Logged"), this);
        m_title5 = new QLabel(tr("Joined queues"), this);
        m_title6 = new QLabel(tr("UnPaused"), this);

        m_gridlayout->addWidget(m_title1, 0, 0, 1, 2, Qt::AlignCenter );
        m_gridlayout->addWidget(m_title2, 0, 2, Qt::AlignCenter );
        m_gridlayout->addWidget(m_title3, 0, 3, Qt::AlignCenter );
        m_gridlayout->addWidget(m_title4, 0, 5, 1, 2, Qt::AlignCenter );
        m_gridlayout->addWidget(m_title5, 0, 8, 1, 2, Qt::AlignCenter );
        m_gridlayout->addWidget(m_title6, 0, 12, 1, 3, Qt::AlignCenter );
        m_gridlayout->setColumnStretch( 15, 1 );
        m_gridlayout->setRowStretch( 100, 1 );
}

AgentsPanel::~AgentsPanel()
{
        // qDebug() << "AgentsPanel::~AgentsPanel()";
}

void AgentsPanel::setUserInfo(const UserInfo * ui)
{
        m_userinfo = ui;
}

void AgentsPanel::updateAgentPresence(const QString & agentname, const QString & presence)
{
        // qDebug() << "AgentsPanel::updateAgentPresence" << agentname << presence;
        if(agentname.size() > 0) {
                if(m_agent_presence.contains(agentname)) {
                        QPixmap * m_square = new QPixmap(12, 12);
                        if(presence == "available")
                                m_square->fill(Qt::green);
                        else
                                m_square->fill(Qt::gray);
                        m_agent_presence[agentname]->setPixmap(QPixmap(* m_square));
                }
        }
}

void AgentsPanel::updatePeerAgent(const QString &, const QString & agentstatus)
{
        // qDebug() << "AgentsPanel::updatePeerAgent()" << agentstatus;
        QStringList params = agentstatus.split("/");
        QString command = params[0];
        if(command == "queuememberstatus") {
                QString agname = params[2];
                if(m_agent_labels.contains(agname)) {
                        QString qname = params[3];
                        QString status = params[4];
                        if(status == "1") {
                                QPixmap * m_square = new QPixmap(12, 12);
                                m_square->fill(Qt::green);
                                m_agent_logged_status[agname]->setPixmap(QPixmap(* m_square));
                                m_agent_logged_status[agname]->setProperty("logged", "y");
                                m_agent_logged_action[agname]->setIcon(QIcon(":/images/cancel.png"));
                        } else if(status == "5") {
                                QPixmap * m_square = new QPixmap(12, 12);
                                m_square->fill(Qt::red);
                                m_agent_logged_status[agname]->setPixmap(QPixmap(* m_square));
                                m_agent_logged_status[agname]->setProperty("logged", "n");
                                m_agent_logged_action[agname]->setIcon(QIcon(":/images/button_ok.png"));
                        } else if(status == "3") {
                                qDebug() << "AgentsPanel::updatePeerAgent()" << agentstatus;
                        } else {
                                qDebug() << "AgentsPanel::updatePeerAgent()" << agentstatus;
                        }
                }
        } else if(command == "joinqueue") {
                QString astid = params[1];
                QString agname = params[2];
                if(m_agent_labels.contains(agname)) {
                        QString qname = params[3];
                        if(! m_agent_joined_list[agname].contains(qname)) {
                                QPixmap * m_square = new QPixmap(12, 12);
                                m_square->fill(Qt::green);
                                m_agent_joined_list[agname].append(qname);
                                m_agent_joined_number[agname]->setText(QString::number(m_agent_joined_list[agname].size()));
                                m_agent_joined_status[agname]->setPixmap(QPixmap(* m_square));
                        }
                }
        } else if(command == "leavequeue") {
                QString astid = params[1];
                QString agname = params[2];
                if(m_agent_labels.contains(agname)) {
                        QString qname = params[3];
                        if(m_agent_joined_list[agname].contains(qname)) {
                                QPixmap * m_square = new QPixmap(12, 12);
                                m_agent_joined_list[agname].removeAll(qname);
                                m_agent_joined_number[agname]->setText(QString::number(m_agent_joined_list[agname].size()));
                                if(m_agent_joined_list[agname].size() > 0)
                                        m_square->fill(Qt::green);
                                else
                                        m_square->fill(Qt::gray);
                                m_agent_joined_status[agname]->setPixmap(QPixmap(* m_square));
                        }
                }
        } else if(command == "agentlogin") {
                QString astid = params[1];
                QString agname = params[2];
                if(m_agent_labels.contains(agname)) {
                        QPixmap * m_square = new QPixmap(12, 12);
                        m_square->fill(Qt::green);
                        m_agent_logged_status[agname]->setPixmap(QPixmap(* m_square));
                        m_agent_logged_status[agname]->setProperty("logged", "y");
                        m_agent_logged_action[agname]->setIcon(QIcon(":/images/cancel.png"));
                }
        } else if(command == "agentlogout") {
                QString astid = params[1];
                QString agname = params[2];
                if(m_agent_labels.contains(agname)) {
                        QPixmap * m_square = new QPixmap(12, 12);
                        m_square->fill(Qt::red);
                        m_agent_logged_status[agname]->setPixmap(QPixmap(* m_square));
                        m_agent_logged_status[agname]->setProperty("logged", "n");
                        m_agent_logged_action[agname]->setIcon(QIcon(":/images/button_ok.png"));
                }
        } else if((command == "agentlink") || (command == "phonelink")) {
                QString astid = params[1];
                QString agname = params[2];
                QPixmap * m_square = new QPixmap(12, 12);
                m_square->fill(Qt::green);
                if(m_agent_busy.contains(agname))
                        m_agent_busy[agname]->setPixmap(QPixmap(* m_square));
        } else if((command == "agentunlink") || (command == "phoneunlink")) {
                QString astid = params[1];
                QString agname = params[2];
                QPixmap * m_square = new QPixmap(12, 12);
                m_square->fill(Qt::gray);
                if(m_agent_busy.contains(agname))
                        m_agent_busy[agname]->setPixmap(QPixmap(* m_square));
        }
}

void AgentsPanel::setAgentList(const QString & alist)
{
        qDebug() << "AgentsPanel::setAgentList()" << alist;
        QPixmap * m_square = new QPixmap(12, 12);
        QStringList asl = alist.split(";");
        if(asl.size() > 1) {
                QStringList agents;
                QString astid = asl[0];
                for(int i = 1 ; i < asl.size(); i++)
                        agents << asl[i];
                agents.sort();

                for(int i = 0 ; i < agents.size(); i++) {
                        QStringList ags = agents[i].split(":");
                        QString agnum = ags[0];
                        QString agstatus = ags[1];
                        QString agfullname = ags[2];
                        QString agqjoined = ags[3];
                        QString agqunjoined = ags[4];

                        if(! m_agent_labels.contains(agnum)) {
                                QFrame * qvline1 = new QFrame(this);
                                qvline1->setFrameShape(QFrame::VLine);
                                qvline1->setLineWidth(1);
                                QFrame * qvline2 = new QFrame(this);
                                qvline2->setFrameShape(QFrame::VLine);
                                qvline2->setLineWidth(1);
                                QFrame * qvline3 = new QFrame(this);
                                qvline3->setFrameShape(QFrame::VLine);
                                qvline3->setLineWidth(1);

                                m_agent_labels[agnum] = new QLabel(agfullname + " (" + agnum + ")", this);
                                m_agent_more[agnum] = new QPushButton(this);
                                m_agent_more[agnum]->setProperty("astid", astid);
                                m_agent_more[agnum]->setProperty("agentid", agnum);
                                m_agent_more[agnum]->setProperty("action", "changeagent");
                                connect( m_agent_more[agnum], SIGNAL(clicked()),
                                         this, SLOT(agentClicked()));

                                m_agent_busy[agnum] = new QLabel(this);
                                m_agent_presence[agnum] = new QLabel(this);
                                m_agent_logged_status[agnum] = new QLabel(this);
                                m_agent_logged_action[agnum] = new QPushButton(this);
                                m_agent_logged_action[agnum]->setProperty("astid", astid);
                                m_agent_logged_action[agnum]->setProperty("agentid", agnum);
                                m_agent_logged_action[agnum]->setProperty("action", "loginoff");
                                connect( m_agent_logged_action[agnum], SIGNAL(clicked()),
                                         this, SLOT(agentClicked()));

                                m_agent_joined_number[agnum] = new QLabel(this);
                                m_agent_joined_status[agnum] = new QLabel(this);
                                m_agent_paused_number[agnum] = new QLabel(this);
                                m_agent_paused_status[agnum] = new QLabel(this);
                                m_agent_paused_action[agnum] = new QPushButton(this);

                                m_agent_more[agnum]->setIconSize(QSize(10, 10));
                                m_agent_more[agnum]->setIcon(QIcon(":/images/rightarrow.png"));
                                m_agent_logged_action[agnum]->setIconSize(QSize(8, 8));
                                m_agent_paused_action[agnum]->setIconSize(QSize(8, 8));
                                
                                m_square->fill(Qt::gray);
                                m_agent_busy[agnum]->setPixmap(QPixmap(* m_square));
                                m_agent_presence[agnum]->setPixmap(QPixmap(* m_square));

                                if(agstatus == "1") {
                                        m_square->fill(Qt::green);
                                        m_agent_logged_action[agnum]->setIcon(QIcon(":/images/cancel.png"));
                                        m_agent_logged_status[agnum]->setProperty("logged", "y");
                                } else {
                                        m_square->fill(Qt::red);
                                        m_agent_logged_action[agnum]->setIcon(QIcon(":/images/button_ok.png"));
                                        m_agent_logged_status[agnum]->setProperty("logged", "n");
                                }
                                m_agent_logged_status[agnum]->setPixmap(QPixmap(* m_square));


                                if(agqjoined.size() > 0)
                                        m_agent_joined_list[agnum] = agqjoined.split(",");
                                int njoined = m_agent_joined_list[agnum].size();
                                if(njoined > 0) {
                                        m_square->fill(Qt::green);
                                } else {
                                        m_square->fill(Qt::gray);
                                }
                                m_agent_joined_number[agnum]->setText(QString::number(njoined));
                                m_agent_joined_status[agnum]->setPixmap(QPixmap(* m_square));


                                if(agqunjoined.size() > 0)
                                        m_agent_paused_list[agnum] = agqunjoined.split(",");
                                int nunpaused = m_agent_paused_list[agnum].size();
                                if(nunpaused > 0) {
                                        m_square->fill(Qt::green);
                                        m_agent_paused_action[agnum]->setIcon(QIcon(":/images/cancel.png"));
                                } else {
                                        m_square->fill(Qt::red);
                                        m_agent_paused_action[agnum]->setIcon(QIcon(":/images/button_ok.png"));
                                }
                                m_agent_paused_number[agnum]->setText(QString::number(nunpaused));
                                m_agent_paused_status[agnum]->setPixmap(QPixmap(* m_square));

                                int colnum = 0;
                                int linenum = m_agent_labels.size();
                                m_gridlayout->addWidget( m_agent_labels[agnum], linenum, colnum++, Qt::AlignLeft );
                                m_gridlayout->addWidget( m_agent_more[agnum], linenum, colnum++, Qt::AlignCenter );
                                m_gridlayout->addWidget( m_agent_busy[agnum], linenum, colnum++, Qt::AlignCenter );
                                m_gridlayout->addWidget( m_agent_presence[agnum], linenum, colnum++, Qt::AlignCenter );
                                m_gridlayout->addWidget( qvline1, linenum, colnum++, Qt::AlignLeft );
                                m_gridlayout->addWidget( m_agent_logged_status[agnum], linenum, colnum++, Qt::AlignCenter );
                                m_gridlayout->addWidget( m_agent_logged_action[agnum], linenum, colnum++, Qt::AlignLeft );
                                m_gridlayout->addWidget( qvline2, linenum, colnum++, Qt::AlignLeft );
                                m_gridlayout->addWidget( m_agent_joined_number[agnum], linenum, colnum++, Qt::AlignRight );
                                m_gridlayout->addWidget( m_agent_joined_status[agnum], linenum, colnum++, Qt::AlignCenter );
                                colnum++;
                                m_gridlayout->addWidget( qvline3, linenum, colnum++, Qt::AlignLeft );
                                m_gridlayout->addWidget( m_agent_paused_number[agnum], linenum, colnum++, Qt::AlignRight );
                                m_gridlayout->addWidget( m_agent_paused_status[agnum], linenum, colnum++, Qt::AlignCenter );
                                m_gridlayout->addWidget( m_agent_paused_action[agnum], linenum, colnum++, Qt::AlignLeft );
                        }
                }
        }
}


void AgentsPanel::agentClicked()
{
        // qDebug() << "AgentsPanel::agentClicked()" << this->sender()->property("agentid");
        QString astid = this->sender()->property("astid").toString();
        QString agentid = this->sender()->property("agentid").toString();
        QString action = this->sender()->property("action").toString();
        if(action == "changeagent")
                changeWatchedAgent(astid + " " + agentid);
        else if(action == "loginoff") {
                QString prop = m_agent_logged_status[agentid]->property("logged").toString();
                qDebug() << "loginoff" << astid << agentid << prop;
                if(prop == "y")
                        agentAction("logout " + astid + " " + agentid);
                else
                        agentAction("login " + astid + " " + agentid);
        }
}


void AgentsPanel::setAgentStatus(const QString & status)
{
        QStringList newstatuses = status.split("/");
        // qDebug() << "AgentsPanel::setAgentstatus()" << newstatuses;
        if (newstatuses.size() == 4) {
                QString command = newstatuses[0];
                if (command == "queuechannels") {
                        QString astid = newstatuses[1];
                        QString queuename = newstatuses[2];
                        QString busyness = newstatuses[3];
                }
        }
}

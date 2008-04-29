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

#include "agentspanel.h"

/*! \brief Constructor
 */
AgentsPanel::AgentsPanel(QWidget * parent)
        : QWidget(parent)
{
	m_layout = new QVBoxLayout(this);
        m_scrollarea = new QScrollArea(this);
	m_widget = new QWidget(this);
	m_scrollarea->setWidget(m_widget);
	m_gridlayout = new QGridLayout(m_widget);
        m_scrollarea->setWidgetResizable(true);
	m_layout->addWidget(m_scrollarea);

        m_title1 = new QLabel(tr("Agent"), this);
        m_title2 = new QLabel(tr("Busy"), this);
        m_title3 = new QLabel(tr("Logged"), this);
        m_title4 = new QLabel(tr("Joined queues"), this);
        m_title5 = new QLabel(tr("Paused"), this);

        m_maxbusy = 0;
        m_gridlayout->addWidget(m_title1, 0, 0, Qt::AlignCenter );
        m_gridlayout->addWidget(m_title2, 0, 1, Qt::AlignCenter );
        m_gridlayout->addWidget(m_title3, 0, 2, 1, 2, Qt::AlignCenter );
        m_gridlayout->addWidget(m_title4, 0, 4, 1, 3, Qt::AlignCenter );
        m_gridlayout->addWidget(m_title5, 0, 7, 1, 3, Qt::AlignCenter );
        m_gridlayout->setColumnStretch( 15, 1 );
        m_gridlayout->setRowStretch( 100, 1 );
}

AgentsPanel::~AgentsPanel()
{
        qDebug() << "AgentsPanel::~AgentsPanel()";
}

void AgentsPanel::setAgentList(const QString & alist)
{
        qDebug() << "AgentsPanel::setAgentList()" << alist;
        QPixmap * m_square = new QPixmap(8, 8);
        QStringList asl = alist.split(";");
        if(asl[1].size() > 0) {
                QStringList agents = asl[1].split(",");
                agents.sort();
                for(int i = 0 ; i < agents.size(); i++) {
                        QStringList ags = agents[i].split(":");
                        qDebug() << ags;
                        if(! m_agentlabels.keys().contains(ags[0])) {
                                m_agentlabels[ags[0]] = new QPushButton(ags[0], this);
                                m_agentlabels[ags[0]]->setProperty("agentid", ags[0]);
                                connect( m_agentlabels[ags[0]], SIGNAL(clicked()),
                                         this, SLOT(agentClicked()));

                                m_agent_busy[ags[0]] = new QLabel(this);
                                m_agent_logged_status[ags[0]] = new QLabel(this);
                                m_agent_logged_action[ags[0]] = new QPushButton(this);
                                m_agent_joined_number[ags[0]] = new QLabel(this);
                                m_agent_joined_status[ags[0]] = new QLabel(this);
                                m_agent_joined_action[ags[0]] = new QPushButton(this);
                                m_agent_paused_number[ags[0]] = new QLabel(this);
                                m_agent_paused_status[ags[0]] = new QLabel(this);
                                m_agent_paused_action[ags[0]] = new QPushButton(this);

                                m_agent_logged_action[ags[0]]->setIconSize(QSize(8, 8));
                                m_agent_joined_action[ags[0]]->setIconSize(QSize(8, 8));
                                m_agent_paused_action[ags[0]]->setIconSize(QSize(8, 8));


                                if(ags[1] == "1") {
                                        m_square->fill(Qt::green);
                                        m_agent_logged_action[ags[0]]->setIcon(QIcon(":/images/cancel.png"));
                                } else {
                                        m_square->fill(Qt::red);
                                        m_agent_logged_action[ags[0]]->setIcon(QIcon(":/images/button_ok.png"));
                                }
                                m_agent_logged_status[ags[0]]->setPixmap(QPixmap(* m_square));


                                if(ags[2].toInt() > 0) {
                                        m_square->fill(Qt::green);
                                        m_agent_joined_action[ags[0]]->setIcon(QIcon(":/images/cancel.png"));
                                } else {
                                        m_square->fill(Qt::red);
                                        m_agent_joined_action[ags[0]]->setIcon(QIcon(":/images/button_ok.png"));
                                }
                                m_agent_joined_number[ags[0]]->setText(ags[2]);
                                m_agent_joined_status[ags[0]]->setPixmap(QPixmap(* m_square));


                                if(ags[3].toInt() > 0) {
                                        m_square->fill(Qt::green);
                                        m_agent_paused_action[ags[0]]->setIcon(QIcon(":/images/cancel.png"));
                                } else {
                                        m_square->fill(Qt::red);
                                        m_agent_paused_action[ags[0]]->setIcon(QIcon(":/images/button_ok.png"));
                                }
                                m_agent_paused_number[ags[0]]->setText(ags[3]);
                                m_agent_paused_status[ags[0]]->setPixmap(QPixmap(* m_square));


                                m_gridlayout->addWidget( m_agentlabels[ags[0]], i + 1, 0, Qt::AlignCenter );
                                m_gridlayout->addWidget( m_agent_busy[ags[0]], i + 1, 1, Qt::AlignCenter );
                                m_gridlayout->addWidget( m_agent_logged_status[ags[0]], i + 1, 2, Qt::AlignCenter );
                                m_gridlayout->addWidget( m_agent_logged_action[ags[0]], i + 1, 3, Qt::AlignLeft );
                                m_gridlayout->addWidget( m_agent_joined_number[ags[0]], i + 1, 4, Qt::AlignRight );
                                m_gridlayout->addWidget( m_agent_joined_status[ags[0]], i + 1, 5, Qt::AlignCenter );
                                m_gridlayout->addWidget( m_agent_joined_action[ags[0]], i + 1, 6, Qt::AlignLeft );
                                m_gridlayout->addWidget( m_agent_paused_number[ags[0]], i + 1, 7, Qt::AlignRight );
                                m_gridlayout->addWidget( m_agent_paused_status[ags[0]], i + 1, 8, Qt::AlignCenter );
                                m_gridlayout->addWidget( m_agent_paused_action[ags[0]], i + 1, 9, Qt::AlignLeft );
                        }
                }
        }
}


void AgentsPanel::agentClicked()
{
        // qDebug() << "AgentsPanel::agentClicked()" << this->sender()->property("agentid");
        QString agentid = this->sender()->property("agentid").toString();
        changeWatchedAgent(agentid);
}


void AgentsPanel::setAgentStatus(const QString & status)
{
        QStringList newstatuses = status.split("/");
        qDebug() << "AgentsPanel::setAgentstatus()" << newstatuses;
        if (newstatuses.size() == 4) {
                QString command = newstatuses[0];
                if (command == "queuechannels") {
                        QString astid = newstatuses[1];
                        QString queuename = newstatuses[2];
                        QString busyness = newstatuses[3];
                }
        }
}

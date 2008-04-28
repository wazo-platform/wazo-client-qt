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

        m_maxbusy = 0;
        m_gridlayout->setColumnStretch( 5, 1 );
        m_gridlayout->setRowStretch( 100, 1 );
}

AgentsPanel::~AgentsPanel()
{
        qDebug() << "AgentsPanel::~AgentsPanel()";
}

void AgentsPanel::setAgentList(const QString & qlist)
{
        qDebug() << "AgentsPanel::setQueueList()" << qlist;
        QPixmap * m_square = new QPixmap(8, 8);
        QStringList qsl = qlist.split(";");
        if(qsl[1].size() > 0) {
                QStringList agents = qsl[1].split(",");
                agents.sort();
                for(int i = 0 ; i < agents.size(); i++) {
                        if(! m_agentlabels.keys().contains(agents[i])) {
                                m_square->fill(QColor(rand() % 256, rand() % 256, rand() % 256));
                                m_agentlabels[agents[i]] = new QPushButton(agents[i], this);
                                m_agentlabels[agents[i]]->setProperty("agentid", agents[i]);
                                connect( m_agentlabels[agents[i]], SIGNAL(clicked()),
                                         this, SLOT(agentClicked()));
                                m_agent_a[agents[i]] = new QPushButton(this);
                                m_agent_b[agents[i]] = new QPushButton(this);
                                m_agent_c[agents[i]] = new QPushButton(this);
                                m_agent_d[agents[i]] = new QPushButton(this);
                                m_agent_a[agents[i]]->setIconSize(QSize(8, 8));
                                m_agent_b[agents[i]]->setIconSize(QSize(8, 8));
                                m_agent_c[agents[i]]->setIconSize(QSize(8, 8));
                                m_agent_d[agents[i]]->setIconSize(QSize(8, 8));
                                m_agent_a[agents[i]]->setIcon(QIcon(QPixmap(* m_square)));
                                m_agent_b[agents[i]]->setIcon(QIcon(QPixmap(* m_square)));
                                m_agent_c[agents[i]]->setIcon(QIcon(QPixmap(* m_square)));
                                m_agent_d[agents[i]]->setIcon(QIcon(QPixmap(* m_square)));
                                m_gridlayout->addWidget( m_agentlabels[agents[i]], i, 0, Qt::AlignLeft );
                                m_gridlayout->addWidget( m_agent_a[agents[i]], i, 1, Qt::AlignCenter );
                                m_gridlayout->addWidget( m_agent_b[agents[i]], i, 2, Qt::AlignCenter );
                                m_gridlayout->addWidget( m_agent_c[agents[i]], i, 3, Qt::AlignCenter );
                                m_gridlayout->addWidget( m_agent_d[agents[i]], i, 4, Qt::AlignCenter );
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
                int maxbusy = 0;
                if (command == "queuechannels") {
                        QString astid = newstatuses[1];
                        QString queuename = newstatuses[2];
                        QString busyness = newstatuses[3];
                }
        }
}

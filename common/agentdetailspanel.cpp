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
	m_layout = new QVBoxLayout(this);
        m_scrollarea = new QScrollArea(this);
	m_widget = new QWidget(this);
	m_scrollarea->setWidget(m_widget);
	m_gridlayout = new QGridLayout(m_widget);
        m_scrollarea->setWidgetResizable(true);
	m_layout->addWidget(m_scrollarea);

        m_agentname = new QLabel("", this);
        m_agentstatus = new QLabel("", this);
        m_maxbusy = 0;
        m_gridlayout->setColumnStretch( 5, 1 );
        m_gridlayout->setRowStretch( 100, 1 );
        m_gridlayout->addWidget(m_agentname, 0, 0);
        m_gridlayout->addWidget(m_agentstatus, 0, 1);
}

AgentdetailsPanel::~AgentdetailsPanel()
{
        // qDebug() << "AgentdetailsPanel::~AgentdetailsPanel()";
}

void AgentdetailsPanel::newAgent(const QStringList & agentstatus)
{
        // qDebug() << "AgentdetailsPanel::newAgent()" << agentstatus;
        m_agentname->setText("<b>" + agentstatus[1] + "</b>");
        if(agentstatus.size() == 5) {
                if(agentstatus[2] == "0")
                        m_agentstatus->setText("off");
                else
                        m_agentstatus->setText("on");
                QStringList queues;
                if(agentstatus[3].size() > 0)
                        queues << agentstatus[3].split(",");
                if(agentstatus[4].size() > 0)
                        queues << agentstatus[4].split(",");
                queues.sort();

                for(int i = 0 ; i < queues.size(); i++) {
                        if(m_queuelabels.contains(queues[i])) {
                                delete m_queuelabels[queues[i]];
                                delete m_queuestatus[queues[i]];
                        }
                        m_queuelabels[queues[i]] = new QPushButton(queues[i], this);
                        m_queuelabels[queues[i]]->setProperty("queueid", queues[i]);
                        connect( m_queuelabels[queues[i]], SIGNAL(clicked()),
                                 this, SLOT(queueClicked()));
                        m_queuestatus[queues[i]] = new QLabel(this);
                        if(agentstatus[3].split(",").contains(queues[i]))
                                m_queuestatus[queues[i]]->setText("on");
                        if(agentstatus[4].split(",").contains(queues[i]))
                                m_queuestatus[queues[i]]->setText("off");
                        m_gridlayout->addWidget( m_queuelabels[queues[i]], i + 1, 1, Qt::AlignLeft );
                        m_gridlayout->addWidget( m_queuestatus[queues[i]], i + 1, 2, Qt::AlignLeft );
                }
        }
}

void AgentdetailsPanel::queueClicked()
{
        // qDebug() << "AgentdetailsPanel::queueClicked()" << this->sender()->property("queueid");
        QString queueid = this->sender()->property("queueid").toString();
        changeWatchedQueue(queueid);
}

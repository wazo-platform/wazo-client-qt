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

#include "queuedetailspanel.h"

/*! \brief Constructor
 */
QueuedetailsPanel::QueuedetailsPanel(QWidget * parent)
        : QWidget(parent)
{
	m_gridlayout = new QGridLayout(this);

        m_label = new QLabel("", this);
        m_maxbusy = 0;
        m_gridlayout->setColumnStretch( 5, 1 );
        m_gridlayout->setRowStretch( 100, 1 );
        m_gridlayout->addWidget(m_label, 0, 0);
}

QueuedetailsPanel::~QueuedetailsPanel()
{
        // qDebug() << "QueuedetailsPanel::~QueuedetailsPanel()";
}

void QueuedetailsPanel::updatePeerAgent(const QString &, const QString &, const QString &)
{
        // qDebug() << "QueuedetailsPanel::updatePeerAgent()";
}

void QueuedetailsPanel::newQueue(const QStringList & queuestatus)
{
        qDebug() << "QueuedetailsPanel::newQueue()" << queuestatus;
        m_label->setText("<b>" + queuestatus[1] + "</b>");
        QHashIterator<QString, QPushButton *> i(m_agentlabels);
        while (i.hasNext()) {
                i.next();
                delete i.value();
        }
        QHashIterator<QString, QLabel *> j(m_agentstatus);
        while (j.hasNext()) {
                j.next();
                delete j.value();
        }
        m_agentlabels.clear();
        m_agentstatus.clear();

        if(queuestatus.size() > 2) {
                QString astid = queuestatus[0];
                QStringList agents;
                for(int i = 2 ; i < queuestatus.size(); i++)
                        if(queuestatus[i].size() > 0)
                                agents << queuestatus[i].split(",")[0];
                agents.sort();

                for(int i = 0 ; i < agents.size(); i++) {
                        m_agentlabels[agents[i]] = new QPushButton(agents[i], this);
                        m_agentlabels[agents[i]]->setProperty("astid", astid);
                        m_agentlabels[agents[i]]->setProperty("agentid", agents[i]);
                        connect( m_agentlabels[agents[i]], SIGNAL(clicked()),
                                 this, SLOT(agentClicked()));
                        m_agentstatus[agents[i]] = new QLabel("", this);
                        m_gridlayout->addWidget( m_agentlabels[agents[i]], i + 1, 1, Qt::AlignLeft );
                        m_gridlayout->addWidget( m_agentstatus[agents[i]], i + 1, 2, Qt::AlignLeft );
                }
        }
}

void QueuedetailsPanel::agentClicked()
{
        qDebug() << "AgentsPanel::agentClicked()" << this->sender()->property("agentid");
        QString astid = this->sender()->property("astid").toString();
        QString agentid = this->sender()->property("agentid").toString();
        changeWatchedAgent(astid + " " + agentid);
}

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
        m_queuelegend_agentid = new QLabel(tr("Agent Id"), this);
        m_queuelegend_status = new QLabel(tr("Status"), this);
        m_maxbusy = 0;
        m_gridlayout->setColumnStretch( 5, 1 );
        m_gridlayout->setRowStretch( 100, 1 );
        m_gridlayout->addWidget(m_label, 0, 0);
        m_gridlayout->addWidget(m_queuelegend_agentid, 1, 0);
        m_gridlayout->addWidget(m_queuelegend_status, 1, 1);
        m_queuelegend_agentid->hide();
        m_queuelegend_status->hide();
}

QueuedetailsPanel::~QueuedetailsPanel()
{
        // qDebug() << "QueuedetailsPanel::~QueuedetailsPanel()";
}

void QueuedetailsPanel::updatePeerAgent(const QString &,
                                        const QString & what,
                                        const QStringList & params)
{
        if(what != "agentstatus")
                return;
        if(params.size() < 3)
                return;
        // qDebug() << "QueuedetailsPanel::updatePeerAgent()" << params;
        QString command = params[0];
        QString astid = params[1];
        QString agname = params[2];
        
        if(command == "joinqueue") {
                QString qname = params[3];
                if((astid == m_astid) && (qname == m_queueid)) {
                        if(! m_agentlist.contains(agname)) {
                                m_agentlist.append(agname);
                                m_agentlist.sort();
                                update();
                        }
                }
        } else if(command == "leavequeue") {
                QString qname = params[3];
                if((astid == m_astid) && (qname == m_queueid))
                        if(m_agentlist.contains(agname)) {
                                m_agentlist.removeAll(agname);
                                m_agentlist.sort();
                                update();
                        }
        } else if(command == "queuememberstatus") {
                QString qname = params[3];
                if((astid == m_astid) && (qname == m_queueid)) {
                        if(m_agentlist.contains(agname))
                                qDebug() << "qms c" << params;
                        else
                                qDebug() << "qms n" << params;
                }
        } else {
                qDebug() << "QueuedetailsPanel::updatePeerAgent()" << params;
        }
}

void QueuedetailsPanel::update()
{
        // qDebug() << "QueuedetailsPanel::update()";
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

        for(int i = 0 ; i < m_agentlist.size(); i++) {
                m_agentlabels[m_agentlist[i]] = new QPushButton(m_agentlist[i], this);
                m_agentlabels[m_agentlist[i]]->setProperty("agentid", m_agentlist[i]);
                connect( m_agentlabels[m_agentlist[i]], SIGNAL(clicked()),
                         this, SLOT(agentClicked()));
                m_agentstatus[m_agentlist[i]] = new QLabel("", this);
                m_gridlayout->addWidget( m_agentlabels[m_agentlist[i]], i + 2, 0, Qt::AlignLeft );
                m_gridlayout->addWidget( m_agentstatus[m_agentlist[i]], i + 2, 1, Qt::AlignLeft );
        }
}

void QueuedetailsPanel::newQueue(const QStringList & queuestatus)
{
        // qDebug() << "QueuedetailsPanel::newQueue()" << queuestatus;

        QStringList prevlist = m_agentlist;
        if(queuestatus.size() > 2) {
                int nagents = queuestatus[2].toInt();
                m_queuelegend_agentid->show();
                m_queuelegend_status->show();
                m_agentlist.clear();
                for(int i = 3 ; i < 3 + nagents; i++)
                        if(queuestatus[i].size() > 0) {
                                QString agname = queuestatus[i].split(",")[0];
                                if(agname.startsWith("Agent/"))
                                        m_agentlist << agname.mid(6);
                                else
                                        m_agentlist << agname;
                        }
                m_agentlist.sort();
        }

        if ((m_astid == queuestatus[0]) && (m_queueid == queuestatus[1])) {
                if (prevlist != m_agentlist)
                        update();
        } else {
                m_astid = queuestatus[0];
                m_queueid = queuestatus[1];
                m_label->setText("<b>" + m_queueid + "</b> " + tr("on") + " <b>" + m_astid + "</b>");
                update();
        }
}

void QueuedetailsPanel::agentClicked()
{
        // qDebug() << "QueuedetailsPanel::agentClicked()" << sender()->property("agentid");
        QString agentid = sender()->property("agentid").toString();
        changeWatchedAgent(m_astid + " " + agentid);
}

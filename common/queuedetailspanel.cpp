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
        m_queuelegend_agentid = new QLabel(tr("Agent"), this);
        m_queuelegend_status = new QLabel(tr("Status"), this);
        m_queuelegend_callstaken = new QLabel(tr("Calls Taken"), this);
        m_maxbusy = 0;
        m_gridlayout->setRowStretch( 100, 1 );
        m_gridlayout->addWidget(m_label, 0, 0);
        m_gridlayout->addWidget(m_queuelegend_agentid, 1, 0);
        m_gridlayout->addWidget(m_queuelegend_status, 1, 2);
        m_gridlayout->addWidget(m_queuelegend_callstaken, 1, 4);
        m_gridlayout->setColumnStretch( 5, 1 );
        m_gridlayout->setVerticalSpacing(0);
        m_queuelegend_agentid->hide();
        m_queuelegend_status->hide();
        m_queuelegend_callstaken->hide();
}

QueuedetailsPanel::~QueuedetailsPanel()
{
        // qDebug() << "QueuedetailsPanel::~QueuedetailsPanel()";
}

void QueuedetailsPanel::setGuiOptions(const QVariant &)
{
}

void QueuedetailsPanel::setUserInfo(const UserInfo *)
{
}

void QueuedetailsPanel::updatePeerAgent(int,
                                        const QString &,
                                        const QString & what,
                                        const QVariant & params)
{
        if(what != "agentstatus")
                return;
        // qDebug() << "QueuedetailsPanel::updatePeerAgent()" << params;
        QString action = params.toMap()["action"].toString();
        QString astid = params.toMap()["astid"].toString();
        QString agentnum = params.toMap()["agent_channel"].toString().mid(6);
        QString qname = params.toMap()["queuename"].toString();
        
        if(action == "joinqueue") {
                if((astid == m_astid) && (qname == m_queueid)) {
                        if(! m_agentlist.contains(agentnum)) {
                                m_agentlist.append(agentnum);
                                m_agentlist.sort();
                                update();
                        }
                }
        } else if(action == "leavequeue") {
                if((astid == m_astid) && (qname == m_queueid))
                        if(m_agentlist.contains(agentnum)) {
                                m_agentlist.removeAll(agentnum);
                                m_agentlist.sort();
                                update();
                        }
        } else if(action == "queuememberstatus") {
                if((astid == m_astid) && (qname == m_queueid)) {
                        if(m_agentlist.contains(agentnum))
                                qDebug() << "QueuedetailsPanel::updatePeerAgent()" << "qms c" << params;
                        else
                                qDebug() << "QueuedetailsPanel::updatePeerAgent()" << "qms n" << params;
                }
                // } else {
                // qDebug() << "QueuedetailsPanel::updatePeerAgent()" << params;
        }
}

void QueuedetailsPanel::update()
{
        // qDebug() << "QueuedetailsPanel::update()";
        QHashIterator<QString, QLabel *> i(m_agentlabels);
        while (i.hasNext()) {
                i.next();
                delete i.value();
        }
        QHashIterator<QString, QPushButton *> j(m_agentmore);
        while (j.hasNext()) {
                j.next();
                delete j.value();
        }
        QHashIterator<QString, QLabel *> k(m_agentstatus);
        while (k.hasNext()) {
                k.next();
                delete k.value();
        }
        m_agentlabels.clear();
        m_agentmore.clear();
        m_agentstatus.clear();

        for(int i = 0 ; i < m_agentlist.size(); i++) {
                QString agentnum = m_agentlist[i];
                QString agdisplay = agentnum;
                if(m_agentlists.contains(m_astid))
                        if(m_agentlists[m_astid].toMap().contains(agentnum))
                                agdisplay = m_agentlists[m_astid].toMap()[agentnum].toMap()["firstname"].toString()
                                        + " "
                                        + m_agentlists[m_astid].toMap()[agentnum].toMap()["lastname"].toString()
                                        + " (" + agentnum + ")";
                m_agentlabels[agentnum] = new QLabel(agdisplay, this);
                m_agentmore[agentnum] = new QPushButton(this);
                m_agentmore[agentnum]->setProperty("agentid", agentnum);
                m_agentmore[agentnum]->setIconSize(QSize(10, 10));
                m_agentmore[agentnum]->setIcon(QIcon(":/images/add.png"));
                connect( m_agentmore[agentnum], SIGNAL(clicked()),
                         this, SLOT(agentClicked()));
                m_agentstatus[agentnum] = new QLabel("", this);
                
                QFrame * qvline = new QFrame(this);
                qvline->setFrameShape(QFrame::VLine);
                qvline->setLineWidth(1);
                
                int colnum = 0;
                m_gridlayout->addWidget( m_agentlabels[agentnum], i + 2, colnum++, Qt::AlignLeft );
                m_gridlayout->addWidget( m_agentmore[agentnum], i + 2, colnum++, Qt::AlignCenter );
                m_gridlayout->addWidget( m_agentstatus[agentnum], i + 2, colnum++, Qt::AlignLeft );
                m_gridlayout->addWidget( qvline, i + 2, colnum++, Qt::AlignHCenter );
        }
}

void QueuedetailsPanel::setAgentList(int, const QVariant & alist)
{
        // qDebug() << "QueuedetailsPanel::setAgentList()" << timeref;
        QString astid = alist.toMap()["astid"].toString();
        m_agentlists[astid] = alist.toMap()["newlist"].toMap();
}

void QueuedetailsPanel::newQueue(const QString & astid, const QString & queueid, const QVariant & queuestatus)
{
        // qDebug() << "QueuedetailsPanel::newQueue()" << astid << queueid << queuestatus;
        QStringList prevlist = m_agentlist;
        m_queuelegend_agentid->show();
        m_queuelegend_status->show();
        m_queuelegend_callstaken->show();
        m_agentlist.clear();
        QVariantMap queuestatusmap = queuestatus.toMap();
        foreach(QString agname, queuestatusmap["agents"].toMap().keys()) {
                // qDebug() << agname
                // << queuestatus["agents"].toMap()[agname].toMap()["Status"].toString()
                // << queuestatus["agents"].toMap()[agname].toMap()["CallsTaken"].toString();
                // << queuestatus["agents"].toMap()[agname].toMap()["Paused"].toString();
                if(agname.startsWith("Agent/"))
                        m_agentlist << agname.mid(6);
                else
                        m_agentlist << agname;
        }
        m_agentlist.sort();
        
        if ((m_astid == astid) && (m_queueid == queueid)) {
                if (prevlist != m_agentlist)
                        update();
        } else {
                m_astid = astid;
                m_queueid = queueid;
                m_label->setText("<b>" + m_queueid + "</b> " + tr("on") + " <b>" + m_astid + "</b>");
                update();
        }
}

void QueuedetailsPanel::agentClicked()
{
        // qDebug() << "QueuedetailsPanel::agentClicked()" << sender()->property("agentid");
        QString agentid = sender()->property("agentid").toString();
        changeWatchedAgent(m_astid + " " + agentid, true);
}

/* XIVO CTI clients
 * Copyright (C) 2007-2009  Proformatique
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
#include <QScrollArea>
#include <QVariant>

#include "queueentrydetailspanel.h"

/*! \brief Constructor
 */
QueueentrydetailsPanel::QueueentrydetailsPanel(QWidget * parent)
        : QWidget(parent)
{
	m_gridlayout = new QGridLayout(this);

        m_label = new QLabel("", this);
        m_maxbusy = 0;
        m_gridlayout->setColumnStretch( 5, 1 );
        m_gridlayout->setRowStretch( 100, 1 );
        m_gridlayout->addWidget(m_label, 0, 0);
        startTimer(1000);
}

QueueentrydetailsPanel::~QueueentrydetailsPanel()
{
        // qDebug() << "QueueentrydetailsPanel::~QueueentrydetailsPanel()";
}

void QueueentrydetailsPanel::setGuiOptions(const QVariant &)
{
}

void QueueentrydetailsPanel::setUserInfo(const UserInfo *)
{
}

void QueueentrydetailsPanel::updatePeerAgent(double timeref,
                                             const QString &,
                                             const QString &,
                                             const QVariant &)
{
        m_timesrv = timeref;
        m_timeclt = QDateTime::currentDateTime();
}

void QueueentrydetailsPanel::newQueue(double timeref,
                                      const QString & astid,
                                      const QString & queueid,
                                      const QVariant & queuestatus)
{
        // qDebug() << "QueueentrydetailsPanel::newQueue()" << astid << queueid << queuestatus;
        m_astid = astid;
        m_queueid = queueid;
        m_label->setText(tr("<b>%1</b> on <b>%2</b>").arg(m_queueid).arg(m_astid));
        
        foreach(QString q, m_entrypos.keys())
                delete m_entrypos[q];
        foreach(QString q, m_entrytime.keys())
                delete m_entrytime[q];
        m_entrypos.clear();
        m_entrytime.clear();
        
        int k = 0;
        QVariantMap queuestatusmap = queuestatus.toMap();
        foreach(QString channel, queuestatusmap["entries"].toMap().keys()) {
                QVariantMap entryinfos = queuestatusmap["entries"].toMap()[channel].toMap();
                qDebug() << "QueueentrydetailsPanel::newQueue()" << astid << queueid << channel << entryinfos;
                
                m_entrypos[channel] = new QLabel(this);
                int time_spent = int(timeref - entryinfos["entrytime"].toDouble() + 0.5);
                m_entrypos[channel]->setProperty("entrytime", QDateTime::currentDateTime().addSecs(- time_spent));
                m_entrypos[channel]->setProperty("position", entryinfos["position"]);
                m_entrypos[channel]->setProperty("cidname", entryinfos["calleridname"]);
                m_entrypos[channel]->setProperty("cidnum", entryinfos["calleridnum"]);
                m_gridlayout->addWidget( m_entrypos[channel], k + 1, 0, Qt::AlignLeft );
                updateEntryChannel(channel);
                //m_gridlayout->addWidget( m_entrytime[entryname], k + 1, 1, Qt::AlignLeft );
                k ++;
        }
}

void QueueentrydetailsPanel::updateEntryChannel(const QString & channel)
{
        if(m_entrypos.contains(channel)) {
                QDateTime inittime = m_entrypos[channel]->property("entrytime").toDateTime();
                int nsec = inittime.secsTo(QDateTime::currentDateTime());
                m_entrypos[channel]->setText(QString("%1 : %2 %3 : %4 sec")
                                             .arg(m_entrypos[channel]->property("position").toString())
                                             .arg(m_entrypos[channel]->property("cidname").toString())
                                             .arg(m_entrypos[channel]->property("cidnum").toString())
                                             .arg(nsec));
        }
}

void QueueentrydetailsPanel::agentClicked()
{
        // qDebug() << "QueueentrydetailsPanel::agentClicked()" << sender()->property("agentid");
        QString astid = sender()->property("astid").toString();
        QString agentid = sender()->property("agentid").toString();
        changeWatchedAgent(QString("%1 %2").arg(astid).arg(agentid));
}

void QueueentrydetailsPanel::timerEvent(QTimerEvent *)
{
        // qDebug() << "QueueentrydetailsPanel::timerEvent()";
        foreach(QString channel, m_entrypos.keys())
                updateEntryChannel(channel);
}

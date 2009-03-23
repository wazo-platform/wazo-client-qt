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

#include "baseengine.h"
#include "queueentrydetailspanel.h"
#include "queueinfo.h"

/*! \brief Constructor
 */
QueueentrydetailsPanel::QueueentrydetailsPanel(BaseEngine * engine,
                                               const QVariant &, // options
                                               QWidget * parent)
    : QWidget(parent), m_engine(engine)
{
    m_gridlayout = new QGridLayout(this);
    
    m_queuedescription = new QLabel("", this);
    m_gridlayout->setColumnStretch( 5, 1 );
    m_gridlayout->setRowStretch( 100, 1 );
    m_gridlayout->addWidget(m_queuedescription, 0, 0);
    startTimer(1000);
}

/*! \brief Destructor
 */
QueueentrydetailsPanel::~QueueentrydetailsPanel()
{
    // qDebug() << "QueueentrydetailsPanel::~QueueentrydetailsPanel()";
}

/*! \brief 
 */
void QueueentrydetailsPanel::newQueueList()
{
    // qDebug() << "QueuedetailsPanel::newQueueList()";
    if(m_engine->queues().contains(m_monitored_queueid))
        updatePanel();
}

/*! \brief 
 */
void QueueentrydetailsPanel::newAgentList()
{
    // qDebug() << "QueuedetailsPanel::newAgentList()";
    if(m_engine->queues().contains(m_monitored_queueid))
        updatePanel();
}

/*! \brief 
 */
void QueueentrydetailsPanel::monitorThisQueue(const QString & queueid)
{
    // qDebug() << "QueueentrydetailsPanel::monitorThisQueue" << queueid;
    if(m_engine->queues().contains(queueid)) {
        m_monitored_queueid = queueid;
        m_monitored_astid = m_engine->queues()[queueid]->astid();
        m_monitored_context = m_engine->queues()[queueid]->context();
        m_monitored_queuename = m_engine->queues()[queueid]->queuename();
        updatePanel();
    }
}

/*! \brief 
 */
void QueueentrydetailsPanel::clearPanel()
{
    foreach(QString q, m_entrypos.keys())
        delete m_entrypos[q];
    foreach(QString q, m_entrytime.keys())
        delete m_entrytime[q];
    
    m_entrypos.clear();
    m_entrytime.clear();
}

/*! \brief update entries
 */
void QueueentrydetailsPanel::updatePanel()
{
    QueueInfo * qinfo = m_engine->queues()[m_monitored_queueid];
    QVariantMap properties = qinfo->properties();
    QVariantMap channels = properties["channels"].toMap();
    
    m_queuedescription->setText(tr("<b>%1</b> on <b>%2</b> (%3)").arg(qinfo->queuename()).arg(qinfo->astid()).arg(qinfo->context()));
    
    // queue legends
    clearPanel();
    
    foreach(QString channel, channels.keys()) {
        m_entrypos[channel] = new QLabel(this);
        updateEntryChannel(channel);
    }
}

void QueueentrydetailsPanel::updateEntryChannel(const QString & channel)
{
    QueueInfo * qinfo = m_engine->queues()[m_monitored_queueid];
    QVariantMap properties = qinfo->properties();
    QVariantMap channels = properties["channels"].toMap();
    
    if(m_entrypos.contains(channel)) {
        QVariantMap entryinfos = channels[channel].toMap();
        int time_spent = m_engine->timeServer() - entryinfos["entrytime"].toDouble(); // from the server point of view
        QDateTime inittime = m_engine->timeClient().addSecs(- time_spent); // from the client point of view
        int nsec = inittime.secsTo(QDateTime::currentDateTime());
        int position = entryinfos["position"].toInt();
        
        m_entrypos[channel]->setText(QString("%1 : %2 %3 : %4 sec")
                                     .arg(position)
                                     .arg(entryinfos["calleridname"].toString())
                                     .arg(entryinfos["calleridnum"].toString())
                                     .arg(nsec));
        m_gridlayout->addWidget( m_entrypos[channel], position, 0, Qt::AlignLeft );
    }
}

void QueueentrydetailsPanel::timerEvent(QTimerEvent *)
{
    // qDebug() << "QueueentrydetailsPanel::timerEvent()";
    foreach(QString channel, m_entrypos.keys())
        updateEntryChannel(channel);
}

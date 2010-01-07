/* XiVO Client
 * Copyright (C) 2007-2010, Proformatique
 *
 * This file is part of XiVO Client.
 *
 * XiVO Client is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version, with a Section 7 Additional
 * Permission as follows:
 *   This notice constitutes a grant of such permission as is necessary
 *   to combine or link this software, or a modified version of it, with
 *   the OpenSSL project's "OpenSSL" library, or a derivative work of it,
 *   and to copy, modify, and distribute the resulting work. This is an
 *   extension of the special permission given by Trolltech to link the
 *   Qt code with the OpenSSL library (see
 *   <http://doc.trolltech.com/4.4/gpl.html>). The OpenSSL library is
 *   licensed under a dual license: the OpenSSL License and the original
 *   SSLeay license.
 *
 * XiVO Client is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with XiVO Client.  If not, see <http://www.gnu.org/licenses/>.
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
                                               QWidget * parent)
    : XLet(engine, parent)
{
    setTitle( tr("Calls of a Queue") );
    m_gridlayout = new QGridLayout(this);
    
    m_queuedescription = new QLabel("", this);
    m_gridlayout->setColumnStretch( 5, 1 );
    m_gridlayout->setRowStretch( 100, 1 );
    m_gridlayout->addWidget(m_queuedescription, 0, 0);
    startTimer(1000);
    // connect signal slots to engine
    connect( m_engine, SIGNAL(newAgentList(const QStringList &)),
             this, SLOT(newAgentList(const QStringList &)) );
    connect( m_engine, SIGNAL(newQueueList(const QStringList &)),
             this, SLOT(newQueueList(const QStringList &)) );

    connect( m_engine, SIGNAL(changeWatchedQueueSignal(const QString &)),
             this, SLOT(monitorThisQueue(const QString &)) );
}

/*! \brief Destructor
 */
QueueentrydetailsPanel::~QueueentrydetailsPanel()
{
    // qDebug() << "QueueentrydetailsPanel::~QueueentrydetailsPanel()";
}

/*! \brief 
 */
void QueueentrydetailsPanel::newQueueList(const QStringList &)
{
    // qDebug() << "QueuedetailsPanel::newQueueList()";
    if(m_engine->queues().contains(m_monitored_queueid))
        updatePanel();
}

/*! \brief 
 */
void QueueentrydetailsPanel::newAgentList(const QStringList &)
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

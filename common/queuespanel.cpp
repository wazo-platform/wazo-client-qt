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
#include <QProgressBar>
#include <QScrollArea>
#include <QVariant>

#include "queuespanel.h"

/*! \brief Constructor
 */
QueuesPanel::QueuesPanel(QWidget * parent)
        : QWidget(parent)
{
	QVBoxLayout * layout = new QVBoxLayout(this);
        QScrollArea * scrollarea = new QScrollArea(this);
	QWidget * widget = new QWidget(this);
	scrollarea->setWidget(widget);
	m_gridlayout = new QGridLayout(widget);
        scrollarea->setWidgetResizable(true);
	layout->addWidget(scrollarea);

        m_maxbusy = 0;
 	m_gridlayout->setColumnStretch( 2, 1 );
 	m_gridlayout->setRowStretch( 100, 1 );
}

QueuesPanel::~QueuesPanel()
{
        // qDebug() << "QueuesPanel::~QueuesPanel()";
}

void QueuesPanel::updatePeerAgent(const QString & pname, const QString & agentstatus)
{
}

void QueuesPanel::setQueueList(const QString & qlist)
{
        // qDebug() << "QueuesPanel::setQueueList()" << qlist;
        QStringList qsl = qlist.split(";");
        if(qsl[2].size() > 0) {
                QStringList queues = qsl[2].split(",");
                queues.sort();
                for(int i = 0 ; i < queues.size(); i++) {
                        if(! m_queuelabels.keys().contains(queues[i])) {
                                m_queuelabels[queues[i]] = new QPushButton(queues[i], this);
                                m_queuelabels[queues[i]]->setProperty("queueid", queues[i]);
                                connect( m_queuelabels[queues[i]], SIGNAL(clicked()),
                                         this, SLOT(queueClicked()));
                                m_queuebusies[queues[i]] = new QProgressBar(this);
                                m_gridlayout->addWidget( m_queuelabels[queues[i]], i, 0, Qt::AlignLeft );
                                m_gridlayout->addWidget( m_queuebusies[queues[i]], i, 1, Qt::AlignCenter );
                                
                                m_queuebusies[queues[i]]->setFormat("%v");
                                m_queuebusies[queues[i]]->setRange(0, m_maxbusy + 1);
                                m_queuebusies[queues[i]]->setValue(0);
                        }
                }
        }
}

void QueuesPanel::queueClicked()
{
        // qDebug() << "QueuesPanel::queueClicked()" << this->sender()->property("queueid");
        QString queueid = this->sender()->property("queueid").toString();
        changeWatchedQueue(queueid);
}


void QueuesPanel::setQueueStatus(const QString & status)
{
        QStringList newstatuses = status.split("/");
        // qDebug() << "QueuesPanel::setQueueStatus()" << newstatuses;
        if (newstatuses.size() == 4) {
                QString command = newstatuses[0];
                int maxbusy = 0;
                if (command == "queuechannels") {
                        QString astid = newstatuses[1];
                        QString queuename = newstatuses[2];
                        QString busyness = newstatuses[3];
                        m_queuebusies[queuename]->setValue(busyness.toInt());
                        foreach (QProgressBar * qpb, m_queuebusies)
                                if(maxbusy < qpb->value())
                                        maxbusy = qpb->value();
                        if(maxbusy != m_maxbusy) {
                                m_maxbusy = maxbusy;
                                qDebug() << "maxbusy" << m_maxbusy;
                                foreach (QProgressBar * qpb, m_queuebusies) {
                                        qpb->setRange(0, m_maxbusy + 1);
                                        int value = qpb->value();
                                        qpb->setValue(0); // trick in order to refresh
                                        qpb->setValue(value);
                                }
                        }
                }
        }
}

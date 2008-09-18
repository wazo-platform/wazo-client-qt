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

#include "baseengine.h"
#include "queuespanel.h"
#include "servercommand.h"

const QString commonqss = "QProgressBar {border: 2px solid black;border-radius: 3px;text-align: center;width: 100px; height: 15px}";

/*! \brief Constructor
 */
QueuesPanel::QueuesPanel(BaseEngine * engine, QWidget * parent)
        : QWidget(parent),
          m_engine(engine)
{
	m_gridlayout = new QGridLayout(this);

        m_maxbusy = 0;

        m_title1 = new QLabel(tr("Queue"), this);
        m_title2 = new QLabel(tr("Busy"), this);
        m_title_infos_a = new QLabel(tr("Completed"), this);
        m_title_infos_b = new QLabel(tr("Abandoned"), this);
        m_title_infos_c = new QLabel(tr("HoldTime (s)"), this);
        m_title_infos = new QLabel(tr("Other Stats"), this);
        m_gridlayout->addWidget( m_title1, 0, 0, Qt::AlignCenter );
        m_gridlayout->addWidget( m_title2, 0, 1, Qt::AlignCenter );
        m_gridlayout->addWidget( m_title_infos_a, 0, 2, Qt::AlignCenter );
        m_gridlayout->addWidget( m_title_infos_b, 0, 3, Qt::AlignCenter );
        m_gridlayout->addWidget( m_title_infos_c, 0, 4, Qt::AlignCenter );
        m_gridlayout->addWidget( m_title_infos, 0, 5, Qt::AlignCenter );

 	m_gridlayout->setColumnStretch( 6, 1 );
 	m_gridlayout->setRowStretch( 100, 1 );
        m_gridlayout->setVerticalSpacing(0);
}

QueuesPanel::~QueuesPanel()
{
        // qDebug() << "QueuesPanel::~QueuesPanel()";
}

void QueuesPanel::setEngine(BaseEngine * engine)
{
	m_engine = engine;
}

void QueuesPanel::updatePeerAgent(const QString &,
                                  const QString &,
                                  const QStringList &)
{
}

void QueuesPanel::setQueueList(bool, const QString & qlist)
{
        // qDebug() << "QueuesPanel::setQueueList()" << qlist;
        ServerCommand * sc = new ServerCommand(qlist);
        QString astid = sc->getString("astid");
        QStringList queues = sc->getStringList("queuestats");
        
        if(queues.size() > 0) {
                queues.sort();
                for(int i = 0 ; i < queues.size(); i++) {
                        QStringList qparams = queues[i].split(":");
                        QHash <QString, QString> infos;
                        QString ncalls = "0";
                        infos["Calls"] = "0";
                        for(int j = 1 ; j < (qparams.size() - 1) / 2; j ++)
                                infos[qparams[j * 2 - 1]] = qparams[j * 2];
                        QString queuename = qparams[0];
                        if(! m_queuelabels.contains(queuename)) {
                                m_queuelabels[queuename] = new QPushButton(queuename, this);
                                m_queuelabels[queuename]->setProperty("astid", astid);
                                m_queuelabels[queuename]->setProperty("queueid", queuename);
                                connect( m_queuelabels[queuename], SIGNAL(clicked()),
                                         this, SLOT(queueClicked()));
                                m_queuebusies[queuename] = new QProgressBar(this);
                                m_queuebusies[queuename]->setProperty("queueid", queuename);
                                m_queuebusies[queuename]->setStyleSheet(commonqss + "QProgressBar::chunk {background-color: #ffffff;}");
                                m_queuebusies[queuename]->setFormat("%v");
                                m_queueinfos_a[queuename] = new QLabel();
                                m_queueinfos_b[queuename] = new QLabel();
                                m_queueinfos_c[queuename] = new QLabel();
                                m_queueinfos[queuename] = new QLabel();
                                int linenum = m_queuelabels.size();
                                m_gridlayout->addWidget( m_queuelabels[queuename], linenum + 1, 0, Qt::AlignLeft );
                                m_gridlayout->addWidget( m_queuebusies[queuename], linenum + 1, 1, Qt::AlignCenter );
                                m_gridlayout->addWidget( m_queueinfos_a[queuename], linenum + 1, 2, Qt::AlignRight );
                                m_gridlayout->addWidget( m_queueinfos_b[queuename], linenum + 1, 3, Qt::AlignRight );
                                m_gridlayout->addWidget( m_queueinfos_c[queuename], linenum + 1, 4, Qt::AlignRight );
                                m_gridlayout->addWidget( m_queueinfos[queuename], linenum + 1, 5, Qt::AlignLeft );
                        }

                        m_queuebusies[queuename]->setProperty("value", infos["Calls"]);
                        m_queueinfos_a[queuename]->setText(infos["Completed"]);
                        m_queueinfos_b[queuename]->setText(infos["Abandoned"]);
                        m_queueinfos_c[queuename]->setText(infos["Holdtime"]);
                        m_queueinfos[queuename]->setText("SP=" + infos["ServicelevelPerf"] +
                                                         " Mx=" + infos["Max"] +
                                                         " SL=" + infos["ServiceLevel"] +
                                                         " Wt=" + infos["Weight"]);
                }
                update();
        }
}

void QueuesPanel::update()
{
        m_maxbusy = 0;
        foreach (QProgressBar * qpb, m_queuebusies) {
                quint32 val = qpb->property("value").toUInt();
                if(val > m_maxbusy)
                        m_maxbusy = val;
        }
        qDebug() << "QueuesPanel::update() maxbusy =" << m_maxbusy;
        
        foreach (QProgressBar * qpb, m_queuebusies) {
                QString qname = qpb->property("queueid").toString();
                unsigned int val = qpb->property("value").toUInt();
                qpb->setRange(0, m_maxbusy + 1);
                // qpb->setValue(0); // trick in order to refresh
                qpb->setValue(val);

                if(val <= m_engine->queueLevel("green"))
                        qpb->setStyleSheet(commonqss + "QProgressBar::chunk {background-color: #00ff00;}");
                else if(val <= m_engine->queueLevel("orange"))
                        qpb->setStyleSheet(commonqss + "QProgressBar::chunk {background-color: #0000ff;}");
                else
                        qpb->setStyleSheet(commonqss + "QProgressBar::chunk {background-color: #ff0000;}");
        }
}

void QueuesPanel::queueClicked()
{
        // qDebug() << "QueuesPanel::queueClicked()" << this->sender()->property("queueid");
        QString astid = this->sender()->property("astid").toString();
        QString queueid = this->sender()->property("queueid").toString();
        changeWatchedQueue(astid + " " + queueid);
}


void QueuesPanel::setQueueStatus(const QString & status)
{
        QStringList newstatuses = status.split(";");
        // qDebug() << "QueuesPanel::setQueueStatus()" << newstatuses;
        if (newstatuses.size() >= 4) {
                QString command = newstatuses[0];
                if (command == "queuechannels") {
                        QString astid = newstatuses[1];
                        QString queuename = newstatuses[2];
                        QString busyness = newstatuses[3];
                        if(m_queuebusies.contains(queuename)) {
                                m_queuebusies[queuename]->setProperty("value", busyness);
                                update();
                        }
                } else if (command == "queueentry") {
                        qDebug() << "QueuesPanel::setQueueStatus()" << status;
                }
        }
}

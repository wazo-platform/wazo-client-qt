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
}

QueueentrydetailsPanel::~QueueentrydetailsPanel()
{
        // qDebug() << "QueueentrydetailsPanel::~QueueentrydetailsPanel()";
}

void QueueentrydetailsPanel::updatePeerAgent(const QString &, const QString &, const QString &)
{
        // qDebug() << "QueueentrydetailsPanel::updatePeerAgent()";
}

void QueueentrydetailsPanel::newQueue(const QStringList & queuestatus)
{
        // qDebug() << "QueueentrydetailsPanel::newQueue()" << queuestatus;
        m_astid = queuestatus[0];
        m_queueid = queuestatus[1];
        m_label->setText("<b>" + m_queueid + "</b> " + tr("on") + " <b>" + m_astid + "</b>");

        QHashIterator<QString, QLabel *> i(m_entrypos);
        while (i.hasNext()) {
                i.next();
                delete i.value();
        }
        QHashIterator<QString, QLabel *> j(m_entrytime);
        while (j.hasNext()) {
                j.next();
                delete j.value();
        }
        m_entrypos.clear();
        m_entrytime.clear();

        if(queuestatus.size() > 2) {
                int nagents = queuestatus[2].toInt();
                int nentries = queuestatus[nagents + 3].toInt();
                QString astid = queuestatus[0];
                for(int i = nagents + 4 ; i < nagents + 4 + nentries; i++)
                        if(queuestatus[i].size() > 0) {
                                QStringList entryinfos = queuestatus[i].split(",");
                                if(entryinfos.size() > 4) {
                                        // QString entryname = entryinfos[0];
                                        QString entryname = entryinfos[4] + " <" + entryinfos[3] + ">";
                                        QString entrypos = entryinfos[1];
                                        QString entrytime = entryinfos[2];
                                        m_entrypos[entryname] = new QLabel(entryname + " :: " + entrypos, this);
                                        m_entrytime[entryname] = new QLabel(entrytime, this);
                                        m_gridlayout->addWidget( m_entrypos[entryname], i + 1, 1, Qt::AlignLeft );
                                        m_gridlayout->addWidget( m_entrytime[entryname], i + 1, 2, Qt::AlignLeft );
                                }
                        }
        }
}

void QueueentrydetailsPanel::agentClicked()
{
        qDebug() << "AgentsPanel::agentClicked()" << sender()->property("agentid");
        QString astid = sender()->property("astid").toString();
        QString agentid = sender()->property("agentid").toString();
        changeWatchedAgent(astid + " " + agentid);
}

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

#include <QComboBox>
#include <QDebug>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>

#include "conferencepanel.h"
#include "userinfo.h"

/*! \brief Constructor
 */
ConferencePanel::ConferencePanel(QWidget * parent)
        : QWidget(parent), m_ui(NULL)
{
        qDebug() << "ConferencePanel::ConferencePanel()";
	m_glayout = new QGridLayout(this);
        m_idline = 0;
        QLabel * title = new QLabel(tr("Conferences and their Members"));
        m_glayout->addWidget( title, m_idline, 0, Qt::AlignLeft );
        m_idline ++;
        m_glayout->setRowStretch( m_idline, 1 );
	// m_glayout->setMargin(0);
        // m_glayout->setColumnStretch( 0, 1 );
}

ConferencePanel::~ConferencePanel()
{
}

void ConferencePanel::setGuiOptions(const QVariant &)
{
}

void ConferencePanel::setUserInfo(const UserInfo * ui)
{
        m_ui = ui;
}

void ConferencePanel::meetmeEvent(const QStringList & meetmelist)
{
        // qDebug() << "ConferencePanel::meetmeEvent()" << meetmelist;
        QString eventname = meetmelist[0];
        QString astid = meetmelist[1];
        QString roomnum = meetmelist[2];
        QString which = meetmelist[3];
        QString channel = meetmelist[4];
        // int busy = meetmelist[5].toInt();

        QString ref = astid + "-" + roomnum + "-" + channel;

        if(eventname == "join") {
                m_infos[ref] = new QLabel("<b>" + roomnum + "</b> " + tr("on") + " <b>" + astid + "</b> : " + which + " : " + channel);
                m_actions[ref] = new QPushButton(tr("Kick"), this);
                m_actions[ref]->setIcon(QIcon(":/images/cancel.png"));
                m_actions[ref]->setIconSize(QSize(16, 16));
                m_actions[ref]->setProperty("astid", astid);
                m_actions[ref]->setProperty("room", roomnum);
                m_actions[ref]->setProperty("usernum", which);
                m_actions[ref]->setProperty("reference", ref);
                m_actions[ref]->setProperty("channel", channel);
                connect(m_actions[ref], SIGNAL(clicked()),
                        this, SLOT(doMeetMeAction()));
                m_glayout->addWidget( m_infos[ref], m_idline, 0, Qt::AlignLeft );
                m_glayout->addWidget( m_actions[ref], m_idline, 1, Qt::AlignLeft );
                m_glayout->setRowStretch( m_idline, 0 );
                m_idline ++;
                m_glayout->setRowStretch( m_idline, 1 );
        } else if(eventname == "leave") {
                if(m_infos.contains(ref))
                        delete m_infos[ref];
                if(m_actions.contains(ref))
                        delete m_actions[ref];
        }
}

void ConferencePanel::doMeetMeAction()
{
        // qDebug() << "ConferencePanel::doMeetMeAction()";
        meetmeAction("kick " + sender()->property("astid").toString() +
                     " " + sender()->property("room").toString() +
                     " " + sender()->property("usernum").toString() +
                     " " + sender()->property("channel").toString());
        m_actions[sender()->property("reference").toString()]->setIconSize(QSize(8, 8));
}

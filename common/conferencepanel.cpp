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
#include <QTabWidget>

#include "conferencepanel.h"
#include "userinfo.h"

/*! \brief Constructor
 */
ConferencePanel::ConferencePanel(QWidget * parent)
        : QWidget(parent), m_ui(NULL)
{
        qDebug() << "ConferencePanel::ConferencePanel()";
	m_glayout = new QGridLayout(this);
        m_tw = new QTabWidget(this);
        m_glayout->addWidget( m_tw, 0, 0 );
        m_glayout->setRowStretch( 0, 1 );
        m_glayout->setColumnStretch( 0, 1 );
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

void ConferencePanel::meetmeInit(const QVariant & meetme)
{
        // qDebug() << "ConferencePanel::meetmeInit()" << meetme;
        foreach(QString astid, meetme.toMap().keys()) {
                QVariant astrooms = meetme.toMap()[astid];
                foreach(QString idx, astrooms.toMap().keys()) {
                        QString roomname = astrooms.toMap()[idx].toMap()["name"].toString();
                        QString roomnumber = astrooms.toMap()[idx].toMap()["number"].toString();
                        QString idx = astid + "-" + roomnumber;
                        QWidget * w = new QWidget();
                        m_layout[idx] = new QGridLayout(w);
                        m_layout[idx]->setColumnStretch(0, 1);
                        m_layout[idx]->setColumnStretch(5, 1);
                        m_layout[idx]->setRowStretch(100, 1);
                        m_tw->addTab(w, tr("Room %1 (%2) on %3").arg(roomname, roomnumber, astid));
                }
        }
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
        QString idx = astid + "-" + roomnum;
        
        if(eventname == "join") {
                if(! m_infos.contains(ref)) {
                        m_infos[ref] = new QLabel(which);
                        
                        m_action_kick[ref] = new QPushButton(tr("Kick"));
                        m_action_kick[ref]->setIcon(QIcon(":/images/cancel.png"));
                        m_action_kick[ref]->setIconSize(QSize(16, 16));
                        m_action_kick[ref]->setProperty("astid", astid);
                        m_action_kick[ref]->setProperty("room", roomnum);
                        m_action_kick[ref]->setProperty("usernum", which);
                        m_action_kick[ref]->setProperty("reference", ref);
                        m_action_kick[ref]->setProperty("channel", channel);
                        m_action_kick[ref]->setProperty("action", "kick");
                        connect(m_action_kick[ref], SIGNAL(clicked()),
                                this, SLOT(doMeetMeAction()));
                        
                        m_action_record[ref] = new QPushButton(tr("Record"));
                        m_action_record[ref]->setIcon(QIcon(":/images/cancel.png"));
                        m_action_record[ref]->setIconSize(QSize(16, 16));
                        m_action_record[ref]->setProperty("astid", astid);
                        m_action_record[ref]->setProperty("room", roomnum);
                        m_action_record[ref]->setProperty("usernum", which);
                        m_action_record[ref]->setProperty("reference", ref);
                        m_action_record[ref]->setProperty("channel", channel);
                        m_action_record[ref]->setProperty("action", "record");
                        connect(m_action_record[ref], SIGNAL(clicked()),
                                this, SLOT(doMeetMeAction()));
                        
                        m_action_mute[ref] = new QPushButton(tr("Mute"));
                        m_action_mute[ref]->setIcon(QIcon(":/images/cancel.png"));
                        m_action_mute[ref]->setIconSize(QSize(16, 16));
                        m_action_mute[ref]->setProperty("astid", astid);
                        m_action_mute[ref]->setProperty("room", roomnum);
                        m_action_mute[ref]->setProperty("usernum", which);
                        m_action_mute[ref]->setProperty("reference", ref);
                        m_action_mute[ref]->setProperty("channel", channel);
                        m_action_mute[ref]->setProperty("action", "mute");
                        connect(m_action_mute[ref], SIGNAL(clicked()),
                                this, SLOT(doMeetMeAction()));
                        // QPushButton * qp2 = new QPushButton(tr("Spy"));
                        
                        m_layout[idx]->addWidget( m_infos[ref], 0, 1 );
                        m_layout[idx]->addWidget( m_action_kick[ref], 0, 2 );
                        m_layout[idx]->addWidget( m_action_record[ref], 0, 3 );
                        m_layout[idx]->addWidget( m_action_mute[ref], 0, 4 );
                        // glayout->addWidget( qp2, 0, 3 );
                }
        } else if(eventname == "leave") {
                if(m_infos.contains(ref)) {
                        delete m_infos[ref];
                        delete m_action_kick[ref];
                        delete m_action_record[ref];
                        delete m_action_mute[ref];
                }
        }
}

void ConferencePanel::doMeetMeAction()
{
        // qDebug() << "ConferencePanel::doMeetMeAction()";
        QString action = sender()->property("action").toString();
        meetmeAction(action,
                     sender()->property("astid").toString() +
                     " " + sender()->property("room").toString() +
                     " " + sender()->property("usernum").toString() +
                     " " + sender()->property("channel").toString());
        if(action == "kick")
                m_action_kick[sender()->property("reference").toString()]->setIconSize(QSize(8, 8));
        else if(action == "record")
                m_action_record[sender()->property("reference").toString()]->setIconSize(QSize(8, 8));
        else if(action == "mute")
                m_action_mute[sender()->property("reference").toString()]->setIconSize(QSize(8, 8));
}

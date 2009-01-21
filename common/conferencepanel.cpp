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
#include <QDateTime>
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
        startTimer(1000);
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

void ConferencePanel::meetmeInit(int timeref, const QVariant & meetme)
{
        // qDebug() << "ConferencePanel::meetmeInit()" << meetme;
        foreach (QString astid, meetme.toMap().keys()) {
                QVariant astrooms = meetme.toMap()[astid];
                foreach (QString idx, astrooms.toMap().keys()) {
                        QString roomname = astrooms.toMap()[idx].toMap()["name"].toString();
                        QString roomnum = astrooms.toMap()[idx].toMap()["number"].toString();
                        QString adminid = astrooms.toMap()[idx].toMap()["adminid"].toString();
                        QVariantMap channels = astrooms.toMap()[idx].toMap()["channels"].toMap();
                        
                        if(channels.size() > 0) {
                                addRoomTab(astid, roomnum, roomname);
                                foreach (QString chan, channels.keys()) {
                                        setProperties(timeref, "join", adminid, astid, roomnum, chan, channels[chan]);
                                        setProperties(timeref, "mutestatus", adminid, astid, roomnum, chan, channels[chan]);
                                        setProperties(timeref, "recordstatus", adminid, astid, roomnum, chan, channels[chan]);
                                }
                        } else {
                                delRoomTab(astid, roomnum);
                        }
                }
        }
}

void ConferencePanel::delRoomTab(const QString & astid,
                                 const QString & roomnum)
{
        QString idxroom = astid + "-" + roomnum;
        if(m_layout.contains(idxroom)) {
                QWidget * w = qobject_cast<QWidget *>(m_layout[idxroom]->parent());
                if(w)
                        m_tw->removeTab(m_tw->indexOf(w));
                delete m_layout[idxroom];
                m_layout.remove(idxroom);
        }
}

void ConferencePanel::addRoomTab(const QString & astid,
                                 const QString & roomnum,
                                 const QString & roomname)
{
        QString idxroom = astid + "-" + roomnum;
        if(! m_layout.contains(idxroom)) {
                QWidget * w = new QWidget();
                m_layout[idxroom] = new QGridLayout(w);
                m_layout[idxroom]->setColumnStretch(0, 1);
                m_layout[idxroom]->setColumnStretch(6, 1);
                m_layout[idxroom]->setRowStretch(100, 1);
                m_tw->addTab(w, tr("Room %1 (%2) on %3").arg(roomname, roomnum, astid));
        }
}

void ConferencePanel::meetmeEvent(int timeref, const QVariant & meetme)
{
        // qDebug() << "ConferencePanel::meetmeEvent()" << meetme;
        QString astid = meetme.toMap()["astid"].toString();
        QString roomnum = meetme.toMap()["roomnum"].toString();
        QString roomname = meetme.toMap()["roomname"].toString();
        QString idxroom = astid + "-" + roomnum;
        QString adminid = meetme.toMap()["adminid"].toString();
        addRoomTab(astid, roomnum, roomname);
        
        setProperties(timeref,
                      meetme.toMap()["action"].toString(),
                      adminid,
                      astid,
                      roomnum,
                      meetme.toMap()["channel"].toString(),
                      meetme.toMap()["details"]);
}

void ConferencePanel::setProperties(int timeref,
                                    const QString & action,
                                    const QString & adminid,
                                    const QString & astid,
                                    const QString & roomnum,
                                    const QString & channel,
                                    const QVariant & details)
{
        // qDebug() << "ConferencePanel::setProperties()" << action << adminid << astid << roomnum << channel << details;
        QString idxroom = astid + "-" + roomnum;
        QString ref = astid + "-" + roomnum + "-" + channel;
        if(action == "join") {
                QString usernum = details.toMap()["usernum"].toString();
                QString fullname = details.toMap()["fullname"].toString();
                QString phonenum = details.toMap()["phonenum"].toString();
                QString userid = details.toMap()["userid"].toString();
                int time_spent = timeref - details.toMap()["time_start"].toInt();
                if(! m_infos.contains(ref)) {
                        m_infos[ref] = new QLabel(fullname + " <" + phonenum + ">");
                        m_infos[ref]->setProperty("astid", astid);
                        m_infos[ref]->setProperty("room", roomnum);
                        
                        m_timespent[ref] = new QLabel();
                        m_timespent[ref]->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
                        m_timespent[ref]->setProperty("inittime", QDateTime::currentDateTime().addSecs(- time_spent));
                        
                        m_action_kick[ref] = new QPushButton(tr("Kick"));
                        m_action_kick[ref]->setIcon(QIcon(":/images/cancel.png"));
                        m_action_kick[ref]->setIconSize(QSize(16, 16));
                        m_action_kick[ref]->setProperty("astid", astid);
                        m_action_kick[ref]->setProperty("room", roomnum);
                        m_action_kick[ref]->setProperty("usernum", usernum);
                        m_action_kick[ref]->setProperty("reference", ref);
                        m_action_kick[ref]->setProperty("channel", channel);
                        m_action_kick[ref]->setProperty("action", "kick");
                        if((m_ui->userid() == adminid) || (m_ui->userid() == userid))
                                connect(m_action_kick[ref], SIGNAL(clicked()),
                                        this, SLOT(doMeetMeAction()));
                        else
                                m_action_kick[ref]->hide();
                        
                        m_action_record[ref] = new QPushButton(tr("Record"));
                        m_action_record[ref]->setIcon(QIcon(":/images/cancel.png"));
                        m_action_record[ref]->setIconSize(QSize(16, 16));
                        m_action_record[ref]->setProperty("astid", astid);
                        m_action_record[ref]->setProperty("room", roomnum);
                        m_action_record[ref]->setProperty("usernum", usernum);
                        m_action_record[ref]->setProperty("reference", ref);
                        m_action_record[ref]->setProperty("channel", channel);
                        m_action_record[ref]->setProperty("action", "record");
                        m_action_record[ref]->setProperty("recordstatus", "off");
                        if(m_ui->userid() == adminid)
                                connect(m_action_record[ref], SIGNAL(clicked()),
                                        this, SLOT(doMeetMeAction()));
                        else
                                m_action_record[ref]->hide();
                        
                        m_action_mute[ref] = new QPushButton(tr("Mute"));
                        m_action_mute[ref]->setIcon(QIcon(":/images/cancel.png"));
                        m_action_mute[ref]->setIconSize(QSize(16, 16));
                        m_action_mute[ref]->setProperty("astid", astid);
                        m_action_mute[ref]->setProperty("room", roomnum);
                        m_action_mute[ref]->setProperty("usernum", usernum);
                        m_action_mute[ref]->setProperty("reference", ref);
                        m_action_mute[ref]->setProperty("channel", channel);
                        m_action_mute[ref]->setProperty("action", "mute");
                        m_action_mute[ref]->setProperty("mutestatus", "off");
                        if((m_ui->userid() == adminid) || (m_ui->userid() == userid))
                                connect(m_action_mute[ref], SIGNAL(clicked()),
                                        this, SLOT(doMeetMeAction()));
                        else
                                m_action_mute[ref]->hide();
                        // QPushButton * qp2 = new QPushButton(tr("Spy"));
                        
                        m_layout[idxroom]->addWidget( m_infos[ref], usernum.toInt(), 1 );
                        m_layout[idxroom]->addWidget( m_action_kick[ref], usernum.toInt(), 2 );
                        m_layout[idxroom]->addWidget( m_action_record[ref], usernum.toInt(), 3 );
                        m_layout[idxroom]->addWidget( m_action_mute[ref], usernum.toInt(), 4 );
                        m_layout[idxroom]->addWidget( m_timespent[ref], usernum.toInt(), 5 );
                        // glayout->addWidget( qp2, 0, 3 );
                }
        } else if(action == "leave") {
                if(m_infos.contains(ref)) {
                        m_layout[idxroom]->removeWidget( m_infos[ref] );
                        m_layout[idxroom]->removeWidget( m_action_kick[ref] );
                        m_layout[idxroom]->removeWidget( m_action_record[ref] );
                        m_layout[idxroom]->removeWidget( m_action_mute[ref] );
                        m_layout[idxroom]->removeWidget( m_timespent[ref] );
                        delete m_infos[ref];
                        delete m_action_kick[ref];
                        delete m_action_record[ref];
                        delete m_action_mute[ref];
                        delete m_timespent[ref];
                        m_infos.remove(ref);
                        m_action_kick.remove(ref);
                        m_action_record.remove(ref);
                        m_action_mute.remove(ref);
                        m_timespent.remove(ref);
                }
                int count = 0;
                foreach (QString r, m_infos.keys())
                        if((m_infos[r]->property("astid").toString() == astid) && (m_infos[r]->property("room").toString() == roomnum))
                                count ++;
                if(count == 0)
                        delRoomTab(astid, roomnum);
        } else if(action == "mutestatus") {
                QString mutestatus = details.toMap()["mutestatus"].toString();
                if(m_infos.contains(ref)) {
                        if(mutestatus == "on") {
                                if (m_action_mute[ref]->property("mutestatus").toString() == "off") {
                                        m_action_mute[ref]->setText(tr("Unmute"));
                                        m_action_mute[ref]->setProperty("mutestatus", "on");
                                }
                        } else if(mutestatus == "off") {
                                if (m_action_mute[ref]->property("mutestatus").toString() == "on") {
                                        m_action_mute[ref]->setText(tr("Mute"));
                                        m_action_mute[ref]->setProperty("mutestatus", "off");
                                }
                        } else {
                                qDebug() << "ConferencePanel::meetmeEvent() unknown mutestatus" << mutestatus << ref;
                        }
                        m_action_mute[ref]->setIconSize(QSize(16, 16));
                }
        } else if(action == "recordstatus") {
                QString recordstatus = details.toMap()["recordstatus"].toString();
                if(m_infos.contains(ref)) {
                        if(recordstatus == "on") {
                                m_action_record[ref]->setText(tr("Stop Record"));
                                m_action_record[ref]->setProperty("recordstatus", "on");
                        } else if(recordstatus == "off") {
                                m_action_record[ref]->setText(tr("Record"));
                                m_action_record[ref]->setProperty("recordstatus", "off");
                        } else {
                                qDebug() << "ConferencePanel::meetmeEvent() unknown recordstatus" << recordstatus << ref;
                        }
                }
        }
}

void ConferencePanel::doMeetMeAction()
{
        // qDebug() << "ConferencePanel::doMeetMeAction()";
        QString action = sender()->property("action").toString();
        QString ref = sender()->property("reference").toString();
        
        if(action == "kick") {
                meetmeAction(action,
                             sender()->property("astid").toString() +
                             " " + sender()->property("room").toString() +
                             " " + sender()->property("usernum").toString() +
                             " " + sender()->property("channel").toString());
                m_action_kick[ref]->setIconSize(QSize(8, 8));
        } else if(action == "record") {
                if(m_action_record[ref]->property("recordstatus").toString() == "off") {
                        meetmeAction("record",
                                     sender()->property("astid").toString() +
                                     " " + sender()->property("room").toString() +
                                     " " + sender()->property("usernum").toString() +
                                     " " + sender()->property("channel").toString());
                        m_action_record[ref]->setProperty("recordstatus", "on");
                        m_action_record[ref]->setText(tr("Stop Record"));
                } else {
                        meetmeAction("unrecord",
                                     sender()->property("astid").toString() +
                                     " " + sender()->property("room").toString() +
                                     " " + sender()->property("usernum").toString() +
                                     " " + sender()->property("channel").toString());
                        m_action_record[ref]->setProperty("recordstatus", "off");
                        m_action_record[ref]->setText(tr("Record"));
                }
        } else if(action == "mute") {
                QString mutestatus = m_action_mute[ref]->property("mutestatus").toString();
                if(mutestatus == "off") {
                        meetmeAction("mute",
                                     sender()->property("astid").toString() +
                                     " " + sender()->property("room").toString() +
                                     " " + sender()->property("usernum").toString() +
                                     " " + sender()->property("channel").toString());
                } else if(mutestatus == "on") {
                        meetmeAction("unmute",
                                     sender()->property("astid").toString() +
                                     " " + sender()->property("room").toString() +
                                     " " + sender()->property("usernum").toString() +
                                     " " + sender()->property("channel").toString());
                } else {
                        qDebug() << "ConferencePanel::doMeetMeAction() unknown mutestatus" << mutestatus << ref;
                }
                m_action_mute[ref]->setIconSize(QSize(8, 8));
        }
}

void ConferencePanel::timerEvent(QTimerEvent *)
{
        foreach (QString ref, m_timespent.keys()) {
                QDateTime inittime = m_timespent[ref]->property("inittime").toDateTime();
                int nsec = inittime.secsTo(QDateTime::currentDateTime());
                int dmin = nsec / 60;
                int dsec = nsec % 60;
                QString displayedtime;
                if(dmin > 0)
                        displayedtime = tr("%1 min %2 sec").arg(QString::number(dmin), QString::number(dsec));
                else
                        displayedtime = tr("%1 sec").arg(QString::number(dsec));
                m_timespent[ref]->setText(displayedtime);
        }
}

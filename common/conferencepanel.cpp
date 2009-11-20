/* XiVO Client
 * Copyright (C) 2007-2009, Proformatique
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
#include <QDateTime>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QTabWidget>

#include "baseengine.h"
#include "conferencepanel.h"
#include "userinfo.h"

/*! \brief Constructor
 */
ConferencePanel::ConferencePanel(BaseEngine * engine,
                                 QWidget * parent)
    : XLet(engine, parent)
{
    // qDebug() << "ConferencePanel::ConferencePanel()";
    setTitle( tr("Conference") );
    m_glayout = new QGridLayout(this);
    m_tw = new QTabWidget(this);
    m_glayout->addWidget( m_tw, 0, 0 );
    QScrollArea * scrollarea = new QScrollArea(this);
    scrollarea->setWidgetResizable(true);
    m_summary = new QLabel( scrollarea );
    scrollarea->setWidget(m_summary);
    m_glayout->addWidget( scrollarea, 1, 0 );
    m_glayout->setRowStretch( 0, 1 );
    m_glayout->setColumnStretch( 0, 1 );
    m_show_record = false;
    
    setGuiOptions(m_engine->getGuiOptions("merged_gui"));
    startTimer(1000);
    
    // connect signal/slots
    connect( m_engine, SIGNAL(meetmeEvent(double, const QVariant &)),
             this, SLOT(meetmeEvent(double, const QVariant &)) );
    connect( m_engine, SIGNAL(meetmeInit(double, const QVariant &)),
             this, SLOT(meetmeInit(double, const QVariant &)) );
    connect( this, SIGNAL(meetmeAction(const QString &, const QString &)),
             m_engine, SLOT(meetmeAction(const QString &, const QString &)) );
}

/*! \brief Destructor */
ConferencePanel::~ConferencePanel()
{
}

/*! \brief set gui options
 *
 * Set m_show_record.
 */
void ConferencePanel::setGuiOptions(const QVariantMap & optionsMap)
{
    if(optionsMap.contains("conference-allowrecord"))
        m_show_record = optionsMap["conference-allowrecord"].toBool();
}

/*! \brief Initial setup
 */
void ConferencePanel::meetmeInit(double timeref, const QVariant & meetme)
{
    // qDebug() << "ConferencePanel::meetmeInit()" << meetme;
    foreach (QString astid, meetme.toMap().keys()) {
        QVariantMap astrooms = meetme.toMap()[astid].toMap();
        foreach (QString idx, astrooms.keys()) {
            QString roomname = astrooms[idx].toMap()["name"].toString();
            QString roomnum = astrooms[idx].toMap()["number"].toString();
            QString adminid = astrooms[idx].toMap()["adminid"].toString();
            QVariantMap uniqueids = astrooms[idx].toMap()["uniqueids"].toMap();
            // qDebug() << "ConferencePanel::meetmeInit()" << astid << idx << roomname << roomnum << adminid << uniqueids;
            
            if(uniqueids.size() > 0) {
                addRoomTab(astid, roomnum, roomname);
                foreach (QString uid, uniqueids.keys()) {
                    setProperties(timeref, "join",
                                  adminid, astid, roomnum, uid, uniqueids[uid].toMap());
                    setProperties(timeref, "mutestatus",
                                  adminid, astid, roomnum, uid, uniqueids[uid].toMap());
                    setProperties(timeref, "recordstatus",
                                  adminid, astid, roomnum, uid, uniqueids[uid].toMap());
                }
            } else {
                delRoomTab(astid, roomnum);
            }
        }
    }
    updateSummary();
}

/*! \brief delete a Tab
 */
void ConferencePanel::delRoomTab(const QString & astid,
                                 const QString & roomnum)
{
    QString idxroom = QString("%1-%2").arg(astid).arg(roomnum);
    if(m_layout.contains(idxroom)) {
        QWidget * w = qobject_cast<QWidget *>(m_layout[idxroom]->parent());
        if(w)
            m_tw->removeTab(m_tw->indexOf(w));
        m_layout[idxroom]->deleteLater();
        m_layout.remove(idxroom);
    }
}

/*! \brief add a tab for the conference if there is not already one
 */
void ConferencePanel::addRoomTab(const QString & astid,
                                 const QString & roomnum,
                                 const QString & roomname)
{
    QString idxroom = QString("%1-%2").arg(astid).arg(roomnum);
    if(! m_layout.contains(idxroom)) {
        QWidget * w = new QWidget();
        m_layout[idxroom] = new QGridLayout(w);
        m_layout[idxroom]->setProperty("astid", astid);
        m_layout[idxroom]->setProperty("roomnum", roomnum);
        m_layout[idxroom]->setProperty("roomname", roomname);
        m_layout[idxroom]->setColumnStretch(0, 1);
        m_layout[idxroom]->setColumnStretch(6, 1);
        m_layout[idxroom]->setRowStretch(100, 1);
        int i = m_tw->addTab(w, tr("%1 (%2)").arg(roomname, roomnum));
        m_tw->setTabToolTip(i, tr("Room %1 (%2) on %3").arg(roomname, roomnum, astid));
    }
}

/*! \brief Process meetme events
 */
void ConferencePanel::meetmeEvent(double timeref, const QVariant & meetme)
{
    QVariantMap meetmeMap = meetme.toMap();
    //qDebug() << "ConferencePanel::meetmeEvent()" << meetmeMap;
    QString astid = meetmeMap["astid"].toString();
    QString roomnum = meetmeMap["roomnum"].toString();
    QString roomname = meetmeMap["roomname"].toString();
    QString idxroom = QString("%1-%2").arg(astid).arg(roomnum);
    QString adminid = meetmeMap["adminid"].toString();
    addRoomTab(astid, roomnum, roomname);
    
    setProperties(timeref,
                  meetmeMap["action"].toString(),
                  adminid,
                  astid,
                  roomnum,
                  meetmeMap["uniqueid"].toString(),
                  meetmeMap["details"].toMap());

    updateSummary();
}

/*! \brief create/update widgets which display person info
 */
void ConferencePanel::setProperties(double timeref,
                                    const QString & action,
                                    const QString & adminid,
                                    const QString & astid,
                                    const QString & roomnum,
                                    const QString & uniqueid,
                                    const QVariantMap & details)
{
    // qDebug() << "ConferencePanel::setProperties()" << action << adminid << astid << roomnum << uniqueid << details;
    QString idxroom = QString("%1-%2").arg(astid).arg(roomnum);
    QString ref = QString("%1-%2-%3").arg(astid).arg(roomnum).arg(uniqueid);
    if(action == "join") {
        QString usernum = details["usernum"].toString();
        QString fullname = details["fullname"].toString();
        QString phonenum = details["phonenum"].toString();
        QString userid = details["userid"].toString();
        int time_spent = int(timeref - details["time_start"].toDouble() + 0.5);
        if(! m_infos.contains(ref)) {
            UserInfo * userinfo = m_engine ? m_engine->getXivoClientUser() : NULL;
            
            m_infos[ref] = new QLabel(QString("%1 <%2>").arg(fullname).arg(phonenum));
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
            m_action_kick[ref]->setProperty("uniqueid", uniqueid);
            m_action_kick[ref]->setProperty("action", "kick");
            if(userinfo) {
                if((userinfo->userid() == adminid) || (userinfo->userid() == userid))
                    connect(m_action_kick[ref], SIGNAL(clicked()),
                            this, SLOT(doMeetMeAction()));
                else
                    m_action_kick[ref]->hide();
            }
            m_action_record[ref] = new QPushButton(tr("Record"));
            m_action_record[ref]->setIcon(QIcon(":/images/cancel.png"));
            m_action_record[ref]->setIconSize(QSize(16, 16));
            m_action_record[ref]->setProperty("astid", astid);
            m_action_record[ref]->setProperty("room", roomnum);
            m_action_record[ref]->setProperty("usernum", usernum);
            m_action_record[ref]->setProperty("reference", ref);
            m_action_record[ref]->setProperty("uniqueid", uniqueid);
            m_action_record[ref]->setProperty("action", "record");
            m_action_record[ref]->setProperty("recordstatus", "off");
            
            if(userinfo) {
                if((userinfo->userid() == adminid) && m_show_record)
                    connect(m_action_record[ref], SIGNAL(clicked()),
                            this, SLOT(doMeetMeAction()));
                else
                    m_action_record[ref]->hide();
            }
            
            m_action_mute[ref] = new QPushButton(tr("Mute"));
            m_action_mute[ref]->setIcon(QIcon(":/images/cancel.png"));
            m_action_mute[ref]->setIconSize(QSize(16, 16));
            m_action_mute[ref]->setProperty("astid", astid);
            m_action_mute[ref]->setProperty("room", roomnum);
            m_action_mute[ref]->setProperty("usernum", usernum);
            m_action_mute[ref]->setProperty("reference", ref);
            m_action_mute[ref]->setProperty("uniqueid", uniqueid);
            m_action_mute[ref]->setProperty("action", "mute");
            m_action_mute[ref]->setProperty("mutestatus", "off");
            if(userinfo) {
                if((userinfo->userid() == adminid) || (userinfo->userid() == userid))
                    connect(m_action_mute[ref], SIGNAL(clicked()),
                            this, SLOT(doMeetMeAction()));
                else
                    m_action_mute[ref]->hide();
            }
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
        QString mutestatus = details["mutestatus"].toString();
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
        QString recordstatus = details["recordstatus"].toString();
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

/*! \brief execute meet me actions (kick/record/mute)
 */
void ConferencePanel::doMeetMeAction()
{
    QString action = sender()->property("action").toString();
    QString ref = sender()->property("reference").toString();
    qDebug() << "ConferencePanel::doMeetMeAction()" << action << ref;
    
    if(action == "kick") {
        meetmeAction(action,
                     sender()->property("astid").toString() +
                     " " + sender()->property("room").toString() +
                     " " + sender()->property("usernum").toString() +
                     " " + sender()->property("uniqueid").toString());
        m_action_kick[ref]->setIconSize(QSize(8, 8));
    } else if(action == "record") {
        if(m_action_record[ref]->property("recordstatus").toString() == "off") {
            meetmeAction("record",
                         sender()->property("astid").toString() +
                         " " + sender()->property("room").toString() +
                         " " + sender()->property("usernum").toString() +
                         " " + sender()->property("uniqueid").toString());
            m_action_record[ref]->setProperty("recordstatus", "on");
            m_action_record[ref]->setText(tr("Stop Record"));
        } else {
            meetmeAction("stoprecord",
                         sender()->property("astid").toString() +
                         " " + sender()->property("room").toString() +
                         " " + sender()->property("usernum").toString() +
                         " " + sender()->property("uniqueid").toString());
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
                         " " + sender()->property("uniqueid").toString());
        } else if(mutestatus == "on") {
            meetmeAction("unmute",
                         sender()->property("astid").toString() +
                         " " + sender()->property("room").toString() +
                         " " + sender()->property("usernum").toString() +
                         " " + sender()->property("uniqueid").toString());
        } else {
            qDebug() << "ConferencePanel::doMeetMeAction() unknown mutestatus" << mutestatus << ref;
        }
        m_action_mute[ref]->setIconSize(QSize(8, 8));
    }
}

/*! \brief update displayed times
 */
void ConferencePanel::timerEvent(QTimerEvent *)
{
    foreach (QString ref, m_timespent.keys()) {
        QDateTime inittime = m_timespent[ref]->property("inittime").toDateTime();
        int nsec = inittime.secsTo(QDateTime::currentDateTime());
        QString displayedtime;
        
        int dhr  = nsec / 3600;
        int dmin = (nsec - dhr * 3600) / 60;
        int dsec = nsec % 60;
        if(dhr > 0)
            displayedtime = tr("%1 hr %2 min %3 sec").arg(dhr).arg(dmin).arg(dsec);
        else if(dmin > 0)
            displayedtime = tr("%1 min %2 sec").arg(dmin).arg(dsec);
        else
            displayedtime = tr("%1 sec").arg(dsec);
        
        m_timespent[ref]->setText(displayedtime);
    }
}

/*! \brief Update the list of how many members in each room
 */
void ConferencePanel::updateSummary()
{
    QString summary;
    int count = 0;
    //qDebug() << "ConferencePanel::updateSummary()" << m_tw->count() << m_infos.keys() << m_layout.keys();
/*
    foreach(QString idxroom, m_layout.keys()) {
        //qDebug() << "    " << idxroom << m_layout[idxroom]->count();
        //qDebug() << "    " << idxroom << m_layout[idxroom]->rowCount();
        count = 0;
        foreach(QString ref, m_infos.keys()) {
            if(ref.startsWith(idxroom))
                count++;
        }
        qDebug() << "    " << idxroom
                 << m_layout[idxroom]->property("astid").toString()
                 << m_layout[idxroom]->property("roomnum").toString()
                 << m_layout[idxroom]->property("roomname").toString()
                 << count;
        summary.append(tr("%1 (%2) : %3\n").arg(m_layout[idxroom]->property("roomname").toString()).arg(m_layout[idxroom]->property("roomnum").toString()).arg(tr("%n member(s)", "", count)));
    }
*/
    if(!m_engine)
        return;
    foreach(QString astid, m_engine->meetme().keys()) {
        foreach(QString meetmeid, m_engine->meetme()[astid].keys()) {
            //qDebug() << astid << meetmeid << m_engine->meetme()[astid][meetmeid];
            QString roomname = m_engine->meetme()[astid][meetmeid].m_name;
            QString roomnum = m_engine->meetme()[astid][meetmeid].m_number;
            count = m_engine->meetme()[astid][meetmeid].m_uniqueids.count();
            summary.append(tr("%1 (%2) : %3\n").arg(roomname).arg(roomnum).arg(tr("%n member(s)", "", count)));
        }
    }
    m_summary->setText( summary );
}

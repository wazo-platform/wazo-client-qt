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
#include <QDateTime>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QTabWidget>

#include "baseengine.h"
#include "conferencepanel.h"
#include "userinfo.h"
#include "meetmeinfo.h"

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
    connect( m_engine, SIGNAL(meetmeEvent(double, const QString &, const QString &, const QString &, const QString &)),
             this, SLOT(meetmeEvent(double, const QString &, const QString &, const QString &, const QString &)) );
    connect( m_engine, SIGNAL(meetmeInit(double)),
             this, SLOT(meetmeInit(double)) );
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
void ConferencePanel::meetmeInit(double timeref)
{
    QString our_userid = m_engine->getXivoClientUser()->userid();
    //qDebug() << "ConferencePanel::meetmeInit()" << meetme;
    
    foreach (QString astid, m_engine->meetme().keys()) {
        QHashIterator<QString, MeetmeInfo *> i(m_engine->meetme()[astid]);
        while(i.hasNext()) {
            i.next();
            const MeetmeInfo * meetmeinfo = i.value();
            QString meetmeid = i.key();
            QVariantMap uniqueids = meetmeinfo->uniqueids();
            bool are_we_in_room = false;
            
            foreach (QString uniqueid, uniqueids.keys()) {
                //qDebug()<< "Conf::" << uniqueids[uniqueid].toMap()["userid"].toString() << "\n";
                if (uniqueids[uniqueid].toMap()["userid"].toString() == our_userid) {
                    are_we_in_room = true;
                    break;
                }
            }
            
            if(uniqueids.size() > 0) {
                addRoomTab(astid, meetmeid, are_we_in_room);
                foreach (QString uid, uniqueids.keys()) {
                    setProperties(timeref, "join",
                                  astid, meetmeid, uid, uniqueids[uid].toMap());
                    setProperties(timeref, "mutestatus",
                                  astid, meetmeid, uid, uniqueids[uid].toMap());
                    setProperties(timeref, "recordstatus",
                                  astid, meetmeid, uid, uniqueids[uid].toMap());
                    setProperties(timeref, "changeroompausedstate",
                                  astid, meetmeid, uid, uniqueids[uid].toMap());
                }
                updateButtons(astid, meetmeid);
            } else {
                delRoomTab(astid, meetmeid);
            }
        }
    }
    updateSummary();
}

/*! \brief delete a Tab
 */
void ConferencePanel::delRoomTab(const QString & astid,
                                 const QString & meetmeid)
{
    const MeetmeInfo * meetmeinfo = m_engine->meetme()[astid][meetmeid];
    QString roomname = meetmeinfo->roomname();
    QString idxroom = QString("%1-%2").arg(astid).arg(roomname);
    if(m_layout.contains(idxroom)) {
        QWidget * w = qobject_cast<QWidget *>(m_layout[idxroom]->parent()->parent());
        if(w)
            m_tw->removeTab(m_tw->indexOf(w));
        m_layout[idxroom]->deleteLater();
        m_layout.remove(idxroom);
    }
}

/*! \brief make the not authed user list and the button to pause the room on the left part of ui
 */
QWidget* ConferencePanel::createLeftUserList(QWidget * parent,
                                             const QString & astid,
                                             const QString & roomname)
{
    QWidget * w = new QWidget(parent);
    QVBoxLayout * vlayout = new QVBoxLayout(w);
    QString idxroom = QString("%1-%2").arg(astid).arg(roomname);

    m_user_not_authed_list[idxroom] = new QTreeWidget(w);
    m_user_not_authed_list[idxroom]->setColumnCount(4);
    m_user_not_authed_list[idxroom]->setColumnWidth(0, 150);
    m_user_not_authed_list[idxroom]->header()->hide();
    m_user_not_authed_list[idxroom]->setSelectionMode(QAbstractItemView::NoSelection);

    QTreeWidgetItem * header  = new QTreeWidgetItem(m_user_not_authed_list[idxroom]);

    header->setText(0, tr("User(s) not (yet)? acknowledged"));
    header->setFirstColumnSpanned(true);
    header->setFlags(Qt::ItemIsEnabled);
    header->setChildIndicatorPolicy(QTreeWidgetItem::DontShowIndicator);
    header->setExpanded(true);

    vlayout->addWidget(m_user_not_authed_list[idxroom]);

    connect(m_user_not_authed_list[idxroom], SIGNAL(itemClicked(QTreeWidgetItem*, int)),
            this, SLOT(doMeetMeAdminOnlyAction(QTreeWidgetItem*, int)));

    m_action_chamber_toggle_pause[idxroom] = new QPushButton(tr("&Put the conference room in pause"),w);
    m_action_chamber_toggle_pause[idxroom]->setProperty("astid", astid);
    m_action_chamber_toggle_pause[idxroom]->setProperty("roomname", roomname);
    m_action_chamber_toggle_pause[idxroom]->setProperty("togglepause", "on");
    connect(m_action_chamber_toggle_pause[idxroom], SIGNAL(clicked()), this, SLOT(changeRoomPauseState()));

    vlayout->addWidget(m_action_chamber_toggle_pause[idxroom]);

    return w;
}

/*! \brief add a tab for the conference if there is not already one
 */
void ConferencePanel::addRoomTab(const QString & astid,
                                 const QString & meetmeid,
                                 int are_we_inside)
{
    const MeetmeInfo * meetmeinfo = m_engine->meetme()[astid][meetmeid];
    QString roomname = meetmeinfo->roomname();
    QString roomnumber = meetmeinfo->roomnumber();
    QString idxroom = QString("%1-%2").arg(astid).arg(roomname);
    QString addtabformat;
    QString tabtooltipformat;
    if(roomnumber.isEmpty()) {
        addtabformat = QString("%1").arg(roomname);
        tabtooltipformat = tr("Room %1 on %2").arg(roomname).arg(astid);
    } else {
        addtabformat = QString("%1 (%2)").arg(roomname).arg(roomnumber);
        tabtooltipformat = tr("Room %1 (%2) on %3").arg(roomname).arg(roomnumber).arg(astid);
    }
    
    if(! m_layout.contains(idxroom)) {
        QWidget * w = new QWidget();
        QHBoxLayout * hlayout = new QHBoxLayout(w);
        QWidget * shown_to_everyone = new QWidget(w);
        
        hlayout->addWidget(createLeftUserList(w, astid, roomname));
        hlayout->addWidget(shown_to_everyone);
        
        m_layout[idxroom] = new QGridLayout(shown_to_everyone);
        m_layout[idxroom]->setProperty("astid", astid);
        m_layout[idxroom]->setProperty("roomname", roomname);
        m_layout[idxroom]->setProperty("roomnumber", roomnumber);
        m_layout[idxroom]->setColumnStretch(0, 1);
        m_layout[idxroom]->setColumnStretch(6, 1);
        m_layout[idxroom]->setRowStretch(100, 1);
        
        if(are_we_inside) {
            int i = m_tw->addTab(w, addtabformat);
            m_tw->setTabToolTip(i, tabtooltipformat);
        }
    } else {
        if(are_we_inside) {
            // a dirty way to see if our tab has ever been added
            QWidget * shown_to_everyone = m_layout[idxroom]->parentWidget();
            QWidget * w = shown_to_everyone->parentWidget();

            if(w->parentWidget()!=m_tw) {
                int i = m_tw->addTab(w, addtabformat);
                m_tw->setTabToolTip(i, tabtooltipformat);
            }
        }
    }
}

/*! \brief Process meetme events
 */
void ConferencePanel::meetmeEvent(double timeref,
                                  const QString & action,
                                  const QString & astid,
                                  const QString & meetmeid,
                                  const QString & uniqueid)
{
    qDebug() << "ConferencePanel::meetmeEvent()" << action << astid << meetmeid << uniqueid;
    if (! m_engine->meetme()[astid].contains(meetmeid))
        return;
    const MeetmeInfo * meetmeinfo = m_engine->meetme()[astid][meetmeid];
    
    QVariantMap uniqueids = meetmeinfo->uniqueids();
    bool is_this_us = false;
    QString our_userid = m_engine->getXivoClientUser()->userid();
    QVariantMap details = uniqueids[uniqueid].toMap();
    if (details["userid"].toString() == our_userid)
        is_this_us = true;
    addRoomTab(astid, meetmeid, is_this_us);
    
    setProperties(timeref,
                  action,
                  astid,
                  meetmeid,
                  uniqueid,
                  uniqueids[uniqueid].toMap());
    
    updateButtons(astid, meetmeid);
    
    updateSummary();
}

/*! \brief Update the Kick/mute buttons
 */
void ConferencePanel::updateButtons(const QString & astid,
                                    const QString & meetmeid)
{
    if(!m_engine)
        return;
    const MeetmeInfo * thismeetmeinfo = m_engine->meetme()[astid][meetmeid];
    QString roomnumber = thismeetmeinfo->roomnumber();
    QString roomname = thismeetmeinfo->roomname();
    
    QString idxroom = QString("%1-%2").arg(astid).arg(roomname);
    UserInfo * userinfo = m_engine->getXivoClientUser();
    QString userid = (userinfo ? userinfo->userid() : QString(""));
    QHashIterator<QString, MeetmeInfo *> i(m_engine->meetme()[astid]);
    while(i.hasNext()) {
        i.next();
        const MeetmeInfo * meetmeinfo = i.value();
        if(meetmeinfo->roomname() == roomname) {
            //qDebug() << "ConferencePanel::updateButtons" << meetmeinfo->m_uniqueids;
            bool isAdmin = (userid == meetmeinfo->adminid()) || meetmeinfo->adminlist().contains(userid);
            qDebug() << "ConferencePanel::updateButtons" << roomname << "userid" << userid << "=?" << "isAdmin:" << isAdmin;
            
            bool isAuthed = false;
            {
                QMapIterator<QString, QVariant> j(meetmeinfo->uniqueids());
                while(j.hasNext()) {
                    j.next();
                    if ( (userid == j.value().toMap()["userid"].toString()) &&
                         (j.value().toMap()["authed"].toBool())) {
                        isAuthed = true;
                        break;
                    }
                }
            }
            
            // non admin don't have to see the list of user not yet allowed
            if (!isAdmin) {
                m_user_not_authed_list[idxroom]->hide();
                m_action_chamber_toggle_pause[idxroom]->hide();
            } else {
                m_user_not_authed_list[idxroom]->show();
                m_action_chamber_toggle_pause[idxroom]->show();
            }
            
            QMapIterator<QString, QVariant> j(meetmeinfo->uniqueids());
            while(j.hasNext()) {
                j.next();
                QString ref = QString("%1-%2-%3").arg(astid).arg(roomname).arg(j.key());
                // We have the right to kick and mute if
                //    1) we are the admin (userinfo->userid() == adminid)
                // or 2) it is ourself    (userinfo->userid() == userid)

                // all non authed user don't have to appear here, they should see nothing too
                if ((!isAuthed)||(!j.value().toMap()["authed"].toBool())) {
                    m_timespent[ref]->hide();
                    m_infos[ref]->hide();
                    m_action_kick[ref]->hide();
                    m_action_mute[ref]->hide();
                    m_action_record[ref]->hide();
                } else {
                    if (j.value().toMap()["authed"].toBool()) {
                        m_timespent[ref]->show();
                        m_infos[ref]->show();
                    }
                    if(isAdmin || (userid == j.value().toMap()["userid"].toString())) {
                        m_action_kick[ref]->show();
                        m_action_mute[ref]->show();
                    } else {
                        m_action_kick[ref]->hide();
                        m_action_mute[ref]->hide();
                    }
                    if(isAdmin && m_show_record) {
                        m_action_record[ref]->show();
                    } else {
                        m_action_record[ref]->hide();
                    }
                }
            }
        }
    }
}

/*! \brief create/update widgets which display person info
 */
void ConferencePanel::setProperties(double timeref,
                                    const QString & action,
                                    const QString & astid,
                                    const QString & meetmeid,
                                    const QString & uniqueid,
                                    const QVariantMap & details)
{
    const MeetmeInfo * meetmeinfo = m_engine->meetme()[astid][meetmeid];
    QString roomname = meetmeinfo->roomname();
    qDebug() << "ConferencePanel::setProperties()" << action << astid << roomname << uniqueid << details;
    QString idxroom = QString("%1-%2").arg(astid).arg(roomname);
    QString ref = QString("%1-%2-%3").arg(astid).arg(roomname).arg(uniqueid);
    
    if(action == "join") {
        QString adminnum = meetmeinfo->adminnum();
        QString usernum = details["usernum"].toString();
        QString fullname = details["fullname"].toString();
        QString phonenum = details["phonenum"].toString();
        QString userid = details["userid"].toString();
        bool authed = details["authed"].toBool();
        
        if (!authed) {
            if(! m_user_not_authed.contains(ref)) {
                m_user_not_authed[ref] = new QTreeWidgetItem(m_user_not_authed_list[idxroom]->topLevelItem(0));
                
                m_user_not_authed[ref]->setText(0, details["fullname"].toString() + "<" + details["phonenum"].toString() + ">");
                m_user_not_authed[ref]->setText(1, tr("screw him"));
                m_user_not_authed[ref]->setText(2, tr("talk to"));
                m_user_not_authed[ref]->setText(3, tr("allow him in"));
                // QTreeWidgetItem isn't inheriting from Q_OBJECT so we can't set any property
                // but we can still store extra data in hidden columns
                m_user_not_authed[ref]->setText(4, astid);
                m_user_not_authed[ref]->setText(5, roomname);
                m_user_not_authed[ref]->setText(6, usernum);
                m_user_not_authed[ref]->setText(7, adminnum);
            }
        }
        
        int time_spent = int(timeref - details["time_start"].toDouble() + 0.5);
        if(! m_infos.contains(ref)) {
            //UserInfo * userinfo = m_engine ? m_engine->getXivoClientUser() : NULL;
            
            m_infos[ref] = new QLabel(QString("%1 <%2>").arg(fullname).arg(phonenum));
            m_infos[ref]->setProperty("astid", astid);
            m_infos[ref]->setProperty("roomname", roomname);
            
            m_timespent[ref] = new QLabel();
            m_timespent[ref]->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
            m_timespent[ref]->setProperty("inittime", QDateTime::currentDateTime().addSecs(- time_spent));
            
            m_action_kick[ref] = new QPushButton(tr("Kick"));
            m_action_kick[ref]->setIcon(QIcon(":/images/cancel.png"));
            m_action_kick[ref]->setIconSize(QSize(16, 16));
            m_action_kick[ref]->setProperty("astid", astid);
            m_action_kick[ref]->setProperty("roomname", roomname);
            m_action_kick[ref]->setProperty("usernum", usernum);
            m_action_kick[ref]->setProperty("reference", ref);
            m_action_kick[ref]->setProperty("uniqueid", uniqueid);
            m_action_kick[ref]->setProperty("userid", userid);
            m_action_kick[ref]->setProperty("action", "kick");
            connect(m_action_kick[ref], SIGNAL(clicked()),
                    this, SLOT(doMeetMeAction()));
/*
            if(userinfo) {
                qDebug() << "ConferencePanel::setProperties" << userinfo->userid() << adminid << userid;
                if((userinfo->userid() == adminid) || (userinfo->userid() == userid))
                    m_action_kick[ref]->show();
                else
                    m_action_kick[ref]->hide();
            }
*/
            m_action_record[ref] = new QPushButton(tr("Record"));
            m_action_record[ref]->setIcon(QIcon(":/images/cancel.png"));
            m_action_record[ref]->setIconSize(QSize(16, 16));
            m_action_record[ref]->setProperty("astid", astid);
            m_action_record[ref]->setProperty("roomname", roomname);
            m_action_record[ref]->setProperty("usernum", usernum);
            m_action_record[ref]->setProperty("reference", ref);
            m_action_record[ref]->setProperty("uniqueid", uniqueid);
            m_action_record[ref]->setProperty("action", "record");
            m_action_record[ref]->setProperty("recordstatus", "off");
            connect(m_action_record[ref], SIGNAL(clicked()),
                    this, SLOT(doMeetMeAction()));
/*
            if(userinfo) {
                if((userinfo->userid() == adminid) && m_show_record)
                    m_action_record[ref]->show();
                else
                    m_action_record[ref]->hide();
            }
*/           
            m_action_mute[ref] = new QPushButton(tr("Mute"));
            m_action_mute[ref]->setIcon(QIcon(":/images/cancel.png"));
            m_action_mute[ref]->setIconSize(QSize(16, 16));
            m_action_mute[ref]->setProperty("astid", astid);
            m_action_mute[ref]->setProperty("roomname", roomname);
            m_action_mute[ref]->setProperty("usernum", usernum);
            m_action_mute[ref]->setProperty("reference", ref);
            m_action_mute[ref]->setProperty("uniqueid", uniqueid);
            m_action_mute[ref]->setProperty("action", "mute");
            m_action_mute[ref]->setProperty("mutestatus", "off");
            connect(m_action_mute[ref], SIGNAL(clicked()),
                    this, SLOT(doMeetMeAction()));
/*
            if(userinfo) {
                if((userinfo->userid() == adminid) || (userinfo->userid() == userid))
                    m_action_mute[ref]->show();
                else
                    m_action_mute[ref]->hide();
            }
*/
            // QPushButton * qp2 = new QPushButton(tr("Spy"));
            
            m_layout[idxroom]->addWidget( m_infos[ref], usernum.toInt(), 1 );
            m_layout[idxroom]->addWidget( m_action_kick[ref], usernum.toInt(), 2 );
            m_layout[idxroom]->addWidget( m_action_record[ref], usernum.toInt(), 3 );
            m_layout[idxroom]->addWidget( m_action_mute[ref], usernum.toInt(), 4 );
            m_layout[idxroom]->addWidget( m_timespent[ref], usernum.toInt(), 5 );
            // glayout->addWidget( qp2, 0, 3 );
        }
    } else if(action == "leave") {
        if(m_user_not_authed.contains(ref)) {
            delete m_user_not_authed[ref];
            m_user_not_authed.remove(ref);
        }
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
        foreach (QString r, m_infos.keys()) {
            if((m_infos[r]->property("astid").toString() == astid) && (m_infos[r]->property("roomname").toString() == roomname)) {
                count ++;
            }
        }
        if(count == 0)
            delRoomTab(astid, meetmeid);
    } else if(action == "auth") {
        delete m_user_not_authed[ref];
        m_user_not_authed.remove(ref);
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
    } else if(action == "changeroompausedstate") {
        QString pause_state_label;
        bool paused = m_engine->meetme()[astid][meetmeid]->paused();
        if (paused) {
            pause_state_label = tr("&Start/Continue the conference");
        } else {
            pause_state_label = tr("&Put the conference room in pause");
        }

        m_action_chamber_toggle_pause[idxroom]->setProperty("astid", astid);
        m_action_chamber_toggle_pause[idxroom]->setProperty("roomname", roomname);
        m_action_chamber_toggle_pause[idxroom]->setProperty("togglepause", QString((paused)?"off":"on"));
        m_action_chamber_toggle_pause[idxroom]->setText(pause_state_label);
    }
}

/*! \brief execute meet me Admin actions (screw/talk with/allow in)
 */
void ConferencePanel::doMeetMeAdminOnlyAction(QTreeWidgetItem * item, int column)
{
    QString action = sender()->property("action").toString();
    QString ref = sender()->property("reference").toString();

    QString cast_id = item->text(4);
    QString meet_num = item->text(5);
    QString user_id = item->text(6);
    QString admin_id = item->text(7).right(item->text(7).length()-item->text(7).indexOf('/')-1) ;

    switch (column) {
        case 1: // screw him
            meetmeAction("MeetmeKick", cast_id + " " + meet_num + " " + user_id + " " + admin_id);
            break;
        case 2: // talk to him
            meetmeAction("MeetmeTalk", cast_id + " " + meet_num + " " + user_id + " " + admin_id);
            break;
        case 3: // accept him in
            meetmeAction("MeetmeAccept", cast_id + " " + meet_num + " " + user_id + " " + admin_id);
            break;
        default:
            break;
    }

    item->setSelected(true);
}

/*! \brief change the paused state of the room
 */
void ConferencePanel::changeRoomPauseState()
{
    QString action = sender()->property("action").toString();
    
    meetmeAction("MeetmePause",
                 sender()->property("astid").toString() +
                 " " + sender()->property("roomname").toString() +
                 " " + sender()->property("togglepause").toString());
}

/*! \brief execute meet me actions (kick/record/mute)
 */
void ConferencePanel::doMeetMeAction()
{
    QString action = sender()->property("action").toString();
    QString ref = sender()->property("reference").toString();
    qDebug() << "ConferencePanel::doMeetMeAction()" << action << ref;
    
    QString meetmeidentity = QString("%1 %2 %3 %4")
        .arg(sender()->property("astid").toString())
        .arg(sender()->property("roomname").toString())
        .arg(sender()->property("usernum").toString())
        .arg(sender()->property("uniqueid").toString());
    
    if(action == "kick") {
        meetmeAction(action, meetmeidentity);
        m_action_kick[ref]->setIconSize(QSize(8, 8));
    } else if(action == "record") {
        if(m_action_record[ref]->property("recordstatus").toString() == "off") {
            meetmeAction("record", meetmeidentity);
            m_action_record[ref]->setProperty("recordstatus", "on");
            m_action_record[ref]->setText(tr("Stop Record"));
        } else {
            meetmeAction("stoprecord", meetmeidentity);
            m_action_record[ref]->setProperty("recordstatus", "off");
            m_action_record[ref]->setText(tr("Record"));
        }
    } else if(action == "mute") {
        QString mutestatus = m_action_mute[ref]->property("mutestatus").toString();
        if(mutestatus == "off") {
            meetmeAction("mute", meetmeidentity);
        } else if(mutestatus == "on") {
            meetmeAction("unmute", meetmeidentity);
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
    QString summaryformat;
    QString membercountformat;
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
            QString roomname = m_engine->meetme()[astid][meetmeid]->roomname();
            QString roomnumber = m_engine->meetme()[astid][meetmeid]->roomnumber();
            count = m_engine->meetme()[astid][meetmeid]->uniqueids().count();
            membercountformat = tr("%n member(s)", "", count);
            
            if(roomnumber.isEmpty())
                summaryformat = tr("%1 : %2\n").arg(roomname).arg(membercountformat);
            else
                summaryformat = tr("%1 (%2) : %3\n").arg(roomname).arg(roomnumber).arg(membercountformat);
            summary.append(summaryformat);
        }
    }
    m_summary->setText( summary );
}

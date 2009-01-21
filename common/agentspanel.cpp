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
#include <QFrame>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QVariant>

#include "agentspanel.h"
#include "userinfo.h"

/*! \brief Constructor
 */
AgentsPanel::AgentsPanel(const QVariant & optionmap,
                         QWidget * parent)
        : QWidget(parent)
{
        m_gui_buttonsize = 10;
        
	m_gridlayout = new QGridLayout(this);
        m_title1 = new QLabel(tr("Agent"), this);
        m_title2 = new QLabel(tr("Record"), this);
        m_title3 = new QLabel(tr("Listen"), this);
        m_title4 = new QLabel(tr("On Line"), this);
        m_title5 = new QLabel(tr("Presence"), this);
        m_title_logged = new QLabel(tr("Logged"), this);
        m_title_paused = new QLabel(tr("Paused"), this);
        m_title_njoined = new QLabel(tr("Joined\nqueues"), this);
        m_title_npaused = new QLabel(tr("Paused\nqueues"), this);
        
        m_gridlayout->addWidget(m_title1, 0, 0, 1, 2, Qt::AlignLeft );
        m_gridlayout->addWidget(m_title2, 0, 2, 1, 1, Qt::AlignCenter );
        m_gridlayout->addWidget(m_title3, 0, 3, 1, 1, Qt::AlignCenter );
        m_gridlayout->addWidget(m_title4, 0, 4, 1, 1, Qt::AlignCenter );
        m_gridlayout->addWidget(m_title5, 0, 6, 1, 1, Qt::AlignCenter );
        m_gridlayout->addWidget(m_title_logged, 0, 8, 1, 2, Qt::AlignCenter );
        m_gridlayout->addWidget(m_title_njoined, 0, 10, 1, 1, Qt::AlignCenter );
        m_gridlayout->addWidget(m_title_paused, 0, 12, 1, 2, Qt::AlignCenter );
        m_gridlayout->addWidget(m_title_npaused, 0, 14, 1, 1, Qt::AlignCenter );
        m_gridlayout->setColumnStretch( 15, 1 );
        m_gridlayout->setRowStretch( 100, 1 );
        m_gridlayout->setVerticalSpacing(0);
        
        setGuiOptions(optionmap);
}

AgentsPanel::~AgentsPanel()
{
        // qDebug() << "AgentsPanel::~AgentsPanel()";
}

void AgentsPanel::setGuiOptions(const QVariant & optionmap)
{
        if(optionmap.toMap().contains("fontname") && optionmap.toMap().contains("fontsize"))
                m_gui_font = QFont(optionmap.toMap()["fontname"].toString(),
                                   optionmap.toMap()["fontsize"].toInt());
        if(optionmap.toMap().contains("iconsize"))
                m_gui_buttonsize = optionmap.toMap()["iconsize"].toInt();
        
        // setFont(m_gui_font);
        m_title1->setFont(m_gui_font);
        m_title2->setFont(m_gui_font);
        m_title3->setFont(m_gui_font);
        m_title4->setFont(m_gui_font);
        m_title5->setFont(m_gui_font);
        m_title_logged->setFont(m_gui_font);
        m_title_paused->setFont(m_gui_font);
        m_title_njoined->setFont(m_gui_font);
        m_title_npaused->setFont(m_gui_font);
}

void AgentsPanel::setUserInfo(const UserInfo * ui)
{
        m_userinfo = ui;
}

void AgentsPanel::updateAgentPresence(const QString & agentname, const QVariant & presencestatus)
{
        // qDebug() << "AgentsPanel::updateAgentPresence()" << agentname << presencestatus;
        if(agentname.size() > 0)
                if(m_agent_presence.contains(agentname)) {
                        QColor color = QColor(presencestatus.toMap()["color"].toString());
                        QPixmap * p_square = new QPixmap(m_gui_buttonsize, m_gui_buttonsize);
                        p_square->fill(color);
                        m_agent_presence[agentname]->setPixmap(QPixmap(* p_square));
                }
}

void AgentsPanel::updatePeerAgent(int,
                                  const QString &,
                                  const QString & what,
                                  const QVariant & params)
{
        if(what != "agentstatus")
                return;
        // qDebug() << "AgentsPanel::updatePeerAgent()" << what << params;
        QString action = params.toMap()["action"].toString();
        QString astid = params.toMap()["astid"].toString();
        QString agentnum = params.toMap()["agent_channel"].toString().mid(6);
        QString qname = params.toMap()["queuename"].toString();
        QString jstatus = params.toMap()["joinedstatus"].toString();
        QString pstatus = params.toMap()["pausedstatus"].toString();
        
        if(action == "queuememberstatus") {
                if(m_agent_labels.contains(agentnum)) {
                        if(jstatus == "1") {
                                QPixmap * p_square = new QPixmap(m_gui_buttonsize, m_gui_buttonsize);
                                p_square->fill(Qt::green);
                                m_agent_logged_status[agentnum]->setPixmap(QPixmap(* p_square));
                                m_agent_logged_status[agentnum]->setProperty("logged", true);
                                m_agent_logged_action[agentnum]->setIcon(QIcon(":/images/cancel.png"));
                        } else if(jstatus == "5") {
                                QPixmap * p_square = new QPixmap(m_gui_buttonsize, m_gui_buttonsize);
                                p_square->fill(Qt::red);
                                m_agent_logged_status[agentnum]->setPixmap(QPixmap(* p_square));
                                m_agent_logged_status[agentnum]->setProperty("logged", false);
                                m_agent_logged_action[agentnum]->setIcon(QIcon(":/images/button_ok.png"));
                        } else if(jstatus == "3") {
                                qDebug() << "AgentsPanel::updatePeerAgent()" << what << jstatus;
                        } else {
                                qDebug() << "AgentsPanel::updatePeerAgent()" << what << jstatus;
                        }
                }
        } else if(action == "joinqueue") {
                if(m_agent_labels.contains(agentnum)) {
                        if(! m_agent_joined_list[agentnum].contains(qname)) {
                                QPixmap * p_square = new QPixmap(m_gui_buttonsize, m_gui_buttonsize);
                                p_square->fill(Qt::green);
                                m_agent_joined_list[agentnum].append(qname);
                                // m_agent_joined_status[agentnum]->setPixmap(QPixmap(* p_square));
                                
                                if(pstatus == "0") {
                                        if(! m_agent_paused_list[agentnum].contains(qname)) {
                                                QPixmap * p_square = new QPixmap(m_gui_buttonsize, m_gui_buttonsize);
                                                p_square->fill(Qt::green);
                                                m_agent_paused_list[agentnum].append(qname);
                                        }
                                } else {
                                        if(m_agent_paused_list[agentnum].contains(qname)) {
                                                m_agent_paused_list[agentnum].removeAll(qname);
                                        }
                                }
                        }
                        m_agent_joined_number[agentnum]->setText(QString::number(m_agent_joined_list[agentnum].size()));
                        showPausedStatus(agentnum,
                                         m_agent_joined_list[agentnum].size() - m_agent_paused_list[agentnum].size());
                }
        } else if(action == "leavequeue") {
                if(m_agent_labels.contains(agentnum)) {
                        if(m_agent_joined_list[agentnum].contains(qname)) {
                                m_agent_joined_list[agentnum].removeAll(qname);
                                m_agent_paused_list[agentnum].removeAll(qname);
                        }
                        m_agent_joined_number[agentnum]->setText(QString::number(m_agent_joined_list[agentnum].size()));
                        showPausedStatus(agentnum,
                                         m_agent_joined_list[agentnum].size() - m_agent_paused_list[agentnum].size());
                }
        } else if(action == "unpaused") {
                if(m_agent_labels.contains(agentnum)) {
                        if(! m_agent_paused_list[agentnum].contains(qname))
                                m_agent_paused_list[agentnum].append(qname);
                        showPausedStatus(agentnum,
                                         m_agent_joined_list[agentnum].size() - m_agent_paused_list[agentnum].size());
                }
        } else if(action == "paused") {
                if(m_agent_labels.contains(agentnum)) {
                        if(m_agent_paused_list[agentnum].contains(qname))
                                m_agent_paused_list[agentnum].removeAll(qname);
                        showPausedStatus(agentnum,
                                         m_agent_joined_list[agentnum].size() - m_agent_paused_list[agentnum].size());
                }
        } else if(action == "agentlogin") {
                if(m_agent_labels.contains(agentnum)) {
                        QPixmap * p_square = new QPixmap(m_gui_buttonsize, m_gui_buttonsize);
                        p_square->fill(Qt::green);
                        m_agent_logged_status[agentnum]->setPixmap(QPixmap(* p_square));
                        m_agent_logged_status[agentnum]->setProperty("logged", true);
                        m_agent_logged_action[agentnum]->setIcon(QIcon(":/images/cancel.png"));
                }
        } else if(action == "agentlogout") {
                if(m_agent_labels.contains(agentnum)) {
                        QPixmap * p_square = new QPixmap(m_gui_buttonsize, m_gui_buttonsize);
                        p_square->fill(Qt::red);
                        m_agent_logged_status[agentnum]->setPixmap(QPixmap(* p_square));
                        m_agent_logged_status[agentnum]->setProperty("logged", false);
                        m_agent_logged_action[agentnum]->setIcon(QIcon(":/images/button_ok.png"));
                }
        } else if((action == "agentlink") || (action == "phonelink")) {
                QPixmap * p_square = new QPixmap(m_gui_buttonsize, m_gui_buttonsize);
                p_square->fill(Qt::green);
                if(m_agent_busy.contains(agentnum))
                        if(astid == m_agent_busy[agentnum]->property("astid").toString())
                                m_agent_busy[agentnum]->setPixmap(QPixmap(* p_square));
        } else if((action == "agentunlink") || (action == "phoneunlink")) {
                QPixmap * p_square = new QPixmap(m_gui_buttonsize, m_gui_buttonsize);
                p_square->fill(Qt::gray);
                if(m_agent_busy.contains(agentnum))
                        if(astid == m_agent_busy[agentnum]->property("astid").toString())
                                m_agent_busy[agentnum]->setPixmap(QPixmap(* p_square));
        }
}

void AgentsPanel::setAgentList(int, const QVariant & alist)
{
        if(m_userinfo == NULL)
                return;
        qDebug() << "AgentsPanel::setAgentList()" << alist.toMap().keys();
        QPixmap * p_square = new QPixmap(m_gui_buttonsize, m_gui_buttonsize);
        QVariantMap alistmap = alist.toMap();
        QString astid = alistmap["astid"].toString();
        // if(m_userinfo->astid() != astid) return;
        QStringList agentids = alistmap["newlist"].toMap().keys();
        agentids.sort();
        foreach (QString agnum, agentids) {
                QString firstname = alistmap["newlist"].toMap()[agnum].toMap()["firstname"].toString();
                QString lastname = alistmap["newlist"].toMap()[agnum].toMap()["lastname"].toString();
                QString agentcontext = alistmap["newlist"].toMap()[agnum].toMap()["context"].toString();
                if(! m_userinfo->contexts().contains(agentcontext))
                        continue;
                QString agfullname = QString("%1 %2").arg(firstname).arg(lastname);
                
                QVariant properties = alistmap["newlist"].toMap()[agnum].toMap()["properties"];
                QVariantMap agqjoined = alistmap["newlist"].toMap()[agnum].toMap()["queues"].toMap();
                QString agstatus = properties.toMap()["status"].toString();
                QString phonenum = properties.toMap()["phonenum"].toString();
                bool link = properties.toMap()["link"].toBool();
                // qDebug() << "AgentsPanel::setAgentList()" << agnum << agstatus << agfullname << phonenum << link;
                
                if(! m_agent_labels.contains(agnum)) {
                        QFrame * qvline1 = new QFrame(this);
                        qvline1->setFrameShape(QFrame::VLine);
                        qvline1->setLineWidth(1);
                        QFrame * qvline2 = new QFrame(this);
                        qvline2->setFrameShape(QFrame::VLine);
                        qvline2->setLineWidth(1);
                        QFrame * qvline3 = new QFrame(this);
                        qvline3->setFrameShape(QFrame::VLine);
                        qvline3->setLineWidth(1);
                        
                        m_agent_labels[agnum] = new QLabel(QString("%1 (%2)").arg(agfullname).arg(agnum), this);
                        m_agent_more[agnum] = new QPushButton(this);
                        m_agent_more[agnum]->setProperty("astid", astid);
                        m_agent_more[agnum]->setProperty("agentid", agnum);
                        m_agent_more[agnum]->setProperty("action", "changeagent");
                        connect( m_agent_more[agnum], SIGNAL(clicked()),
                                 this, SLOT(agentClicked()));
                        
                        m_agent_record[agnum] = new QPushButton(this);
                        m_agent_record[agnum]->setProperty("astid", astid);
                        m_agent_record[agnum]->setProperty("agentid", agnum);
                        m_agent_record[agnum]->setProperty("action", "record");
                        connect( m_agent_record[agnum], SIGNAL(clicked()),
                                 this, SLOT(agentClicked()));
                        
                        m_agent_listen[agnum] = new QPushButton(this);
                        m_agent_listen[agnum]->setProperty("astid", astid);
                        m_agent_listen[agnum]->setProperty("agentid", agnum);
                        m_agent_listen[agnum]->setProperty("action", "listen");
                        connect( m_agent_listen[agnum], SIGNAL(clicked()),
                                 this, SLOT(agentClicked()));
                        
                        m_agent_busy[agnum] = new QLabel(this);
                        m_agent_busy[agnum]->setProperty("astid", astid);
                        m_agent_presence[agnum] = new QLabel(this);
                        m_agent_logged_status[agnum] = new QLabel(this);
                        m_agent_logged_action[agnum] = new QPushButton(this);
                        m_agent_logged_action[agnum]->setProperty("astid", astid);
                        m_agent_logged_action[agnum]->setProperty("agentid", agnum);
                        m_agent_logged_action[agnum]->setProperty("action", "loginoff");
                        connect( m_agent_logged_action[agnum], SIGNAL(clicked()),
                                 this, SLOT(agentClicked()));
                        
                        m_agent_joined_number[agnum] = new QLabel(this);
                        // m_agent_joined_status[agnum] = new QLabel(this);
                        m_agent_paused_number[agnum] = new QLabel(this);
                        m_agent_paused_status[agnum] = new QLabel(this);
                        m_agent_paused_action[agnum] = new QPushButton(this);
                        m_agent_paused_action[agnum]->setProperty("astid", astid);
                        m_agent_paused_action[agnum]->setProperty("agentid", agnum);
                        m_agent_paused_action[agnum]->setProperty("action", "pause");
                        connect( m_agent_paused_action[agnum], SIGNAL(clicked()),
                                 this, SLOT(agentClicked()));
                        
                        m_agent_more[agnum]->setIconSize(QSize(m_gui_buttonsize, m_gui_buttonsize));
                        m_agent_more[agnum]->setIcon(QIcon(":/images/add.png"));
                        m_agent_record[agnum]->setIconSize(QSize(m_gui_buttonsize, m_gui_buttonsize));
                        m_agent_record[agnum]->setIcon(QIcon(":/images/player_stop.png"));
                        m_agent_listen[agnum]->setIconSize(QSize(m_gui_buttonsize, m_gui_buttonsize));
                        m_agent_listen[agnum]->setIcon(QIcon(":/images/player_play.png"));
                        m_agent_logged_action[agnum]->setIconSize(QSize(m_gui_buttonsize, m_gui_buttonsize));
                        m_agent_paused_action[agnum]->setIconSize(QSize(m_gui_buttonsize, m_gui_buttonsize));
                        
                        if(link)
                                p_square->fill(Qt::green);
                        else
                                p_square->fill(Qt::gray);
                        m_agent_busy[agnum]->setPixmap(QPixmap(* p_square));
                        p_square->fill(Qt::gray);
                        m_agent_presence[agnum]->setPixmap(QPixmap(* p_square));
                        
                        if(agstatus == "AGENT_IDLE") {
                                p_square->fill(Qt::green);
                                m_agent_logged_action[agnum]->setIcon(QIcon(":/images/cancel.png"));
                                m_agent_logged_status[agnum]->setProperty("logged", true);
                        } else if(agstatus == "AGENT_LOGGEDOFF") {
                                p_square->fill(Qt::red);
                                m_agent_logged_action[agnum]->setIcon(QIcon(":/images/button_ok.png"));
                                m_agent_logged_status[agnum]->setProperty("logged", false);
                        } else {
                                qDebug() << "AgentsPanel::setAgentList() unknown status" << agstatus;
                        }
                        m_agent_logged_status[agnum]->setPixmap(QPixmap(* p_square));
                        
                        foreach (QString qname, agqjoined.keys()) {
                                QVariant qv = agqjoined[qname];
                                QString pstatus = qv.toMap()["Paused"].toString();
                                QString sstatus = qv.toMap()["Status"].toString();
                                m_agent_joined_list[agnum] << qname;
                                if(pstatus == "0")
                                        m_agent_paused_list[agnum] << qname;
                        }
                        
                        int njoined = m_agent_joined_list[agnum].size();
                        m_agent_joined_number[agnum]->setText(QString::number(njoined));
                        
                        int nunpaused = m_agent_paused_list[agnum].size();
                        showPausedStatus(agnum, njoined - nunpaused);
                        
                        int colnum = 0;
                        int linenum = m_agent_labels.size();
                        m_gridlayout->addWidget( m_agent_labels[agnum], linenum, colnum++, Qt::AlignLeft );
                        m_gridlayout->addWidget( m_agent_more[agnum], linenum, colnum++, Qt::AlignCenter );
                        m_gridlayout->addWidget( m_agent_record[agnum], linenum, colnum++, Qt::AlignCenter );
                        m_gridlayout->addWidget( m_agent_listen[agnum], linenum, colnum++, Qt::AlignCenter );
                        m_gridlayout->addWidget( m_agent_busy[agnum], linenum, colnum++, Qt::AlignCenter );
                        m_gridlayout->addWidget( qvline1, linenum, colnum++, Qt::AlignHCenter );
                        m_gridlayout->addWidget( m_agent_presence[agnum], linenum, colnum++, Qt::AlignCenter );
                        m_gridlayout->addWidget( qvline2, linenum, colnum++, Qt::AlignHCenter );
                        m_gridlayout->addWidget( m_agent_logged_status[agnum], linenum, colnum++, Qt::AlignCenter );
                        m_gridlayout->addWidget( m_agent_logged_action[agnum], linenum, colnum++, Qt::AlignCenter );
                        m_gridlayout->addWidget( m_agent_joined_number[agnum], linenum, colnum++, Qt::AlignRight );
                        m_gridlayout->addWidget( qvline3, linenum, colnum++, Qt::AlignHCenter );
                        m_gridlayout->addWidget( m_agent_paused_status[agnum], linenum, colnum++, Qt::AlignCenter );
                        m_gridlayout->addWidget( m_agent_paused_action[agnum], linenum, colnum++, Qt::AlignCenter );
                        m_gridlayout->addWidget( m_agent_paused_number[agnum], linenum, colnum++, Qt::AlignRight );
                }
        }
}

void AgentsPanel::showPausedStatus(const QString & agentnum, int npaused)
{
        QPixmap * p_square = new QPixmap(m_gui_buttonsize, m_gui_buttonsize);
        m_agent_paused_number[agentnum]->setText(QString::number(npaused));
        if(npaused > 0) {
                p_square->fill(QColor(255, 128, 0)); // orange
                m_agent_paused_action[agentnum]->setIcon(QIcon(":/images/button_ok.png"));
                m_agent_paused_action[agentnum]->setProperty("action", "unpause");
        } else {
                p_square->fill(Qt::green);
                m_agent_paused_action[agentnum]->setIcon(QIcon(":/images/cancel.png"));
                m_agent_paused_action[agentnum]->setProperty("action", "pause");
        }
        m_agent_paused_status[agentnum]->setPixmap(QPixmap(* p_square));
}

void AgentsPanel::agentClicked()
{
        // qDebug() << "AgentsPanel::agentClicked()" << sender()->property("agentid");
        QString astid   = sender()->property("astid").toString();
        QString agentid = sender()->property("agentid").toString();
        QString action  = sender()->property("action").toString();
        if(action == "changeagent")
                changeWatchedAgent(QString("%1 %2").arg(astid).arg(agentid), true);
        else if(action == "loginoff") {
                if(m_agent_logged_status[agentid]->property("logged").toBool())
                        agentAction(QString("logout %1 %2").arg(astid).arg(agentid));
                else
                        agentAction(QString("login %1 %2").arg(astid).arg(agentid));
        } else if(action == "unpause") {
                agentAction(QString("unpause_all %1 %2").arg(astid).arg(agentid));
        } else if(action == "pause") {
                agentAction(QString("pause_all %1 %2").arg(astid).arg(agentid));
        } else if(action == "listen") {
                agentAction(QString("listen %1 %2").arg(astid).arg(agentid));
                m_agent_listen[agentid]->setProperty("action", "stoplisten");
                m_agent_listen[agentid]->setStyleSheet("QPushButton {background: #fbb638}");
        } else if(action == "stoplisten") {
                agentAction(QString("stoplisten %1 %2").arg(astid).arg(agentid));
                m_agent_listen[agentid]->setProperty("action", "listen");
                m_agent_listen[agentid]->setStyleSheet("");
        } else if(action == "record") {
                agentAction(QString("record %1 %2").arg(astid).arg(agentid));
                m_agent_record[agentid]->setProperty("action", "stoprecord");
                m_agent_record[agentid]->setStyleSheet("QPushButton {background: #fbb638}");
        } else if(action == "stoprecord") {
                agentAction(QString("stoprecord %1 %2").arg(astid).arg(agentid));
                m_agent_record[agentid]->setProperty("action", "record");
                m_agent_record[agentid]->setStyleSheet("");
        }
}

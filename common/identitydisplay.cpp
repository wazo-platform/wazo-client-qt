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
#include <QLineEdit>
#include <QMouseEvent>
#include <QSizePolicy>
#include <QProgressBar>
#include <QPushButton>
#include <QRegExp>
#include <QScrollArea>

#include "identitydisplay.h"
#include "userinfo.h"

/*! \brief Constructor
 */
IdentityDisplay::IdentityDisplay(const QVariant & options,
                                 QWidget * parent)
        : QWidget(parent),
          m_ui(NULL), m_agentstatus(false), m_queuechangeallow(true), m_maxqueues(5)
{
        m_gui_buttonsize = 16;
        
	QGridLayout * glayout = new QGridLayout(this);
	// glayout->setMargin(0);
        m_user = new QLabel(this);
        m_user->setObjectName("fullname");
        m_user->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        m_user->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        
        m_phonenum = new QLabel(this);
        m_presencelabel = new QLabel(tr("Status"), this);
        m_presencevalue = new QComboBox(this);
        m_presencevalue->setSizeAdjustPolicy(QComboBox::AdjustToContents);
        m_presencevalue->setProperty("function", "presence");
        
        m_voicemail = new QLabel(this);
        
        m_qf = new QFrame(this);
        m_qf->setFrameShape(QFrame::HLine);
        m_qf->setLineWidth(2);
        
        m_agent = new QLabel("", this);
        m_agentstatus_label = new QLabel(tr("Connection"), this);
        m_agentstatus_value = new QPushButton(tr("Logout"), this);
        m_queueleaveall = new QPushButton(tr("Leave All"), this);
        m_queueleaveall->setIcon(QIcon(":/images/cancel.png"));
        m_queuejoinall = new QPushButton(tr("Join All"), this);
        m_queuejoinall->setIcon(QIcon(":/images/add.png"));
        m_queueaction = new QPushButton(tr("Leave"), this);
        
        m_queuelist = new QComboBox(this);
        m_queuelist->setSizeAdjustPolicy(QComboBox::AdjustToContents);
        m_queuelist->setProperty("function", "queuelist");
        
        connect(m_queuelist, SIGNAL(currentIndexChanged(const QString &)),
                this, SLOT(idxChanged(const QString &)));
        connect(m_presencevalue, SIGNAL(currentIndexChanged(const QString &)),
                this, SLOT(idxChanged(const QString &)));
        if(options.toMap()["logagent"].toBool())
                connect(m_agentstatus_value, SIGNAL(clicked()),
                        this, SLOT(doAgentAction()));
        connect(m_queueaction, SIGNAL(clicked()),
                this, SLOT(doQueueAction()));
        connect(m_queueleaveall, SIGNAL(clicked()),
                this, SLOT(doQueueLeaveAll()));
        connect(m_queuejoinall, SIGNAL(clicked()),
                this, SLOT(doQueueJoinAll()));
        
        int idline = 0;
	glayout->addWidget( m_user, idline, 0, 1, 2, Qt::AlignCenter );
	glayout->addWidget( m_agent, idline, 2, 1, 2, Qt::AlignCenter );
	glayout->addWidget( m_phonenum, idline, 4, Qt::AlignCenter );
        idline ++;
	glayout->addWidget( m_presencelabel, idline, 0, Qt::AlignRight );
	glayout->addWidget( m_presencevalue, idline, 1, Qt::AlignLeft );
        glayout->addWidget( m_agentstatus_label, idline, 2, Qt::AlignRight );
	glayout->addWidget( m_agentstatus_value, idline, 3, Qt::AlignLeft );
	glayout->addWidget( m_voicemail, idline, 4, Qt::AlignCenter );
        idline ++;
	// glayout->addWidget( m_qf, idline, 0, 1, 7, 0 );
	// glayout->addWidget( m_agent, idline, 0, Qt::AlignCenter );
        // glayout->addWidget( m_queueaction, idline, 4, Qt::AlignCenter );
	// glayout->addWidget( m_agentstatus_value, idline, 1, Qt::AlignCenter );
        // glayout->addWidget( m_queuelist, idline, 3, Qt::AlignCenter );
	// glayout->addWidget( m_queuejoinall, idline, 5, Qt::AlignCenter );
	// glayout->addWidget( m_queueleaveall, idline, 6, Qt::AlignCenter );
        
        // although it might be convenient in some cases (prevent some expansions),
        // in the basic xivoclient/grid case, it fills too much room without no resizing available
        // glayout->setRowStretch( idline, 1 );
        
        hideAgentProps();
        
        m_queueaction->hide();
        m_queuelist->hide();
        m_queueleaveall->hide();
        m_queuejoinall->hide();
        
        setGuiOptions(options);
        // 	glayout->setColumnStretch( 0, 1 );
}

void IdentityDisplay::setGuiOptions(const QVariant & options)
{
        // qDebug() << "IdentityDisplay::setGuiOptions()" << options;
        if(options.toMap().contains("fontname") && options.toMap().contains("fontsize"))
                m_gui_font = QFont(options.toMap()["fontname"].toString(),
                                   options.toMap()["fontsize"].toInt());
        if(options.toMap().contains("iconsize"))
                m_gui_buttonsize = options.toMap()["iconsize"].toInt();
        
        m_loginkind = options.toMap()["loginkind"].toUInt();
        m_functions = options.toMap()["functions"].toStringList();
        
        // setFont(m_gui_font);
        m_user->setFont(m_gui_font);
        m_phonenum->setFont(m_gui_font);
        m_presencevalue->setFont(m_gui_font);
        m_presencelabel->setFont(m_gui_font);
        m_agent->setFont(m_gui_font);
        m_agentstatus_label->setFont(m_gui_font);
        m_agentstatus_value->setFont(m_gui_font);
        m_queueleaveall->setFont(m_gui_font);
        m_queuejoinall->setFont(m_gui_font);
        m_queueaction->setFont(m_gui_font);
        m_queuelist->setFont(m_gui_font);
        
        m_agentstatus_value->setIconSize(QSize(m_gui_buttonsize, m_gui_buttonsize));
        m_queueleaveall->setIconSize(QSize(m_gui_buttonsize, m_gui_buttonsize));
        m_queuejoinall->setIconSize(QSize(m_gui_buttonsize, m_gui_buttonsize));
        m_queueaction->setIconSize(QSize(m_gui_buttonsize, m_gui_buttonsize));
        m_queuelist->setIconSize(QSize(m_gui_buttonsize, m_gui_buttonsize));
}

void IdentityDisplay::updatePresence(const QVariant & presence)
{
        // qDebug() << "IdentityDisplay::updatePresence()" << presence;
        m_presencelabel->hide();
        m_presencevalue->hide();
        if(! m_functions.contains("presence"))
                return;
        
        disconnect(m_presencevalue, SIGNAL(currentIndexChanged(const QString &)),
                   this, SLOT(idxChanged(const QString &)));
        QVariantMap presencemap = presence.toMap();
        if(presencemap.contains("names")) {
                foreach (QString avstate, presencemap["names"].toMap().keys()) {
                        QString name = presencemap["names"].toMap()[avstate].toMap()["longname"].toString();
                        if(m_presencevalue->findText(name) == -1) {
                                m_presencevalue->addItem(name);
                                m_presence_names[avstate] = name;
                        }
                }
        }
        if(presencemap.contains("allowed")) {
                QMapIterator<QString, QVariant> capapres(presencemap["allowed"].toMap());
                while (capapres.hasNext()) {
                        capapres.next();
                        QString avstate = capapres.key();
                        bool allow = capapres.value().toBool();
                        if(m_presence_names.contains(avstate)) {
                                QString name = m_presence_names[avstate];
                                int idx = m_presencevalue->findText(name);
                                if(idx != -1) {
                                        if(! allow)
                                                m_presencevalue->removeItem(idx);
                                }
                        }
                }
        }
        if(presencemap.contains("state")) {
                QString avstate = presencemap["state"].toMap()["stateid"].toString();
                if(m_presence_names.contains(avstate)) {
                        QString name = m_presence_names[avstate];
                        int idx = m_presencevalue->findText(name);
                        m_presencevalue->setCurrentIndex(idx);
                }
        }
        connect(m_presencevalue, SIGNAL(currentIndexChanged(const QString &)),
                this, SLOT(idxChanged(const QString &)));
        m_presencelabel->show();
        m_presencevalue->show();
}


void IdentityDisplay::setUserInfo(const UserInfo * ui)
{
        // qDebug() << "IdentityDisplay::setUserInfo()";
        m_ui = ui;

        m_user->setText(m_ui->fullname());
//         m_user->setMinimumSize(m_user->sizeHint().width() * 4,
//                                m_user->sizeHint().height() * 2);
        m_phonenum->setText(m_ui->phonenum());
        // + tr("on") + " <b>" + m_ui->astid() + "</b>");
        // m_presencevalue->setText(m_ui->availstate());
        QStringList vm = m_ui->mwi();
        if(vm.size() > 2)
                m_voicemail->setText(tr("Voicemail") + "\n" +
                                     // vm[0] + " " + tr("waiting") + " " +
                                     vm[1] + " " + tr("old") + " " +
                                     vm[2] + " " + tr("new"));
        // m_voicemail->hide();
        // changes the "watched agent" only if no one else has done it before
        changeWatchedAgent(m_ui->astid() + " " + m_ui->agentid(), false);
}

void IdentityDisplay::setAgentList(const QVariant & alist)
{
        // qDebug() << "IdentityDisplay::setAgentList()" << m_loginkind << alist;
        if (m_loginkind == 0)
                return;
        if (m_ui == NULL)
                return;
        QVariantMap alistmap = alist.toMap();
        QString astid = alistmap["astid"].toString();
        qDebug() << "IdentityDisplay::setAgentList()" << m_ui->agentid() << astid << m_ui->astid();
        if (astid != m_ui->astid())
                return;
        
        QStringList agentids = alistmap["newlist"].toMap().keys();
        agentids.sort();
        foreach (QString agnum, agentids) {
                if(agnum == m_ui->agentid()) {
                        QVariant properties = alistmap["newlist"].toMap()[agnum].toMap()["properties"];
                        QVariantList agqjoined = alistmap["newlist"].toMap()[agnum].toMap()["queues"].toList();
                        QString agstatus = properties.toMap()["status"].toString();
                        QString agfullname = properties.toMap()["name"].toString();
                        QString phonenum = properties.toMap()["phonenum"].toString();
                        
                        m_agent->setText("Agent " + agnum);
                        m_agentstatus_value->setText(phonenum);
                        showAgentProps();
                        
                        if(agstatus == "AGENT_LOGGEDOFF") {
                                m_agentstatus_value->setIcon(QIcon(":/images/cancel.png"));
                                m_agentstatus = false;
                                m_queueaction->hide();
                                m_queuelist->hide();
                                m_queueleaveall->hide();
                                m_queuejoinall->hide();
                        } else if(agstatus == "AGENT_IDLE") {
                                m_agentstatus_value->setIcon(QIcon(":/images/button_ok.png"));
                                m_agentstatus = true;
//                                 if(m_queuesindexes.size() > 0) {
//                                         if(m_queuechangeallow) {
//                                                 m_queueaction->show();
//                                                 m_queueleaveall->show();
//                                                 m_queuejoinall->show();
//                                         }
//                                         // m_queuelist->show();
//                                 }
                        }
                        
                        foreach (QVariant qv, agqjoined) {
                                QStringList agqprops = qv.toStringList();
                                qDebug() << "IdentityDisplay::setAgentList()" << agqprops;
                                QString queuename = agqprops[0];
                                if (m_queuesindexes.contains(queuename)) {
                                        int idx = m_queuesindexes[queuename];
                                        if (agqprops.size() > 1) {
                                                m_queuelist->setItemIcon(idx, QIcon(":/images/button_ok.png"));
                                                m_queuesstatuses[queuename] = true;
                                        } else {
                                                m_queuelist->setItemIcon(idx, QIcon(":/images/cancel.png"));
                                                m_queuesstatuses[queuename] = false;
                                        }
                                        if(queuename == m_queuelist->currentText())
                                                idxChanged(queuename);
                                }
                        }
                }
        }
}

void IdentityDisplay::setQueueList(bool changeallow, const QVariant & qlist)
{
        if (m_loginkind == 0)
                return;
        m_queuechangeallow = changeallow;
        // qDebug() << "IdentityDisplay::setQueueList()" << qlist;
        if(m_ui == NULL)
                return;
        QVariantMap qlistmap = qlist.toMap();
        QString astid = qlistmap["astid"].toString();
        if (astid != m_ui->astid())
                return;
        QStringList queues = qlistmap["queuestats"].toMap().keys();
        if (queues.size() == 0)
                return;
        queues.sort();
        
        for(int i = 0 ; i < queues.size(); i++) {
                QStringList qparams = queues[i].split(":");
                QString qname = qparams[0];
                if(m_queuelist->findText(qname) == -1) {
                        m_queuelist->addItem(qname);
                        m_queuelist->setItemIcon(i, QIcon(":/images/cancel.png"));
                        m_queuesindexes[qname] = i;
                }
        }
//         if((queues.size() > 0) && (m_agentstatus)) {
//                 if(m_queuechangeallow) {
//                         m_queueaction->show();
//                         m_queuejoinall->show();
//                         m_queueleaveall->show();
//                 }
//                 // m_queuelist->show();
//         }
}

void IdentityDisplay::updatePeer(UserInfo * ui,
                                 const QString &,
                                 const QVariant &)
{
        if(m_ui == NULL)
                return;
        if(ui != m_ui)
                return;
        // qDebug() << "IdentityDisplay::updatePeer()" << ui->astid() << ui->userid() << ui->fullname() << sipstatus << chanlist;
        // QString ext = ui->userid();
        // QString name = ui->fullname();
}


void IdentityDisplay::showAgentProps()
{
        m_agent->show();
        // m_qf->show();
        m_agentstatus_label->show();
        m_agentstatus_value->show();
}

void IdentityDisplay::hideAgentProps()
{
        m_agent->hide();
        m_qf->hide();
        m_agentstatus_label->hide();
        m_agentstatus_value->hide();
}

void IdentityDisplay::updatePeerAgent(const QString & userid,
                                      const QString & what,
                                      const QVariant & newstatuses)
{
        if (m_loginkind == 0)
                return;
        if(m_ui == NULL)
                return;
        if(userid != m_ui->userid())
                return;
        if(what != "agentstatus")
                return;

        QString action = newstatuses.toMap()["action"].toString();
        QString astid = newstatuses.toMap()["astid"].toString();
        QString agentnum = newstatuses.toMap()["agent_channel"].toString().mid(6);
        m_agent->setText("Agent " + agentnum);
        
        if (action == "agentlogin") {
                QString phonenum = newstatuses.toMap()["phonenum"].toString();
                showAgentProps();
                m_agentstatus_value->setIcon(QIcon(":/images/button_ok.png"));
                m_agentstatus_value->setText(phonenum);
                m_agentstatus = true;
                //if(m_queuesindexes.size() > 0) {
                //if(m_queuechangeallow) {
                //m_queueaction->show();
                //m_queueleaveall->show();
                //m_queuejoinall->show();
                //}
                //// m_queuelist->show();
                //}
        } else if (action == "agentlogout") {
                QString phonenum = newstatuses.toMap()["phonenum"].toString();
                showAgentProps();
                m_agentstatus_value->setIcon(QIcon(":/images/cancel.png"));
                m_agentstatus_value->setText(phonenum);
                m_agentstatus = false;
                m_queueaction->hide();
                m_queuelist->hide();
                m_queueleaveall->hide();
                m_queuejoinall->hide();
        } else if (action == "joinqueue") {
                QString queuename = newstatuses.toMap()["queuename"].toString();
                if (m_queuesindexes.contains(queuename)) {
                        int idx = m_queuesindexes[queuename];
                        m_queuelist->setItemIcon(idx, QIcon(":/images/button_ok.png"));
                        m_queuesstatuses[queuename] = true;
                        if(queuename == m_queuelist->currentText())
                                idxChanged(queuename);
                }
        } else if (action == "leavequeue") {
                QString queuename = newstatuses.toMap()["queuename"].toString();
                if (m_queuesindexes.contains(queuename)) {
                        int idx = m_queuesindexes[queuename];
                        m_queuelist->setItemIcon(idx, QIcon(":/images/cancel.png"));
                        m_queuesstatuses[queuename] = false;
                        if(queuename == m_queuelist->currentText())
                                idxChanged(queuename);
                }
//         } else {
//                 qDebug() << "IdentityDisplay::updatePeerAgent()" << newstatuses;
        }
}

void IdentityDisplay::doAgentAction()
{
        if(m_agentstatus)
                agentAction("logout");
        else
                agentAction("login");
}

void IdentityDisplay::doQueueAction()
{
        QString ctext = m_queuelist->currentText();
        bool status = m_queuesstatuses[ctext];
        if(status) {
                agentAction("leave " + ctext);
        } else {
                agentAction("join " + ctext);
        }
}

void IdentityDisplay::doQueueLeaveAll()
{
        QHashIterator<QString, bool> statiter(m_queuesstatuses);
        while(statiter.hasNext()) {
                statiter.next();
                if(statiter.value())
                        agentAction("leave " + statiter.key());
        }
}

void IdentityDisplay::doQueueJoinAll()
{
        QHashIterator<QString, bool> statiter(m_queuesstatuses);
        while(statiter.hasNext()) {
                statiter.next();
                if(! statiter.value())
                        agentAction("join " + statiter.key());
        }
}

void IdentityDisplay::idxChanged(const QString & newidx)
{
        // qDebug() << "IdentityDisplay::idxChanged" << newidx;
        QString function = sender()->property("function").toString();
        if(function == "queuelist") {
                if (m_queuesstatuses[newidx]) {
                        m_queueaction->setText(tr("Leave"));
                        m_queueaction->setIcon(QIcon(":/images/cancel.png"));
                } else {
                        m_queueaction->setText(tr("Join"));
                        m_queueaction->setIcon(QIcon(":/images/add.png"));
                }
        } else if(function == "presence") {
                foreach (QString avstate, m_presence_names.keys())
                        if(m_presence_names[avstate] == newidx)
                                setAvailState(avstate, true);
        }
}

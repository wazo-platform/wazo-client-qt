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

#include <QAction>
#include <QComboBox>
#include <QDebug>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QMouseEvent>
#include <QSizePolicy>
#include <QProgressBar>
#include <QPushButton>
#include <QRegExp>
#include <QScrollArea>

#include "extendedlabel.h"
#include "identitydisplay.h"
#include "userinfo.h"

/*! \brief Constructor
 */
IdentityDisplay::IdentityDisplay(const QVariant & options,
                                 QWidget * parent)
        : QWidget(parent),
          m_ui(NULL), m_agentstatus(false)
{
        m_gui_buttonsize = 16;
        
	QGridLayout * glayout = new QGridLayout(this);
	// glayout->setMargin(0);
        m_user = new QLabel(this);
        m_user->setObjectName("fullname");
        m_user->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        m_user->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        
        m_phonenum = new QLabel(this);
        m_presencevalue = new QComboBox(this);
        m_presencevalue->setSizeAdjustPolicy(QComboBox::AdjustToContents);
        m_presencevalue->setProperty("function", "presence");
        
        m_voicemail_old = new QLabel(this);
        m_voicemail_new = new QLabel(this);
        
        m_agent = new QLabel("", this);
        m_agentstatus_value = new QPushButton(tr("Logout"), this);
        m_agentpause_value = new QPushButton(tr("Withdraw"), this);
        
        connect(m_presencevalue, SIGNAL(currentIndexChanged(const QString &)),
                this, SLOT(idxChanged(const QString &)));
        if(options.toMap()["logagent"].toBool())
                connect(m_agentstatus_value, SIGNAL(clicked()),
                        this, SLOT(doAgentLogActions()));
        if(options.toMap()["pauseagent"].toBool())
                connect(m_agentpause_value, SIGNAL(clicked()),
                        this, SLOT(doAgentPauseActions()));
        
        int bigiconsize = 64;
        m_icon_user = new ExtendedLabel();
        m_icon_agent = new ExtendedLabel();
        m_icon_voicemail = new ExtendedLabel();
        
        m_icon_user->setPixmap(QPixmap(":/images/personal.png").scaled(QSize(bigiconsize, bigiconsize)));
        m_icon_agent->setPixmap(QPixmap(":/images/applixware.png").scaled(QSize(bigiconsize, bigiconsize)));
        m_icon_voicemail->setPixmap(QPixmap(":/images/kthememgr.png").scaled(QSize(bigiconsize, bigiconsize)));
        m_icon_user->setProperty("icon", "user");
        m_icon_agent->setProperty("icon", "agent");
        m_icon_voicemail->setProperty("icon", "voicemail");
        
        connect( m_icon_user, SIGNAL(context_menu(QContextMenuEvent *)),
                 this, SLOT(contextMenuEvent(QContextMenuEvent *)) );
        connect( m_icon_agent, SIGNAL(context_menu(QContextMenuEvent *)),
                 this, SLOT(contextMenuEvent(QContextMenuEvent *)) );
        connect( m_icon_voicemail, SIGNAL(context_menu(QContextMenuEvent *)),
                 this, SLOT(contextMenuEvent(QContextMenuEvent *)) );
        
        m_qvline1 = new QFrame(this);
        m_qvline1->setFrameShape(QFrame::VLine);
        m_qvline1->setLineWidth(2);
        m_qvline2 = new QFrame(this);
        m_qvline2->setFrameShape(QFrame::VLine);
        m_qvline2->setLineWidth(2);
        
        glayout->addWidget( m_icon_user, 0, 1, 4, 1, Qt::AlignCenter );
        glayout->addWidget( m_icon_agent, 0, 4, 4, 1, Qt::AlignCenter );
        glayout->addWidget( m_icon_voicemail, 0, 7, 4, 1, Qt::AlignCenter );
        glayout->addWidget( m_qvline1, 0, 3, 4, 1 );
        glayout->addWidget( m_qvline2, 0, 6, 4, 1 );
        
        int idline = 0;
	glayout->addWidget( m_user, idline, 2, Qt::AlignCenter );
	glayout->addWidget( m_agent, idline, 5, Qt::AlignCenter );
        idline ++;
	glayout->addWidget( m_phonenum, idline, 2, Qt::AlignCenter );
	glayout->addWidget( m_agentstatus_value, idline, 5, Qt::AlignCenter );
	glayout->addWidget( m_voicemail_old, idline, 8, Qt::AlignCenter );
        idline ++;
	glayout->addWidget( m_agentpause_value, idline, 5, Qt::AlignCenter );
	glayout->addWidget( m_voicemail_new, idline, 8, Qt::AlignCenter );
        idline ++;
	glayout->addWidget( m_presencevalue, idline, 2, Qt::AlignCenter );
        idline ++;
        
        glayout->setColumnStretch( 0, 1 );
        glayout->setColumnStretch( 9, 1 );
        
        // although it might be convenient in some cases (prevent some expansions),
        // in the basic xivoclient/grid case, it fills too much room without no resizing available
        // glayout->setRowStretch( idline, 1 );
        
        hideAgentProps();
        
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
        m_agent->setFont(m_gui_font);
        m_agentstatus_value->setFont(m_gui_font);
        m_agentpause_value->setFont(m_gui_font);
        
        m_agentstatus_value->setIconSize(QSize(m_gui_buttonsize, m_gui_buttonsize));
}

void IdentityDisplay::contextMenuEvent(QContextMenuEvent * event)
{
        if(sender() != NULL) {
                QString iconname = sender()->property("icon").toString();
                QMenu contextMenu(this);
                QAction * noAction = new QAction(iconname, this);
                contextMenu.addAction(noAction);
                contextMenu.exec(event->globalPos());
        }
}

void IdentityDisplay::updatePresence(const QVariant & presence)
{
        // qDebug() << "IdentityDisplay::updatePresence()" << presence;
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
        m_presencevalue->show();
}


void IdentityDisplay::setUserInfo(const UserInfo * ui)
{
        // qDebug() << "IdentityDisplay::setUserInfo()";
        m_ui = ui;

        m_user->setText(m_ui->fullname());
        m_phonenum->setText(m_ui->phonenum());
        QStringList vm = m_ui->mwi();
        if(vm.size() > 2) {
                m_voicemail_old->setText(tr("%1 old").arg(vm[1]));
                m_voicemail_new->setText(tr("%1 new").arg(vm[2]));
        }
        // m_voicemail->hide();
        // changes the "watched agent" only if no one else has done it before
        changeWatchedAgent(m_ui->astid() + " " + m_ui->agentid(), false);
}

void IdentityDisplay::setAgentList(int, const QVariant & alist)
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
                        } else if(agstatus == "AGENT_IDLE") {
                                m_agentstatus_value->setIcon(QIcon(":/images/button_ok.png"));
                                m_agentstatus = true;
                        }
                }
        }
}

void IdentityDisplay::setQueueList(const QVariant & qlist)
{
        if (m_loginkind == 0)
                return;
        // qDebug() << "IdentityDisplay::setQueueList()" << qlist;
        if(m_ui == NULL)
                return;
        QVariantMap qlistmap = qlist.toMap();
        QString astid = qlistmap["astid"].toString();
        if (astid != m_ui->astid())
                return;
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
        m_agentstatus_value->show();
        m_agentpause_value->show();
        m_icon_agent->show();
        m_qvline2->show();
}

void IdentityDisplay::hideAgentProps()
{
        m_agent->hide();
        m_agentstatus_value->hide();
        m_agentpause_value->hide();
        m_icon_agent->hide();
        m_qvline2->hide();
}

void IdentityDisplay::updatePeerAgent(int,
                                      const QString & userid,
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
        } else if (action == "agentlogout") {
                QString phonenum = newstatuses.toMap()["phonenum"].toString();
                showAgentProps();
                m_agentstatus_value->setIcon(QIcon(":/images/cancel.png"));
                m_agentstatus_value->setText(phonenum);
                m_agentstatus = false;
        }
}

void IdentityDisplay::doAgentLogActions()
{
        if(m_agentstatus)
                agentAction("logout");
        else
                agentAction("login");
}

void IdentityDisplay::doAgentPauseActions()
{
        if(m_agentstatus)
                agentAction("pause_all");
        else
                agentAction("unpause_all");
}

void IdentityDisplay::idxChanged(const QString & newidx)
{
        QString function = sender()->property("function").toString();
        // qDebug() << "IdentityDisplay::idxChanged" << newidx << sender() << function;
        if(function == "presence") {
                foreach (QString avstate, m_presence_names.keys())
                        if(m_presence_names[avstate] == newidx)
                                setAvailState(avstate, false);
        }
}

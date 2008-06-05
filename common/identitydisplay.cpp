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
IdentityDisplay::IdentityDisplay(QWidget * parent)
        : QWidget(parent), m_agentstatus(false), m_queuechangeallow(false)
{
	QGridLayout * glayout = new QGridLayout(this);
	// glayout->setMargin(0);
        m_user = new SizeableLabel( "", QSize(3000, 40), this );
        m_user->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        m_user->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

        m_agent = new QLabel("", this);
        m_agentaction = new QPushButton(tr("Logout"), this);
        m_agentaction->setIconSize(QSize(16, 16));
        m_queueleaveall = new QPushButton(tr("Leave All"), this);
        m_queueleaveall->setIcon(QIcon(":/images/cancel.png"));
        m_queueleaveall->setIconSize(QSize(16, 16));
        m_queuejoinall = new QPushButton(tr("Join All"), this);
        m_queuejoinall->setIcon(QIcon(":/images/add.png"));
        m_queuejoinall->setIconSize(QSize(16, 16));
        m_queueaction = new QPushButton(tr("Leave"), this);
        m_queueaction->setIconSize(QSize(16, 16));

        m_queuebusy = new QProgressBar(this);
        m_queuebusy->setRange(0, 5);
        // m_queuebusy->setFormat("%v");
        m_queuebusy->setTextVisible(false);
        m_queuelist = new QComboBox(this);

        connect(m_queuelist, SIGNAL(currentIndexChanged(const QString &)),
                this, SLOT(idxChanged(const QString &)));
        connect(m_agentaction, SIGNAL(clicked()),
                this, SLOT(doAgentAction()));
        connect(m_queueaction, SIGNAL(clicked()),
                this, SLOT(doQueueAction()));
        connect(m_queueleaveall, SIGNAL(clicked()),
                this, SLOT(doQueueLeaveAll()));
        connect(m_queuejoinall, SIGNAL(clicked()),
                this, SLOT(doQueueJoinAll()));

	glayout->addWidget( m_user, 0, 0, 1, 7, Qt::AlignCenter );
	glayout->addWidget( m_agent, 1, 0, Qt::AlignCenter );
	glayout->addWidget( m_agentaction, 1, 1, Qt::AlignCenter );
	glayout->addWidget( m_queueaction, 1, 2, Qt::AlignCenter );
	glayout->addWidget( m_queuelist, 1, 3, Qt::AlignCenter );
	glayout->addWidget( m_queuebusy, 1, 4, Qt::AlignCenter );
	glayout->addWidget( m_queuejoinall, 1, 5, Qt::AlignCenter );
	glayout->addWidget( m_queueleaveall, 1, 6, Qt::AlignCenter );

        m_agent->hide();
        m_agentaction->hide();

        m_queueaction->hide();
        m_queuelist->hide();
        m_queueleaveall->hide();
        m_queuejoinall->hide();
        m_queuebusy->hide();

// 	glayout->setColumnStretch( 0, 1 );
// 	glayout->setRowStretch( 0, 1 );
}

void IdentityDisplay::setUserInfo(const UserInfo * ui)
{
        m_user->setText(ui->fullname());
        m_ui = ui;
}


void IdentityDisplay::setQueueList(const QString & qlist)
{
        qDebug() << "IdentityDisplay::setQueueList()" << qlist;
        QStringList qsl = qlist.split(";");
        if(qsl[1] == "1")
                m_queuechangeallow = true;
        if(qsl[2].size() > 0) {
                QString astid = qsl[0];
                if(astid == m_ui->astid()) {
                        QStringList queues = qsl[2].split(",");
                        queues.sort();
                        for(int i = 0 ; i < queues.size(); i++) {
                                QStringList qparams = queues[i].split(":");
                                QString qname = qparams[0];
                                if(m_queuelist->findText(qname) == -1) {
                                        m_queuelist->addItem(qname);
                                        m_queuelist->setItemIcon(i, QIcon(":/images/cancel.png"));
                                        m_queuesindexes[qname] = i;
                                        if(qparams.size() > 1)
                                                m_queuesbusyness[qname] = qparams[1];
                                        else
                                                m_queuesbusyness[qname] = "0";
                                }
                        }
                        if((queues.size() > 0) && (m_agentstatus)) {
                                if(m_queuechangeallow) {
                                        m_queueaction->show();
                                        m_queuejoinall->show();
                                        m_queueleaveall->show();
                                }
                                m_queuelist->show();
                                m_queuebusy->show();
                        }
                }
        }
}

void IdentityDisplay::updatePeerAgent(const QString & userid, const QString & status)
{
        //qDebug() << "IdentityDisplay::updatePeerAgent" << userid << status;
        if(userid != m_ui->userid())
                return;
        QStringList newstatuses = status.split("/");
        if (newstatuses.size() >= 4) {
                QString command = newstatuses[0];
                QString astid = newstatuses[1];
                QString agentnum = newstatuses[2];
                QString arg = newstatuses[3];
                m_agent->setText("Agent " + agentnum);

                if (command == "agentlogin") {
                        m_agent->show();
                        m_agentaction->setIcon(QIcon(":/images/button_ok.png"));
                        m_agentaction->setText(arg);
                        m_agentaction->show();
                        m_agentstatus = true;
                        if(m_queuesindexes.size() > 0) {
                                if(m_queuechangeallow) {
                                        m_queueaction->show();
                                        m_queueleaveall->show();
                                        m_queuejoinall->show();
                                }
                                m_queuelist->show();
                                m_queuebusy->show();
                        }
                } else if (command == "agentlogout") {
                        m_agent->show();
                        m_agentaction->show();
                        m_agentaction->setIcon(QIcon(":/images/cancel.png"));
                        m_agentaction->setText(arg);
                        m_agentstatus = false;
                        m_queueaction->hide();
                        m_queuelist->hide();
                        m_queueleaveall->hide();
                        m_queuejoinall->hide();
                        m_queuebusy->hide();
                } else if (command == "joinqueue") {
                        if (m_queuesindexes.contains(arg)) {
                                int idx = m_queuesindexes[arg];
                                m_queuelist->setItemIcon(idx, QIcon(":/images/button_ok.png"));
                                m_queuesstatuses[arg] = true;
                                if(arg == m_queuelist->currentText())
                                        idxChanged(arg);
                        }
                } else if (command == "leavequeue") {
                        if (m_queuesindexes.contains(arg)) {
                                int idx = m_queuesindexes[arg];
                                m_queuelist->setItemIcon(idx, QIcon(":/images/cancel.png"));
                                m_queuesstatuses[arg] = false;
                                if(arg == m_queuelist->currentText())
                                        idxChanged(arg);
                        }
                } else if (command == "queuememberstatus") {
                        if (newstatuses.size() == 6) {
                                // qDebug() << "IdentityDisplay::setAgentStatus()" << newstatuses;
                        }
                } else
                        qDebug() << "IdentityDisplay::setAgentStatus()" << newstatuses;
        }
}

void IdentityDisplay::setQueueStatus(const QString & status)
{
        QStringList newstatuses = status.split("/");
        qDebug() << "IdentityDisplay::setQueueStatus()" << newstatuses;
        if (newstatuses.size() == 4) {
                QString command = newstatuses[0];
                if (command == "queuechannels") {
                        QString astid = newstatuses[1];
                        QString queuename = newstatuses[2];
                        QString busyness = newstatuses[3];
                        m_queuesbusyness[queuename] = busyness;
                        if(queuename == m_queuelist->currentText())
                                m_queuebusy->setValue(busyness.toInt());
                }
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
        if (m_queuesstatuses[newidx]) {
                m_queueaction->setText(tr("Leave"));
                m_queueaction->setIcon(QIcon(":/images/cancel.png"));
        } else {
                m_queueaction->setText(tr("Join"));
                m_queueaction->setIcon(QIcon(":/images/add.png"));
        }
        m_queuebusy->setValue(m_queuesbusyness[newidx].toInt());
}

SizeableLabel::SizeableLabel(const QString &text, const QSize &size, QWidget *parent)
        : QLabel(parent)
{
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        setText(text);
        m_size = size;
}

QSize SizeableLabel::sizeHint() const
{
        //        QSize size = QLabel::sizeHint();
        return m_size;
}

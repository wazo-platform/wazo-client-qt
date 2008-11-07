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
#include <QLineEdit>
#include <QPushButton>
#include <QVariant>

#include "statuspanel.h"
#include "userinfo.h"
#include "xivoconsts.h"

/*! \brief Constructor
 */
StatusPanel::StatusPanel(QWidget * parent)
        : QWidget(parent)
{
	m_glayout = new QGridLayout(this);
        m_lbl = new QLabel( "", this );
        m_linenum = 0;
        
        m_actionkey[Qt::Key_F1] = (QStringList() << "answer" << tr("Answer"));
        m_actionkey[Qt::Key_F2] = (QStringList() << "hangup" << tr("Hangup"));
        m_actionkey[Qt::Key_F3] = (QStringList() << "dtransfer" << tr("D. Transfer"));
        m_actionkey[Qt::Key_F4] = (QStringList() << "itransfer" << tr("I. Transfer"));
        m_actionkey[Qt::Key_F5] = (QStringList() << "ilink" << tr("I. Link"));
        m_actionkey[Qt::Key_F6] = (QStringList() << "icancel" << tr("I. Cancel"));
        m_actionkey[Qt::Key_F7] = (QStringList() << "park" << tr("Park"));
        // m_actionkey[Qt::Key_Return] = (QStringList() << "numreturn" << tr("Call Number"));
        
	m_glayout->addWidget( m_lbl, 0, 0, 1, m_actionkey.size() + 4, Qt::AlignHCenter | Qt::AlignVCenter );
        m_glayout->setRowStretch(100, 1);
}

void StatusPanel::newCall(const QString & chan)
{
        m_vlinesl[chan] = new QFrame(this);
        m_vlinesr[chan] = new QFrame(this);
        m_statuses[chan] = new QLabel("none", this);
        m_tnums[chan] = new QLineEdit("", this);
        m_linenum ++;

        m_vlinesl[chan]->setFrameShape(QFrame::VLine);
        m_vlinesl[chan]->setLineWidth(1);
        m_vlinesr[chan]->setFrameShape(QFrame::VLine);
        m_vlinesr[chan]->setLineWidth(1);
        m_tnums[chan]->hide();

        changeCurrentChannel(m_currentchannel, chan);
        m_currentchannel = chan;
        
        QHash<QString, QPushButton *> k;
        QMapIterator<int, QStringList> act(m_actionkey);
        while (act.hasNext()) {
                act.next();
                QString actionname = act.value()[0];
                k[actionname] = new QPushButton("", this);
                k[actionname]->hide();
                if((act.key() >= Qt::Key_F1) && (act.key() <= Qt::Key_F12))
                        k[actionname]->setText(act.value()[1] + " (F" + QString::number(act.key() - Qt::Key_F1 + 1) + ")");
                else if(act.key() == Qt::Key_Return)
                        k[actionname]->setText(act.value()[1] + " (Return)");
                else
                        k[actionname]->setText(act.value()[1] + " (?)");
                k[actionname]->setProperty("channel", chan);
                k[actionname]->setProperty("function", act.key());
                connect( k[actionname], SIGNAL(clicked()),
                         this, SLOT(clicked()) );
        }
        
        m_actions[chan] = k;
}

void StatusPanel::updateLine(const QString & chan, const QStringList & allowed)
{
        int colnum = 1;
        m_glayout->addWidget( m_vlinesl[chan], m_linenum, 0, Qt::AlignLeft );
	m_glayout->addWidget( m_statuses[chan], m_linenum, colnum++, Qt::AlignHCenter );

        QMapIterator<int, QStringList> act(m_actionkey);
        while (act.hasNext()) {
                act.next();
                QString actionname = act.value()[0];
                m_actions[chan][actionname]->hide();
                m_glayout->removeWidget(m_actions[chan][actionname]);
        }
        act.toFront();
        while (act.hasNext()) {
                act.next();
                QString actionname = act.value()[0];
                if (allowed.contains(actionname)) {
                        m_glayout->addWidget( m_actions[chan][actionname], m_linenum, colnum++, Qt::AlignHCenter );
                        m_actions[chan][actionname]->show();
                }
        }
        m_glayout->addWidget( m_tnums[chan],   m_linenum, m_actionkey.size() + 2, Qt::AlignHCenter );
        m_glayout->addWidget( m_vlinesr[chan], m_linenum, m_actionkey.size() + 3, Qt::AlignRight );
}

void StatusPanel::clicked()
{
        QString channel = sender()->property("channel").toString();
        int function = sender()->property("function").toInt();
        qDebug() << "StatusPanel::clicked()" << channel << function;
        m_currentchannel = channel;
        functionKeyPressed(function);
}

void StatusPanel::setUserInfo(const UserInfo * ui)
{
        m_ui = ui;
        m_lbl->setText(ui->fullname());
}

void StatusPanel::dtransfer()
{
        if(m_callchannels.contains(m_currentchannel)) {
                qDebug() << "Direct   Transfer" << m_peerchan[m_currentchannel];
                if(m_linestatuses[m_currentchannel] == WDTransfer) {
                        m_tnums[m_currentchannel]->hide();
                        m_statuses[m_currentchannel]->setFocus();
                        m_linestatuses[m_currentchannel] = Ringing;
                        disconnect( m_tnums[m_currentchannel], SIGNAL(returnPressed()),
                                    this, SLOT(xferPressed()) );
                } else {
                        m_tnums[m_currentchannel]->show();
                        m_tnums[m_currentchannel]->setFocus();
                        m_linestatuses[m_currentchannel] = WDTransfer;
                        connect( m_tnums[m_currentchannel], SIGNAL(returnPressed()),
                                 this, SLOT(xferPressed()) );
                }
        }
}

void StatusPanel::itransfer()
{
        if(m_callchannels.contains(m_currentchannel)) {
                qDebug() << "Indirect Transfer" << m_peerchan[m_currentchannel];
                if(m_linestatuses[m_currentchannel] == WITransfer) {
                        m_tnums[m_currentchannel]->hide();
                        m_statuses[m_currentchannel]->setFocus();
                        m_linestatuses[m_currentchannel] = Online;
                        disconnect( m_tnums[m_currentchannel], SIGNAL(returnPressed()),
                                    this, SLOT(xferPressed()) );
                } else {
                        m_tnums[m_currentchannel]->show();
                        m_tnums[m_currentchannel]->setFocus();
                        m_linestatuses[m_currentchannel] = WITransfer;
                        connect( m_tnums[m_currentchannel], SIGNAL(returnPressed()),
                                 this, SLOT(xferPressed()) );
                }
        }
}


void StatusPanel::xferPressed()
{
        QString num = m_tnums[m_currentchannel]->text();
        qDebug() << "StatusPanel::xferPressed()" << m_currentchannel << m_peerchan[m_currentchannel] << m_linestatuses[m_currentchannel] << num;
        if(m_linestatuses[m_currentchannel] == WDTransfer)
                transferCall("chan:special:me:" + m_peerchan[m_currentchannel], "ext:" + num);
        else if(m_linestatuses[m_currentchannel] == WITransfer) {
                atxferCall("chan:special:me:" + m_currentchannel, "ext:" + num);
                updateLine(m_currentchannel, (QStringList() << "hangup" << "ilink" << "icancel"));
        }
}


void StatusPanel::functionKeyPressed(int keynum)
{
        qDebug() << "StatusPanel::functionKeyPressed()" << keynum << m_currentchannel;
        QString action;
        if(m_actionkey.contains(keynum))
                action = m_actionkey[keynum][0];
        else
                return;
        
        if(m_callchannels.contains(m_currentchannel)) {
                Line linestatus = m_linestatuses[m_currentchannel];
                qDebug() << "StatusPanel::functionKeyPressed()" << keynum << action << m_currentchannel << linestatus;
                if(linestatus == Ringing) {
                        if(action == "answer")
                                pickUp(m_ui);
                        else if(action == "hangup")
                                simplehangupCall(m_ui, m_peerchan[m_currentchannel]);
                        else if(action == "dtransfer")
                                dtransfer();
                } else if(linestatus == Online) {
                        if(action == "hangup") {
                                hangupCall(m_ui, m_peerchan[m_currentchannel]);
                        } else if(action == "dtransfer") {
                                dtransfer();
                                updateLine(m_currentchannel, (QStringList() << "hangup" << "dtransfer" << "itransfer" << "numreturn"));
                        } else if(action == "itransfer") {
                                itransfer();
                                updateLine(m_currentchannel, (QStringList() << "hangup" << "dtransfer" << "itransfer" << "numreturn"));
                        } else if(action == "park") {
                                transferCall("chan:special:me:" + m_peerchan[m_currentchannel], "ext:special:parkthecall");
                        }
                } else if(linestatus == Wait) {
                        if(action == "unpark")
                                qDebug() << "StatusPanel::functionKeyPressed()" << "F1 when Wait : Take back";
                } else if(linestatus == WDTransfer) {
                        if(action == "hangup") {
                                simplehangupCall(m_ui, m_peerchan[m_currentchannel]);
                        } else if(action == "dtransfer") {
                                dtransfer(); // cancel D. Transfer request
                                updateLine(m_currentchannel, (QStringList() << "hangup" << "dtransfer" << "itransfer"));
                        } else if(action == "itransfer") {
                                itransfer();
                                updateLine(m_currentchannel, (QStringList() << "hangup" << "dtransfer" << "itransfer" << "numreturn"));
                        } else if(action == "answer") {
                                // only when not picked up yet
                                pickUp(m_ui);
                        }
                } else if(linestatus == WITransfer) {
                        if(action == "hangup") {
                                simplehangupCall(m_ui, m_peerchan[m_currentchannel]);
                        } else if(action == "dtransfer") {
                                dtransfer();
                                updateLine(m_currentchannel, (QStringList() << "hangup" << "dtransfer" << "itransfer" << "numreturn"));
                        } else if(action == "itransfer") {
                                itransfer(); // cancel I. Transfer request
                                updateLine(m_currentchannel, (QStringList() << "hangup" << "dtransfer" << "itransfer"));
                        } else if(action == "ilink") {
                                simplehangupCall(m_ui, m_currentchannel);
                        } else if(action == "icancel") {
                                simplehangupCall(m_ui, m_tferchannel);
                        }
                }
                
                if(keynum == Qt::Key_Up) {
                        int ci = m_callchannels.indexOf(m_currentchannel);
                        if(ci == 0)
                                ci = 0; // ci = m_callchannels.size() - 1;
                        else
                                ci = ci - 1;
                        changeCurrentChannel(m_currentchannel, m_callchannels[ci]);
                        m_currentchannel = m_callchannels[ci];
                } else if(keynum == Qt::Key_Down) {
                        int ci = m_callchannels.indexOf(m_currentchannel);
                        if(ci == m_callchannels.size() - 1)
                                ci = m_callchannels.size() - 1; // ci = 0;
                        else
                                ci = ci + 1;
                        changeCurrentChannel(m_currentchannel, m_callchannels[ci]);
                        m_currentchannel = m_callchannels[ci];
                }
        }
}

void StatusPanel::changeCurrentChannel(const QString & before, const QString & after)
{
        // qDebug() << "StatusPanel::changeCurrentChannel" << before << after;
        if(before != after) {
                if(m_vlinesl.contains(before) && m_vlinesr.contains(before)) {
                        m_vlinesl[before]->setLineWidth(1);
                        m_vlinesr[before]->setLineWidth(1);
                }
                if(m_vlinesl.contains(after) && m_vlinesr.contains(after)) {
                        m_vlinesl[after]->setLineWidth(3);
                        m_vlinesr[after]->setLineWidth(3);
                }
        }
}

void StatusPanel::updatePeer(UserInfo * ui,
                             const QString &,
                             const QHash<QString, QStringList> & chanlist)
{
        qDebug() << "StatusPanel::updatePeer()" << ui->userid() << chanlist;
        if (ui == m_ui) {
                QHashIterator<QString, QStringList> ccallchannel(chanlist);
                while (ccallchannel.hasNext()) {
                        ccallchannel.next();
                        // qDebug() << ccallchannel.key() << ": " << ccallchannel.value();
                        const QString callchannel = ccallchannel.key();
                        const QString status = ccallchannel.value()[0];
                        const QString peerchan = ccallchannel.value()[3];
                        const QString num = ccallchannel.value()[4];
                        
                        if(status == "Ringing") {
                                if(m_callchannels.contains(callchannel) == false) {
                                        newCall(callchannel);
                                        m_callchannels << callchannel;
                                        
                                        m_linestatuses[callchannel] = Ringing;
                                        m_peerchan[callchannel] = peerchan;
                                        updateLine(callchannel, (QStringList() << "answer" << "hangup" << "dtransfer"));
                                        m_statuses[callchannel]->setText(num + " " + tr("Ringing"));
                                        m_statuses[callchannel]->show();
                                }
                        } else if(status == "On the phone") {
                                if(m_callchannels.contains(callchannel) == false) {
                                        newCall(callchannel);
                                        m_callchannels << callchannel;
                                }
                                m_linestatuses[callchannel] = Online;
                                m_peerchan[callchannel] = peerchan;
                                updateLine(callchannel, (QStringList() << "hangup" << "dtransfer" << "itransfer" << "park"));
                                m_statuses[callchannel]->setText(tr("Link") + " " + num);
                                m_statuses[callchannel]->show();
                        } else if(status == "Hangup") {
                                if(m_callchannels.contains(callchannel) == true) {
                                        m_linestatuses[callchannel] = Ready;
                                        m_statuses[callchannel]->hide();
                                        updateLine(callchannel, (QStringList()));
                                        delete m_vlinesl[callchannel];
                                        delete m_vlinesr[callchannel];
                                        delete m_statuses[callchannel];
                                        delete m_tnums[callchannel];
                                        m_vlinesl.remove(callchannel);
                                        m_vlinesr.remove(callchannel);
                                        m_statuses.remove(callchannel);
                                        m_linestatuses.remove(callchannel);
                                        m_tnums.remove(callchannel);

                                        if(m_currentchannel == callchannel) {
                                                if(m_callchannels.size() > 1) {
                                                        int ci = m_callchannels.indexOf(m_currentchannel);
                                                        if(ci == m_callchannels.size() - 1)
                                                                ci --;
                                                        changeCurrentChannel(m_currentchannel, m_callchannels[ci]);
                                                        m_currentchannel = m_callchannels[ci];
                                                } else
                                                        m_currentchannel = "";
                                        }
                                        m_callchannels.removeAll(callchannel);
                                }
                        } else {
                                qDebug() << status << ccallchannel.key() << ccallchannel.value();
                                if(m_callchannels.contains(callchannel) == true) {
                                        m_linestatuses[callchannel] = Ready;
                                        m_statuses[callchannel]->setText(status + " " + num);
                                        updateLine(callchannel, (QStringList()));
                                        m_tnums[callchannel]->hide();
                                }
                        }
                }
        } else if (ui->astid() == m_ui->astid()) {
                if(chanlist.size() > 0) {
                        QHashIterator<QString, QStringList> ccallchannel(chanlist);
                        while (ccallchannel.hasNext()) {
                                ccallchannel.next();
                                if(m_ui->phonenum() == ccallchannel.value()[4]) {
                                        qDebug() << "not me" << ui->fullname() << ccallchannel.key() << ": " << ccallchannel.value();
                                        m_tferchannel = ccallchannel.value()[3];
                                }
                        }
                }
        }
}

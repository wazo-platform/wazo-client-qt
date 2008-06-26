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

        m_actionnames = (QStringList() << "Fa" << "Fb" << "Fc");
	m_glayout->addWidget( m_lbl, 0, 0, 1, m_actionnames.size() + 4, Qt::AlignHCenter | Qt::AlignVCenter );
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
        foreach (QString actionname, m_actionnames) {
                k[actionname] = new QPushButton("", this);
                k[actionname]->hide();
                k[actionname]->setProperty("channel", chan);
                connect( k[actionname], SIGNAL(clicked()),
                         this, SLOT(clicked()) );
        }
        k["Fa"]->setText("Answer (F1)");
        k["Fa"]->setProperty("function", 1);
        k["Fb"]->setText("Refuse (F2)");
        k["Fb"]->setProperty("function", 2);
        k["Fc"]->setText("D. Transfer (F3)");
        k["Fc"]->setProperty("function", 3);

        m_actions[chan] = k;

        int colnum  = 1;
        m_glayout->addWidget( m_vlinesl[chan], m_linenum, 0, Qt::AlignLeft );
	m_glayout->addWidget( m_statuses[chan], m_linenum, colnum++, Qt::AlignHCenter );
        foreach (QString actionname, m_actionnames) {
                m_glayout->addWidget( m_actions[chan][actionname], m_linenum, colnum++, Qt::AlignHCenter );
        }
	m_glayout->addWidget( m_tnums[chan],   m_linenum, colnum++, Qt::AlignHCenter );
        m_glayout->addWidget( m_vlinesr[chan], m_linenum, m_actionnames.size() + 3, Qt::AlignRight );
}

void StatusPanel::clicked()
{
        QString channel = this->sender()->property("channel").toString();
        int function = this->sender()->property("function").toInt();
        qDebug() << "StatusPanel::clicked()" << channel << function;
        m_currentchannel = channel;
        functionKeyPressed(Qt::Key_F1 + function - 1);
}

void StatusPanel::setUserInfo(const UserInfo * ui)
{
        m_ui = ui;
        m_lbl->setText(ui->fullname());
}

//m_engine, SLOT(transferCall(const QString&, const QString&)) );

void StatusPanel::dtransfer()
{
        if(m_callchannels.contains(m_currentchannel)) {
                qDebug() << "Direct   Transfer" << m_peerchan[m_currentchannel];
                if(m_linestatuses[m_currentchannel] == WDTransfer) {
                        m_tnums[m_currentchannel]->hide();
                        m_statuses[m_currentchannel]->setFocus();
                        m_linestatuses[m_currentchannel] = Online;
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
        if(m_linestatuses[m_currentchannel] == WDTransfer)
                transferCall(m_ui, m_currentchannel, num);
        else if(m_linestatuses[m_currentchannel] == WITransfer)
                atxferCall(m_ui, m_peerchan[m_currentchannel], num);
}


void StatusPanel::functionKeyPressed(int keynum)
{
        qDebug() << "StatusPanel::functionKeyPressed" << keynum;
        if(m_callchannels.contains(m_currentchannel)) {
                Line linestatus = m_linestatuses[m_currentchannel];
                if(linestatus == Ringing) {
                        if(keynum == Qt::Key_F1) {
                                pickUp(m_ui);
                                m_actions[m_currentchannel]["Fa"]->setText("Hangup (F2)");
                                m_actions[m_currentchannel]["Fb"]->setText("D. Transfer (F3)");
                                m_actions[m_currentchannel]["Fc"]->setText("I. Transfer (F4)");
                                m_actions[m_currentchannel]["Fa"]->setProperty("function", 2);
                                m_actions[m_currentchannel]["Fb"]->setProperty("function", 3);
                                m_actions[m_currentchannel]["Fc"]->setProperty("function", 4);
                        }
                        else if(keynum == Qt::Key_F2)
                                simpleHangUp(m_ui, m_peerchan[m_currentchannel]);
                        else if(keynum == Qt::Key_F3)
                                dtransfer();
                } else if(linestatus == Online) {
                        if(keynum == Qt::Key_F2) {
                                simpleHangUp(m_ui, m_peerchan[m_currentchannel]);
                        } else if(keynum == Qt::Key_F3) {
                                dtransfer();
                                m_actions[m_currentchannel]["Fb"]->setText("(# + Return)");
                        } else if(keynum == Qt::Key_F4) {
                                itransfer();
                                m_actions[m_currentchannel]["Fc"]->setText("(# + Return)");
                        } else if(keynum == Qt::Key_F6) {
                                qDebug() << "StatusPanel::functionKeyPressed" << "F6 when Online : Wait";
                        } else if(keynum == Qt::Key_F7) {
                                transferCall(m_ui, m_currentchannel, "700");
                        } else if(keynum == Qt::Key_F8) {
                                // transferCall(m_ui, m_currentchannel, num);
                        }
                } else if(linestatus == Wait) {
                        if(keynum == Qt::Key_F1)
                                qDebug() << "StatusPanel::functionKeyPressed" << "F1 when Wait : Take back";
                } else if(linestatus == WDTransfer) {
                        if(keynum == Qt::Key_F3) {
                                dtransfer(); // cancel D. Transfer request
                                m_actions[m_currentchannel]["Fb"]->setText("D. Transfer (F3)");
                                m_actions[m_currentchannel]["Fc"]->setText("I. Transfer (F4)");
                        } else if(keynum == Qt::Key_F4) {
                                itransfer();
                                m_actions[m_currentchannel]["Fb"]->setText("D. Transfer (F3)");
                                m_actions[m_currentchannel]["Fc"]->setText("(# + Return)");
                        } else if(keynum == Qt::Key_F2) {
                                simpleHangUp(m_ui, m_peerchan[m_currentchannel]);
                        } else if(keynum == Qt::Key_F1) {
                                // only when not picked up yet
                                pickUp(m_ui);
                                m_actions[m_currentchannel]["Fa"]->setText("Hangup (F2)");
                                m_actions[m_currentchannel]["Fb"]->setText("D. Transfer (F3)");
                                m_actions[m_currentchannel]["Fc"]->setText("I. Transfer (F4)");
                        }
                } else if(linestatus == WITransfer) {
                        if(keynum == Qt::Key_F3) {
                                dtransfer();
                                m_actions[m_currentchannel]["Fb"]->setText("(# + Return)");
                                m_actions[m_currentchannel]["Fc"]->setText("I. Transfer (F4)");
                        } else if(keynum == Qt::Key_F4) {
                                itransfer(); // cancel I. Transfer request
                                m_actions[m_currentchannel]["Fb"]->setText("D. Transfer (F3)");
                                m_actions[m_currentchannel]["Fc"]->setText("I. Transfer (F4)");
                        } else if(keynum == Qt::Key_F2) {
                                simpleHangUp(m_ui, m_peerchan[m_currentchannel]);
                        } else if(keynum == Qt::Key_F5) {
                                simpleHangUp(m_ui, m_currentchannel);
                        } else if(keynum == Qt::Key_F6) {
                                simpleHangUp(m_ui, m_tferchannel);
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

void StatusPanel::updatePeer(const UserInfo * ui,
                             const QString &,
                             const QStringList & g, const QStringList & h,
                             const QStringList & i, const QStringList & j)
{
        // QString name = ui->fullname();
        qDebug() << "StatusPanel::updatePeer()" << ui->userid() << g << h << i << j;
        if (ui->userid() == m_ui->userid()) {
                qDebug() << "StatusPanel::updatePeer()" << ui->userid() << g << h << i << j;
                foreach (QString callchannel, g) {
                        int index = g.indexOf(callchannel);
                        const QString status = h[index];
                        const QString num = i[index];
                        const QString peerchan = j[index];
                        qDebug() << status << num << peerchan;
                        if(status == "Ringing") {
                                if(m_callchannels.contains(callchannel) == false) {
                                        newCall(callchannel);
                                        m_linestatuses[callchannel] = Ringing;
                                        m_peerchan[callchannel] = peerchan;
                                        m_callchannels << callchannel;
                                        m_statuses[callchannel]->setText(status + num);
                                        foreach (QString actionname, m_actionnames)
                                                m_actions[callchannel][actionname]->show();
                                        m_statuses[callchannel]->show();
                                }
                        } else if(status == "On the phone") {
                                if(m_callchannels.contains(callchannel) == true) {
                                        m_linestatuses[callchannel] = Online;
                                        m_peerchan[callchannel] = peerchan;
                                        m_statuses[callchannel]->setText(status + num);
                                        foreach (QString actionname, m_actionnames)
                                                m_actions[callchannel][actionname]->show();
                                        m_statuses[callchannel]->show();
                                }
                        } else if(status == "Hangup") {
                                if(m_callchannels.contains(callchannel) == true) {
                                        m_linestatuses[callchannel] = Ready;
                                        m_statuses[callchannel]->hide();
                                        foreach (QString actionname, m_actionnames)
                                                m_actions[callchannel][actionname]->hide();

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
                                if(m_callchannels.contains(callchannel) == true) {
                                        m_linestatuses[callchannel] = Ready;
                                        m_statuses[callchannel]->setText(status + num);
                                        foreach (QString actionname, m_actionnames)
                                                m_actions[callchannel][actionname]->hide();
                                        m_tnums[callchannel]->hide();
                                }
                        }
                }
        } else {
                // qDebug() << "StatusPanel::updatePeer() not me" << a << g << h << i << j;
                if(j.size() > 0)
                        m_tferchannel = j[0];
        }
}

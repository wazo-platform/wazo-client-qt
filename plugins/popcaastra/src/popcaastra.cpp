/* XiVO Client
 * Copyright (C) 2007-2011, Proformatique
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
#include <QLabel>
#include <QVBoxLayout>
#include <QTableWidget>

#include "popcaastra.h"
#include "../ui_popcaastra.h"
#include "userinfo.h"
#include "channelinfo.h"
#include "aastrasipnotify.h"
#include "xivoconsts.h"
#include "incomingwidget.h"

#define MAX_LINES 4

PopcAastra::PopcAastra(QWidget *parent)
    : XLet(parent), m_ui(new Ui::PopcAastra)
{
    m_ui->setupUi(this);
    setTitle(tr("POPC Aastra operator"));

    m_calls_list = new QVBoxLayout(m_ui->m_calls_layout);
    // Signals / slots
    connect(b_engine, SIGNAL(monitorPeer(UserInfo *)),
            this, SLOT(monitorPeer(UserInfo *)));
    connect(b_engine, SIGNAL(updateUserConfig(const QString &)),
            this, SLOT(updateUserConfig(const QString &)));
    connect(b_engine, SIGNAL(updateUserStatus(const QString &)),
            this, SLOT(updateUserStatus(const QString &)));
    connect(b_engine, SIGNAL(updatePhoneConfig(const QString &)),
            this, SLOT(updatePhoneConfig(const QString &)));
    connect(b_engine, SIGNAL(updatePhoneStatus(const QString &)),
            this, SLOT(updatePhoneStatus(const QString &)));
    connect(b_engine, SIGNAL(updateChannelStatus(const QString &)),
            this, SLOT(updateChannelStatus(const QString &)));

    connect(m_ui->btn_vol_up, SIGNAL(clicked()), this, SLOT(volUp()));
    connect(m_ui->btn_vol_down, SIGNAL(clicked()), this, SLOT(volDown()));
    connect(m_ui->btn_right, SIGNAL(clicked()), this, SLOT(navRight()));
    connect(m_ui->btn_hangup, SIGNAL(clicked()), this, SLOT(hangup()));
}

/*! \brief update display according to call list
 *
 * Read m_calllist and update m_afflist accordingly.
 */
void PopcAastra::updateDisplay()
{
    qDebug() << Q_FUNC_INFO;
    foreach (QString key, m_incomingcalls.keys()) {
        m_incomingcalls[key]->updateWidget();
    }
}

void PopcAastra::updateChannelStatus(const QString & xchannel)
{
    //qDebug() << Q_FUNC_INFO << xchannel;
    removeDefuncWidgets();
    const ChannelInfo * channelinfo = b_engine->channels().value(xchannel);
    if (channelinfo == NULL) {
        qDebug() << Q_FUNC_INFO << "null chaninfo";
        return;
    }

    const UserInfo * me = b_engine->user(b_engine->getFullId());
    if (me == NULL) {
        qDebug() << Q_FUNC_INFO << " Null user";
        return;
    }

    if (me->fullname() != channelinfo->thisdisplay()) {
        //qDebug() << Q_FUNC_INFO << " not our channel";
        return;
    }

    // Check if this channel is already tracked
    if (m_incomingcalls.contains(xchannel)) {
        // Already tracked
        //qDebug() << Q_FUNC_INFO << " Old channel updating";
        //qDebug() << Q_FUNC_INFO << " commstatus " << channelinfo->commstatus();
    } else {
        // New channel
        //qDebug() << Q_FUNC_INFO << " New channel adding";
        int guessedline = findFirstAvailableLine();
        qDebug() << Q_FUNC_INFO << "The guessed line number for this call is " << guessedline;
        IncomingWidget * newcall = new IncomingWidget(guessedline, xchannel, this);
        m_incomingcalls[xchannel] = newcall;
        m_calls_list->addWidget(newcall);
        connect(newcall, SIGNAL(doHangUp(int)), this, SLOT(hupline(int)));
        connect(newcall, SIGNAL(doBlindTransfer(int)), this, SLOT(blindTransfer(int)));
        connect(newcall, SIGNAL(doAttendedTransfer(int)), this, SLOT(attendedTransfer(int)));
        connect(newcall, SIGNAL(selectLine(int)), this, SLOT(selectLine(int)));
    }
    IncomingWidget * current = m_incomingcalls[xchannel];
    current->updateWidget();
}

/*! \brief check for defunc channels that are still shown in the call list */
void PopcAastra::removeDefuncWidgets()
{
    //qDebug() << Q_FUNC_INFO;
    int removed = 0;
    if (m_incomingcalls.size() == 0) return;
    const QHash<QString, ChannelInfo *> & channels = b_engine->channels();
    foreach (const QString channel, m_incomingcalls.keys()) {
        if (! channels.contains(channel)) {
            delete m_incomingcalls[channel];
            m_incomingcalls.remove(channel);
            removed++;
        }/* else {
            foreach (QString channel, channels.keys()) {
                qDebug() << Q_FUNC_INFO << " Remaining: " << channel;
            }
        }*/
    }
    if (removed)
        qDebug() << Q_FUNC_INFO << "Removed " << removed << " defunc channel(s)";
}

/*! \brief prints the content of m_incomingcalls */
void PopcAastra::debugIncomingCalls() const
{
    qDebug() << Q_FUNC_INFO << "Number of incoming calls " << m_incomingcalls.size();
    for (QHash<QString, IncomingWidget *>::const_iterator i = m_incomingcalls.constBegin();
        i != m_incomingcalls.constEnd(); ++i) {
        IncomingWidget * current = i.value();
        qDebug() << Q_FUNC_INFO << i.key() << current->toString();
    }
}

/*! \brief finds the first line available to place this channel
 *  The goal is to know/guess on which line a call is to avoid
 *  mistakes when doing operations to a call since we are dealing
 *  with lines on the phone device when using aastra xml api
 *  Return -1 when all the lines are taken
 */
int PopcAastra::findFirstAvailableLine() const {
    if (m_incomingcalls.size() == 0) {
        return 1;
    }
    for (int i = 0; i < MAX_LINES; i++) {
        bool free = true;
        foreach (QString channel, m_incomingcalls.keys()) {
            if (m_incomingcalls[channel]->line() == i + 1) free = false;
        }
        if (free) return i + 1;
    }
    return -1;
}

void PopcAastra::updatePhoneStatus(const QString & xphoneid)
{
    qDebug() << Q_FUNC_INFO << xphoneid;
    removeDefuncWidgets();
}

void PopcAastra::hupline(int line)
{
    qDebug() << Q_FUNC_INFO << line;
    QList<QString> commands;
    commands.append(getKeyUri(LINE, line));
    commands.append(getKeyUri(GOODBYE));
    emit ipbxCommand(getAastraSipNotify(commands, SPECIAL_ME));
}

/*! \brief blind transfer the call on the line to the number in the name/number field */
void PopcAastra::blindTransfer(int line)
{
    //qDebug() << Q_FUNC_INFO << line;
    QList<QString> commands;
    commands.append(getKeyUri(LINE, line));
    commands.append(getKeyUri(XFER));
    QString number = m_ui->txt_number_name->text();
    for (int i = 0; i < number.size(); ++i) {
        const QChar c = number[i];
        if (c.isDigit()) {
            commands.append(getKeyUri(KEYPAD, c.digitValue()));
        }
    }
    commands.append(getKeyUri(XFER));
    emit ipbxCommand(getAastraSipNotify(commands, SPECIAL_ME));
}

/*! \brief attended transfer to the line in the number/name field */
void PopcAastra::attendedTransfer(int line)
{
    qDebug() << Q_FUNC_INFO << line;
    QList<QString> commands;
    commands.append(getKeyUri(LINE, line));
    commands.append(getKeyUri(XFER));
    QString number = m_ui->txt_number_name->text();
    for (int i = 0; i < number.size(); ++i) {
        const QChar c = number[i];
        if (c.isDigit())
            commands.append(getKeyUri(KEYPAD, c.digitValue()));
    }
    commands.append(getKeyUri(LINE, findFirstAvailableLine()));
    emit ipbxCommand(getAastraSipNotify(commands, SPECIAL_ME));
}

/*! \brief select the line */
void PopcAastra::selectLine(int line)
{
    qDebug() << Q_FUNC_INFO << line;
    emit ipbxCommand(getAastraKeyNotify(LINE, SPECIAL_ME, line));
}

/*! \brief park the call on line line */
void PopcAastra::parkcall(int line)
{
    qDebug() << Q_FUNC_INFO << line;
}

void PopcAastra::updatePhoneConfig(const QString & phone)
{
    qDebug() << Q_FUNC_INFO << phone;
}

void PopcAastra::updateUserConfig(const QString & xuserid)
{
    qDebug() << Q_FUNC_INFO << xuserid;
    updateDisplay();
}

void PopcAastra::updateUserStatus(const QString & xuserid)
{
    qDebug() << Q_FUNC_INFO << xuserid;
    updateDisplay();
}

/*! \brief turns the volume up */
void PopcAastra::volUp()
{
    qDebug() << Q_FUNC_INFO;
    emit ipbxCommand(getAastraKeyNotify(VOL_UP, SPECIAL_ME));
}

/*! \brief turns the volume down */
void PopcAastra::volDown()
{
    qDebug() << Q_FUNC_INFO;
    emit ipbxCommand(getAastraKeyNotify(VOL_DOWN, SPECIAL_ME));
}

/*! \brief press the navigation right button of the device */
void PopcAastra::navRight()
{
    qDebug() << Q_FUNC_INFO;
    emit ipbxCommand(getAastraKeyNotify(NAV_RIGHT, SPECIAL_ME));
}

/*! \brief hang up the active line */
void PopcAastra::hangup()
{
    qDebug() << Q_FUNC_INFO;
    emit ipbxCommand(getAastraKeyNotify(GOODBYE, SPECIAL_ME));
}

/*! \brief destructor
 */
PopcAastra::~PopcAastra()
{
    qDebug() << Q_FUNC_INFO;
    delete m_ui;
}


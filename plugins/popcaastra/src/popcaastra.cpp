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
#include <QHBoxLayout>
#include <QTableWidget>

#include "popcaastra.h"
#include "../ui_popcaastra.h"
#include "userinfo.h"
#include "channelinfo.h"
#include "aastrasipnotify.h"
#include "callwidget.h"
#include "xivoconsts.h"

PopcAastra::PopcAastra(QWidget *parent)
    : XLet(parent), m_ui(new Ui::PopcAastra), m_monitored_ui(0)
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
}

/*! \brief update display according to call list
 *
 * Read m_calllist and update m_afflist accordingly.
 */
void PopcAastra::updateDisplay()
{
    qDebug() << Q_FUNC_INFO;
    if (m_monitored_ui == NULL)
        return;
    updatePhoneStatus(m_monitored_ui->phonelist()[0]);
}

void PopcAastra::updateChannelStatus(const QString & xchannel)
{
    qDebug() << Q_FUNC_INFO << xchannel;
    const ChannelInfo * channelinfo = b_engine->channels().value(xchannel);
    qDebug() << Q_FUNC_INFO << channelinfo->toString();
    if (channelinfo == NULL) {  // never happens???
        qDebug() << Q_FUNC_INFO << "null chaninfo";
        return;
    }
    QString status = channelinfo->commstatus();
    //                 if (map.contains("time-dial"))
    //                     ts = map.value("time-dial").toUInt() + current_ts;
    //                 if (map.contains("timestamp-dial"))
    //                     ts = map.value("timestamp-dial").toDouble() + b_engine->timeDeltaServerClient();
    //                 if (map.contains("time-link"))
    //                     ts = map.value("time-link").toUInt() + current_ts;
    //                 if (map.contains("timestamp-link"))
    //                     ts = map.value("timestamp-link").toDouble() + b_engine->timeDeltaServerClient();
    qDebug() << Q_FUNC_INFO << "Channel: "<< xchannel << " Status: " << status;
    // Don't show our own channels
    if (channelinfo->direction() == "out")
        return;
    // dont display hangup channels !
    if (status == CHAN_STATUS_HANGUP) { // Never happend??
        qDebug() << Q_FUNC_INFO << "Hang-up received";
        return;
    }
    // activeChannels << channelme;
    qDebug() << Q_FUNC_INFO << "adding/updating" << xchannel << m_affhash;
    if (m_affhash.contains(xchannel))
        m_affhash[xchannel]->updateWidget(xchannel);
    else {
         CallWidget * callwidget = new CallWidget(m_monitored_ui,
                                                  xchannel,
                                                  this);
         connect(callwidget, SIGNAL(doHangUp(const QString &)),
                 this, SLOT(hupchan(const QString &)));
         connect(callwidget, SIGNAL(doTransferToNumber(const QString &)),
                 this, SLOT(transftonumberchan(const QString &)));
         connect(callwidget, SIGNAL(doParkCall(const QString &)),
                 this, SLOT(parkcall(const QString &)));
         m_calls_list->insertWidget(m_calls_list->count() - 1, callwidget,
                                0, Qt::AlignTop);
         m_affhash[xchannel] = callwidget;
    }
}

void PopcAastra::monitorPeer(UserInfo * userInfo)
{
    qDebug() << Q_FUNC_INFO << b_engine->getFullId() << userInfo->xid();
    //emptyList();
    if ((b_engine->getFullId() == userInfo->xid()) ||
        (b_engine->enabledFunction("switchboard"))) {
        m_monitored_ui = userInfo;
        //changeTitle(tr("Monitoring : %1").arg(userInfo->fullname()));
        updateDisplay();
    }
}

void PopcAastra::updatePhoneStatus(const QString & xphoneid)
{
    // Clear orphan callwidgets (callwidgets with NULL channel)
    foreach (const QString xchannel, m_affhash.keys()) {
        CallWidget * callwidget = m_affhash[xchannel];
        const QString channel = callwidget->channel();
        if (! b_engine->channels().contains(channel)) {
            delete callwidget;
            m_affhash.remove(xchannel);
            qDebug() << Q_FUNC_INFO << "Removed channel " << xchannel;
        }
    }
    qDebug() << Q_FUNC_INFO << xphoneid;
    const PhoneInfo* info = b_engine->phone(xphoneid);
    if (info == NULL) {
        qDebug() << Q_FUNC_INFO << "Phone info is null";
        return;
    }
    const QStringList channels = info->xchannels();
    qDebug() << Q_FUNC_INFO << "Printing channels...";
    for (int i = 0 ; i < channels.size(); i++) {
        qDebug() << Q_FUNC_INFO << "Channel: " << channels.at(i);
    }
    if (m_monitored_ui == NULL) {
        return;
    }
    if (! m_monitored_ui->phonelist().contains(xphoneid))
        return;
    const PhoneInfo * phoneinfo = b_engine->phone(xphoneid);
    if (phoneinfo == NULL)
        return;

    qDebug() << Q_FUNC_INFO << xphoneid << m_affhash.keys() << phoneinfo->channels();

    foreach (const QString xchannel, m_affhash.keys()) {
        qDebug() << Q_FUNC_INFO << "Cleaning up remaining widgets";
        CallWidget * callwidget = m_affhash[xchannel];
        QString channel = callwidget->channel();
        if (! phoneinfo->channels().contains(channel)) {
            delete callwidget;
            m_affhash.remove(xchannel);
            qDebug() << Q_FUNC_INFO << "Removed: " << xchannel;
        }
    }

    foreach (const QString channel, phoneinfo->channels()) {
        QString xchannel = QString("%1/%2").arg(phoneinfo->ipbxid()).arg(channel);
        if (! m_affhash.contains(xchannel)) {
            CallWidget * callwidget = new CallWidget(m_monitored_ui,
                                                     xchannel,
                                                     this);
            connect(callwidget, SIGNAL(doHangUp(const QString &)),
                    this, SLOT(hupchan(const QString &)));
            connect(callwidget, SIGNAL(doTransferToNumber(const QString &)),
                    this, SLOT(transftonumberchan(const QString &)));
            connect(callwidget, SIGNAL(doParkCall(const QString &)),
                    this, SLOT(parkcall(const QString &)));
            m_calls_list->insertWidget(m_calls_list->count() - 1, callwidget,
                                   0, Qt::AlignTop);
            m_affhash[xchannel] = callwidget;
        }
    }
}

void PopcAastra::hupchan(const QString &chan)
{
    qDebug() << Q_FUNC_INFO << chan;
}

void PopcAastra::transftonumberchan(const QString & chan)
{
    qDebug() << Q_FUNC_INFO << chan;
}

void PopcAastra::parkcall(const QString & chan)
{
    qDebug() << Q_FUNC_INFO << chan;
}

void PopcAastra::updatePhoneConfig(const QString & phone)
{
    qDebug() << Q_FUNC_INFO;
}

void PopcAastra::updateUserConfig(const QString & xuserid)
{
    if (m_monitored_ui)
        qDebug() << Q_FUNC_INFO << m_monitored_ui->xid() << xuserid;
    else
        qDebug() << Q_FUNC_INFO << xuserid;
//     if (ui == m_monitored_ui) {
//         // we need to update the display
         updateDisplay();
//     }
}

void PopcAastra::updateUserStatus(const QString & xuserid)
{
    if (m_monitored_ui)
        qDebug() << Q_FUNC_INFO << m_monitored_ui->xid() << xuserid;
    else
        qDebug() << Q_FUNC_INFO << xuserid;
//     if (ui == m_monitored_ui) {
//         // we need to update the display
         updateDisplay();
//     }
}

void PopcAastra::volUp()
{
    emit ipbxCommand(getAastraKeyNotify(VOL_UP, SPECIAL_ME));
}

void PopcAastra::volDown()
{
    emit ipbxCommand(getAastraKeyNotify(VOL_DOWN, SPECIAL_ME));
}

/*! \brief destructor
 */
PopcAastra::~PopcAastra()
{
    qDebug() << Q_FUNC_INFO;
    delete m_ui;
}


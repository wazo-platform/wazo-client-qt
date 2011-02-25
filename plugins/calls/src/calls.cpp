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

#include "calls.h"

Q_EXPORT_PLUGIN2(xletcallsplugin, XLetCallsPlugin);

XLet* XLetCallsPlugin::newXLetInstance(QWidget *parent)
{
    b_engine->registerTranslation(":/calls_%1");
    return new XletCalls(parent);
}


XletCalls::XletCalls(QWidget *parent)
    : XLet(parent), m_monitored_ui(0)
{
    setTitle(tr("Calls"));
    qDebug() << Q_FUNC_INFO;
    QVBoxLayout *toplayout = new QVBoxLayout(this);
    toplayout->setMargin(0);
    QLabel *titleLabel = new QLabel("                     ", this);
    toplayout->addWidget( titleLabel, 0, Qt::AlignCenter);

    QScrollArea *scrollarea = new QScrollArea(this);
    scrollarea->setWidgetResizable(true);
    QWidget *w = new QWidget(scrollarea);
    scrollarea->setWidget(w);
    m_layout = new QVBoxLayout(w);
    setObjectName("scroller");
    setAcceptDrops(true);
    m_layout->addStretch(1);
    toplayout->addWidget(scrollarea);

    // connect signals/slots
    connect(b_engine, SIGNAL(userUpdated(UserInfo *)),
            this, SLOT(updateUser(UserInfo *)));
    connect(this, SIGNAL(changeTitle(const QString &)),
            titleLabel, SLOT(setText(const QString &)));

    connect(b_engine, SIGNAL(monitorPeer(UserInfo *)),
            this, SLOT(monitorPeer(UserInfo *)));
    connect(b_engine, SIGNAL(updatePhoneConfig(const QString &)),
            this, SLOT(updatePhoneConfig(const QString &)));
    connect(b_engine, SIGNAL(updatePhoneStatus(const QString &)),
            this, SLOT(updatePhoneStatus(const QString &)));
    connect(b_engine, SIGNAL(updateChannelStatus(const QString &)),
            this, SLOT(updateChannelStatus(const QString &)));
}

/*! \brief update display if needed
 *
 * Check if this is about the monitored user
 * and call updateDisplay().
 */
void XletCalls::updateUser(UserInfo *ui)
{
    if(ui == m_monitored_ui) {
        // we need to update the display
        updateDisplay();
    }
}

/*! \brief hang up channel
 */
void XletCalls::hupchan(const QString &hangupchan)
{
    b_engine->actionCall("hangup", "chan:" + m_monitored_ui->userid() + ":" + hangupchan); // Call
}

/*! \brief transfers the channel to a number
 */
void XletCalls::transftonumberchan(const QString &chan)
{
    b_engine->actionCall("transfer", "chan:" + m_monitored_ui->userid() + ":" + chan, "ext:special:dialxlet"); // Call
}

/*! \brief transfers the channel to a number
 */
void XletCalls::parkcall(const QString &chan)
{
    b_engine->actionCall("transfer", "chan:" + m_monitored_ui->userid() + ":" + chan, "ext:special:parkthecall"); // Call
}

void XletCalls::updatePhoneConfig(const QString & xphoneid)
{
}

void XletCalls::updatePhoneStatus(const QString & xphoneid)
{
    if (m_monitored_ui == NULL)
        return;
    if (! m_monitored_ui->phonelist().contains(xphoneid))
        return;
    const PhoneInfo * phoneinfo = b_engine->phones().value(xphoneid);
    if (phoneinfo == NULL)
        return;

    foreach (const QString channel, phoneinfo->channels()) {
        QString xchannel = QString("%1/%2").arg(phoneinfo->ipbxid()).arg(channel);
        qDebug() << Q_FUNC_INFO << xchannel;
        uint current_ts = QDateTime::currentDateTime().toTime_t();
        uint ts = current_ts;
        if (m_affhash.contains(xchannel))
            {}
        // m_affhash[xchannel]->updateWidget(xchannel, ts);
        else {
            CallWidget * callwidget = new CallWidget(m_monitored_ui,
                                                     xchannel,
                                                     ts,
                                                     this);
            connect(callwidget, SIGNAL(doHangUp(const QString &)),
                    this, SLOT(hupchan(const QString &)));
            connect(callwidget, SIGNAL(doTransferToNumber(const QString &)),
                    this, SLOT(transftonumberchan(const QString &)));
            connect(callwidget, SIGNAL(doParkCall(const QString &)),
                    this, SLOT(parkcall(const QString &)));
            m_layout->insertWidget(m_layout->count() - 1, callwidget,
                                   0, Qt::AlignTop);
            m_affhash[xchannel] = callwidget;
        }
    }
}

void XletCalls::updateChannelStatus(const QString & xchannel)
{
    qDebug() << Q_FUNC_INFO << xchannel;
    const ChannelInfo * channelinfo = b_engine->channels().value(xchannel);
    if(channelinfo == NULL)
        return;
    QString status = channelinfo->status();
    uint current_ts = QDateTime::currentDateTime().toTime_t();
    uint ts = current_ts;
    //                 if(map.contains("time-dial"))
    //                     ts = map.value("time-dial").toUInt() + current_ts;
    //                 if(map.contains("timestamp-dial"))
    //                     ts = map.value("timestamp-dial").toDouble() + b_engine->timeDeltaServerClient();
    //                 if(map.contains("time-link"))
    //                     ts = map.value("time-link").toUInt() + current_ts;
    //                 if(map.contains("timestamp-link"))
    //                     ts = map.value("timestamp-link").toDouble() + b_engine->timeDeltaServerClient();
    // qDebug() << Q_FUNC_INFO << it.key() << channelme << "status" << status;
    // dont display hangup channels !
    if (status == CHAN_STATUS_HANGUP)
        return;
    // activeChannels << channelme;
    // activeUids << it.key(); XXX
    qDebug() << Q_FUNC_INFO << "adding/updating" << xchannel << m_affhash;
    if (m_affhash.contains(xchannel))
        m_affhash[xchannel]->updateWidget(xchannel, ts);
    else {
//         CallWidget * callwidget = new CallWidget(m_monitored_ui,
//                                                  xchannel,
//                                                  ts,
//                                                  this,
//                                                  phoneinfo);
//         connect(callwidget, SIGNAL(doHangUp(const QString &)),
//                 this, SLOT(hupchan(const QString &)));
//         connect(callwidget, SIGNAL(doTransferToNumber(const QString &)),
//                 this, SLOT(transftonumberchan(const QString &)));
//         connect(callwidget, SIGNAL(doParkCall(const QString &)),
//                 this, SLOT(parkcall(const QString &)));
//         m_layout->insertWidget(m_layout->count() - 1, callwidget,
//                                0, Qt::AlignTop);
//         m_affhash[xchannel] = callwidget;
    }
}

/*! \brief update display according to call list
 *
 * Read m_calllist and update m_afflist accordingly.
 */
void XletCalls::updateDisplay()
{
    uint current_ts = QDateTime::currentDateTime().toTime_t();
    //qDebug() << Q_FUNC_INFO;
    CallWidget * callwidget = NULL;

    //QStringList activeChannels;  // list of active channels to be displayed
    QStringList activeUids;

    if (m_monitored_ui) {
        QString ipbxid = m_monitored_ui->ipbxid();
        foreach (const QString phoneid, m_monitored_ui->phonelist()) {
            QString xphoneid = QString("%1/%2").arg(ipbxid).arg(phoneid);
            const PhoneInfo * phoneinfo = b_engine->phones().value(xphoneid);
            if (phoneinfo == NULL)
                continue;
            foreach (const QString channel, phoneinfo->channels()) {
                const ChannelInfo * channelinfo = b_engine->channels().value(channel);
                if(channelinfo == NULL)
                    continue;
                QString status = channelinfo->status();
                uint ts = current_ts;
//                 if(map.contains("time-dial"))
//                     ts = map.value("time-dial").toUInt() + current_ts;
//                 if(map.contains("timestamp-dial"))
//                     ts = map.value("timestamp-dial").toDouble() + b_engine->timeDeltaServerClient();
//                 if(map.contains("time-link"))
//                     ts = map.value("time-link").toUInt() + current_ts;
//                 if(map.contains("timestamp-link"))
//                     ts = map.value("timestamp-link").toDouble() + b_engine->timeDeltaServerClient();
                // qDebug() << Q_FUNC_INFO << it.key() << channelme << "status" << status;
                // dont display hangup channels !
                if (status == CHAN_STATUS_HANGUP)
                    continue;
                // activeChannels << channelme;
                // activeUids << it.key(); XXX
                // qDebug() << Q_FUNC_INFO << "adding/updating" << channelme;
                if (m_affhash.contains(channel))
                    m_affhash[channel]->updateWidget(channel, ts);
                else {
                    callwidget = new CallWidget(m_monitored_ui,
                                                channel,
                                                ts,
                                                this);
                    connect(callwidget, SIGNAL(doHangUp(const QString &)),
                            this, SLOT(hupchan(const QString &)));
                    connect(callwidget, SIGNAL(doTransferToNumber(const QString &)),
                            this, SLOT(transftonumberchan(const QString &)));
                    connect(callwidget, SIGNAL(doParkCall(const QString &)),
                            this, SLOT(parkcall(const QString &)));
                    m_layout->insertWidget(m_layout->count() - 1, callwidget,
                                           0, Qt::AlignTop);
                    m_affhash[channel] = callwidget;
                }
            }
        }
    }

    // qDebug() << Q_FUNC_INFO << "activeChannels" << activeChannels;
    // qDebug() << Q_FUNC_INFO << "activeUids" << activeUids;
    // qDebug() << Q_FUNC_INFO << "m_affhash" << m_affhash.keys();
    // remove old channels
    foreach (const QString uid, m_affhash.keys()) {
        if( !activeUids.contains(uid)) {
            // qDebug() << Q_FUNC_INFO << "removing" << chan;
            qDebug() << Q_FUNC_INFO << "removing" << uid;
            m_affhash.take(uid)->deleteLater();
        }
    }
}

/*! \brief filter events based on the mimetype
 */
void XletCalls::dragEnterEvent(QDragEnterEvent *event)
{
    // qDebug() << Q_FUNC_INFO << event->mimeData()->formats();
    if (event->mimeData()->hasFormat(XUSERID_MIMETYPE)) {
        event->acceptProposedAction();
    }
}

/*! \brief updates the peer to be monitored
 *
 * can be called from reset(), dropEvent(), or at the beginning
 * of a session
 */
void XletCalls::monitorPeer(UserInfo *ui)
{
    qDebug() << Q_FUNC_INFO << b_engine->getFullId()<< ui->ipbxid() << ui->userid();
    //emptyList();
    if ((b_engine->getFullId() == ui->userid()) ||
        (b_engine->enabledFunction("switchboard"))) {
        m_monitored_ui = ui;
        changeTitle(tr("Monitoring : %1").arg(ui->fullname()));
        updateDisplay();
    }
}

/*! \brief receive drop Events.
 *
 * check if the dropped "text" is a Peer "id"
 * and start to monitor it
 */
void XletCalls::dropEvent(QDropEvent *event)
{
    if (!event->mimeData()->hasFormat(XUSERID_MIMETYPE)) {
        event->ignore();
        return;
    }
    qDebug() << Q_FUNC_INFO << event->mimeData()->data(XUSERID_MIMETYPE);
    b_engine->monitorPeerRequest(event->mimeData()->data(XUSERID_MIMETYPE));
    event->acceptProposedAction();
}

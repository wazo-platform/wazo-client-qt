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

    connect(this, SIGNAL(monitorPeerRequest(const QString &)),
            b_engine, SLOT(monitorPeerRequest(const QString &)));
    connect(b_engine, SIGNAL(monitorPeer(UserInfo *)),
            this, SLOT(monitorPeer(UserInfo *)));
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

/*! \brief update display according to call list
 *
 * Read m_calllist and update m_afflist accordingly.
 */
void XletCalls::updateDisplay()
{
    uint current_ts = QDateTime::currentDateTime().toTime_t();
    //qDebug() << Q_FUNC_INFO;
    CallWidget *callwidget = NULL;

    //QStringList activeChannels;  // list of active channels to be displayed
    QStringList activeUids;

    if (m_monitored_ui) {
        foreach (const QString phone, m_monitored_ui->phonelist()) {
            const PhoneInfo * pi = m_monitored_ui->getPhoneInfo(phone);
            if (!pi) {
                continue;
            }
            QMapIterator<QString, QVariant> it = QMapIterator<QString, QVariant>( pi->comms() );
            while (it.hasNext()) {
                it.next();
                QVariantMap map = it.value().toMap();
                // qDebug() << it.key() << map;
                QString channelme = map.value("thischannel").toString();
                QString status = map.value("status").toString();
                uint ts = current_ts;
                if(map.contains("time-dial"))
                    ts = map.value("time-dial").toUInt() + current_ts;
                if(map.contains("timestamp-dial"))
                    ts = map.value("timestamp-dial").toDouble() + b_engine->timeDeltaServerClient();
                if(map.contains("time-link"))
                    ts = map.value("time-link").toUInt() + current_ts;
                if(map.contains("timestamp-link"))
                    ts = map.value("timestamp-link").toDouble() + b_engine->timeDeltaServerClient();
                QString channelpeer = map.value("peerchannel").toString();
                QString callerid = map.value("calleridnum").toString();
                QString calleridname = map.value("calleridname").toString();
                // qDebug() << Q_FUNC_INFO << it.key() << channelme << "status" << status;
                // dont display hangup channels !
                if (status == CHAN_STATUS_HANGUP) {
                    continue;
                }
                //activeChannels << channelme;
                activeUids << it.key();
//                qDebug() << Q_FUNC_INFO << "adding/updating" << channelme;
                if (m_affhash.contains(/*channelme*/it.key())) {
                    m_affhash[it.key()/*channelme*/]->updateWidget(status, ts, channelpeer,
                                                                   callerid, calleridname, pi);
                } else {
                    callwidget = new CallWidget(m_monitored_ui,
                                                channelme,
                                                status,
                                                ts,
                                                channelpeer,
                                                callerid,
                                                calleridname,
                                                this,
                                                pi);
                    connect(callwidget, SIGNAL(doHangUp(const QString &)),
                            this, SLOT(hupchan(const QString &)));
                    connect(callwidget, SIGNAL(doTransferToNumber(const QString &)),
                            this, SLOT(transftonumberchan(const QString &)));
                    connect(callwidget, SIGNAL(doParkCall(const QString &)),
                            this, SLOT(parkcall(const QString &)));
                    m_layout->insertWidget(m_layout->count() - 1, callwidget,
                                           0, Qt::AlignTop);
                    m_affhash[it.key()/*channelme*/] = callwidget;
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
    if (event->mimeData()->hasFormat(USERID_MIMETYPE)) {
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
    qDebug() << Q_FUNC_INFO << b_engine->getFullId()<< ui->astid() << ui->userid();
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
    if (!event->mimeData()->hasFormat(USERID_MIMETYPE)) {
        event->ignore();
        return;
    }
    qDebug() << Q_FUNC_INFO << event->mimeData()->data(USERID_MIMETYPE);
    monitorPeerRequest(event->mimeData()->data(USERID_MIMETYPE));
    event->acceptProposedAction();
}

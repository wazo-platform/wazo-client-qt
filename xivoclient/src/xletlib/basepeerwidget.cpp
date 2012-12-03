/* XiVO Client
 * Copyright (C) 2007-2012, Avencall
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

#include <QDebug>
#include <QApplication>
#include <QAction>
#include <QMenu>
#include <QMessageBox>
#include <QInputDialog>
#include <QSettings>
#include <QtAlgorithms>

#include "basepeerwidget.h"
#include "baseengine.h"
#include "xivoconsts.h"
#include <channelinfo.h>
#include <phoneinfo.h>
#include <userinfo.h>
#include <parkinginfo.h>


static
bool channelTimestampLessThan(const QString &channelxid1, const QString &channelxid2)
{
    const ChannelInfo * channel1 , * channel2;
    channel1 = b_engine->channel(channelxid1);
    channel2 = b_engine->channel(channelxid2);
    if (channel1 && channel2) {
        return channel1->timestamp() < channel2->timestamp();
    }

    return false;

}

BasePeerWidget::BasePeerWidget(const UserInfo * ui)
    : m_ui_remote(ui), m_editable(false), m_transferred(false), m_contextMenu(NULL)
{
    m_ui_local = b_engine->getXivoClientUser();
    if (m_ui_remote) {
        setProperty("xuserid", m_ui_remote->xid());

        m_chitchatAction = new QAction(tr("&Open a chat window"), this);
        m_chitchatAction->setStatusTip(tr("Open a chat window with this user"));
        m_chitchatAction->setProperty("xuserid", m_ui_remote->xid());
        connect(m_chitchatAction, SIGNAL(triggered()),
                ChitChatWindow::chitchat_instance, SLOT(writeMessageTo()));
    }

    m_interceptAction = new QAction(tr("&Intercept"), this);
    m_interceptAction->setStatusTip(tr("Intercept call"));
    connect(m_interceptAction, SIGNAL(triggered()),
            this, SLOT(intercept()));

    connect(this, SIGNAL(selectedNumber(const QStringList &)),
            b_engine, SLOT(receiveNumberSelection(const QStringList &)));

    m_maxWidthWanted = 200;
    setMaximumWidth(m_maxWidthWanted);
    setAcceptDrops(true);
}

void BasePeerWidget::reloadSavedName()
{
    QSettings *settings = b_engine->getSettings();
    settings->beginGroup("renamed_items");
    const QVariant &value = settings->value(id());
    if (!value.isNull()) {
        setName(value.toString());
    }
    settings->endGroup();
}

void BasePeerWidget::dial()
{
    QString number;
    if (sender()) {
        number = sender()->property("number").toString();
    }

    if (m_ui_remote) {
        const QString &phone_id = m_ui_remote->phonelist().value(0);
        const PhoneInfo *p = b_engine->phone(phone_id);
        if (p) {
            number = p->number();
        }
    }

    if (! number.isEmpty()) {
        b_engine->actionDialNumber(number);
    } else {
        qDebug() << "Failed to dial: Empty number";
    }
}

void BasePeerWidget::dialMobilePhone()
{
    b_engine->actionDialNumber(m_ui_remote->mobileNumber());
}

void BasePeerWidget::peerdial()
{
    if (m_ui_remote) {
        b_engine->actionCall("originate",
                             QString("user:%1").arg(m_ui_remote->xid()),
                             QString("exten:%0/%1")
                             .arg(m_ui_remote->ipbxid())
                             .arg(sender()->property("number").toString()));
    } else {
        b_engine->actionCall("originate",
                             "ext:" + m_number,
                             "ext:" + sender()->property("number").toString());
    }
}

void BasePeerWidget::hangup()
{
    if (! m_ui_remote) {
        return;
    }

    const QString &xchannel = sender()->property("xchannel").toString();
    b_engine->actionCall("hangup", QString("chan:%1").arg(xchannel));
}

void BasePeerWidget::intercept()
{
    if (! m_ui_remote) {
        return;
    }

    const QString &xchannel = sender()->property("xchannel").toString();
    b_engine->actionCall("intercept",
                         QString("user:%0").arg(b_engine->getFullId()),
                         QString("chan:%0").arg(xchannel));
}

void BasePeerWidget::transfer()
{
    const QString &xchannel = sender()->property("xchannel").toString();
    const QString &src = QString("chan:%1").arg(xchannel);
    const QString &dst = QString("exten:%0/%1")
        .arg(m_ui_remote->ipbxid())
        .arg(sender()->property("number").toString());
    b_engine->actionCall("transfer", src, dst);
    m_transferred = true;
}

void BasePeerWidget::itransfer()
{
    const QString &xchannel = sender()->property("xchannel").toString();
    const QString &src = QString("chan:%1").arg(xchannel);
    const QString &dst_number = sender()->property("number").toString();
    const QString &ipbx = (m_ui_remote ? m_ui_remote->ipbxid()
                           : m_ui_local->ipbxid());
    const QString &dst = QString("exten:%0/%1").arg(ipbx).arg(dst_number);

    b_engine->actionCall("atxfer", src, dst);
}

void BasePeerWidget::itransfercancel()
{
    if (! m_ui_remote) {
        return;
    }

    const QString &xchannel = sender()->property("xchannel").toString();
    b_engine->actionCall("transfercancel", QString("chan:%1").arg(xchannel));
}

void BasePeerWidget::parkcall()
{
    if (m_ui_remote) {
        QString xchannel = sender()->property("xchannel").toString();
        QString parking_id = sender()->property("id").toString();
        b_engine->actionCall("parking",
                             QString("chan:%1").arg(xchannel),
                             QString("parking:%1").arg(parking_id));
    }
}

void BasePeerWidget::vmtransfer()
{
    if (! m_ui_remote) {
        return;
    }

    const QString &xchannel = sender()->property("xchannel").toString();
    b_engine->actionCall("transfer",
                         QString("chan:%1").arg(xchannel),
                         "voicemail:" + m_ui_remote->xvoicemailid());
}

void BasePeerWidget::inviteConfRoom()
{
    if (! m_ui_remote) {
        return;
    }

    const QString &invitee = QString("%0:%1").arg("user").arg(m_ui_remote->xid());
    b_engine->inviteConfRoom(invitee);
}

void BasePeerWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (! m_ui_remote || ! m_ui_local) {
        return;
    }

    if (event->button() != Qt::LeftButton) {
        return;
    }

    if (const QWidget *w = childAt(event->pos())) {
        const QString &subwidgetkind = w->property("kind").toString();
        if (subwidgetkind == "mobile") {
            return b_engine->actionDialNumber(m_ui_remote->mobileNumber());
        }
    }

    foreach(const ChannelInfo * channelinfo, loopOverChannels(m_ui_local)) {
        const QString &status = channelinfo->commstatus();
        if ((status == CHAN_STATUS_LINKED_CALLER) ||
            (status == CHAN_STATUS_LINKED_CALLED)) {
            QString action = b_engine->getConfig("doubleclick.searchpanel").toString();
            if (action == "atxfer") {
                QString to;
                if (m_ui_remote) {
                    to = "user:" + m_ui_remote->xid();
                } else {
                    to = "ext:" + m_number;
                }
                return b_engine->actionCall("atxfer",
                                            QString("chan:%1").arg(channelinfo->xid()),
                                            to);
            }
        }
    }

    // "I" have no current communications, intercept if the person is being called
    foreach(const ChannelInfo * channel, loopOverChannels(m_ui_remote)) {
        const QString &status = channel->commstatus();
        if (status != CHAN_STATUS_RINGING) {
            break;
        }
        return b_engine->actionCall("transfer",
                                    QString("chan:%1:%2").arg(m_ui_remote->xid()).arg(channel->id()),
                                    "user:special:me");
    }
    dial();
}

void BasePeerWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_dragstartpos = event->pos();

        QStringList numbers;
        if (m_ui_remote) {
            foreach (const QString &xphoneid, m_ui_remote->phonelist()) {
                const PhoneInfo * phone = b_engine->phone(xphoneid);
                if (phone) numbers.append(phone->number());
            }
            if (! m_ui_remote->mobileNumber().isEmpty())
                numbers.append(m_ui_remote->mobileNumber());
        }
        emit selectedNumber(numbers);
    }
}

bool BasePeerWidget::isSwitchBoard() const
{
    return (m_ui_local && m_ui_local->isSwitchBoard());
}

bool BasePeerWidget::isLeftClick(const QMouseEvent *event) const
{
    return (event->buttons() & Qt::LeftButton) == true;
}

void BasePeerWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (! canDrag(event)) {
        return;
    }

    if ((event->pos() - m_dragstartpos).manhattanLength()
        < QApplication::startDragDistance()) {
        return;
    }

    QDrag *drag = new QDrag(this);
    QMimeData * mimeData = new QMimeData;
    if (m_ui_remote) {
        mimeData->setData(XUSERID_MIMETYPE, m_ui_remote->xid().toAscii());
        mimeData->setData(XPHONEID_MIMETYPE, m_ui_remote->phonelist().join("").toAscii());
    } else {
        mimeData->setText(m_number);
        mimeData->setData(NUMBER_MIMETYPE, m_number.toAscii());
    }
    drag->setMimeData(mimeData);

    drag->start(Qt::CopyAction | Qt::MoveAction);
}

QList<const ChannelInfo *> BasePeerWidget::loopOverChannels(const UserInfo * userinfo)
{
    QList<const ChannelInfo *> channels;
    foreach (const QString &xphoneid, userinfo->phonelist()) {
        if (const PhoneInfo * phoneinfo = b_engine->phone(xphoneid)) {
            foreach (const QString &xchannel, phoneinfo->xchannels()) {
                if (const ChannelInfo * channelinfo = b_engine->channels().value(xchannel)) {
                    channels << channelinfo;
                }
            }
        }
    }
    return channels;
}

bool BasePeerWidget::isme() const
{
    return m_ui_local && m_ui_remote && m_ui_local == m_ui_remote;
}

void BasePeerWidget::updateMenuSelf(QMenu * menu, QContextMenuEvent * /*event*/)
{
    addHangupMenu(menu);
}

void BasePeerWidget::updateMenuPeer(QMenu * menu, QContextMenuEvent * /*event*/)
{
    addDialMenu(menu);
    addChitChatMenu(menu);
    addInterceptMenu(menu);
    addMeetmeMenu(menu);
    addTxferMenu(menu, true);
    addTxferMenu(menu, false);
}

void BasePeerWidget::addDialMenu(QMenu * menu)
{
    if (m_ui_remote->phonelist().size() > 1) {
        if (QMenu * submenu = new QMenu(tr("&Call"), menu)) {
            foreach (const QString & phonexid, m_ui_remote->phonelist()) {
                if (const PhoneInfo * p = b_engine->phone(phonexid)) {
                    const QString & number = p->number();
                    if (! number.isEmpty()) {
                        if (QAction * action = new QAction(number, this)) {
                            action->setProperty("number", number);
                            connect(action, SIGNAL(triggered()), this, SLOT(dial()));
                            submenu->addAction(action);
                        }
                    }
                }
            }
            if (! submenu->isEmpty()) {
                m_submenus.append(submenu);
                menu->addMenu(m_submenus.last());
            }
        }
    } else {
        if (const PhoneInfo * p = b_engine->phone(m_ui_remote->phonelist().value(0))) {
            const QString & number = p->number();
            if (! number.isEmpty()) {
                if (QAction * action = new QAction(tr("&Call"), this)) {
                    action->setProperty("number", number);
                    connect(action, SIGNAL(triggered()), this, SLOT(dial()));
                    menu->addAction(action);
                }
            }
        }
    }

    if (! m_ui_remote->mobileNumber().isEmpty()) {
        if (QAction * call_mobile = new QAction(tr("Call &mobile"), this)) {
            call_mobile->setProperty("number", m_ui_remote->mobileNumber());
            connect(call_mobile, SIGNAL(triggered()), this, SLOT(dial()));
            menu->addAction(call_mobile);
        }
    }
}

void BasePeerWidget::addChitChatMenu(QMenu * menu)
{
    if (m_ui_remote->enableclient() && m_ui_remote->connected()) {
        menu->addAction(m_chitchatAction);
    }
}

void BasePeerWidget::addEditMenu(QMenu * menu)
{
    if (m_editable) {
        menu->addAction(tr("&Edit"), this, SLOT(edit()));
    }
}

void BasePeerWidget::addHangupMenu(QMenu * menu)
{
    static QStringList can_hangup = QStringList()
        << CHAN_STATUS_LINKED_CALLER
        << CHAN_STATUS_LINKED_CALLED
        << CHAN_STATUS_RINGING;
    QStringList channels = m_ui_local->xchannels();
    qSort(channels.begin(), channels.end(), channelTimestampLessThan);
    int callorder = 1;
    foreach (const QString & channelxid, channels) {
        if (const ChannelInfo * c = b_engine->channel(channelxid)) {
            if (can_hangup.contains(c->commstatus())
                || c->talkingto_kind().contains("meetme")) {
                QAction * action = new QAction(tr("&Hangup call") + " " + QString::number(callorder), this);
                action->setProperty("xchannel", c->xid());
                menu->addAction(action);
                connect(action, SIGNAL(triggered()), this, SLOT(hangup()));
            }
        }
        ++callorder;
    }
}

void BasePeerWidget::addInterceptMenu(QMenu * menu)
{
    foreach (const QString & channelxid, m_ui_remote->xchannels()) {
        if (const ChannelInfo * c = b_engine->channel(channelxid)) {
            if (c->commstatus() == CHAN_STATUS_RINGING) {
                QString s = m_transferred ? tr("Cancel transfer") : tr("&Intercept");
                QAction * action = new QAction(s, this);
                action->setProperty("xchannel",
                                    (QString("%0/%1")
                                     .arg(c->ipbxid())
                                     .arg(c->talkingto_id())));
                connect(action, SIGNAL(triggered()), this, SLOT(intercept()));
                menu->addAction(action);
            }
        }
    }
}

void BasePeerWidget::addMeetmeMenu(QMenu * menu)
{
    if (m_ui_local && b_engine->isInMeetme()) {
        QAction * action = new QAction(tr("Invite in conference room"), this);
        connect(action, SIGNAL(triggered()), this, SLOT(inviteConfRoom()));
        menu->addAction(action);
    }
}

void BasePeerWidget::addParkingMenu(QMenu * menu)
{
    static QStringList can_park = QStringList()
        << CHAN_STATUS_RINGING
        << CHAN_STATUS_LINKED_CALLED
        << CHAN_STATUS_LINKED_CALLER;

    bool park = false;
    const QStringList & channels = m_ui_local->xchannels();
    QString string;
    if (isme()) {
        foreach (const QString & channelxid, channels) {
            if (const ChannelInfo * c = b_engine->channel(channelxid)) {
                if (can_park.contains(c->commstatus())) {
                    string = tr("&Park correspondant");
                    park = true;
                    break;
                }
            }
        }
    } else if (m_ui_remote->isTalkingTo(m_ui_local->xid())) {
        string = tr("&Park");
        park = true;
    }

    if (park) {
        QMenu * parkMenu = new QMenu(string, menu);
        foreach (XInfo * x, b_engine->iterover("parkinglots")) {
            ParkingInfo * p = static_cast<ParkingInfo *>(x);
            QAction * action = new QAction(p->name(), this);
            action->setProperty("id", p->xid());
            if (const ChannelInfo * c = b_engine->channel(channels.value(0))) {
                QString peers_chan = QString("%1/%2")
                    .arg(b_engine->ipbxid()).arg(c->talkingto_id());
                action->setProperty("xchannel", peers_chan);
                connect (action, SIGNAL(triggered()), this, SLOT(parkcall()));
                parkMenu->addAction(action);
            }
        }
        m_submenus.append(parkMenu);
        menu->addMenu(m_submenus.last());
    }
}

void BasePeerWidget::addTxferMenu(QMenu * menu, bool blind)
{
    if (m_ui_local == NULL) {
        return;
    }

    if (m_ui_local->enablexfer() == false) {
        return;
    }

    if (m_ui_local->xchannels().size() == 0) {
        return;
    }

    if (m_ui_remote->isTalkingTo(m_ui_local->xid())) {
        return;
    }

    QString title = blind ? tr("Blind &Transfer") : tr("&Attended Transfer");

    const QStringList &numbers = this->getPeerNumbers();

    QList<QAction *> transfer_actions;

    foreach (const QString & chanxid, m_ui_local->xchannels()) {
        const ChannelInfo * channel = b_engine->channel(chanxid);
        if (channel == NULL) {
            continue;
        }
        if (channel->canBeTransferred() == false) {
            continue;
        }

        foreach (const QString &number, numbers) {
            QAction *action = NULL;
            if (blind) {
                action = this->newBlindTransferAction(number, *channel);
            } else {
                action = this->newAttendedTransferAction(number, *channel);
            }
            if (action != NULL) {
                transfer_actions << action;
            }
        }
    }

    bool multiple_numbers = transfer_actions.size() > 1;
    QMenu *transfer_menu = this->getTransferMenu(menu, title, multiple_numbers);
    foreach (QAction *transfer_action, transfer_actions) {
        transfer_action->setParent(transfer_menu);
        transfer_menu->addAction(transfer_action);
    }
}

QStringList BasePeerWidget::getPeerNumbers() const
{
    QStringList numbers;
    foreach (const QString &phonexid, m_ui_remote->phonelist()) {
        const PhoneInfo * phone = b_engine->phone(phonexid);
        if (phone == NULL) {
            continue;
        }
        const QString &number = phone->number();
        if (number.isEmpty() == false) {
            numbers << number;
        }
    }

    if (! m_ui_remote->mobileNumber().isEmpty()) {
        numbers << m_ui_remote->mobileNumber();
    }

    return numbers;
}

QMenu * BasePeerWidget::getTransferMenu(QMenu *basemenu,
                                        const QString &title,
                                        bool add_sub_menu)
{
    if (add_sub_menu) {
        QMenu *submenu = new QMenu(title, basemenu);
        if (submenu == NULL) {
            return basemenu;
        }
        m_submenus.append(submenu);
        basemenu->addMenu(m_submenus.last());
        return submenu;
    }
    return basemenu;
}

QAction * BasePeerWidget::newBlindTransferAction(const QString &number,
                                                 const ChannelInfo &channel)
{
    QString blind_transfer_label_pattern = tr("Blind transfer <%0>");
    QString label = QString(blind_transfer_label_pattern).arg(number);
    QAction * action = new QAction(label, this);
    if (action == NULL) {
        return NULL;
    }
    QString targeted_channel = QString("%0/%1").arg(channel.ipbxid()).arg(channel.talkingto_id());
    action->setProperty("number", number);
    action->setProperty("xchannel", targeted_channel);
    connect(action, SIGNAL(triggered()), this, SLOT(transfer()));

    return action;
}

QAction * BasePeerWidget::newAttendedTransferAction(const QString &number,
                                                    const ChannelInfo &channel)
{
    QString attended_transfer_label_pattern = tr("Attended transfer <%0>");
    QString label = QString(attended_transfer_label_pattern).arg(number);
    QAction * action = new QAction(label, this);
    if (action == NULL) {
        return NULL;
    }
    const QString &targeted_channel = channel.xid();
    action->setProperty("number", number);
    action->setProperty("xchannel", targeted_channel);
    connect(action, SIGNAL(triggered()), this, SLOT(itransfer()));

    return action;
}

void BasePeerWidget::addTxferVmMenu(QMenu * menu)
{
    if (! m_ui_remote || (m_ui_remote && m_ui_remote->voicemailid().isEmpty())) {
        return;
    }

    foreach (const QString &channelxid, m_ui_local->xchannels()) {
        const ChannelInfo * c = b_engine->channel(channelxid);
        if (! c || (c && ! c->canBeTransferred())) {
            continue;
        }

        QAction * action = new QAction(tr("Transfer to &voice mail"), this);
        if (! action) {
            continue;
        }

        QString chan_to_transfer = QString("%0/%1").arg(c->ipbxid()).arg(c->talkingto_id());
        action->setProperty("xchannel", chan_to_transfer);
        connect(action, SIGNAL(triggered()), this, SLOT(vmtransfer()));
        menu->addAction(action);
        break;
    }
}

void BasePeerWidget::contextMenuEvent(QContextMenuEvent *event)
{
    if (! m_ui_local || ! m_ui_remote) return;

    if(! m_contextMenu)
        m_contextMenu = new QMenu(this);
    else{
        m_contextMenu->clear();
        foreach(QMenu* submenu, m_submenus)
            delete submenu;
        m_submenus.erase(m_submenus.begin(), m_submenus.end());
    }
    if (isme()) {
        updateMenuSelf(m_contextMenu, event);
    } else {
        updateMenuPeer(m_contextMenu, event);
    }
    addEditMenu(m_contextMenu);
    addTxferVmMenu(m_contextMenu);

    if (! m_contextMenu->isEmpty()) {
        m_contextMenu->popup(event->globalPos());
    }
}

void BasePeerWidget::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat(XUSERID_MIMETYPE)  ||
        event->mimeData()->hasFormat(XPHONEID_MIMETYPE) ||
        event->mimeData()->hasFormat(NUMBER_MIMETYPE)   ||
        event->mimeData()->hasFormat(CHANNEL_MIMETYPE)) {
        if (event->proposedAction() & (Qt::CopyAction|Qt::MoveAction)) {
            event->acceptProposedAction();
        }
    }
}

void BasePeerWidget::dragMoveEvent(QDragMoveEvent *event)
{
    event->accept(rect());
    if (event->proposedAction() & (Qt::CopyAction | Qt::MoveAction)) {
        event->acceptProposedAction();
    }
}

void BasePeerWidget::dropEvent(QDropEvent *event)
{
    QString userid_from = QString::fromAscii(event->mimeData()->data(XUSERID_MIMETYPE));
    QString channel_from = QString::fromAscii(event->mimeData()->data(CHANNEL_MIMETYPE));
    QString to;
    if (m_ui_remote) {
        to = "user:" + m_ui_remote->xid();
    } else {
        to = "ext:" + m_number;
    }

    switch(event->proposedAction()) {
    case Qt::CopyAction:
        if (event->mimeData()->hasFormat(CHANNEL_MIMETYPE)) {
            event->acceptProposedAction();
            b_engine->actionCall("transfer", "chan:" + userid_from + ":" + channel_from, to);
        } else if (event->mimeData()->hasFormat(XUSERID_MIMETYPE)) {
            event->acceptProposedAction();
            b_engine->actionCall("originate", "user:" + userid_from, to);
        } else if (event->mimeData()->hasFormat(NUMBER_MIMETYPE)) {
            event->acceptProposedAction();
            b_engine->actionCall("originate", to, "ext:" + event->mimeData()->text());
        }
        break;
    case Qt::MoveAction:
        event->acceptProposedAction();
        b_engine->actionCall("atxfer",
                             QString("chan:%1").arg(channel_from),
                             to);
        break;
    default:
        qDebug() << "Unrecognized action" << event->proposedAction();
        break;
    }
}

QString BasePeerWidget::id() const
{
    if (m_ui_remote) {
        return (QString("userid-") + m_ui_remote->xid());
    } else {
        return (QString("number-") + number());
    }
}

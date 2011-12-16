/* XiVO Client
 * Copyright (C) 2007-2011, Avencall
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

#include "basepeerwidget.h"
#include "baseengine.h"
#include "xivoconsts.h"

BasePeerWidget::BasePeerWidget(const UserInfo * ui)
    : m_ui_remote(ui), m_editable(false), m_transfered(false)
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

    m_removeAction = new QAction(tr("&Remove"), this);
    m_removeAction->setStatusTip(tr("Remove this peer from the panel"));
    connect(m_removeAction, SIGNAL(triggered()),
            this, SLOT(tryRemoveFromPanel()));

    m_renameAction = new QAction(tr("Re&name"), this);
    m_renameAction->setStatusTip(tr("Rename this peer"));
    connect(m_renameAction, SIGNAL(triggered()),
            this, SLOT(rename()));

    m_interceptAction = new QAction(tr("&Intercept"), this);
    m_interceptAction->setStatusTip(tr("Intercept call"));
    connect(m_interceptAction, SIGNAL(triggered()),
            this, SLOT(intercept()));

    connect(this, SIGNAL(selectedNumber(const QStringList &)),
            b_engine, SLOT(receiveNumberSelection(const QStringList &)));

    m_maxWidthWanted = 200;
    m_maxWidthWanted = b_engine->getConfig("guioptions.maxwidthwanted").toInt();
    if (m_maxWidthWanted < 50) {
        m_maxWidthWanted = 200;
    }
    setMaximumWidth(m_maxWidthWanted);
    setAcceptDrops(true);
}

void BasePeerWidget::reloadSavedName()
{
    // retrieve saved name
    QSettings *settings = b_engine->getSettings();
    settings->beginGroup("renamed_items");
    QVariant value = settings->value(id());
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
    } else if (m_ui_remote) {
        if (const PhoneInfo * p = b_engine->phone(
                m_ui_remote->phonelist().value(0))) {
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

/*! \brief hangup a channel
 *
 * uses "channel" property of the sender
 */
void BasePeerWidget::hangup()
{
    if (m_ui_remote) {
        QString xchannel = sender()->property("xchannel").toString();
        b_engine->actionCall("hangup",
                             QString("chan:%1").arg(xchannel));
    }
}

/*! \brief intercept a call
 *
 * uses "channel" property of the sender
 */
void BasePeerWidget::intercept()
{
    if (m_ui_remote) {
        QString xchannel = sender()->property("xchannel").toString();
        b_engine->actionCall("intercept",
                             QString("user:%0").arg(b_engine->getFullId()),
                             QString("chan:%0").arg(xchannel));
    }
}

/*! \brief Direct transfer
 *
 * uses "channel" property of the sender
 */
void BasePeerWidget::transfer()
{
    QString xchannel = sender()->property("xchannel").toString();
    QString src = QString("chan:%1").arg(xchannel);
    QString dst = QString("exten:%0/%1")
        .arg(m_ui_remote->ipbxid())
        .arg(sender()->property("number").toString());
    b_engine->actionCall("transfer", src, dst);
    m_transfered = true;
}

/*! \brief Indirect Transfer
 */
void BasePeerWidget::itransfer()
{
    QString xchannel = sender()->property("xchannel").toString();
    QString src = QString("chan:%1").arg(xchannel);
    QString dst_number = sender()->property("number").toString();
    const QString ipbx = (m_ui_remote ? m_ui_remote->ipbxid()
                          : m_ui_local->ipbxid());
    QString dst = QString("exten:%0/%1").arg(ipbx).arg(dst_number);
    b_engine->actionCall("atxfer", src, dst);
}

/*! \brief Cancel an Indirect Transfer
 */
void BasePeerWidget::itransfercancel()
{
    if (m_ui_remote) {
        QString xchannel = sender()->property("xchannel").toString();
        b_engine->actionCall("transfercancel",
                             QString("chan:%1").arg(xchannel));
    }
}

/*! \brief transfer to voice mail
 */
void BasePeerWidget::vmtransfer()
{
    if (m_ui_remote) {
        QString xchannel = sender()->property("xchannel").toString();
        b_engine->actionCall("transfer",
                             QString("chan:%1").arg(xchannel),
                             "voicemail:" + m_ui_remote->xvoicemailid());
    }
}

/*! \brief handle double click
 *
 * dial or indirect transfer if left mouse button used
 */
void BasePeerWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (! m_ui_remote)
        return;
    if (! m_ui_local)
        return;
    if (event->button() == Qt::LeftButton) {
        QWidget *w = childAt(event->pos());
        QString subwidgetkind;
        if (w) {
            subwidgetkind = w->property("kind").toString();
        }
        foreach(const ChannelInfo * channelinfo, loopOverChannels(m_ui_local)) {
            const QString status = channelinfo->commstatus();
            if ((status == CHAN_STATUS_LINKED_CALLER) ||
                (status == CHAN_STATUS_LINKED_CALLED)) {
                QString action ;
                if (parentWidget()->metaObject()->className() == QString("XletSwitchBoard")) {
                    action = b_engine->getConfig("doubleclick.switchboard").toString();
                } else {
                    action = b_engine->getConfig("doubleclick.searchpanel").toString();
                }
                if (action == "atxfer") {
                    QString to;
                    if (m_ui_remote) {
                        to = "user:" + m_ui_remote->xid();
                    } else {
                        to = "ext:" + m_number;
                    }
                    // Initiate an indirect transfer.
                    b_engine->actionCall("atxfer",
                                         QString("chan:%1").arg(channelinfo->xid()),
                                         to);
                    return;
                } else {
                    // Do nothing, get out of the loop, and eventually dial
                }
                
            }
        }
        // "I" have no current communications, intercept if the person is being called
        foreach(const ChannelInfo * channelinfo, loopOverChannels(m_ui_remote)) {
            const QString status = channelinfo->commstatus();
            if (status == CHAN_STATUS_RINGING) {
                b_engine->actionCall("transfer",
                                     QString("chan:%1:%2").arg(m_ui_remote->xid()).arg(channelinfo->id()),
                                     "user:special:me");
                return;
            }
        }
        if (subwidgetkind == "mobile") {
            b_engine->actionDialNumber(m_ui_remote->mobileNumber());
        } else {
            // just dial the person
            dial();
        }
    }
}

/*! \brief handle mouse press
 *
 * store mouse position for further processing
 * signals a QStringList of numbers for the selected peer
 */
void BasePeerWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_dragstartpos = event->pos();

        QStringList numbers;
        if (m_ui_remote) {
            foreach (QString xphoneid, m_ui_remote->phonelist()) {
                const PhoneInfo * phone = b_engine->phone(xphoneid);
                if (phone) numbers.append(phone->number());
            }
            if (! m_ui_remote->mobileNumber().isEmpty())
                numbers.append(m_ui_remote->mobileNumber());
        }
        emit selectedNumber(numbers);
    }
}

bool BasePeerWidget::canDrag(const QMouseEvent *event) const
{
    if (! isLeftClick(event)) return false;
    if (! isme() && ! isSwitchBoard()) return false;
    return true;
}

bool BasePeerWidget::isSwitchBoard() const
{
    return (m_ui_local && m_ui_local->isSwitchBoard());
}

bool BasePeerWidget::isLeftClick(const QMouseEvent *event) const
{
    return (event->buttons() & Qt::LeftButton) == true;
}

/*! \brief start drag if necessary
 */
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
        // XXX mimeData->setText(m_ui_remote->phoneNumber());
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
    QString ipbxid = userinfo->ipbxid();
    foreach (const QString xphoneid, userinfo->phonelist()) {
        const PhoneInfo * phoneinfo = b_engine->phone(xphoneid);
        if (phoneinfo == NULL)
            continue;
        foreach (const QString xchannel, phoneinfo->xchannels()) {
            const ChannelInfo * channelinfo = b_engine->channels().value(xchannel);
            if (channelinfo == NULL)
                continue;
            channels << channelinfo;
        }
    }
    return channels;
}

/*!
 * Check if this peer is the current user
 */
bool BasePeerWidget::isme() const
{
    return m_ui_local && m_ui_remote && m_ui_local == m_ui_remote;
}

/*!
 * Add items to the contextual menu for self click
 * \param menu The contextual menu to modify
 * \param event The click event that was received
 */
void BasePeerWidget::updateMenuSelf(QMenu * menu, QContextMenuEvent * /*event*/)
{
    addHangupMenu(menu);
}

/*!
 * Add items to the contextual menu for clicked peers
 * \param menu The contextual menu to modify
 * \param event The click event that was received
 */
void BasePeerWidget::updateMenuPeer(QMenu * menu, QContextMenuEvent * /*event*/)
{
    addDialMenu(menu);
    addChitChatMenu(menu);
    addInterceptMenu(menu);
    addMeetmeMenu(menu);
    addTxferMenu(menu, true);
    addTxferMenu(menu, false);
}

void BasePeerWidget::updateMenuSwitchboard(QMenu * menu)
{
    menu->addSeparator();
    menu->addAction(m_removeAction);
    menu->addAction(m_renameAction);
}

/*!
 * Add a dial entry to the contextual menu
 *
 * Will also check if the user has a mobile number to add the call mobile action
 *
 * \param menu The contextual menu to modify
 */
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
                menu->addMenu(submenu);
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

/*!
 * Add a chat entry to the contextual menu
 * \param menu The menu to modify
 */
void BasePeerWidget::addChitChatMenu(QMenu * menu)
{
    if (m_ui_remote->enableclient() && m_ui_remote->connected()) {
        menu->addAction(m_chitchatAction);
    }
}

/*!
 * Add an edit entry to the contextual menu
 */
void BasePeerWidget::addEditMenu(QMenu * menu)
{
    if (m_editable) {
        menu->addAction(tr("&Edit"), this, SLOT(edit()));
    }
}

/*!
 * Add an hang up action to the contextual menu
 */
void BasePeerWidget::addHangupMenu(QMenu * menu)
{
    static QStringList can_hangup = QStringList()
        << CHAN_STATUS_LINKED_CALLER
        << CHAN_STATUS_LINKED_CALLED;
    const QStringList & channels = m_ui_local->xchannels();
    foreach (const QString & channelxid, channels) {
        if (const ChannelInfo * c = b_engine->channel(channelxid)) {
            if (can_hangup.contains(c->commstatus())
                || c->talkingto_kind().contains("meetme")) {
                QAction * action = new QAction(tr("&Hangup"), this);
                action->setProperty("xchannel", c->xid());
                menu->addAction(action);
                connect(action, SIGNAL(triggered()), this, SLOT(hangup()));
            }
        }
    }
}

/*!
 * Add an intercept entry in the contextual menu
 */
void BasePeerWidget::addInterceptMenu(QMenu * menu)
{
    if (! b_engine->getCapasIpbxCommands().contains("intercept")
            && ! m_transfered) {
        return;
    }
    foreach (const QString & channelxid, m_ui_remote->xchannels()) {
        if (const ChannelInfo * c = b_engine->channel(channelxid)) {
            if (c->commstatus() == CHAN_STATUS_RINGING) {
                /* tr() can't be factorised, e.g. tr(..?..:..), because lupdate
                 * can't evaluate the ternary operator and will not detect the
                 * strings */
                QString s = m_transfered ? tr("Cancel transfer") : tr("&Intercept");
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

/*!
 * Add an invite to meetme menu
 *
 * If the current user is part of a conference, he can invite other peers
 */
void BasePeerWidget::addMeetmeMenu(QMenu * menu)
{
    const QStringList & my_channels = m_ui_local->xchannels();
    foreach (const QString & meetmexid, b_engine->iterover("meetmes").keys()) {
        if (const MeetmeInfo * m = b_engine->meetme(meetmexid)) {
            foreach (const QString & key, m->channels().keys()) {
                if (my_channels.contains(QString("%0/%1")
                                         .arg(m->ipbxid()).arg(key))) {
                    QAction * action = new QAction(tr("Invite in meetme room %0")
                                                   .arg(m->name()), this);
                    action->setProperty("number", m->number());
                    connect(action, SIGNAL(triggered()), this, SLOT(peerdial()));
                    menu->addAction(action);
                }
            }
        }
    }
}

/*!
 * Add switchboard entries to the contextual menu
 */
void BasePeerWidget::addSwitchboardMenu(QMenu * menu)
{
    if (xletName() == QString("XletSwitchBoard")) {
        updateMenuSwitchboard(menu);
    }
}

/*!
 * Add a transfer entry to the contextual menu
 *
 * This entry is shown when the user has an active line
 *
 * \param menu The menu to modify
 * \param blind True for blind transfer, false for attended transfer
 */
void BasePeerWidget::addTxferMenu(QMenu * menu, bool blind)
{
    // XXX (pcm) We are ignoring attended transfers until they work properlly
    if (! blind) return;
    if (! m_ui_local->xchannels().size()) return;
    if (m_ui_remote->isTalkingTo(m_ui_local->xid())) return;
    QString string = blind ? tr("Direct &Transfer") : tr("&Indirect Transfer");

    QStringList numbers;
    foreach (const QString phonexid, m_ui_remote->phonelist()) {
        if (const PhoneInfo * p = b_engine->phone(phonexid)) {
            numbers << p->number();
        }
    }
    if (! m_ui_remote->mobileNumber().isEmpty()) {
        numbers << m_ui_remote->mobileNumber();
    }

    foreach (const QString & chanxid, m_ui_local->xchannels()) {
        if (const ChannelInfo * c = b_engine->channel(chanxid)) {
            if ((c->commstatus() == CHAN_STATUS_LINKED_CALLER ||
                 c->commstatus() == CHAN_STATUS_LINKED_CALLED)
                && ! c->talkingto_kind().contains("meetme")) {
                QMenu * submenu = 0;
                if (numbers.size() > 1) {
                    submenu = new QMenu(string, menu);
                    menu->addMenu(submenu);
                } else {
                    submenu = menu;
                }
                foreach (const QString & number, numbers) {
                    if (QAction * action = new QAction(numbers.size() == 1 ? string : number, this)) {
                        action->setProperty("number", number);
                        if (blind) {
                            action->setProperty("xchannel", QString("%0/%1").arg(c->ipbxid()).arg(c->talkingto_id()));
                            connect(action, SIGNAL(triggered()),
                                    this, SLOT(transfer()));
                        } else  {
                            action->setProperty("xchannel", c->xid());
                            connect(action, SIGNAL(triggered()),
                                    this, SLOT(itransfer()));
                        }
                        submenu->addAction(action);
                    }
                }
            }
        }
    }
}

/*!
 * Add a menu entry to transfer to voicemail
 */
void BasePeerWidget::addTxferVmMenu(QMenu * menu)
{
    if (! m_ui_remote->voicemailid().isEmpty()) {
        foreach (const QString channelxid, m_ui_local->xchannels()) {
            if (const ChannelInfo * c = b_engine->channel(channelxid)) {
                if (! c->talkingto_kind().contains("meetme")) {
                    if (QAction * action = new QAction(
                            tr("Transfer to &voice mail"), this)) {
                        QString chan_to_transfer = QString("%0/%1")
                            .arg(c->ipbxid()).arg(c->talkingto_id());
                        action->setProperty("xchannel", chan_to_transfer);
                        connect(action, SIGNAL(triggered()),
                                this, SLOT(vmtransfer()));
                        menu->addAction(action);
                    }
                }
            }
        }
    }
}

/*! \brief open a context menu
 */
void BasePeerWidget::contextMenuEvent(QContextMenuEvent *event)
{
    if (! m_ui_local || ! m_ui_remote) return;

    QMenu contextMenu(this);
    if (isme()) {
        updateMenuSelf(&contextMenu, event);
    } else {
        updateMenuPeer(&contextMenu, event);
    }
    addEditMenu(&contextMenu);
    addSwitchboardMenu(&contextMenu);
    addTxferVmMenu(&contextMenu);

    if (! contextMenu.isEmpty()) {
        contextMenu.exec(event->globalPos());
    }
}

/*!
 *
 * This method would be subclassed by subclasses
 * which don't have m_ui_remote valid.
 */
QString BasePeerWidget::name() const
{
    if (m_ui_remote) {
        return m_ui_remote->fullname();
    } else {
        return QString();
    }
}

/*! \brief
 *
 * filters the acceptable drag on the mime type.
 */
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

/*! \brief drag move event
 *
 * filter based on the mimeType.
 */
void BasePeerWidget::dragMoveEvent(QDragMoveEvent *event)
{
    event->accept(rect());
    if (event->proposedAction() & (Qt::CopyAction | Qt::MoveAction)) {
        event->acceptProposedAction();
    }
}

/*! \brief receive drop events
 *
 * initiate an originate or transfer
 */
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
        // transfer the call to the peer "to"
        if (event->mimeData()->hasFormat(CHANNEL_MIMETYPE)) {
            event->acceptProposedAction();
            b_engine->actionCall("transfer", "chan:" + userid_from + ":" + channel_from, to); // Call
        } else if (event->mimeData()->hasFormat(XUSERID_MIMETYPE)) {
            event->acceptProposedAction();
            b_engine->actionCall("originate", "user:" + userid_from, to);
        } else if (event->mimeData()->hasFormat(NUMBER_MIMETYPE)) {
            event->acceptProposedAction();
            b_engine->actionCall("originate", to, "ext:" + event->mimeData()->text());
        }
        break;
    case Qt::MoveAction:
        // can be reached with the shift button
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

/** First ask the user and send the signal if the user accepts
 */
void BasePeerWidget::tryRemoveFromPanel()
{
    QString _number = number();
    if (!_number.isEmpty()) {
        _number = "("+_number+")";
    }
    int ret;
    ret = QMessageBox::question(this,
                                tr("XiVO Client - Removing %1 %2").arg(name()).arg(_number),
                                tr("Removing %1 %2.\n"
                                   "Are you sure ?").arg(name()).arg(_number),
                                QMessageBox::Yes | QMessageBox::Cancel,
                                QMessageBox::Cancel);
    if (ret == QMessageBox::Yes) {
        emit removeFromPanel();
    }
}

/** rename the element
 */
void BasePeerWidget::rename()
{
    // open a prompt to change the name
    bool ok = false;
    QString text = QInputDialog::getText(this, tr("Rename Item"), tr("Rename %1 :").arg(name()),
                                         QLineEdit::Normal, name(), &ok);
    if (ok && !text.isEmpty()) {
        setName(text);
        // save the name
        QSettings *settings = b_engine->getSettings();
        settings->beginGroup("renamed_items");
        settings->setValue(id(), text);
        settings->endGroup();
    }
}

/** Return a unique id for the item
 */
QString BasePeerWidget::id() const
{
    if (m_ui_remote) {
        return (QString("userid-") + m_ui_remote->xid());
    } else {
        return (QString("number-") + number());
    }
}

/*!
 * Returns the name of the parent Xlet
 */
QString BasePeerWidget::xletName() const
{
    return parentWidget()->metaObject()->className();
}

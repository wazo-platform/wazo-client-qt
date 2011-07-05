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
#include <QApplication>
#include <QAction>
#include <QMenu>
#include <QMessageBox>
#include <QInputDialog>
#include <QSettings>

#include "basepeerwidget.h"
#include "baseengine.h"
#include "xivoconsts.h"

/*! \brief Constructor
 *
 * initialize members.
 */
BasePeerWidget::BasePeerWidget(const UserInfo * ui)
    : m_ui_remote(ui), m_editable(false)
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
    if (b_engine->enabledFunction("switchboard")) {
        m_maxWidthWanted = b_engine->getGuiOptions("merged_gui").value("maxwidthwanted").toInt();
        if (m_maxWidthWanted < 50) {
            m_maxWidthWanted = 200;
        }
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

/*! \brief call this peer
 */
void BasePeerWidget::dial()
{
    // qDebug() << Q_FUNC_INFO << m_ui_remote->xid() << sender();
    if (m_ui_remote) {
        b_engine->actionCall("dial",
                             "user:special:me",
                             QString("user:%1").arg(m_ui_remote->xid()));
    } else {
        b_engine->actionCall("dial",
                             "user:special:me",
                             "ext:" + m_number);
    }
}

void BasePeerWidget::dialMobilePhone()
{
    b_engine->actionCall("dial",
                         "user:special:me",
                         "ext:" + m_ui_remote->mobileNumber());
}

/*! \brief make this peer call the number
 */
void BasePeerWidget::peerdial()
{
    if (m_ui_remote) {
        b_engine->actionCall("originate",
                             QString("user:%1").arg(m_ui_remote->xid()),
                             "ext:" + sender()->property("number").toString());
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
                             "user:special:me",
                             QString("chan:%1").arg(xchannel));
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
    QString dst;
    if (m_ui_remote)
        dst = QString("user:%1").arg(m_ui_remote->xid());
    else
        dst = QString("ext:%1").arg(m_number);
    b_engine->actionCall("transfer", src, dst);
}

/*! \brief Indirect Transfer
 */
void BasePeerWidget::itransfer()
{
    QString xchannel = sender()->property("xchannel").toString();
    QString src = QString("chan:%1").arg(xchannel);
    QString dst;
    if (m_ui_remote)
        dst = QString("user:%1").arg(m_ui_remote->xid());
    else
        dst = QString("ext:%1").arg(m_number);
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

/*! \brief park the call
 */
void BasePeerWidget::parkcall()
{
    if (m_ui_remote) {
        QString xchannel = sender()->property("xchannel").toString();
        b_engine->actionCall("park",
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
                             "voicemail:" + m_ui_remote->xid());
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
            b_engine->actionCall("originate",
                                 "user:special:me",
                                 QString("ext:%1").arg(m_ui_remote->mobileNumber()));
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
        }
        if (! m_ui_remote->mobileNumber().isEmpty())
            numbers.append(m_ui_remote->mobileNumber());
        emit selectedNumber(numbers);
    }
}

/*! \brief start drag if necessary
 */
void BasePeerWidget::mouseMoveEvent(QMouseEvent *event)
{
    // TODO : check for the right to drag and drop
    if (!(event->buttons() & Qt::LeftButton)) {
        return;
    }
    if ((event->pos() - m_dragstartpos).manhattanLength()
        < QApplication::startDragDistance()) {
        return;
    }

    if (! b_engine->enabledFunction("switchboard")) {
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
    //qDebug() << Q_FUNC_INFO << "dropAction=" << dropAction;
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

/*! \brief open a context menu
 */
void BasePeerWidget::contextMenuEvent(QContextMenuEvent *event)
{
    if (m_ui_remote == NULL)
        return;
    if (m_ui_local == NULL)
        return;
    bool isitme = false;
    if (m_ui_local == m_ui_remote)
        isitme = true;

    QList<const ChannelInfo *> qlci_local = loopOverChannels(m_ui_local);
    QList<const ChannelInfo *> qlci_remote = loopOverChannels(m_ui_remote);
    qDebug() << Q_FUNC_INFO << b_engine->getCapasIpbxCommands()
             << qlci_local.count() << qlci_remote.count() << isitme;

    // Construct and display the context menu
    QMenu contextMenu(this);
    QAction * action;
    QMenu *interceptMenu = NULL;
    QMenu *hangupMenu = NULL;
    QMenu *transferMenu = NULL;
    QMenu *itransferMenu = NULL;
    QMenu *vmtransferMenu = NULL;
    QMenu *parkMenu = NULL;

    if (parentWidget()->metaObject()->className() == QString("XletSwitchBoard")) {
        contextMenu.addAction(m_removeAction);
        contextMenu.addAction(m_renameAction);
        contextMenu.addSeparator();
    }
    // allow to dial everyone except me !
    if (! isitme) {
        action = new QAction("",this);
        action->setStatusTip(tr("Call this peer"));

        if (metaObject()->className() == QString("PeerWidget")) {
            if (static_cast<PeerWidget*>(this)->pOverMobileLbl(event->pos())) {
                action->setText(tr("&Call mobile"));
                connect(action, SIGNAL(triggered()),
                        this, SLOT(dialMobilePhone()));
            }
        }
        if (action->text() == "") {
            action->setText(tr("&Call"));
            connect(action, SIGNAL(triggered()),
                    this, SLOT(dial()));
        }
        contextMenu.addAction(action);
    }
    if (m_editable) {
        contextMenu.addAction(tr("&Edit"), this, SLOT(edit()));
    }

    if ((!m_ui_remote->ctilogin().isEmpty()) &&
        (b_engine->enabledFunction("chitchat"))) {
        if (b_engine->getFullId() != m_ui_remote->xid()) {
            contextMenu.addAction(m_chitchatAction);
        }
    }

    struct {
        static void aQActionMenu(QMenu *parent,
                                 const QString &text,
                                 const QString &statusTip,
                                 const QVariant &thisXChannel,
                                 QObject *on,
                                 const char *ASLOT)
        {
            QAction *action = new QAction(parent);

            action->setText(text);
            action->setStatusTip(statusTip);
            action->setProperty("xchannel", thisXChannel);
            QObject::connect(action, SIGNAL(triggered()),
                             on, ASLOT);
            parent->addAction(action);
        }
    } build;

    //qDebug() << m_ui_remote->phonelist();
    // TODO : upgrade this when several phones per user will be supported
    // or at least check it's working as expected

    foreach(const ChannelInfo * channelinfo, qlci_local) {
        const QString status = channelinfo->commstatus();
        const QString text = channelinfo->peerdisplay();
        const QString xchannel = channelinfo->xid();

        /* hanging up others communication doesn't make much sense
         * excepting in test environment or in special cases. */
        if (isitme) {
            if ((status == CHAN_STATUS_LINKED_CALLER) ||
                (status == CHAN_STATUS_LINKED_CALLED)) {

                if (!hangupMenu && (qlci_local.count() > 1))
                    hangupMenu = new QMenu(tr("&Hangup"), &contextMenu);

                build.aQActionMenu((hangupMenu) ? hangupMenu : &contextMenu,
                                   qlci_local.count() > 1 ? text : tr("&Hangup"),
                                   tr("Hangup this communication"),
                                   xchannel,
                                   this, SLOT(hangup()));
            }
            /* Parking doesn't make much sense here : people usually park their
             * correspondants, not someone random on the switchboard */
            if ((status == CHAN_STATUS_RINGING) ||
                (status == CHAN_STATUS_LINKED_CALLER) ||
                (status == CHAN_STATUS_LINKED_CALLED)) {

                if (!parkMenu && (qlci_local.count() > 1))
                    parkMenu = new QMenu(tr("&Park"), &contextMenu);

                build.aQActionMenu(parkMenu?parkMenu:&contextMenu,
                                   (qlci_local.count() > 1) ? text : tr("&Park"),
                                   tr("Park this call"),
                                   xchannel,
                                   this, SLOT(parkcall()));
            }
        } else {
            if ( ((status == CHAN_STATUS_RINGING) ||
                  (status == CHAN_STATUS_LINKED_CALLER) ||
                  (status == CHAN_STATUS_LINKED_CALLED)) &&
                 (! channelinfo->isparked()) &&
                 b_engine->getCapasIpbxCommands().contains("intercept")) {

                if (!interceptMenu && (qlci_local.count() > 1))
                    interceptMenu = new QMenu(tr("&Intercept"), &contextMenu);

                build.aQActionMenu((interceptMenu)?interceptMenu:&contextMenu,
                                   qlci_local.count() > 1 ? text : tr("&Intercept"),
                                   tr("Intercept this communication"),
                                   xchannel,
                                   this, SLOT(intercept()));
            }
        }
    }

    // get "my" currently open channels
    //qDebug() << m_ui_remote->xid() << ui;
    if (! isitme) {
        foreach(const ChannelInfo * channelinfo, qlci_local) {
            const QString status = channelinfo->commstatus();
            const QString text = channelinfo->peerdisplay();
            const QString xchannel = channelinfo->xid();

            if (channelinfo->talkingto_kind() == QString("<meetme>")) {
                QAction *meetmeAction = new QAction(tr("Invite in meetme room %1")
                                                    .arg(channelinfo->talkingto_id()),
                                                    & contextMenu);
                meetmeAction->setProperty("number", channelinfo->talkingto_id());
                connect(meetmeAction, SIGNAL(triggered()),
                        this, SLOT(peerdial()));
                contextMenu.addAction(meetmeAction);
            } else {
                if (!transferMenu && (qlci_local.count() > 1))
                    transferMenu = new QMenu(tr("Direct &Transfer"), &contextMenu);
                QAction *transferAction;
                if (transferMenu) {
                    transferAction = new QAction(text, transferMenu);
                    transferAction->setStatusTip(tr("Transfer this communication"));
                } else {
                    transferAction = new QAction(tr("Direct &Transfer"), &contextMenu);
                    transferAction->setStatusTip(tr("Transfer to this person"));
                }
                transferAction->setProperty("xchannel", xchannel);
                connect(transferAction, SIGNAL(triggered()),
                        this, SLOT(transfer()));
                if (transferMenu) {
                    transferMenu->addAction(transferAction);
                } else {
                    contextMenu.addAction(transferAction);
                }
            }

            if (channelinfo->talkingto_kind() != QString("<meetme>")) {
                if (!itransferMenu && qlci_local.count() > 1) {
                    itransferMenu = new QMenu(tr("&Indirect Transfer"), &contextMenu);
                }
                QAction * itransferAction;
                QAction * itransferCancelAction;
                if (itransferMenu) {
                    itransferAction = new QAction(text, itransferMenu);
                    itransferAction->setStatusTip(tr("Transfer this communication"));
                    itransferCancelAction = new QAction(text + " " + tr("(Cancel)"),
                                                        itransferMenu);
                    itransferCancelAction->setStatusTip(tr("Cancel the Transfer"));
                } else {
                    itransferAction = new QAction(tr("&Indirect Transfer"), &contextMenu);
                    itransferAction->setStatusTip(tr("Transfer to this person"));
                    itransferCancelAction = new QAction(tr("&Cancel the Transfer"), &contextMenu);
                    itransferCancelAction->setStatusTip(tr("Cancel the Transfer"));
                }
                itransferAction->setProperty("xchannel", xchannel);
                itransferCancelAction->setProperty("xchannel", xchannel);

                connect(itransferAction, SIGNAL(triggered()),
                        this, SLOT(itransfer()));
                connect(itransferCancelAction, SIGNAL(triggered()),
                        this, SLOT(itransfercancel()));

                if (itransferMenu) {
                    itransferMenu->addAction(itransferAction);
                    itransferMenu->addAction(itransferCancelAction);
                } else {
                    contextMenu.addAction(itransferAction);
                    contextMenu.addAction(itransferCancelAction);
                }
            }
            if (m_ui_remote && channelinfo->talkingto_kind() != QString("<meetme>")) {
                // TODO : check if this really has a Voice Mail
                if (!vmtransferMenu && (qlci_local.count() > 1))
                    vmtransferMenu = new QMenu(tr("Transfer to &voice mail"), &contextMenu);
                QAction *vmtransferAction;
                if (vmtransferMenu) {
                    vmtransferAction = new QAction(text, vmtransferMenu);
                    vmtransferAction->setStatusTip(tr("Transfer to voice mail"));
                } else {
                    vmtransferAction = new QAction(tr("Transfer to &voice mail"), &contextMenu);
                    vmtransferAction->setStatusTip(tr("Transfer to voice mail"));
                }
                vmtransferAction->setProperty("xchannel", xchannel);
                connect(vmtransferAction, SIGNAL(triggered()),
                        this, SLOT(vmtransfer()));
                if (vmtransferMenu) {
                    vmtransferMenu->addAction(vmtransferAction);
                } else {
                    contextMenu.addAction(vmtransferAction);
                }
            }
        }
    }

    // adding submenus to context menu
    foreach (QMenu *menu, ((QList <QMenu*>()) << interceptMenu
                           << hangupMenu
                           << parkMenu
                           << transferMenu
                           << itransferMenu
                           << vmtransferMenu)) {
        if (menu)
            contextMenu.addMenu(menu);
    }

    if (!contextMenu.isEmpty())
        contextMenu.exec(event->globalPos());
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
    // qDebug() << Q_FUNC_INFO << event->mimeData()->formats();
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
    //qDebug() << Q_FUNC_INFO << event->mimeData()->formats() << event->po
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
    qDebug() << Q_FUNC_INFO
             << event << event->keyboardModifiers()
             << event->mimeData() << event->proposedAction();

    if (event->mimeData()->hasFormat(CHANNEL_MIMETYPE))
        qDebug() << Q_FUNC_INFO << "CHANNEL_MIMETYPE";
    if (event->mimeData()->hasFormat(XUSERID_MIMETYPE))
        qDebug() << Q_FUNC_INFO << "XUSERID_MIMETYPE";
    if (event->mimeData()->hasFormat(XPHONEID_MIMETYPE))
        qDebug() << Q_FUNC_INFO << "XPHONEID_MIMETYPE";
    if (event->mimeData()->hasFormat(NUMBER_MIMETYPE))
        qDebug() << Q_FUNC_INFO << "NUMBER_MIMETYPE";

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
        qDebug() << Q_FUNC_INFO << "Unrecognized action" << event->proposedAction();
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
                                tr("Removing %1 %2.\nAre you sure ?").arg(name()).arg(_number),
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

bool BasePeerWidget::event(QEvent *e)
{
    if (!b_engine->enabledFunction("switchboard")) {
        if ((e->type() == QEvent::DragMove) ||
            (e->type() == QEvent::DragEnter) ||
            (e->type() == QEvent::DragLeave) ||
            (e->type() == QEvent::DragResponse )) {
            return 1;
        }
    }

    return QWidget::event(e);
}

/* XiVO Client
 * Copyright (C) 2007-2010, Proformatique
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
#include "baseengine.h"
#include "basepeerwidget.h"
#include "userinfo.h"
#include "phoneinfo.h"
#include "xivoconsts.h"

/*! \brief Constructor
 *
 * initialize members.
 */
BasePeerWidget::BasePeerWidget(UserInfo *ui)
    : m_ui(ui), m_editable(false)
{
    if(m_ui){
        setProperty("userid", m_ui->userid());

        m_chitchatAction = new QAction(tr("&Open a chat window"), this);
        m_chitchatAction->setStatusTip(tr("Open a chat window with this user"));
        m_chitchatAction->setProperty("userid", ui->userid());
        m_chitchatAction->setProperty("astid", ui->astid());
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
    
    m_dialAction = new QAction(tr("&Call"), this);
    m_dialAction->setStatusTip(tr("Call this peer"));
    connect(m_dialAction, SIGNAL(triggered()),
            this, SLOT(dial()));

    m_interceptAction = new QAction(tr("&Intercept"), this);
    m_interceptAction->setStatusTip(tr("Intercept call"));
    connect(m_dialAction, SIGNAL(triggered()),
            this, SLOT(intercept2()));
    

    m_maxWidthWanted = 200;
    if (b_engine->enabledFunction("switchboard")) {
        m_maxWidthWanted = b_engine->getGuiOptions("merged_gui").value("maxwidthwanted").toInt();
        if(m_maxWidthWanted < 50)
            m_maxWidthWanted = 200;
    }
    setMaximumWidth(m_maxWidthWanted);
    setAcceptDrops(true);

}

void BasePeerWidget::reloadSavedName()
{
    // retrieve saved name
    QSettings * settings = b_engine->getSettings();
    settings->beginGroup("renamed_items");
    QVariant value = settings->value(id());
    if(!value.isNull()) {
        setName(value.toString());
    }
    settings->endGroup();
}

/*! \brief call this peer
 */
void BasePeerWidget::dial()
{
    // qDebug() << "PeerWidget::dial()" << m_ui->userid() << sender();
    if(m_ui)
    {
        emit actionCall("originate", "user:special:me", "user:" + m_ui->userid());
    }
    else
    {
        emit actionCall("originate", "user:special:me", "ext:" + m_number);
    }
}

/*! \brief make this peer call the number
 */
void BasePeerWidget::peerdial()
{
    if(m_ui) {
        emit actionCall("originate",
                        "user:" + m_ui->userid(),
                        "ext:" + sender()->property("number").toString());
    } else {
        emit actionCall("originate",
                        "ext:" + m_number,
                        "ext:" + sender()->property("number").toString());
    }
}

/*! \brief hangup a channel
 *
 * uses "thischannel" property of the sender
 */
void BasePeerWidget::hangup()
{
    if(m_ui) {
        emit actionCall("hangup",
                        "chan:" + m_ui->userid() + ":" + sender()->property("thischannel").toString());
    }
}

/*! \brief intercept a call
 *
 * uses "peerchannel" property of the sender
 */
void BasePeerWidget::intercept()
{
    if(m_ui) {
        emit actionCall("transfer",
                        "chan:" + m_ui->userid() + ":" + sender()->property("peerchannel").toString(),
                        "user:special:me");
   }
}
 
/*! \brief intercept a call
 *
 * \todo finish...
 */
void BasePeerWidget::intercept2()
{
    if(m_ui) {
        qDebug() << "BasePeerWidget::intercept2()" << m_ui->userid();
    }
}

/*! \brief Direct transfer
 *
 * uses "thischannel" property of the sender
 */
void BasePeerWidget::transfer()
{
    const UserInfo * ui = b_engine->getXivoClientUser();
    if(m_ui) {
        emit actionCall("transfer",
                        "chan:" + ui->userid() + ":" + sender()->property("peerchannel").toString(),
                        "user:" + m_ui->userid());
    } else {
        emit actionCall("transfer",
                        "chan:" + ui->userid() + ":" + sender()->property("peerchannel").toString(),
                        "ext:" + m_number);
    }
}

/*! \brief Indirect Transfer
 */
void BasePeerWidget::itransfer()
{
    const UserInfo * ui = b_engine->getXivoClientUser();
    if(m_ui) {
        emit actionCall("atxfer",
                        "chan:" + ui->userid() + ":" + sender()->property("thischannel").toString(),
                        "user:" + m_ui->userid());
    } else {
        emit actionCall("atxfer",
                        "chan:" + ui->userid() + ":" + sender()->property("thischannel").toString(),
                        "ext:" + m_number);
    }
}

/*! \brief park the call
 */
void BasePeerWidget::parkcall()
{
    QString chan = sender()->property("thischannel").toString();
    if(m_ui) {
        emit actionCall("transfer",
                        "chan:" + m_ui->userid() + ":" + chan,
                        "ext:special:parkthecall");
    }
}

/*! \brief transfer to voice mail
 */
void BasePeerWidget::vmtransfer()
{
    if(m_ui) {
        const UserInfo * ui = b_engine->getXivoClientUser();
        emit actionCall("transfer",
                        "chan:" + ui->userid() + ":" + sender()->property("peerchannel").toString(),
                        "voicemail:" + m_ui->userid());
    }
}

/*! \brief handle double click
 *
 * dial or indirect transfer if left mouse button used
 */
void BasePeerWidget::mouseDoubleClickEvent(QMouseEvent * event)
{
    if(event->button() == Qt::LeftButton) {
        QWidget * w = childAt(event->pos());
        QString subwidgetkind;
        if(w)
            subwidgetkind = w->property("kind").toString();
        // check if we are in communication
        const UserInfo * ui = b_engine->getXivoClientUser();
        if(ui && !ui->phonelist().isEmpty()) {
            foreach(const QString phone, ui->phonelist()) {
                const PhoneInfo * pi = ui->getPhoneInfo(phone);
                const QMap<QString, QVariant> & comms = pi->comms();
                //qDebug() << pi->phoneid() << pi->comms();
                foreach(const QString ts, comms.keys()) {
                    const QMap<QString, QVariant> & comm = comms[ts].toMap();
                    //qDebug() << pi->phoneid() << ts << comm;
                    const QString status = comm["status"].toString();
                    if(status == CHAN_STATUS_LINKED_CALLER || status == CHAN_STATUS_LINKED_CALLED) {
                        QString to;
                        if(m_ui) {
                            to = "user:" + m_ui->userid();
                        } else {
                            to = "ext:" + m_number;
                        }
                        // Initiate an indirect transfer.
                        emit actionCall("atxfer",
                                        "chan:special:me:" + comm["thischannel"].toString(),
                                        to);
                        return;
                    }
                }
            }
        }
        // "I" have no current communications, intercept if the person is being called
        if(m_ui && !m_ui->phonelist().isEmpty()) {
            foreach(const QString phone, m_ui->phonelist()) {
                const PhoneInfo * pi = m_ui->getPhoneInfo(phone);
                const QMap<QString, QVariant> & comms = pi->comms();
                //qDebug() << pi->phoneid() << pi->comms();
                foreach(const QString ts, comms.keys()) {
                    const QMap<QString, QVariant> & comm = comms[ts].toMap();
                    //qDebug() << pi->phoneid() << ts << comm;
                    const QString status = comm["status"].toString();
                    if(status == CHAN_STATUS_RINGING) {
                        emit actionCall("transfer",
                                        "chan:" + m_ui->userid() + ":" + comm["peerchannel"].toString(),
                                        "user:special:me");
                        return;
                    }
                }
            }
        }
        if ((subwidgetkind == "mobile") && (m_ui != NULL)) {
            emit actionCall("originate",
                            "user:special:me",
                            QString("ext:%1").arg(m_ui->mobileNumber()));
        } else {
            // just dial the person
            dial();
        }
    }
}

/*! \brief handle mouse press
 *
 * store mouse position for further processing
 */
void BasePeerWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
        m_dragstartpos = event->pos();
}

/*! \brief start drag if necessary
 */
void BasePeerWidget::mouseMoveEvent(QMouseEvent *event)
{
    // TODO : check for the right to drag and drop
    if (!(event->buttons() & Qt::LeftButton))
        return;
    if ((event->pos() - m_dragstartpos).manhattanLength()
         < QApplication::startDragDistance())
        return;

    QDrag *drag = new QDrag(this);
    QMimeData *mimeData = new QMimeData;
    if(m_ui) {
        mimeData->setText(m_ui->phoneNumber());
        mimeData->setData(PEER_MIMETYPE, m_ui->userid().toAscii());
        mimeData->setData(USERID_MIMETYPE, m_ui->userid().toAscii());
        mimeData->setData(NAME_MIMETYPE, m_ui->fullname().toUtf8());
    } else {
        mimeData->setText(m_number);
        mimeData->setData(NUMBER_MIMETYPE, m_number.toAscii());
    }
    drag->setMimeData(mimeData);
        
    /*Qt::DropAction dropAction = */
    drag->start(Qt::CopyAction | Qt::MoveAction);
    //qDebug() << "PeerWidget::mouseMoveEvent : dropAction=" << dropAction;
}

/*! \brief open a context menu
 */
void BasePeerWidget::contextMenuEvent(QContextMenuEvent * event)
{
    const UserInfo *ui = b_engine->getXivoClientUser();
    // Construct and display the context menu
    QMenu contextMenu(this);
    if (parentWidget()->metaObject()->className() == QString("SwitchBoardWindow")) {
        contextMenu.addAction(m_removeAction);
        contextMenu.addAction(m_renameAction);
        contextMenu.addSeparator();
    }
    // allow to dial everyone except me !
    if(ui != m_ui)
        contextMenu.addAction(m_dialAction);
    if(m_editable)
        contextMenu.addAction(tr("&Edit"), this, SLOT(edit()));
    QMenu *interceptMenu = NULL;
    QMenu *hangupMenu = NULL;
    QMenu *transferMenu = NULL;
    QMenu *itransferMenu = NULL;
    QMenu *vmtransferMenu = NULL;
    QMenu *parkMenu = NULL;
    if(m_ui) {
        if(!m_ui->ctilogin().isEmpty()&&(b_engine->enabledFunction("chitchat"))) {
            if (b_engine->getFullId() != m_ui->userid())
                contextMenu.addAction(m_chitchatAction);
        }

        //qDebug() << m_ui->phonelist();
        // TODO : upgrade this when several phones per user will be supported
        // or at least check it's working as expected
        int commsCount = m_ui->commsCount();    // number of current comms
        foreach(const QString phone, m_ui->phonelist()) {
            const PhoneInfo * pi = m_ui->getPhoneInfo(phone);
            if(!pi)
                continue;
            int hintstatuscode = -1;
            if(!pi->hintstatus("code").isEmpty())
                hintstatuscode = pi->hintstatus("code").toInt();
            //qDebug() << "hintstatus code" << hintstatuscode;
            //qDebug() << "commsCount" << commsCount;
            //qDebug() << pi->phoneid() << pi->comms();
            //if((commsCount == 0) && (hintstatuscode & 8))
            //    contextMenu.addAction( m_interceptAction);
            const QMap<QString, QVariant> &comms = pi->comms();
            foreach(const QString ts, comms.keys()) {
                const QMap<QString, QVariant> & comm = comms[ts].toMap();
                qDebug() << "BasePeerWidget::contextMenuEvent" << pi->phoneid() << ts << comm;
                const QString status = comm["status"].toString();
                QString text = comm["calleridnum"].toString();
                if(comm.contains("calleridname") && comm["calleridname"] != comm["calleridnum"]) {
                    text.append(" : ");
                    text.append(comm["calleridname"].toString());
                }
                /* hanging up others communication doesn't make much sense
                 * excepting in test environment or in special cases. */
                if(m_ui == ui && (status == CHAN_STATUS_LINKED_CALLER || status == CHAN_STATUS_LINKED_CALLED)) {
                    if(!hangupMenu && commsCount > 1)
                        hangupMenu = new QMenu(tr("&Hangup"), &contextMenu);
                    QAction * hangupAction = new QAction(hangupMenu?hangupMenu:&contextMenu);
                    hangupAction->setText(commsCount > 1 ? text : tr("&Hangup"));
                    hangupAction->setStatusTip(tr("Hangup this communication"));
                    hangupAction->setProperty("thischannel", comm["thischannel"]);
                    hangupAction->setProperty("peerchannel", comm["peerchannel"]);
                    connect(hangupAction, SIGNAL(triggered()),
                             this, SLOT(hangup()));
                    if(hangupMenu)
                        hangupMenu->addAction(hangupAction);
                    else
                        contextMenu.addAction(hangupAction);
                }
                if((m_ui != ui) && 
                    ((status == CHAN_STATUS_RINGING) ||
                     (status == CHAN_STATUS_LINKED_CALLER) ||
                     (status == CHAN_STATUS_LINKED_CALLED)) &&
                     (comm["calleridnum"] != QString("<parked>")) &&
                     (comm["calleridname"] != QString("<parked>")) &&
                     (b_engine->enabledFunction("switchboard"))) {

                    if(!interceptMenu && commsCount > 1)
                        interceptMenu = new QMenu(tr("&Intercept"), &contextMenu);
                    QAction * interceptAction = new QAction(interceptMenu?interceptMenu:&contextMenu);
                    interceptAction->setText(commsCount > 1 ? text : tr("&Intercept"));
                    interceptAction->setStatusTip(tr("Intercept this communication"));
                    interceptAction->setProperty("thischannel", comm["thischannel"]);
                    interceptAction->setProperty("peerchannel", comm["peerchannel"]);
                    connect(interceptAction, SIGNAL(triggered()),
                             this, SLOT(intercept()));
                    if(interceptMenu)
                        interceptMenu->addAction(interceptAction);
                    else
                        contextMenu.addAction(interceptAction);
                }
                /* Parking doesn't make much sense here : people usually park their
                 * correspondants, not someone random on the switchboard */
                if((m_ui == ui) && ((status == CHAN_STATUS_RINGING) ||
                    (status == CHAN_STATUS_LINKED_CALLER) ||
                    (status == CHAN_STATUS_LINKED_CALLED))) {
                    if(!parkMenu && commsCount > 1)
                        parkMenu = new QMenu(tr("&Park"), &contextMenu);
                    QAction * parkAction = new QAction(parkMenu?parkMenu:&contextMenu);
                    parkAction->setText(commsCount > 1 ? text : tr("&Park"));
                    parkAction->setStatusTip(tr("Park this call"));
                    parkAction->setProperty("thischannel", comm["thischannel"]);
                    parkAction->setProperty("peerchannel", comm["peerchannel"]);
                    connect(parkAction, SIGNAL(triggered()),
                             this, SLOT(parkcall()));
                    if(parkMenu)
                        parkMenu->addAction(parkAction);
                    else
                        contextMenu.addAction(parkAction);
                }
            }
        }
    }
    // adding submenus to context menu
    if(interceptMenu)
        contextMenu.addMenu(interceptMenu);
    if(hangupMenu)
        contextMenu.addMenu(hangupMenu);
    if(parkMenu)
        contextMenu.addMenu(parkMenu);
    // get "my" currently open channels
    //qDebug() << m_ui->userid() << ui;
    if(ui && ui != m_ui) {       
        int commsCount = ui->commsCount();    // number of current comms
        foreach(const QString phone, ui->phonelist())
        {
            const PhoneInfo * pi = ui->getPhoneInfo(phone);
            if(!pi)
                continue;
            const QMap<QString, QVariant> & comms = pi->comms();
            //qDebug() << pi->phoneid() << pi->comms();
            foreach(const QString ts, comms.keys()) {
                const QMap<QString, QVariant> & comm = comms[ts].toMap();
                qDebug() << "BasePeerWidget::contextMenuEvent my comms : " << pi->phoneid() << ts << comm;
                const QString status = comm["status"].toString();
                QString calleridnum = comm["calleridnum"].toString();
                QString calleridname = calleridnum;
                QString text = calleridnum;

                if(comm.contains("calleridname") &&
                   comm["calleridname"] != calleridnum) {
                    calleridname = comm["calleridname"].toString();
                    text.append(" : ");
                    text.append(calleridname);
                }

                if(calleridname == QString("<meetme>")) {
                    QAction * meetmeAction = new QAction(tr("Invite in meetme room %1").arg(calleridnum), &contextMenu);
                    meetmeAction->setProperty("number", calleridnum);
                    connect(meetmeAction, SIGNAL(triggered()),
                             this, SLOT(peerdial()));
                    contextMenu.addAction(meetmeAction);
                } else {
                    if(!transferMenu && commsCount > 1)
                        transferMenu = new QMenu(tr("Direct &Transfer"), &contextMenu);
                    QAction * transferAction;
                    if(transferMenu) {
                        transferAction = new QAction(text, transferMenu);
                        transferAction->setStatusTip(tr("Transfer this communication"));
                    } else {
                        transferAction = new QAction(tr("Direct &Transfer"), &contextMenu);
                        transferAction->setStatusTip(tr("Transfer to this person"));
                    }
                    transferAction->setProperty("thischannel", comm["thischannel"]);
                    transferAction->setProperty("peerchannel", comm["peerchannel"]);
                    connect(transferAction, SIGNAL(triggered()),
                             this, SLOT(transfer()));
                    if(transferMenu)
                        transferMenu->addAction(transferAction);
                    else
                        contextMenu.addAction(transferAction);
                }

                if(calleridname != QString("<meetme>")) {
                    if(!itransferMenu && commsCount > 1)
                        itransferMenu = new QMenu(tr("&Indirect Transfer"), &contextMenu);
                    QAction * itransferAction;
                    if(itransferMenu) {
                        itransferAction = new QAction(text, itransferMenu);
                        itransferAction->setStatusTip(tr("Transfer this communication"));
                    } else {
                        itransferAction = new QAction(tr("&Indirect Transfer"), &contextMenu);
                        itransferAction->setStatusTip(tr("Transfer to this person"));
                    }
                    itransferAction->setProperty("thischannel", comm["thischannel"]);
                    itransferAction->setProperty("peerchannel", comm["peerchannel"]);
                    connect(itransferAction, SIGNAL(triggered()),
                             this, SLOT(itransfer()));
                    if(itransferMenu)
                        itransferMenu->addAction(itransferAction);
                    else
                        contextMenu.addAction(itransferAction);
                }
                if(m_ui && calleridname != QString("<meetme>")) {
                    // TODO : check if this really has a Voice Mail
                    if(!vmtransferMenu && commsCount > 1)
                        vmtransferMenu = new QMenu(tr("Transfer to &voice mail"), &contextMenu);
                    QAction * vmtransferAction;
                    if(vmtransferMenu) {
                        vmtransferAction = new QAction(text, vmtransferMenu);
                        vmtransferAction->setStatusTip(tr("Transfer to voice mail"));
                    } else {
                        vmtransferAction = new QAction(tr("Transfer to &voice mail"), &contextMenu);
                        vmtransferAction->setStatusTip(tr("Transfer to voice mail"));
                    }
                    vmtransferAction->setProperty("thischannel", comm["thischannel"]);
                    vmtransferAction->setProperty("peerchannel", comm["peerchannel"]);
                    connect(vmtransferAction, SIGNAL(triggered()),
                             this, SLOT(vmtransfer()));
                    if(vmtransferMenu)
                        vmtransferMenu->addAction(vmtransferAction);
                    else
                        contextMenu.addAction(vmtransferAction);
                }
            }
        }
    }
    // adding submenus to context menu
    if(transferMenu)
        contextMenu.addMenu(transferMenu);
    if(itransferMenu)
        contextMenu.addMenu(itransferMenu);
    if(vmtransferMenu)
        contextMenu.addMenu(vmtransferMenu);
    contextMenu.exec(event->globalPos());
}

/*!
 *
 * This method would be subclassed by subclasses
 * which don't have m_ui valid.
 */
QString BasePeerWidget::name() const
{
    if(m_ui) {
        return m_ui->fullname();
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
    // qDebug() << "PeerWidget::dragEnterEvent()" << event->mimeData()->formats();
    if(event->mimeData()->hasFormat(PEER_MIMETYPE)   ||
       event->mimeData()->hasFormat(NUMBER_MIMETYPE) ||
       event->mimeData()->hasFormat(CHANNEL_MIMETYPE)) {
        if(event->proposedAction() & (Qt::CopyAction|Qt::MoveAction))
            event->acceptProposedAction();
    }
}

/*! \brief drag move event
 *
 * filter based on the mimeType.
 */
void BasePeerWidget::dragMoveEvent(QDragMoveEvent *event)
{
    //qDebug() << "PeerWidget::dragMoveEvent()" << event->mimeData()->formats() << event->po
    event->accept(rect());
    /*if(  event->mimeData()->hasFormat(PEER_MIMETYPE)
      || event->mimeData()->hasFormat(CHANNEL_MIMETYPE))
      {*/
    if(event->proposedAction() & (Qt::CopyAction | Qt::MoveAction))
        event->acceptProposedAction();
    /*}*/
}

/*! \brief receive drop events
 *
 * initiate an originate or transfer
 */
void BasePeerWidget::dropEvent(QDropEvent *event)
{
    QString userid_from = QString::fromAscii(event->mimeData()->data(USERID_MIMETYPE));
    QString channel_from = QString::fromAscii(event->mimeData()->data(CHANNEL_MIMETYPE));
    QString to;
    if(m_ui)
        to = "user:" + m_ui->userid();
    else
        to = "ext:" + m_number;
    qDebug() << "PeerWidget::dropEvent()"
             << event << event->keyboardModifiers()
             << event->mimeData() << event->proposedAction();

    if(event->mimeData()->hasFormat(CHANNEL_MIMETYPE)) {
        qDebug() << "PeerWidget::dropEvent()" << "CHANNEL_MIMETYPE";
    } else if(event->mimeData()->hasFormat(PEER_MIMETYPE)) {
        qDebug() << "PeerWidget::dropEvent()" << "PEER_MIMETYPE";
    } else if(event->mimeData()->hasFormat(NUMBER_MIMETYPE)) {
        qDebug() << "PeerWidget::dropEvent()" << "NUMBER_MIMETYPE";
    }
        
    switch(event->proposedAction()) {
        case Qt::CopyAction:
            // transfer the call to the peer "to"
            if(event->mimeData()->hasFormat(CHANNEL_MIMETYPE)) {
                event->acceptProposedAction();
                actionCall("transfer", "chan:" + userid_from + ":" + channel_from, to); // Call

            } else if(event->mimeData()->hasFormat(PEER_MIMETYPE)) {
                event->acceptProposedAction();
                if (b_engine->enabledFunction("switchboard")) {
                    actionCall("originate", "user:" + userid_from, to); // Call
                }
            } else if(event->mimeData()->hasFormat(NUMBER_MIMETYPE)) {
                event->acceptProposedAction();
                actionCall("originate", to, "ext:" + event->mimeData()->text());
            }
            break;
        case Qt::MoveAction:
            // can be reached with the shift button
            event->acceptProposedAction();
            actionCall("atxfer", "chan:" + userid_from + ":" + channel_from, to); 
            break;
        default:
            qDebug() << "PeerWidget::dropEvent() Unrecognized action" << event->proposedAction();
            break;
    }
}

/** First ask the user and send the signal if the user accepts
 */
void BasePeerWidget::tryRemoveFromPanel()
{
    QString _number = number();
    if(!_number.isEmpty())
        _number = "("+_number+")";
    int ret;
    ret = QMessageBox::question(this,
                                tr("XiVO Client - Removing %1 %2").arg(name()).arg(_number),
                                tr("Removing %1 %2.\nAre you sure ?").arg(name()).arg(_number),
                                QMessageBox::Yes | QMessageBox::Cancel,
                                QMessageBox::Cancel);
    if(ret == QMessageBox::Yes) {
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
    if(ok && !text.isEmpty()) {
        setName(text);
        // save the name
        QSettings * settings = b_engine->getSettings();
        settings->beginGroup("renamed_items");
        settings->setValue(id(), text);
        settings->endGroup();
    }
}

/** Return a unique id for the item
 */
QString BasePeerWidget::id() const
{
    if(m_ui)
        return (QString("userid-") + m_ui->userid());
    else
        return (QString("number-") + number());
}


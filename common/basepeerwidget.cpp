/* XIVO CTI clients
 * Copyright (C) 2007-2009  Proformatique
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
#include <QApplication>
#include <QAction>
#include <QMenu>
#include "baseengine.h"
#include "basepeerwidget.h"
#include "userinfo.h"
#include "phoneinfo.h"
#include "xivoconsts.h"

/*! \brief Constructor
 *
 * initialize members.
 */
BasePeerWidget::BasePeerWidget(BaseEngine * engine, UserInfo * ui, const QVariant & /*options*/)
    : m_engine(engine), m_ui(ui), m_editable(false)
{
    //qDebug() << "BasePeerWidget() options :" << options;
    if(m_ui)
        setProperty( "userid", m_ui->userid() );
    m_removeAction = new QAction( tr("&Remove"), this);
    m_removeAction->setStatusTip( tr("Remove this peer from the panel") );
    connect( m_removeAction, SIGNAL(triggered()),
             this, SIGNAL(removeFromPanel()) );

    m_dialAction = new QAction( tr("&Call"), this);
    m_dialAction->setStatusTip( tr("Call this peer") );
    connect( m_dialAction, SIGNAL(triggered()),
             this, SLOT(dial()) );
    m_maxWidthWanted = m_engine->getGuiOptions("user").toMap()["maxwidthwanted"].toInt();
    if(m_maxWidthWanted < 50)
        m_maxWidthWanted = 100;
    setAcceptDrops(true);
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

/*! \brief hangup a channel
 *
 * uses "thischannel" property of the sender
 */
void BasePeerWidget::hangup()
{
    if(m_ui)
    {
        //qDebug() << "BasePeerWidget::hangup()" << m_ui->userid() << sender()->property("thischannel");
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
    if(m_ui)
    {
        //qDebug() << "BasePeerWidget::intercept()" << m_ui->userid() << sender()->property("thischannel");
        qDebug() << "BasePeerWidget::intercept()" << m_ui->userid() << sender()->property("peerchannel");
        emit actionCall("transfer",
                        "chan:" + m_ui->userid() + ":" + sender()->property("peerchannel").toString(),
                        "user:special:me");
   }
}

/*! \brief Direct transfer
 *
 * uses "thischannel" property of the sender
 */
void BasePeerWidget::transfer()
{
    const UserInfo * ui = m_engine->getXivoClientUser();
    if(m_ui)
    {
        qDebug() << "BasePeerWidget::transfer()" << m_ui->userid() << sender()->property("thischannel").toString();
        emit actionCall("transfer",
                        "chan:" + ui->userid() + ":" + sender()->property("peerchannel").toString(),
                        "user:" + m_ui->userid());
    }
}

/*! \brief Indirect Transfer
 */
void BasePeerWidget::itransfer()
{
    const UserInfo * ui = m_engine->getXivoClientUser();
    if(m_ui)
    {
        emit actionCall("atxfer",
                        "chan:" + ui->userid() + ":" + sender()->property("thischannel").toString(),
                        "user:" + m_ui->userid());
    }
}

/*! \brief park the call
 */
void BasePeerWidget::parkcall()
{
    QString chan = sender()->property("peerchannel").toString();
    //if( chan.isEmpty() )
    //    chan = sender()->property("thischannel").toString();
    emit actionCall("transfer",
                    "chan:" + m_ui->userid() + ":" + chan,
                    "ext:special:parkthecall");
}

/*! \brief transfer to voice mail
 */
void BasePeerWidget::vmtransfer()
{
    const UserInfo * ui = m_engine->getXivoClientUser();
    if(m_ui)
    {
        emit actionCall("transfer",
                        "chan:" + ui->userid() + ":" + sender()->property("thischannel").toString(),
                        "voicemail:" + m_ui->userid());
    }
}

/*! \brief handle double click
 *
 * dial if left mouse button used
 */
void BasePeerWidget::mouseDoubleClickEvent(QMouseEvent * event)
{
    if(event->button() == Qt::LeftButton)
        dial();
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
    if ( (event->pos() - m_dragstartpos).manhattanLength()
         < QApplication::startDragDistance())
        return;

    QDrag *drag = new QDrag(this);
    QMimeData *mimeData = new QMimeData;
    if( m_ui )
        {
            mimeData->setText(m_ui->phonenumber());
            mimeData->setData(PEER_MIMETYPE, m_ui->userid().toAscii());
            mimeData->setData(USERID_MIMETYPE, m_ui->userid().toAscii());
            mimeData->setData(NAME_MIMETYPE, m_ui->fullname().toUtf8());
        }
    else
        {
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
    // Construct and display the context menu
    QMenu contextMenu( this );
    contextMenu.addAction( m_dialAction );
    if( true )
        contextMenu.addAction( m_removeAction );
    if( m_editable )
        contextMenu.addAction( tr("&Edit"), this, SLOT(edit()) );
    if( m_ui )
    {
        QMenu * interceptMenu = NULL;
        QMenu * hangupMenu = NULL;
        QMenu * transferMenu = NULL;
        QMenu * itransferMenu = NULL;
        QMenu * vmtransferMenu = NULL;
        QMenu * parkMenu = NULL;
        //qDebug() << m_ui->phonelist();
        // TODO : upgrade this when several phones per user will be supported
        // or at least check it's working as expected
        foreach(const QString phone, m_ui->phonelist())
        {
            const PhoneInfo * pi = m_ui->getPhoneInfo( phone );
            const QMap<QString, QVariant> & comms = pi->comms();
            //qDebug() << pi->phoneid() << pi->comms();
            foreach(const QString ts, comms.keys())
            {
                const QMap<QString, QVariant> & comm = comms[ts].toMap();
                qDebug() << "BasePeerWidget::contextMenuEvent" << pi->phoneid() << ts << comm;
                const QString status = comm["status"].toString();
                QString text = comm["calleridnum"].toString();
                if( comm.contains("calleridname")
                    && comm["calleridname"] != comm["calleridnum"] )
                {
                    text.append( " : " );
                    text.append( comm["calleridname"].toString() );
                }
                if( status == CHAN_STATUS_LINKED_CALLER || status == CHAN_STATUS_LINKED_CALLED )
                {
                    if( !hangupMenu )
                        hangupMenu = new QMenu( tr("&Hangup"), &contextMenu );
                    QAction * hangupAction = new QAction( hangupMenu );
                    hangupAction->setText( text );
                    hangupAction->setStatusTip( tr("Hangup this communication") );
                    hangupAction->setProperty( "thischannel", comm["thischannel"] );
                    hangupAction->setProperty( "peerchannel", comm["peerchannel"] );
                    connect( hangupAction, SIGNAL(triggered()),
                             this, SLOT(hangup()) );
                    hangupMenu->addAction( hangupAction );
                }
                // TODO : intercept only if the status is right
                if( true ) //status != 
                {
                    if( !interceptMenu )
                        interceptMenu = new QMenu( tr("&Intercept"), &contextMenu );
                    QAction * interceptAction = new QAction( interceptMenu );
                    interceptAction->setText( text );
                    interceptAction->setStatusTip( tr("Intercept this communication") );
                    interceptAction->setProperty( "thischannel", comm["thischannel"] );
                    interceptAction->setProperty( "peerchannel", comm["peerchannel"] );
                    connect( interceptAction, SIGNAL(triggered()),
                             this, SLOT(intercept()) );
                    interceptMenu->addAction( interceptAction );
                }
                // TODO : check for correct status
                if( true )
                {
                    if( !parkMenu )
                        parkMenu = new QMenu( tr("&Park"), &contextMenu );
                    QAction * parkAction = new QAction( parkMenu );
                    parkAction->setText( text );
                    parkAction->setStatusTip( tr("Park this communication") );
                    parkAction->setProperty( "thischannel", comm["thischannel"] );
                    parkAction->setProperty( "peerchannel", comm["peerchannel"] );
                    connect( parkAction, SIGNAL(triggered()),
                             this, SLOT(parkcall()) );
                    parkMenu->addAction( parkAction );
                }
            }
        }
        // get "my" currently open channels
        const UserInfo * ui = m_engine->getXivoClientUser();
        //qDebug() << m_ui->userid() << ui;
        if( ui ) 
        {       
            foreach(const QString phone, ui->phonelist())
            {
                const PhoneInfo * pi = ui->getPhoneInfo( phone );
                const QMap<QString, QVariant> & comms = pi->comms();
                //qDebug() << pi->phoneid() << pi->comms();
                foreach(const QString ts, comms.keys())
                {
                    const QMap<QString, QVariant> & comm = comms[ts].toMap();
                    qDebug() << "BasePeerWidget::contextMenuEvent my comms : " << pi->phoneid() << ts << comm;
                    const QString status = comm["status"].toString();
                    QString text = comm["calleridnum"].toString();
                    if( comm.contains("calleridname")
                        && comm["calleridname"] != comm["calleridnum"] )
                    {
                        text.append( " : " );
                        text.append( comm["calleridname"].toString() );
                    }
                    if( true )
                    {
                        if( !transferMenu )
                            transferMenu = new QMenu( tr("Direct &Transfer"), &contextMenu );
                        QAction * transferAction = new QAction( text, transferMenu );
                        transferAction->setStatusTip( tr("Transfer this communication") );
                        transferAction->setProperty( "thischannel", comm["thischannel"] );
                        transferAction->setProperty( "peerchannel", comm["peerchannel"] );
                        connect( transferAction, SIGNAL(triggered()),
                                 this, SLOT(transfer()) );
                        transferMenu->addAction( transferAction );
                    }
                    if( true )
                    {
                        if( !itransferMenu )
                            itransferMenu = new QMenu( tr("&Indirect Transfer"), &contextMenu );
                        QAction * itransferAction = new QAction( text, itransferMenu );
                        itransferAction->setStatusTip( tr("Transfer this communication") );
                        itransferAction->setProperty( "thischannel", comm["thischannel"] );
                        itransferAction->setProperty( "peerchannel", comm["peerchannel"] );
                        connect( itransferAction, SIGNAL(triggered()),
                                 this, SLOT(itransfer()) );
                        itransferMenu->addAction( itransferAction );
                    }
                    if( true )
                    {
                        if( !vmtransferMenu )
                            vmtransferMenu = new QMenu( tr("Transfer to &voice mail"), &contextMenu );
                        QAction * vmtransferAction = new QAction( text, vmtransferMenu );
                        vmtransferAction->setProperty( "thischannel", comm["thischannel"] );
                        vmtransferAction->setProperty( "peerchannel", comm["peerchannel"] );
                        connect( vmtransferAction, SIGNAL(triggered()),
                                 this, SLOT(vmtransfer()) );
                        vmtransferMenu->addAction( vmtransferAction );
                    }
                }
            }
        }
        // adding submenus to context menu
        if( interceptMenu )
            contextMenu.addMenu( interceptMenu );
        if( hangupMenu )
            contextMenu.addMenu( hangupMenu );
        if( parkMenu )
            contextMenu.addMenu( parkMenu );
        if( transferMenu )
            contextMenu.addMenu( transferMenu );
        if( itransferMenu )
            contextMenu.addMenu( itransferMenu );
        if( vmtransferMenu )
            contextMenu.addMenu( vmtransferMenu );
    }
    contextMenu.exec( event->globalPos() );
}

QString BasePeerWidget::name() const
{
    if(m_ui)
        {
            return m_ui->fullname();
        }
    else
        {
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
    if(  event->mimeData()->hasFormat(PEER_MIMETYPE)
         || event->mimeData()->hasFormat(NUMBER_MIMETYPE)
         || event->mimeData()->hasFormat(CHANNEL_MIMETYPE) )
        {
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
      || event->mimeData()->hasFormat(CHANNEL_MIMETYPE) )
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
            actionCall("originate", "user:" + userid_from, to); // Call
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


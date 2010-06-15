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

#include "extendedtablewidget.h"
#include "xivoconsts.h"
#include "userinfo.h"
#include "phoneinfo.h"
#include "baseengine.h"

/*! \brief Constructor
 */
ExtendedTableWidget::ExtendedTableWidget(QWidget * parent)
    : QTableWidget(parent),
      m_re_number("\\+?[0-9\\s\\.]+"),
      m_editable(false)
{
    setAcceptDrops(true);
    setAlternatingRowColors(true);
    horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
}

/*! \brief Constructor
 */
ExtendedTableWidget::ExtendedTableWidget(int rows, int columns, QWidget * parent)
    : QTableWidget(rows, columns, parent), m_re_number("\\+?[0-9\\s\\.]+")
{
    setAcceptDrops(true);
    setAlternatingRowColors(true);
}

/*! \brief display the context Menu
 */
void ExtendedTableWidget::contextMenuEvent(QContextMenuEvent * event)
{
    qDebug() << "ExtendedTableWidget::contextMenuEvent()" << event;
    qDebug() << "   " << event->pos();
    QTableWidgetItem * item = itemAt( event->pos() );
    if(item)
    {
        event->accept();
        QMenu contextMenu( this );
        if( m_editable )
        {
            QAction * removeAction
                = contextMenu.addAction( tr("&Remove"), this, SLOT(remove()));
            removeAction->setProperty("rowNumber", row(item));
        }   
        if( m_re_number.exactMatch( item->text() ) )
        {
            qDebug() << "phone number detection :" << item->text();
            QAction * dialAction
                = contextMenu.addAction( tr("&Dial"), this, SLOT(dialNumber()) );
            dialAction->setProperty("number", item->text());
            QMenu * transferMenu = new QMenu(tr("Direct &Transfer"), &contextMenu);
            QMenu * indirectTransferMenu = new QMenu(tr("&Indirect Transfer"), &contextMenu);
            UserInfo * ui = NULL;
            if(b_engine)
                ui = b_engine->getXivoClientUser();
            if(ui)
            {
                foreach( const QString phone, ui->phonelist() )
                {
                    const PhoneInfo * pi = ui->getPhoneInfo( phone );
                    if( pi )
                    {
                        QMapIterator<QString, QVariant> it( pi->comms() );
                        while( it.hasNext() )
                        {
                            it.next();
                            QMap<QString, QVariant> call = it.value().toMap();
                            // Add the transfer entry with the callerid name and num
                            QString text;
                            if( call.contains("calleridname") )
                            {
                                text.append( call["calleridname"].toString() );
                                text.append(" : ");
                            }
                            text.append( call["calleridnum"].toString() );
                            QAction * transferAction =
                                transferMenu->addAction( text,
                                                         this, SLOT(dtransfer()) );
                            transferAction->setProperty( "chan", call["peerchannel"] );
                            transferAction->setProperty("number", item->text());
                            QAction * indirectTransferAction =
                                indirectTransferMenu->addAction( text,
                                                                 this, SLOT(itransfer()) );
                            indirectTransferAction->setProperty("chan", call["thischannel"] );
                            indirectTransferAction->setProperty("number", item->text());
                        }
                    }
                }
            }
            if( !transferMenu->isEmpty() )
                contextMenu.addMenu(transferMenu);
            if( !indirectTransferMenu->isEmpty() )
                contextMenu.addMenu(indirectTransferMenu);
        }
        else if(item->text().contains("@"))
        {
            // this is an email address
            qDebug() << "email addr detection :" << item->text();
            QAction * sendMailAction
                = contextMenu.addAction( tr("Send an E-mail"),
                                         this, SLOT(sendMail()) );
            sendMailAction->setProperty("email", item->text());
        }
        if(!contextMenu.isEmpty())
            contextMenu.exec( event->globalPos() );
    }
    else
    {
        event->ignore();
    }
}

void ExtendedTableWidget::mouseMoveEvent(QMouseEvent * event)
{
    // qDebug() << "ExtendedTableWidget::mouseMoveEvent()" << event << event->pos();
    QTableWidgetItem * item = itemAt( event->pos() );
    if(item) {
        QDrag *drag = new QDrag(this);
        QMimeData *mimeData = new QMimeData;
        mimeData->setText(item->text());
        mimeData->setData(NUMBER_MIMETYPE, ""); // ??? 
        drag->setMimeData(mimeData);
        drag->start(Qt::CopyAction | Qt::MoveAction);
    }
}

/*! \brief filter drag events
 */
void ExtendedTableWidget::dragEnterEvent(QDragEnterEvent *event)
{
    // qDebug() << "ExtendedTableWidget::dragEnterEvent" << event->mimeData()->formats() << event->pos();
    if(  event->mimeData()->hasFormat(PEER_MIMETYPE) ||
         event->mimeData()->hasFormat(NUMBER_MIMETYPE) ||
         event->mimeData()->hasFormat(CHANNEL_MIMETYPE) ) {
        event->acceptProposedAction();
    }
}

/*! \brief filter drag move events
 *
 * Detect if the move is over a cell containing a phone number.
 */
void ExtendedTableWidget::dragMoveEvent(QDragMoveEvent *event)
{
    // qDebug() << "ExtendedTableWidget::dragMoveEvent()" << event->pos();
    if(event->proposedAction() & ( Qt::CopyAction | Qt::MoveAction ))
        event->acceptProposedAction();
    QTableWidgetItem * item = itemAt( event->pos() );
    if(item) {
        if(m_re_number.exactMatch( item->text() ))
            event->accept(visualItemRect(item));
        else
            event->ignore(visualItemRect(item));
    } else
        event->ignore();
}

/*! \brief receive drop event
 */
void ExtendedTableWidget::dropEvent(QDropEvent *event)
{
    // qDebug() << "ExtendedTableWidget::dropEvent()" << event->mimeData()->text() << event->pos();
    QTableWidgetItem * item = itemAt( event->pos() );
    if(item && m_re_number.exactMatch( item->text() )) {
        QString userid_from = QString::fromAscii(event->mimeData()->data(USERID_MIMETYPE));
        QString channel_from = QString::fromAscii(event->mimeData()->data(CHANNEL_MIMETYPE));
        if(event->mimeData()->hasFormat(CHANNEL_MIMETYPE)) {
            event->acceptProposedAction();
            emit actionCall("transfer",
                            "chan:" + userid_from + ":" + channel_from,
                            "ext:" + item->text());
        } else if(event->mimeData()->hasFormat(PEER_MIMETYPE)) {
            event->acceptProposedAction();
            emit actionCall("originate",
                            "user:" + userid_from,
                            "ext:" + item->text());
        } else {
            event->ignore();
        }
    }
    else
        event->ignore();
}

void ExtendedTableWidget::dialNumber()
{
    QString number = sender()->property("number").toString();
    if(!number.isEmpty())
    {
        emit actionCall("originate",
                        "user:special:me",
                        "ext:" + number);
    }
}

void ExtendedTableWidget::sendMail()
{
    QString email = sender()->property("email").toString();
    if(!email.isEmpty())
    {
        QDesktopServices::openUrl(QUrl("mailto:" + email));
    }
}

void ExtendedTableWidget::remove()
{
    int _row = sender()->property("rowNumber").toInt();
    removeRow(_row);
}

void ExtendedTableWidget::dtransfer()
{
    QString chan = sender()->property("chan").toString();
    QString number = sender()->property("number").toString();
    if( !chan.isEmpty() && !number.isEmpty() )
    {
        emit actionCall("transfer",
                        "chan:special:me:" + chan,
                        "ext:" + number); 
    }
}

void ExtendedTableWidget::itransfer()
{
    QString chan = sender()->property("chan").toString();
    QString number = sender()->property("number").toString();
    if( !chan.isEmpty() && !number.isEmpty() )
    {
        emit actionCall("atxfer",
                        "chan:special:me:" + chan,
                        "ext:" + number); 
    }
}

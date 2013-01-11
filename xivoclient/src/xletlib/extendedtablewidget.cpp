/* XiVO Client
 * Copyright (C) 2007-2013, Avencall
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
#include <QMessageBox>

#include <xivoconsts.h>
#include <channelinfo.h>
#include <userinfo.h>
#include <phoneinfo.h>
#include <baseengine.h>
#include <phonenumber.h>

#include "extendedtablewidget.h"

/*! \brief Constructor
 */
ExtendedTableWidget::ExtendedTableWidget(QWidget * parent)
    : QTableWidget(parent),
      m_editable(false)
{
    setAcceptDrops(true);
    setAlternatingRowColors(true);
    horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
}

/*! \brief Constructor
 */
ExtendedTableWidget::ExtendedTableWidget(int rows, int columns, QWidget * parent)
    : QTableWidget(rows, columns, parent)
{
    setAcceptDrops(true);
    setAlternatingRowColors(true);
}

/*! \brief display the context Menu
 */
void ExtendedTableWidget::contextMenuEvent(QContextMenuEvent * event)
{
    qDebug() << Q_FUNC_INFO << event;
    qDebug() << "   " << event->pos();
    QTableWidgetItem *item = itemAt(event->pos());
    QAction *action;
    if (item) {
        event->accept();
        QMenu contextMenu( this );
        if (m_editable) {
            action = contextMenu.addAction(tr("&Remove"), this, SLOT(remove()));
            action->setProperty("row", row(item));
        }
        if (PhoneNumber::phone_re().exactMatch(item->text())) {
            action = contextMenu.addAction(tr("&Dial"), this, SLOT(dialNumber()));
            action->setProperty("number", item->text());
        } else if(item->text().contains("@")) { // this is an email address
            action = contextMenu.addAction(tr("Send an E-mail"),
                                           this, SLOT(sendMail()) );
            action->setProperty("email", item->text());
        }
        if(!contextMenu.isEmpty()) {
            contextMenu.exec(event->globalPos());
        }
    } else {
        event->ignore();
    }
}

void ExtendedTableWidget::mouseMoveEvent(QMouseEvent * event)
{
    // qDebug() << Q_FUNC_INFO << event << event->pos();
    QTableWidgetItem *item = itemAt(event->pos());
    if (item) {
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
    // qDebug() << Q_FUNC_INFO << event->mimeData()->formats() << event->pos();
    if (event->mimeData()->hasFormat(XPHONEID_MIMETYPE) ||
        event->mimeData()->hasFormat(NUMBER_MIMETYPE) ||
        event->mimeData()->hasFormat(CHANNEL_MIMETYPE)) {
        event->acceptProposedAction();
    }
}

/*! \brief filter drag move events
 *
 * Detect if the move is over a cell containing a phone number.
 */
void ExtendedTableWidget::dragMoveEvent(QDragMoveEvent *event)
{
    // qDebug() << Q_FUNC_INFO << event->pos();
    if (event->proposedAction() & (Qt::CopyAction | Qt::MoveAction)) {
        event->acceptProposedAction();
    }
    QTableWidgetItem *item = itemAt(event->pos());
    if (item) {
        if (PhoneNumber::phone_re().exactMatch(item->text())) {
            event->accept(visualItemRect(item));
        } else {
            event->ignore(visualItemRect(item));
        }
    } else {
        event->ignore();
    }
}

/*! \brief receive drop event
 */
void ExtendedTableWidget::dropEvent(QDropEvent *event)
{
    // qDebug() << Q_FUNC_INFO << event->mimeData()->text() << event->pos();
    QTableWidgetItem *item = itemAt(event->pos());
    if ((item) && (PhoneNumber::phone_re().exactMatch(item->text()))) {
        QString userid_from = QString::fromAscii(event->mimeData()->data(XUSERID_MIMETYPE));
        QString channel_from = QString::fromAscii(event->mimeData()->data(CHANNEL_MIMETYPE));
        if (event->mimeData()->hasFormat(CHANNEL_MIMETYPE)) {
            event->acceptProposedAction();
            b_engine->actionCall("transfer",
                                 "chan:" + userid_from + ":" + channel_from,
                                 "ext:" + item->text());
        } else if (event->mimeData()->hasFormat(XPHONEID_MIMETYPE)) {
            event->acceptProposedAction();
            b_engine->actionCall("originate",
                                 "user:" + userid_from,
                                 "ext:" + item->text());
        } else {
            event->ignore();
        }
    } else {
        event->ignore();
    }
}

void ExtendedTableWidget::dialNumber()
{
    QString number = sender()->property("number").toString();
    if (! number.isEmpty()) {
        b_engine->actionDialNumber(number);
    }
}

void ExtendedTableWidget::sendMail()
{
    QString email = sender()->property("email").toString();
    if (!email.isEmpty()) {
        QDesktopServices::openUrl(QUrl("mailto:" + email));
    }
}

void ExtendedTableWidget::remove()
{
    int _row = sender()->property("row").toInt();
    int ret = QMessageBox::warning(this, tr("Removing this contact"),
                                   tr("Removing this contact.\n"
                                      "Are you sure ?"),
                                   QMessageBox::Yes|QMessageBox::No);

    if (ret == QMessageBox::Yes) {
        removeRow(_row);
    }
}

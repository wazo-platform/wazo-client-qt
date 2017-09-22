/*
 * Copyright 2007-2017 The Wazo Authors  (see the AUTHORS file)
 *
 * This program is free software: you can redistribute it and/or modify
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
 * This programm is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QDebug>
#include <QMessageBox>
#include <QHeaderView>
#include <QMenu>
#include <QAction>

#include <xivoconsts.h>
#include <storage/userinfo.h>
#include <storage/phoneinfo.h>
#include <baseengine.h>
#include <phonenumber.h>

#include "extendedtablewidget.h"

ExtendedTableWidget::ExtendedTableWidget(QWidget * parent)
    : QTableWidget(parent)
{
    setAlternatingRowColors(true);
    horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);

    connect(horizontalHeader(),
            SIGNAL(sortIndicatorChanged(int, Qt::SortOrder)),
            SLOT(emitColumnSorted(int, Qt::SortOrder)));
}

void ExtendedTableWidget::contextMenuEvent(QContextMenuEvent * event)
{
    QTableWidgetItem *item = itemAt(event->pos());
    QAction *action;
    if (item) {
        event->accept();
        QMenu contextMenu( this );
        if (PhoneNumber::phone_re().exactMatch(item->text())) {
            action = contextMenu.addAction(tr("&Dial"), this, SLOT(dialNumber()));
            action->setProperty("number", item->text());
        } else if(item->text().contains("@")) {
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

void ExtendedTableWidget::dialNumber()
{
    QString number = sender()->property("number").toString();
    if (! number.isEmpty()) {
        b_engine->actionDial(number);
    }
}

void ExtendedTableWidget::sendMail()
{
    QString email = sender()->property("email").toString();
    if (!email.isEmpty()) {
        QDesktopServices::openUrl(QUrl("mailto:" + email));
    }
}

void ExtendedTableWidget::emitColumnSorted(int column, Qt::SortOrder order)
{
    emit columnSorted(column, order);
}

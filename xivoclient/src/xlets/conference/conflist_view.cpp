/* XiVO Client
 * Copyright (C) 2007-2014 Avencall
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

#include <QAction>
#include <QTimer>
#include <QHeaderView>
#include <QMenu>

#include "conflist_view.h"
#include "conflist_model.h"

ConfListView::ConfListView(QWidget *parent)
    : AbstractTableView(parent)
{
    setStyleSheet("ConfListView {"
                    "border: none;"
                    "background: transparent;"
                    "color:black;"
                  "}");

    connect(this, SIGNAL(clicked(const QModelIndex &)),
            this, SLOT(onViewClick(const QModelIndex &)));
}

void ConfListView::onViewClick(const QModelIndex &model)
{
    QString number = model.sibling(model.row(), ConfListModel::ID).data().toString();
    QString roomName = model.sibling(model.row(), ConfListModel::NAME).data().toString();
    QString roomNumber = model.sibling(model.row(), ConfListModel::NUMBER).data().toString();


    if (number != "") {
        b_engine->pasteToDial(roomNumber);
        QTimer *timer = new QTimer(this);
        timer->setSingleShot(true);
        timer->setProperty("number", number);
        connect(timer, SIGNAL(timeout()), parentWidget(), SLOT(openConfRoom()));
        timer->start(10);
    }
}


void ConfListView::contextMenuEvent(QContextMenuEvent * event)
{
    const QModelIndex &index = indexAt(event->pos());

    QString number = index.sibling(index.row(), ConfListModel::ID).data().toString();
    QString roomName = index.sibling(index.row(), ConfListModel::NAME).data().toString();
    QString roomNumber = index.sibling(index.row(), ConfListModel::NUMBER).data().toString();

    QMenu *menu = new QMenu(this);

    QAction *action = new QAction(
        tr("Get in room %1 (%2)").arg(roomName).arg(roomNumber), menu);

    action->setProperty("number", number);
    connect(action, SIGNAL(triggered(bool)),
	    parentWidget(), SLOT(openConfRoom()));
    connect(action, SIGNAL(triggered(bool)),
	    parentWidget(), SLOT(phoneConfRoom()));

    menu->addAction(action);
    menu->exec(QCursor::pos());
}

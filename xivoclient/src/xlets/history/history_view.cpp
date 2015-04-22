/* XiVO Client
 * Copyright (C) 2007-2015 Avencall
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
#include <QMenu>
#include <QString>

#include <baseengine.h>

#include "history_model.h"
#include "history_view.h"

HistoryView::HistoryView(QWidget *parent)
    : AbstractTableView(parent)
{
    connect(this, SIGNAL(clicked(const QModelIndex &)),
            this, SLOT(onViewClick(const QModelIndex &)));
}

void HistoryView::contextMenuEvent(QContextMenuEvent * event)
{
    const QModelIndex &index = indexAt(event->pos());
    QString caller = index.sibling(index.row(), 0).data().toString();
    if (caller.isEmpty()) {
        return;
    }

    QMenu *menu = new QMenu(this);

    QAction *action = new QAction(tr("Call %1").arg(caller), menu);
    action->setProperty("num_to_call", caller);
    connect(action, SIGNAL(triggered(bool)),
            this, SLOT(callOnClick(bool)));

    menu->addAction(action);
    menu->exec(QCursor::pos());

}


void HistoryView::callOnClick(bool)
{
    QAction *calling_action = qobject_cast<QAction *>(sender());
    QString num_to_call = calling_action->property("num_to_call").toString();
    b_engine->actionDial(num_to_call);
}

void HistoryView::onViewClick(const QModelIndex &index)
{
    QString caller = index.sibling(index.row(), 0).data().toString();

    if (caller.isEmpty()) {
        return;
    }

    b_engine->pasteToDial(caller);
}


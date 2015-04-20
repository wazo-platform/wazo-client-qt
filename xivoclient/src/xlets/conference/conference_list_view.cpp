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
#include <QContextMenuEvent>
#include <QTimer>
#include <QMenu>

#include "conference_list_view.h"
#include "conference_list_model.h"

ConferenceListView::ConferenceListView(QWidget *parent)
    : AbstractTableView(parent)
{
    connect(this, SIGNAL(clicked(const QModelIndex &)),
            this, SLOT(onViewClick(const QModelIndex &)));
}

void ConferenceListView::onViewClick(const QModelIndex &model)
{
    QString room_name = model.sibling(model.row(), ConferenceListModel::NAME).data().toString();
    QString room_number = model.sibling(model.row(), ConferenceListModel::NUMBER).data().toString();

    if (room_number != "") {
        b_engine->pasteToDial(room_number);
        emit this->openConfRoom(room_number, room_name);
    }
}

void ConferenceListView::contextMenuEvent(QContextMenuEvent * event)
{
    const QModelIndex &index = indexAt(event->pos());

    m_room_name_clicked = index.sibling(index.row(), ConferenceListModel::NAME).data().toString();
    m_room_number_clicked = index.sibling(index.row(), ConferenceListModel::NUMBER).data().toString();

    QMenu *menu = new QMenu(this);
    QAction *action = new QAction(
        tr("Get in room %1 (%2)").arg(m_room_name_clicked).arg(m_room_number_clicked), menu);

    connect(action, SIGNAL(triggered()),
	        this, SLOT(getInRoom()));

    menu->addAction(action);
    menu->exec(QCursor::pos());
}

void ConferenceListView::getInRoom()
{
    b_engine->actionDial(m_room_number_clicked);
    emit this->openConfRoom(m_room_number_clicked, m_room_name_clicked);
}

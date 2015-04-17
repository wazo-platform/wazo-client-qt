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

#include "baseengine.h"
#include "conference_room_model.h"
#include "conference_room_view.h"

ConferenceRoomView::ConferenceRoomView(QWidget *parent)
    : AbstractTableView(parent)
{
    connect(this, SIGNAL(clicked(const QModelIndex &)),
            this, SLOT(onViewClick(const QModelIndex &)));

    QHeaderView *h = horizontalHeader();
    connect(h, SIGNAL(sectionClicked(int)),
            this, SLOT(sectionHeaderClicked(int)));
}

void ConferenceRoomView::updateHeadersView()
{
    horizontalHeader()->setSectionResizeMode(ConferenceRoomModel::ACTION_MUTE, QHeaderView::Fixed);
    setColumnWidth(ConferenceRoomModel::ACTION_MUTE, 32);
    hideColumn(0);
}

void ConferenceRoomView::sectionHeaderClicked(int index)
{
    if (index == ConferenceRoomModel::ACTION_MUTE) {
        setSortingEnabled(false);
        return ;
    }
    setSortingEnabled(true);
}

void ConferenceRoomView::onViewClick(const QModelIndex &index)
{
    if (index.column() == ConferenceRoomModel::ACTION_MUTE)
    {
        int row = index.row();
        ConferenceRoomModel *model = static_cast<ConferenceRoomModel *>(this->model());
        bool isMuted = model->isRowMuted(row);
        QString room_number = model->getRoomNumber();
        QString user_number = QString("%0").arg(model->userNumberFromRow(row));
        QString action = isMuted ? "MeetmeUnmute" : "MeetmeMute";
        QString param = QString("%0 %1").arg(room_number).arg(user_number);
        b_engine->meetmeAction(action, param);
    }
}

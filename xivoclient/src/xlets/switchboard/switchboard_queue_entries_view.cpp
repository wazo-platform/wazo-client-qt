/*
 * Copyright 2014-2017 The Wazo Authors  (see the AUTHORS file)
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

#include "switchboard_queue_entries_view.h"

#include <QApplication>

SwitchboardQueueEntriesView::SwitchboardQueueEntriesView(QWidget *parent)
    : QueueEntriesView(parent),
      m_end_of_double_click(QDateTime::currentDateTime()),
      m_longer_that_a_double_click(QApplication::doubleClickInterval() * 1.1)
{
    this->setSortingEnabled(false);
    this->setSelectionBehavior(QAbstractItemView::SelectRows);
    this->setSelectionMode(QAbstractItemView::SingleSelection);

    connect(this, SIGNAL(clicked(const QModelIndex &)),
            this, SLOT(trigger(const QModelIndex &)));
    connect(this, SIGNAL(doubleClicked(const QModelIndex &)),
            this, SLOT(trigger(const QModelIndex &)));
    connect(this, SIGNAL(activated(const QModelIndex &)),
            this, SLOT(trigger(const QModelIndex &)));
}

void SwitchboardQueueEntriesView::selectFirstRow()
{
    this->selectRow(0);
    this->setFocus();
}

SwitchboardQueueEntriesView::~SwitchboardQueueEntriesView()
{
}

bool SwitchboardQueueEntriesView::isADoubleClickHappening(const QDateTime &time) const
{
    return time < m_end_of_double_click;
}


void SwitchboardQueueEntriesView::trigger(const QModelIndex &index)
{
    const QDateTime &now = QDateTime::currentDateTime();

    if (isADoubleClickHappening(now)) {
        return;
    }

    m_end_of_double_click = now.addMSecs(m_longer_that_a_double_click);
    emit selected(index);
}

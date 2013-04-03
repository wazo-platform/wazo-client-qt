/* Copyright (C) 2007-2013, Avencall
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

#include "agent_list_view.h"

AgentListView::AgentListView(QWidget * parent)
    : QListView(parent), minimum_height_is_good(false)
{
}

AgentListView::~AgentListView()
{
}

void AgentListView::setModel(QAbstractItemModel * model)
{
    QListView::setModel(model);
    this->minimum_height_is_good = false;
}

void AgentListView::resizeEvent(QResizeEvent * event)
{
    this->minimum_height_is_good = false;
    QListView::resizeEvent(event);
}

void AgentListView::paintEvent(QPaintEvent * event)
{
    this->adjustMinimumHeightIfNecessary();
    QListView::paintEvent(event);
}

void AgentListView::rowsInserted(const QModelIndex & index, int start, int end)
{
    QListView::rowsInserted(index, start, end);
    this->minimum_height_is_good = false;
}

void AgentListView::rowsAboutToBeRemoved(const QModelIndex & index, int start, int end)
{
    QListView::rowsAboutToBeRemoved(index, start, end);
    this->minimum_height_is_good = false;
}

void AgentListView::adjustMinimumHeightIfNecessary()
{
    if (! this->minimum_height_is_good) {
        this->adjustMinimumHeight();
        this->minimum_height_is_good = true;
    }
}

void AgentListView::adjustMinimumHeight()
{
    this->setMinimumHeight(this->contentsSize().height()
                           + this->viewport_border_height
                           + this->spacing());
}

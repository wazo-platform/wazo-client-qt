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

#include <QHeaderView>
#include <QItemSelectionModel>

#include "people_entry_delegate.h"
#include "people_entry_view.h"
#include "people_enum.h"


PeopleEntryView::PeopleEntryView(QWidget *parent)
    : AbstractTableView(parent)
{
    this->viewport()->setAttribute(Qt::WA_Hover);
}

void PeopleEntryView::selectFirstRow()
{
    this->selectRow(0);
    this->setFocus();
}

void PeopleEntryView::keyPressEvent(QKeyEvent * event)
{
    if (event && (event->key() == Qt::Key_F5
                  || event->key() == Qt::Key_F4)) {
        emit activated(this->currentIndex());
    } else {
        AbstractTableView::keyPressEvent(event);
    }
}

void PeopleEntryView::updateColumnsDelegates(const QModelIndex &, int first, int last)
{
    for (int column_index = first ; column_index <= last ; column_index ++) {
        int column_type = this->model()->headerData(column_index, Qt::Horizontal, Qt::UserRole).toInt();
        switch (column_type) {
            case NAME: {
                PeopleEntryDotDelegate *delegate = new PeopleEntryDotDelegate(this);
                this->setItemDelegateForColumn(column_index, delegate);
                break;
            }
            case NUMBER: {
                PeopleEntryNumberDelegate *delegate = new PeopleEntryNumberDelegate(this);
                this->setItemDelegateForColumn(column_index, delegate);
                break;
            }
            case FAVORITE: {
                connect(this, SIGNAL(clicked(const QModelIndex &)),
                        this, SLOT(onViewClick(const QModelIndex &)), Qt::UniqueConnection);
                break;
            }
            case PERSONAL_CONTACT: {
                PeopleEntryPersonalContactDelegate *delegate = new PeopleEntryPersonalContactDelegate(this);
                this->setItemDelegateForColumn(column_index, delegate);
                connect(delegate, SIGNAL(editPersonalContactClicked(const QVariantMap &)),
                        this, SIGNAL(editPersonalContactClicked(const QVariantMap &)));
                connect(delegate, SIGNAL(deletePersonalContactClicked(const QVariantMap &)),
                        this, SIGNAL(deletePersonalContactClicked(const QVariantMap &)));
                break;
            }
        }
    }
}

void PeopleEntryView::updateColumnsVisibility(const QModelIndex &, int first, int last)
{
    for (int column_index = first ; column_index <= last ; column_index ++) {
        int column_type = this->model()->headerData(column_index, Qt::Horizontal, Qt::UserRole).toInt();
        switch (column_type) {
            case CALLABLE: {
                this->hideColumn(column_index);
                break;
            }
        }
    }
}

void PeopleEntryView::onViewClick(const QModelIndex &index)
{
    int column_type = this->model()->headerData(index.column(), Qt::Horizontal, Qt::UserRole).toInt();
    if (column_type == FAVORITE) {
        const QVariantMap &unique_source_entry_id = index.data(UNIQUE_SOURCE_ID_ROLE).toMap();
        emit favoriteToggled(unique_source_entry_id);
    }
}

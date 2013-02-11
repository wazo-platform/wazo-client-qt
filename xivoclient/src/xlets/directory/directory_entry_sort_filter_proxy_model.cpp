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

#include "directory_entry_model.h"
#include "directory_entry_sort_filter_proxy_model.h"

DirectoryEntrySortFilterProxyModel::DirectoryEntrySortFilterProxyModel(QObject *parent = NULL)
    : AbstractSortFilterProxyModel(parent)
{
    this->setDynamicSortFilter(true);
}

bool DirectoryEntrySortFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    if (m_filter.isEmpty()) {
        return false;
    }

    QModelIndex directory_entry_number_index = sourceModel()->index(sourceRow,
                                                                    DirectoryEntryModel::NUMBER,
                                                                    sourceParent);
    QString directory_entry_number = sourceModel()->data(directory_entry_number_index).toString();
    if (directory_entry_number.isEmpty()) {
        return false;
    }
    if (this->filterMatchesColumn(sourceRow, DirectoryEntryModel::NAME, sourceParent)
        || this->filterMatchesColumn(sourceRow, DirectoryEntryModel::NUMBER, sourceParent)) {
        return true;
    } else {
        return false;
    }
}

void DirectoryEntrySortFilterProxyModel::setFilter(const QString & filter)
{
    this->m_filter = filter;
    this->invalidateFilter();
}

bool DirectoryEntrySortFilterProxyModel::filterMatchesColumn(int sourceRow,
                                                             DirectoryEntryModel::Columns column,
                                                             const QModelIndex & sourceParent) const
{
    QModelIndex directory_entry_column_index = sourceModel()->index(sourceRow,
                                                                    column,
                                                                    sourceParent);
    QString value = sourceModel()->data(directory_entry_column_index).toString();

    return value.contains(this->m_filter, Qt::CaseInsensitive);
}

QString DirectoryEntrySortFilterProxyModel::getNumber(const QModelIndex &index)
{
    QModelIndex number_index = this->index(index.row(), DirectoryEntryModel::NUMBER);
    return this->data(number_index, Qt::DisplayRole).toString();
}

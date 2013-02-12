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

DirectoryEntrySortFilterProxyModel::DirectoryEntrySortFilterProxyModel(QObject *parent)
    : AbstractSortFilterProxyModel(parent)
{
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

    return this->filterMatchesEntry(sourceRow, sourceParent);
}

bool DirectoryEntrySortFilterProxyModel::lessThan(const QModelIndex &left,
                                                  const QModelIndex &right) const
{
    QVariant left_data = sourceModel()->data(left);
    QVariant right_data = sourceModel()->data(right);

    if (left_data.type() == QVariant::String) {
        const QString &left_string = left_data.toString();
        if (left_string.isEmpty()) {
            return false;
        }

        const QString &right_string = right_data.toString();
        if (right_string.isEmpty()) {
            return true;
        }

        return QString::localeAwareCompare(left_string, right_string) < 0;
    } else {
        return AbstractSortFilterProxyModel::lessThan(left, right);
    }
}

void DirectoryEntrySortFilterProxyModel::setFilter(const QString & filter)
{
    this->m_filter = filter;
    this->invalidateFilter();
}

bool DirectoryEntrySortFilterProxyModel::filterMatchesEntry(int sourceRow, const QModelIndex & sourceParent) const
{
    QModelIndex directory_entry_index = sourceModel()->index(sourceRow,
                                                             0,
                                                             sourceParent);

    QStringList search_list = sourceModel()->data(directory_entry_index,
                                                  Qt::UserRole).toStringList();

    bool empty = (search_list.filter(this->m_filter, Qt::CaseInsensitive).isEmpty());
    return !empty;
}

QString DirectoryEntrySortFilterProxyModel::getNumber(const QModelIndex &index)
{
    QModelIndex number_index = this->index(index.row(), DirectoryEntryModel::NUMBER);
    return this->data(number_index, Qt::DisplayRole).toString();
}

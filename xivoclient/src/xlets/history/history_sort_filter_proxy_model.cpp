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

#include "history_enum.h"
#include "history_sort_filter_proxy_model.h"

HistorySortFilterProxyModel::HistorySortFilterProxyModel(QObject *parent)
    : AbstractSortFilterProxyModel(parent)
{
}

void HistorySortFilterProxyModel::setFilterMode(HistoryMode mode)
{
    if (m_mode != mode) {
        m_mode = mode;
        invalidateFilter();
    }
}

bool HistorySortFilterProxyModel::filterAcceptsColumn(int source_column,
                                                      const QModelIndex &/*source_parent*/) const
{
    if (source_column == COL_DURATION && m_mode == MISSEDCALL) {
        return false;
    }
    return true;
}

bool HistorySortFilterProxyModel::filterAcceptsRow(int source_row,
                                                   const QModelIndex &source_parent) const
{
    if (m_mode == ALLCALL) {
        return true;
    }

    QModelIndex name_index = sourceModel()->index(source_row, COL_NAME, source_parent);
    QVariant mode = sourceModel()->data(name_index, Qt::UserRole);

    return mode == m_mode;
}

bool HistorySortFilterProxyModel::lessThan(const QModelIndex &left,
                                           const QModelIndex &right) const
{
    QVariant left_data = sourceModel()->data(left, Qt::UserRole);
    QVariant right_data = sourceModel()->data(right, Qt::UserRole);

    if (left.column() == COL_DATE) {
        return left_data < right_data;
    } else if (left.column() == COL_DURATION) {
        return left_data < right_data;
    } else {
        return AbstractSortFilterProxyModel::lessThan(left, right);
    }
}

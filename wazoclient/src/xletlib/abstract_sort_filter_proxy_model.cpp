/*
 * Copyright 2007-2017 The Wazo Authors  (see the AUTHORS file)
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

#include "abstract_sort_filter_proxy_model.h"

AbstractSortFilterProxyModel::AbstractSortFilterProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
    this->setSortCaseSensitivity(Qt::CaseInsensitive);
    this->setSortLocaleAware(true);
    this->setDynamicSortFilter(true);
}

AbstractSortFilterProxyModel::~AbstractSortFilterProxyModel()
{
}

bool AbstractSortFilterProxyModel::lessThan(const QModelIndex &left,
                                            const QModelIndex &right) const
{
    const QString &left_data = sourceModel()->data(left).toString();
    const QString &right_data = sourceModel()->data(right).toString();

    if (left_data != right_data) {
        return QSortFilterProxyModel::lessThan(left, right);
    }

    int left_row = left.row();
    int right_row = right.row();
    int col_count = sourceModel()->columnCount();

    for (int i = 0; i < col_count; i++) {
        const QModelIndex &next_left = left.sibling(left_row, i);
        const QModelIndex &next_right = right.sibling(right_row, i);

        const QString &next_left_data = sourceModel()->data(next_left).toString();
        const QString &next_right_data = sourceModel()->data(next_right).toString();

        if (next_left_data != next_right_data) {
            return QSortFilterProxyModel::lessThan(next_left, next_right);
        }
    }

    return QSortFilterProxyModel::lessThan(left, right);
}

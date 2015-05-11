/* XiVO Client
 * Copyright (C) 2015 Avencall
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


#include <baseengine.h>

#include "item_delegate.h"

ItemDelegate::ItemDelegate(QWidget *parent)
    : QStyledItemDelegate(parent)
{
}

void ItemDelegate::paint(QPainter *painter,
                           const QStyleOptionViewItem &option,
                           const QModelIndex &index) const
{
    this->drawBorder(painter, option);

    QStyleOptionViewItem opt = option;
    this->initStyleOption(&opt, index);

    opt.rect = this->marginsRemovedByColumn(option.rect, index.column());

    QStyledItemDelegate::paint(painter, opt, index);
}

void ItemDelegate::drawBorder(QPainter *painter, const QStyleOptionViewItem &opt) const
{
    painter->save();
    painter->setPen(QColor("#D7D2D0"));
    painter->drawLine(opt.rect.bottomLeft(), opt.rect.bottomRight());
    painter->restore();
}

QRect ItemDelegate::marginsRemovedByColumn(const QRect &rect, int column) const
{
    if (column == 0) {
        return rect.marginsRemoved(QMargins(30,0,0,0));
    } else {
        return rect.marginsRemoved(QMargins(10,0,0,0));
    }
}

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


#include "abstract_item_delegate.h"

AbstractItemDelegate::AbstractItemDelegate(QWidget *parent)
    : QStyledItemDelegate(parent)
{
}

void AbstractItemDelegate::drawBorder(QPainter *painter, const QStyleOptionViewItem &opt) const
{
    painter->save();
    painter->setPen(QColor("#D7D2D0"));
    painter->drawLine(opt.rect.bottomLeft(), opt.rect.bottomRight());
    painter->restore();
}

QRect AbstractItemDelegate::marginsRemovedByColumn(const QRect &rect, int column) const
{
    if (column == 0) {
        return rect.marginsRemoved(QMargins(30,0,0,1));
    } else {
        return rect.marginsRemoved(QMargins(10,0,0,1));
    }
}

QWidget *AbstractItemDelegate::parentWidget() const
{
    // the reinterpret_cast cannot fail in this context since the constructor
    // only accepts a QWidget *, no QObject *
    return reinterpret_cast<QWidget *>(this->parent());
}

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

#include <QPainter>

#include "people_entry_delegate.h"

QSize PeopleEntryDelegate::icon_size = QSize(8, 8);
int PeopleEntryDelegate::icon_text_spacing = 15;


PeopleEntryDelegate::PeopleEntryDelegate(QWidget *parent)
    : QStyledItemDelegate(parent)
{
}

QSize PeopleEntryDelegate::sizeHint(const QStyleOptionViewItem &option,
                            const QModelIndex &index) const
{
    if (! index.data(Qt::BackgroundRole).isNull()) {
        const QSize &original_size = QStyledItemDelegate::sizeHint(option, index);
        int new_width = original_size.width() + icon_size.width() + icon_text_spacing;
        return QSize(new_width, original_size.height());
    } else {
        return QStyledItemDelegate::sizeHint(option, index);
    }
}

void PeopleEntryDelegate::paint(QPainter *painter,
                        const QStyleOptionViewItem &option,
                        const QModelIndex &index) const
{
    if (! index.data(Qt::BackgroundRole).isNull()) {
        QString text = index.data().toString();
        QIcon dot = QIcon(":/images/dot.svg");
        QPixmap new_image = dot.pixmap(QSize(8, 8));

        QPainter tint_painter(&new_image);
        tint_painter.setCompositionMode(QPainter::CompositionMode_SourceAtop);
        tint_painter.fillRect(new_image.rect(), QColor(index.data(Qt::BackgroundRole).value<QColor>()));
        tint_painter.end();

        painter->save();
        painter->drawPixmap(option.rect.x(), option.rect.center().y() - new_image.height() / 2, new_image);
        QRect text_rect = QRect(option.rect.x() + 15, option.rect.y(), option.rect.width(), option.rect.height());
        painter->drawText(text_rect.left(),
                          text_rect.top(),
                          text_rect.width(),
                          text_rect.height(),
                          Qt::AlignVCenter|Qt::AlignLeft,
                          text);
        painter->restore();
    } else {
        QStyledItemDelegate::paint(painter, option, index);
    }
}

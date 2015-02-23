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

QSize PeopleEntryDotDelegate::icon_size = QSize(8, 8);
int PeopleEntryDotDelegate::icon_text_spacing = 7;


PeopleEntryDotDelegate::PeopleEntryDotDelegate(QWidget *parent)
    : QStyledItemDelegate(parent)
{
}

QSize PeopleEntryDotDelegate::sizeHint(const QStyleOptionViewItem &option,
                                       const QModelIndex &index) const
{
    if (index.data(Qt::BackgroundRole).isNull()) {
        return QStyledItemDelegate::sizeHint(option, index);
    }

    const QSize &original_size = QStyledItemDelegate::sizeHint(option, index);
    int new_width = original_size.width() + icon_size.width() + icon_text_spacing;
    return QSize(new_width, original_size.height());
}

void PeopleEntryDotDelegate::paint(QPainter *painter,
                                const QStyleOptionViewItem &option,
                                const QModelIndex &index) const
{
    if (index.data(Qt::BackgroundRole).isNull()) {
        QStyledItemDelegate::paint(painter, option, index);
        return;
    }

    QString text = index.data().toString();
    QIcon dot = QIcon(":/images/dot.svg");
    QPixmap tinted_image = dot.pixmap(QSize(8, 8));

    QPainter tint_painter(&tinted_image);
    tint_painter.setCompositionMode(QPainter::CompositionMode_SourceAtop);
    tint_painter.fillRect(tinted_image.rect(), QColor(index.data(Qt::BackgroundRole).value<QColor>()));
    tint_painter.end();

    painter->save();
    int text_width = option.fontMetrics.size(0, text).width();
    QPoint cell_center = option.rect.center();
    int content_width = text_width + icon_size.width() + icon_text_spacing;
    int icon_left = cell_center.x() - content_width / 2;
    int icon_top = cell_center.y() - tinted_image.height() / 2;
    painter->drawPixmap(icon_left, icon_top, tinted_image);

    int text_left = icon_left + icon_size.width() + icon_text_spacing;
    QRect text_rect = QRect(text_left, option.rect.y(), text_width, option.rect.height());
    painter->drawText(text_rect.left(),
                      text_rect.top(),
                      text_rect.width(),
                      text_rect.height(),
                      Qt::AlignVCenter|Qt::AlignLeft,
                      text);
    painter->restore();
}






QSize PeopleEntryAgentDelegate::icon_size = QSize(20, 20);


PeopleEntryAgentDelegate::PeopleEntryAgentDelegate(QWidget *parent)
    : QStyledItemDelegate(parent)
{
}
QSize PeopleEntryAgentDelegate::sizeHint(const QStyleOptionViewItem &option,
                                         const QModelIndex &index) const
{
    const QSize &original_size = QStyledItemDelegate::sizeHint(option, index);
    return QSize(icon_size.width(), original_size.height());
}

void PeopleEntryAgentDelegate::paint(QPainter *painter,
                                     const QStyleOptionViewItem &option,
                                     const QModelIndex &index) const
{
    QString image_path;
    QColor color;

    QString agent_status = index.data(Qt::UserRole).toString();
    if (agent_status == "logged_in") {
        image_path = ":/images/agent-on.svg";
        color = QColor("#9BC920");
    } else if (agent_status == "logged_out") {
        image_path = ":/images/agent-off.svg";
        color = QColor("#D13224");
    } else {
        return;
    }

    QIcon image = QIcon(image_path);
    QPixmap tinted_image = image.pixmap(icon_size);

    QPainter tint_painter(&tinted_image);
    tint_painter.setCompositionMode(QPainter::CompositionMode_SourceAtop);
    tint_painter.fillRect(tinted_image.rect(), color);
    tint_painter.end();

    painter->save();
    painter->drawPixmap(option.rect.center().x() - tinted_image.width() / 2,
                        option.rect.center().y() - tinted_image.height() / 2,
                        tinted_image);
    painter->restore();
}

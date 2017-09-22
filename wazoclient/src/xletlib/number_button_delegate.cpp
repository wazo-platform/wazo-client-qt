/*
 * Copyright 2015-2017 The Wazo Authors  (see the AUTHORS file)
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

#include <QEvent>
#include <QMouseEvent>
#include <QPainter>

#include <baseengine.h>

#include "number_button_delegate.h"

int NumberButtonDelegate::button_height = 30;
QMargins NumberButtonDelegate::button_margins = QMargins(10, 0, 10, 0);


NumberButtonDelegate::NumberButtonDelegate(QWidget *parent)
    : ItemDelegate(parent),
      pressed(false)
{
}

void NumberButtonDelegate::paint(QPainter *painter,
                                      const QStyleOptionViewItem &option,
                                      const QModelIndex &index) const
{
    if (index.data().isNull() || index.data().toString().isEmpty()) {
        ItemDelegate::paint(painter, option, index);
        return;
    }

    if(option.state & QStyle::State_MouseOver) {
        painter->save();
        QPainterPath path;
        QRect button_rect = this->contentsRect(option.rect);
        path.addRoundedRect(button_rect, 8, 8);
        if (this->pressed) {
            painter->fillPath(path, Qt::black);
        } else {
            painter->fillPath(path, QColor("#58524F"));
        }

        QString text = tr("CALL");
        QRect text_rect(button_rect);
        text_rect.translate(16, 0);
        painter->setPen(QColor("white"));
        painter->drawText(text_rect, Qt::AlignVCenter, text);

        painter->restore();

        ItemDelegate::drawBorder(painter, option);
        return;
    }

    ItemDelegate::paint(painter, option, index);
}

bool NumberButtonDelegate::editorEvent(QEvent *event,
                                            QAbstractItemModel *model,
                                            const QStyleOptionViewItem &option,
                                            const QModelIndex &index)
{
    if (index.data().isNull() || index.data().toString().isEmpty()) {
        return QStyledItemDelegate::editorEvent(event, model, option, index);
    }

    QMouseEvent *mouse_event = static_cast<QMouseEvent*>(event);
    if(event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouse_event = static_cast<QMouseEvent*>(event);
        if (this->contentsRect(option.rect).contains(mouse_event->pos())) {
            this->pressed = true;
        }
    } else if (event->type() == QEvent::MouseButtonRelease) {
        this->pressed = false;
        if (this->contentsRect(option.rect).contains(mouse_event->pos())) {
            QString extension = model->data(index, Qt::DisplayRole).toString();
            b_engine->actionDial(extension);
        }
    }
    return true;
}

QRect NumberButtonDelegate::contentsRect(const QRect &option_rect) const
{
    QRect result = option_rect.marginsRemoved(button_margins);
    result.setHeight(button_height);
    result.moveCenter(option_rect.center());
    return result;
}

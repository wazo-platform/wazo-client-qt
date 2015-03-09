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
#include <QEvent>

#include "people_entry_delegate.h"

QSize PeopleEntryAgentDelegate::icon_size = QSize(20, 20);
QSize PeopleEntryDotDelegate::icon_size = QSize(8, 8);
int PeopleEntryDotDelegate::icon_text_spacing = 7;
QMargins PeopleEntryDotDelegate::button_margins = QMargins(10, 0, 10, 0);


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

PeopleEntryNumberDelegate::PeopleEntryNumberDelegate(QWidget *parent)
    : PeopleEntryDotDelegate(parent),
      pressed(false)
{
}

void PeopleEntryNumberDelegate::paint(QPainter *painter,
                                      const QStyleOptionViewItem &option,
                                      const QModelIndex &index) const
{
    if (index.data().isNull()) {
        QStyledItemDelegate::paint(painter, option, index);
        return;
    }

    if(option.state & QStyle::State_MouseOver) {
        painter->save();
        QPainterPath path;
        QRect button_rect = QRect(option.rect).marginsRemoved(button_margins);
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
        return;
    }

    PeopleEntryDotDelegate::paint(painter, option, index);
}

bool PeopleEntryNumberDelegate::editorEvent(QEvent *event,
                                            QAbstractItemModel *model,
                                            const QStyleOptionViewItem &option,
                                            const QModelIndex &index)
{
    if (index.data().isNull()) {
        return QStyledItemDelegate::editorEvent(event, model, option, index);
    }

    if(event->type() == QEvent::MouseButtonPress) {
        this->pressed = true;
    }
    if (event->type() == QEvent::MouseButtonRelease) {
        this->pressed = false;
        emit clicked(model, index);
    }
    return true;
}

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

    QString agent_status = index.data(Qt::UserRole).toString();
    if (agent_status == "logged_in") {
        image_path = ":/images/agent-on.svg";
    } else if (agent_status == "logged_out") {
        image_path = ":/images/agent-off.svg";
    } else {
        return;
    }

    QPixmap image = QIcon(image_path).pixmap(icon_size);
    painter->save();
    painter->drawPixmap(option.rect.center().x() - image.width() / 2,
                        option.rect.center().y() - image.height() / 2,
                        image);
    painter->restore();
}

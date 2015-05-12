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

#include <QAbstractScrollArea>
#include <QEvent>
#include <QMenu>
#include <QMouseEvent>
#include <QPainter>

#include <xletlib/menu.h>

#include "people_entry_delegate.h"
#include "people_actions.h"

QSize PeopleEntryDotDelegate::icon_size = QSize(8, 8);
int PeopleEntryDotDelegate::icon_text_spacing = 7;

int PeopleEntryNumberDelegate::action_selector_width = 40;
int PeopleEntryNumberDelegate::button_height = 30;
QMargins PeopleEntryNumberDelegate::button_margins = QMargins(10, 0, 10, 0);


PeopleEntryDotDelegate::PeopleEntryDotDelegate(QWidget *parent)
    : ItemDelegate(parent)
{
}

QSize PeopleEntryDotDelegate::sizeHint(const QStyleOptionViewItem &option,
                                       const QModelIndex &index) const
{
    if (index.data(Qt::UserRole+1).isNull()) {
        return ItemDelegate::sizeHint(option, index);
    }

    const QSize &original_size = ItemDelegate::sizeHint(option, index);
    int new_width = original_size.width() + icon_size.width() + icon_text_spacing;
    return QSize(new_width, original_size.height());
}

void PeopleEntryDotDelegate::paint(QPainter *painter,
                                   const QStyleOptionViewItem &option,
                                   const QModelIndex &index) const
{
    if (index.data(Qt::UserRole+1).isNull()) {
        ItemDelegate::paint(painter, option, index);
        return;
    }
    QStyleOptionViewItem opt;

    opt.rect = ItemDelegate::marginsRemovedByColumn(option.rect, index.column());

    QString text = index.data().toString();
    QIcon dot = QIcon(":/images/dot.svg");
    QPixmap tinted_image = dot.pixmap(QSize(8, 8));

    QPainter tint_painter(&tinted_image);
    tint_painter.setCompositionMode(QPainter::CompositionMode_SourceAtop);
    tint_painter.fillRect(tinted_image.rect(), QColor(index.data(Qt::UserRole+1).value<QColor>()));
    tint_painter.end();

    painter->save();
    int text_width = option.fontMetrics.size(0, text).width();
    QPoint cell_center = option.rect.center();
    int content_width = text_width + icon_size.width() + icon_text_spacing;
    int icon_left = cell_center.x() - content_width / 2;
    int icon_top = cell_center.y() - tinted_image.height() / 2;
    painter->drawPixmap(icon_left, icon_top, tinted_image);
    //painter->drawPixmap(opt.rect, tinted_image);

    int text_left = icon_left + icon_size.width() + icon_text_spacing;
    QRect text_rect = QRect(text_left, option.rect.y(), text_width, option.rect.height());
    painter->drawText(text_rect.left(),
                      text_rect.top(),
                      text_rect.width(),
                      text_rect.height(),
                      Qt::AlignVCenter|Qt::AlignLeft,
                      text);
    painter->restore();

    opt.rect = opt.rect.marginsRemoved(QMargins(8,0,0,0));
    ItemDelegate::drawBorder(painter, option);
    //ItemDelegate::paint(painter, opt, index);
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

        QRect selector_rect = this->actionSelectorRect(option.rect);

        QRect separator_rect(selector_rect);
        separator_rect.setWidth(1);

        painter->fillRect(separator_rect, "grey");

        QRect arrow_image_rect;
        QSize arrow_image_size = QSize(9, 6);
        arrow_image_rect.setSize(arrow_image_size);
        arrow_image_rect.moveCenter(selector_rect.center());
        painter->drawPixmap(arrow_image_rect, QIcon(":/images/down-arrow-white.svg").pixmap(arrow_image_size));
        painter->restore();

        PeopleEntryDotDelegate::drawBorder(painter, option);
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
        return ItemDelegate::editorEvent(event, model, option, index);
    }

    if(event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouse_event = static_cast<QMouseEvent*>(event);
        if (this->contentsRect(option.rect).contains(mouse_event->pos())) {
            this->pressed = true;
        }
    }
    if (event->type() == QEvent::MouseButtonRelease) {
        this->pressed = false;

        QMouseEvent *mouse_event = static_cast<QMouseEvent*>(event);
        PeopleActions *people_actions = model->data(index, Qt::UserRole).value<PeopleActions*>();

        if (this->buttonRect(option.rect).contains(mouse_event->pos())) {
            people_actions->call();
        } else if (this->actionSelectorRect(option.rect).contains(mouse_event->pos())) {
            this->showContextMenu(option, people_actions);
        }
        delete people_actions;
    }
    return true;
}

QRect PeopleEntryNumberDelegate::contentsRect(const QRect &option_rect) const
{
    QRect result = option_rect.marginsRemoved(button_margins);
    result.setHeight(button_height);
    result.moveCenter(option_rect.center());
    return result;
}

QRect PeopleEntryNumberDelegate::buttonRect(const QRect &option_rect) const
{
    QRect rect = this->contentsRect(option_rect);
    rect.setRight(rect.right() - action_selector_width);
    return rect;
}

QRect PeopleEntryNumberDelegate::actionSelectorRect(const QRect &option_rect) const
{
    QRect rect = this->contentsRect(option_rect);
    rect.setLeft(rect.right() - action_selector_width);
    return rect;
}

void PeopleEntryNumberDelegate::showContextMenu(const QStyleOptionViewItem &option,
                                                PeopleActions *people_actions)
{
    QAbstractScrollArea *view = static_cast<QAbstractScrollArea*>(option.styleObject);
    if (! view) {
        return;
    }

    QPoint position = this->contentsRect(option.rect).bottomLeft();
    QPoint globalPosition = view->viewport()->mapToGlobal(position);

    Menu menu(view);
    this->fillContextMenu(&menu, people_actions);
    if (! menu.isEmpty()) {
        menu.exec(globalPosition);
    }
}

void PeopleEntryNumberDelegate::fillContextMenu(QMenu *menu,
                                                PeopleActions *people_actions)
{
    if (QAction *mobile_action = people_actions->callMobileAction()) {
        menu->addAction(mobile_action);
    }
}

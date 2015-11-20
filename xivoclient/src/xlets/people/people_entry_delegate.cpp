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
#include <QList>
#include <QMenu>
#include <QMouseEvent>
#include <QPainter>
#include <QVariant>

#include "people_action_generator.h"
#include "people_entry_delegate.h"
#include "people_actions.h"

QSize PeopleEntryDotDelegate::icon_size = QSize(8, 8);
int PeopleEntryDotDelegate::icon_text_spacing = 7;

int PeopleEntryNumberDelegate::action_selector_width = 40;
int PeopleEntryNumberDelegate::button_height = 30;
QMargins PeopleEntryNumberDelegate::button_margins = QMargins(10, 0, 10, 0);

QSize PeopleEntryPersonalContactDelegate::icon_size = QSize(12,12);
int PeopleEntryPersonalContactDelegate::icons_spacing = 7;

PeopleEntryDotDelegate::PeopleEntryDotDelegate(QWidget *parent)
    : AbstractItemDelegate(parent)
{
}

QSize PeopleEntryDotDelegate::sizeHint(const QStyleOptionViewItem &option,
                                       const QModelIndex &index) const
{
    if (index.data(INDICATOR_COLOR_ROLE).isNull()) {
        return AbstractItemDelegate::sizeHint(option, index);
    }

    const QSize &original_size = AbstractItemDelegate::sizeHint(option, index);
    int new_width = original_size.width() + icon_size.width() + icon_text_spacing;
    return QSize(new_width, original_size.height());
}

void PeopleEntryDotDelegate::paint(QPainter *painter,
                                   const QStyleOptionViewItem &option,
                                   const QModelIndex &index) const
{
    AbstractItemDelegate::drawBorder(painter, option);

    QStyleOptionViewItem opt = option;
    opt.rect = AbstractItemDelegate::marginsRemovedByColumn(option.rect, index.column());

    if (index.data(INDICATOR_COLOR_ROLE).isNull()) {
        AbstractItemDelegate::paint(painter, opt, index);
        return;
    }

    QIcon dot = QIcon(":/images/dot.svg");
    QPixmap tinted_image = dot.pixmap(icon_size);

    int icon_left = opt.rect.x();
    int icon_top = opt.rect.center().y() - tinted_image.height() / 2;

    QPainter tint_painter(&tinted_image);
    tint_painter.setCompositionMode(QPainter::CompositionMode_SourceAtop);
    tint_painter.fillRect(tinted_image.rect(), QColor(index.data(INDICATOR_COLOR_ROLE).value<QColor>()));
    tint_painter.end();

    painter->save();
    painter->drawPixmap(icon_left, icon_top, tinted_image);
    painter->restore();

    int icon_total_width = icon_size.width() + icon_text_spacing;
    opt.rect = opt.rect.marginsRemoved(QMargins(icon_total_width,0,0,0));
    AbstractItemDelegate::paint(painter, opt, index);
}

PeopleEntryNumberDelegate::PeopleEntryNumberDelegate(PeopleActionGenerator *generator, QWidget *parent)
    : PeopleEntryDotDelegate(parent),
      pressed(false),
      m_people_action_generator(generator)
{
}

void PeopleEntryNumberDelegate::paint(QPainter *painter,
                                      const QStyleOptionViewItem &option,
                                      const QModelIndex &index) const
{
    if (index.data().isNull()) {
        PeopleEntryDotDelegate::paint(painter, option, index);
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
        PeopleActions people_actions = index.data(NUMBER_ROLE).value<PeopleActions>();
        if (this->shouldShowActionSelectorRect(people_actions, index)) {
            QRect selector_rect = this->actionSelectorRect(option.rect);

            QRect separator_rect(selector_rect);
            separator_rect.setWidth(1);

            painter->fillRect(separator_rect, "grey");

            QRect arrow_image_rect;
            QSize arrow_image_size = QSize(9, 6);
            arrow_image_rect.setSize(arrow_image_size);
            arrow_image_rect.moveCenter(selector_rect.center());
            painter->drawPixmap(arrow_image_rect, QIcon(":/images/down-arrow-white.svg").pixmap(arrow_image_size));
        }
        painter->restore();
        PeopleEntryDotDelegate::drawBorder(painter, option);
        return;
    }

    PeopleEntryDotDelegate::paint(painter, option, index);
}

bool PeopleEntryNumberDelegate::shouldShowActionSelectorRect(PeopleActions &people_actions, const QModelIndex &index) const
{
    bool has_mailto_actions = !people_actions.getMailtoActions().empty();

    return m_people_action_generator->hasCallCallables(index)
        || people_actions.hasChat()
        || m_people_action_generator->hasTransfers(index)
        || has_mailto_actions;
}

bool PeopleEntryNumberDelegate::editorEvent(QEvent *event,
                                            QAbstractItemModel *model,
                                            const QStyleOptionViewItem &option,
                                            const QModelIndex &index)
{
    if (index.data().isNull()) {
        return AbstractItemDelegate::editorEvent(event, model, option, index);
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
        PeopleActions people_actions(model->data(index, NUMBER_ROLE).value<PeopleActions>());

        if (this->buttonRect(option.rect).contains(mouse_event->pos())) {
            if (QAction *call_action = m_people_action_generator->newCallAction(index)) {
                call_action->trigger();
            }
        } else if (this->actionSelectorRect(option.rect).contains(mouse_event->pos())) {
            this->showContextMenu(option, &people_actions, index);
        }
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
                                                PeopleActions *people_actions,
                                                const QModelIndex &index)
{
    QAbstractScrollArea *view = static_cast<QAbstractScrollArea*>(option.styleObject);
    if (! view) {
        return;
    }

    QPoint position = this->contentsRect(option.rect).bottomLeft();
    QPoint globalPosition = view->viewport()->mapToGlobal(position);

    QPointer<Menu> menu = new Menu(view);
    this->fillContextMenu(menu, people_actions, index);
    if (! menu->isEmpty()) {
        menu->exec(globalPosition);
    }
    delete menu;
}

void PeopleEntryNumberDelegate::fillContextMenu(QPointer<Menu> menu,
                                                PeopleActions *people_actions,
                                                const QModelIndex &index)
{
    menu->addActions(people_actions->getMailtoActions(menu));
    menu->addActions(m_people_action_generator->newCallCallableActions(index));
    this->addTransferSubmenu(menu, tr("BLIND TRANSFER"),
                             m_people_action_generator->newBlindTransferActions(index));
    this->addTransferSubmenu(menu, tr("ATTENDED TRANSFER"),
                             m_people_action_generator->newAttendedTransferActions(index));
    if (QAction *chat_action = people_actions->newChatAction(menu)) {
        menu->addAction(chat_action);
    }
}

void PeopleEntryNumberDelegate::addTransferSubmenu(QPointer<Menu> menu,
                                                   const QString &title,
                                                   QList<QAction *> transfer_actions)
{
    if (transfer_actions.empty()) {
        return;
    }

    QPointer<Menu> transfer_menu = new Menu(title, menu);
    transfer_menu->addActions(transfer_actions);
    menu->addMenu(transfer_menu);
}

PeopleEntryPersonalContactDelegate::PeopleEntryPersonalContactDelegate(QWidget *parent)
    : AbstractItemDelegate(parent)
{
}

QSize PeopleEntryPersonalContactDelegate::sizeHint(const QStyleOptionViewItem &option,
                                                   const QModelIndex &index) const
{
    const QSize &original_size = AbstractItemDelegate::sizeHint(option, index);
    int new_width = icon_size.width() + icons_spacing + icon_size.width();
    return QSize(new_width, original_size.height());
}

void PeopleEntryPersonalContactDelegate::paint(QPainter *painter,
                                   const QStyleOptionViewItem &option,
                                   const QModelIndex &index) const
{
    AbstractItemDelegate::drawBorder(painter, option);

    QStyleOptionViewItem opt = option;
    opt.rect = AbstractItemDelegate::marginsRemovedByColumn(option.rect, index.column());

    QPixmap edit_image = QIcon(":/images/edit-contact.svg").pixmap(icon_size);

    int icon_edit_left = opt.rect.left();
    int icon_edit_top = opt.rect.center().y() - edit_image.height() / 2;

    QPixmap trash_image = QIcon(":/images/delete-contact.svg").pixmap(icon_size);

    int icon_trash_left = opt.rect.left() + icon_size.width() + icons_spacing;
    int icon_trash_top = opt.rect.center().y() - trash_image.height() / 2;

    painter->save();
    painter->drawPixmap(icon_edit_left, icon_edit_top, edit_image);
    painter->drawPixmap(icon_trash_left, icon_trash_top, trash_image);
    painter->restore();
}

bool PeopleEntryPersonalContactDelegate::editorEvent(QEvent *event,
                                                     QAbstractItemModel */*model*/,
                                                     const QStyleOptionViewItem &option,
                                                     const QModelIndex &index)
{
    if (event->type() != QEvent::MouseButtonPress) {
        return true;
    }

    QMouseEvent *mouse_event = static_cast<QMouseEvent*>(event);
    QStyleOptionViewItem opt = option;
    opt.rect = AbstractItemDelegate::marginsRemovedByColumn(option.rect, index.column());

    int right_edit_margin = opt.rect.width() - icon_size.width();
    QRect edit_zone = opt.rect.marginsRemoved(QMargins(0, 0, right_edit_margin, 0));

    if (edit_zone.contains(mouse_event->pos())) {
        const QVariantMap &unique_source_entry_id = index.data(UNIQUE_SOURCE_ID_ROLE).toMap();
        emit editPersonalContactClicked(unique_source_entry_id);
    }

    int right_delete_margin = opt.rect.width() - (icon_size.width() + icons_spacing + icon_size.width());
    int left_delete_margin = icon_size.width() + icons_spacing;
    QRect delete_zone = opt.rect.marginsRemoved(QMargins(left_delete_margin, 0, right_delete_margin, 0));

    if (delete_zone.contains(mouse_event->pos())) {
        const QVariantMap &unique_source_entry_id = index.data(UNIQUE_SOURCE_ID_ROLE).toMap();
        emit deletePersonalContactClicked(unique_source_entry_id);
    }

    return true;
}

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

#ifndef __PEOPLE_ENTRY_DELEGATE_H__
#define __PEOPLE_ENTRY_DELEGATE_H__

#include <QStyledItemDelegate>

class PeopleActions;
class QMenu;

class PeopleEntryDotDelegate : public QStyledItemDelegate
{
    Q_OBJECT

    public:
        PeopleEntryDotDelegate(QWidget *parent = NULL);
        QSize sizeHint(const QStyleOptionViewItem &option,
                       const QModelIndex &index) const;
        void paint(QPainter *painter,
                   const QStyleOptionViewItem &option,
                   const QModelIndex &index) const;

    protected:
        static QSize icon_size;
        static int icon_text_spacing;
};

class PeopleEntryNumberDelegate : public PeopleEntryDotDelegate
{
    Q_OBJECT

    public:
        PeopleEntryNumberDelegate(QWidget *parent = NULL);
        bool editorEvent(QEvent *event,
                         QAbstractItemModel *model,
                         const QStyleOptionViewItem &option,
                         const QModelIndex &index);
        void paint(QPainter *painter,
                   const QStyleOptionViewItem &option,
                   const QModelIndex &index) const;

    protected:
        bool pressed;
        static QMargins button_margins;
        static int action_selector_width;

    private:
        QRect buttonRect(const QRect &option_rect) const;
        QRect contentsRect(const QRect &option_rect) const;
        QRect actionSelectorRect(const QRect &option_rect) const;
        void fillContextMenu(QMenu *menu,
                             PeopleActions *people_actions);
        void showContextMenu(const QStyleOptionViewItem &option,
                             PeopleActions *people_actions);
};

class PeopleEntryAgentDelegate : public QStyledItemDelegate
{
    public:
        PeopleEntryAgentDelegate(QWidget *parent = NULL);
        QSize sizeHint(const QStyleOptionViewItem &option,
                       const QModelIndex &index) const;
        void paint(QPainter *painter,
                   const QStyleOptionViewItem &option,
                   const QModelIndex &index) const;
    private:
        static QSize icon_size;
};

#endif

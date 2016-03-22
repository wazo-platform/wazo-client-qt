/* XiVO Client
 * Copyright (C) 2015-2016 Avencall
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

#include <QDebug>
#include <QPointer>
#include <xletlib/item_delegate.h>
#include <xletlib/menu.h>


class PeopleActionGenerator;

class PeopleEntryDotDelegate : public AbstractItemDelegate
{
    Q_OBJECT

    public:
        PeopleEntryDotDelegate(QWidget *parent);
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
        PeopleEntryNumberDelegate(PeopleActionGenerator *generator, QWidget *parent);
        bool editorEvent(QEvent *event,
                         QAbstractItemModel *model,
                         const QStyleOptionViewItem &option,
                         const QModelIndex &index);
        void paint(QPainter *painter,
                   const QStyleOptionViewItem &option,
                   const QModelIndex &index) const;

    protected:
        bool pressed;
        static int action_selector_width;
        static int button_height;
        static QMargins button_margins;

    private:
        QRect buttonRect(const QRect &option_rect) const;
        QRect contentsRect(const QRect &option_rect) const;
        QRect actionSelectorRect(const QRect &option_rect) const;
        void fillContextMenu(QPointer<Menu> menu,
                             const QModelIndex &index);
        void showContextMenu(const QStyleOptionViewItem &option,
                             const QModelIndex &index);
        bool shouldShowActionSelectorRect(const QModelIndex &index) const;
        void addTransferSubmenu(QPointer<Menu> menu,
                                const QString &title,
                                QList<QAction *> transfer_actions);

        PeopleActionGenerator *m_people_action_generator;
};

class PeopleEntryPersonalContactDelegate : public AbstractItemDelegate
{
    Q_OBJECT

    public:
        PeopleEntryPersonalContactDelegate(QWidget *parent);
        QSize sizeHint(const QStyleOptionViewItem &option,
                       const QModelIndex &index) const;
        bool editorEvent(QEvent *event,
                         QAbstractItemModel *model,
                         const QStyleOptionViewItem &option,
                         const QModelIndex &index);
        void paint(QPainter *painter,
                   const QStyleOptionViewItem &option,
                   const QModelIndex &index) const;

    signals:
        void editPersonalContactClicked(const QVariantMap &unique_source_entry_id);
        void deletePersonalContactClicked(const QVariantMap &unique_source_entry_id);

    protected:
        static QSize icon_size;
        static int icons_spacing;
};

class CopyContextMenu : public QWidget
{
    Q_OBJECT

    public:
        CopyContextMenu(const QList<QAction *> &actions, QWidget *parent);

    public slots:
        void showContextMenu(const QPoint &pos);
    private:
        QList<QAction *> m_actions;
};

#endif

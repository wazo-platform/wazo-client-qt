/* XiVO Client
 * Copyright (C) 2007-2014 Avencall
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

#ifndef __PEOPLE_H__
#define __PEOPLE_H__

#include <QObject>
#include <QTimer>

#include <dao/phonedaoimpl.h>
#include <dao/userdaoimpl.h>
#include <xletlib/functests.h>
#include <xletlib/xlet.h>
#include <xletlib/people_entry_manager.h>
#include <ui_people_widget.h>

#include "people_entry_sort_filter_proxy_model.h"

class People: public XLet
{
    Q_OBJECT
    FUNCTESTED

    private:
        static const int delay_before_lookup = 1000;
        static const int min_lookup_length = 3;

    public:
        People(QWidget *parent=0);
        ~People();
    public slots:
        void numberSelectionRequested();
        void focusEntryTable();
        void schedulePeopleLookup(const QString &lookup_pattern);
        void searchPeople();
        void dataChanged(const QModelIndex &, const QModelIndex &);
    private:
        bool alreadySearched(const QString &search_pattern) const;

        Ui::PeopleWidget ui;
        PeopleEntrySortFilterProxyModel *m_proxy_model;
        PeopleEntryModel *m_model;
        PeopleEntryManager m_people_entry_manager;
        QTimer m_remote_lookup_timer;
        QString m_searched_pattern;
        QStringList m_search_history;
};

#endif /* __PEOPLE_H__ */

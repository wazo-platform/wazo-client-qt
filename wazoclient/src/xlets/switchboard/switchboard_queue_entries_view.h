/*
 * Copyright 2014-2017 The Wazo Authors  (see the AUTHORS file)
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

#ifndef __SWITCHBOARD_QUEUE_ENTRIES_VIEW_H__
#define __SWITCHBOARD_QUEUE_ENTRIES_VIEW_H__

#include <xletlib/queue_entries/queue_entries_view.h>

#include <QDateTime>

class SwitchboardQueueEntriesView : public QueueEntriesView
{
    Q_OBJECT

    public:
        SwitchboardQueueEntriesView(QWidget *parent = NULL);
        ~SwitchboardQueueEntriesView();
        void selectFirstRow();

    signals:
        void selected(const QModelIndex &index);

    private slots:
        void trigger(const QModelIndex &index);

    private:
        bool isADoubleClickHappening(const QDateTime &time) const;
        QDateTime m_end_of_double_click;
        int m_longer_that_a_double_click;
};

#endif

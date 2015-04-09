/* Copyright (C) 2015 Avencall
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


#ifndef __PEOPLE_ACTIONS_H__
#define __PEOPLE_ACTIONS_H__

#include <QAction>
#include <QObject>

#include <xletlib/people_entry.h>

class PeopleActions: public QObject
{
    Q_OBJECT

    public:
        PeopleActions(QList< QPair<QString, ColumnType> > fields, const PeopleEntry &entry);
        QAction *callAction();
        QAction *callMobileAction();

    public slots:
        void call();
        void callMobile();

    private:
        const PeopleEntry &m_entry;

        int m_mobile_column;
        int m_number_column;

        QAction *m_call_action;
        QAction *m_call_mobile_action;
};

#endif

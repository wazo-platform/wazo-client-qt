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

#ifndef _CONFROOM_MODEL_H_
#define _CONFROOM_MODEL_H_

#include <QWidget>
#include <QAbstractTableModel>
#include <QModelIndex>

#include "conference.h"
#include "baseengine.h"

class ConfTab;

class ConfRoomModel : public QAbstractTableModel
{
    Q_OBJECT

    public:
        ConfRoomModel(ConfTab *t, QWidget *parent, const QString &, const QVariantMap &);
        QString number() const { return m_number; }
        QString row2participantId(int row) const { return m_row2number[row]; }
        int isAdmin() { return m_admin; }
        int isAuthed() { return m_authed; }
        bool isRowMuted(int row) const;
        const QString &roomNumber() const { return m_number; }
        int userNumberFromRow(int row) const;
    public slots:
        void updateMeetmeConfig(const QVariantMap &);
    private slots:
        void extractRow2IdMap();
        void updateJoinTime();
    private:
        void sort(int, Qt::SortOrder);
        int rowCount(const QModelIndex&) const;
        int columnCount(const QModelIndex&) const;
        QVariant data(const QModelIndex&, int) const;
        QVariant headerData(int, Qt::Orientation, int) const;
        Qt::ItemFlags flags(const QModelIndex &) const;
        ConfTab *m_tab;
        QWidget *m_parent;
        bool m_admin;
        bool m_authed;
        QString m_number;
        QStringList m_row2number;
        QVariantMap m_members;
};

#endif

/* XiVO Client
 * Copyright (C) 2007-2015 Avencall
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

#ifndef __CONFERENCE_ROOM_MODEL_H__
#define __CONFERENCE_ROOM_MODEL_H__

#include <QAbstractTableModel>
#include <QModelIndex>
#include <QWidget>

class ConfTab;

class ConferenceRoomModel : public QAbstractTableModel
{
    Q_OBJECT

    public:
        enum ColOrder {
            ID,
            ACTION_MUTE,
            NAME,
            NUMBER,
            SINCE,
            NB_COL
        };
        ConferenceRoomModel(QWidget *parent);
        QString row2participantId(int row) const;
        const QString & roomNumber() const;

        bool isRowMuted(int row) const;
        int userNumberFromRow(int row) const;
        void setConfRoom(const QString &room_number, const QVariantMap &members);

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
        QString m_room_number;
        QStringList m_row2number;
        QVariantMap m_members;
};

#endif

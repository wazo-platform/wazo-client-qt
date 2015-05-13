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

#include <QList>
#include <QModelIndex>
#include <QWidget>

#include <xletlib/abstract_table_model.h>

#include "conference_room_item.h"

class ConfTab;

class ConferenceRoomModel : public AbstractTableModel
{
    Q_OBJECT

    public:
        ConferenceRoomModel(QWidget *parent);
        const QString & roomNumber() const;

        bool isExtensionMuted(const QString &extension) const;
        int joinOrder(const QString &extension) const;
        void setConfRoom(const QString &room_number, const QVariantMap &members);
        void setMyJoinOrder(int join_order);

    protected:
        virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
        virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
        virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
        virtual QVariant headerData(int section,
                                   Qt::Orientation orientation,
                                   int role = Qt::DisplayRole) const;
        virtual Qt::ItemFlags flags(const QModelIndex &index) const;
        virtual QList<int> columnDisplayBold() const;
        virtual QList<int> columnDisplaySmaller() const;

    private slots:
        void updateJoinTime();

    private:
        int m_my_join_order;
        QString m_room_number;
        static QList<QString> m_headers;
        QList<ConferenceRoomItem> m_confroom_item;
};

#endif

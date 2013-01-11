/* XiVO Client
 * Copyright (C) 2007-2013, Avencall
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

#ifndef __DIRECTORY_ENTRY_MODEL_H__
#define __DIRECTORY_ENTRY_MODEL_H__

#include <QAbstractTableModel>

#include <phoneinfo.h>

class DirectoryEntryModel : public QAbstractTableModel
{
    Q_OBJECT

    public:
        DirectoryEntryModel(QObject *parent = NULL);

        int rowCount(const QModelIndex &parent = QModelIndex()) const;
        int columnCount(const QModelIndex &) const;

        bool removeRows(int, int, const QModelIndex &);

        QVariant data(const QModelIndex&, int role = Qt::DisplayRole) const;
        QVariant headerData(int,
                            Qt::Orientation,
                            int) const;
    public slots:
        void updatePhoneConfig(const QString &xid);
        void removePhoneConfig(const QString &xid);
        void updatePhoneStatus(const QString &xid);
        void clearingCache();

    public:
        enum Columns {
          STATUS_ICON,
          FIRST_NAME,
          LAST_NAME,
          NUMBER,
          NB_COL
        };

    private:
        void refreshEntryRow(const PhoneInfo *phone);
        QPixmap getPhoneIcon(const PhoneInfo *phone) const;

        QVariant dataDisplay(int row, int column) const;
        QVariant dataDecoration(int row, int column) const;
        QVariant dataTooltip(int row, int column) const;

        QString m_headers[NB_COL];
        QList<const PhoneInfo *> m_phones;
};

#endif

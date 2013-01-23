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

#include <QStringList>
#include <QAbstractTableModel>
#include <QDebug>

#include <xletlib/xletlib_export.h>

class XLETLIB_EXPORT QueueEntriesModel : public QAbstractTableModel
{
    Q_OBJECT

    public:
        QueueEntriesModel(QObject *parent = NULL);
        ~QueueEntriesModel();

        void fillHeaders();

        int rowCount(const QModelIndex & index) const;
        int columnCount(const QModelIndex & index) const;

        bool removeRows(int start_row_index, int row_count, const QModelIndex & index);

        QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
        QVariant headerData(int column_index,
                            Qt::Orientation orientation,
                            int role) const;
    public slots:
        void queueEntryUpdate(const QString & queue_id,
                              const QVariantList & entry_list);
        void changeWatchedQueue(const QString & queue_id);
    private slots:
        void increaseTime();

    private:
        QVariant dataDisplay(int row, int column) const;
        void subscribeQueueEntry(const QString & queue_id);
        void refreshColumn(int column_index);

    public:
        enum Columns {
            ID,
            POSITION,
            NAME,
            NUMBER,
            TIME,
            UNIQUE_ID,
            NB_COL
        };

        QString m_headers[NB_COL];
        QString m_queue_id;
        QVariantList m_entries;
        static QString not_available ;
};

/* XiVO Client
 * Copyright (C) 2007-2011, Avencall
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

#ifndef __AGENTSMODEL_H__
#define __AGENTSMODEL_H__

#include <QAbstractTableModel>
#include <QStringList>

class AgentsModel : public QAbstractTableModel
{
    Q_OBJECT

    public:
        AgentsModel(QObject *parent = NULL);

        int rowCount(const QModelIndex &) const;
        int columnCount(const QModelIndex &) const;

        bool removeRows(int, int, const QModelIndex &);

        QVariant data(const QModelIndex&, int role = Qt::DisplayRole) const;
        QVariant headerData(int,
                            Qt::Orientation,
                            int) const;

    public slots:
        void updateAgentConfig(const QString &);
        void removeAgentConfig(const QString &);
        void updateAgentStatus(const QString &);
        void refreshAgentRow(const QString & agent_id);
        void updateAgentListenStatus(const QString &, const QString &, const QString &);

    public:
        enum Columns {
            ID, NUMBER, FIRSTNAME, LASTNAME, SPY_BUTTON, SPY_STATUS, ONLINE, PRESENCE,
            LOGGED_STATUS, JOINED_QUEUES, PAUSE_STATUS, PAUSE_BUTTON, PAUSED_QUEUES, NB_COL
        };

    private:
        int getNumberOfJoinedQueues(const QString &agent_xid);
        QVariant dataDisplay(int row, int column) const;

        QString m_headers[NB_COL];
        QStringList m_row2id;
        static QString not_available ;
};

#endif

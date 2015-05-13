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

#ifndef __CONFERENCE_LIST_MODEL_H__
#define __CONFERENCE_LIST_MODEL_H__

#include <QList>
#include <QModelIndex>
#include <QVariant>
#include <QWidget>

#include <xletlib/abstract_table_model.h>
#include <xletlib/functests.h>
#include <baseengine.h>

#include "conference_enum.h"
#include "conference_list_item.h"

class ConferenceListModel : public AbstractTableModel
{
    Q_OBJECT
    FUNCTESTED

    public:
        ConferenceListModel(QWidget *parent = NULL);

    protected:
        virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
        virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
        virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
        virtual QVariant headerData(int section,
                                    Qt::Orientation orientation,
                                    int role = Qt::DisplayRole) const;
        virtual QList<int> columnDisplayBold() const;
        virtual QList<int> columnDisplaySmaller() const;

    public slots:
        void updateConfList(const QVariantMap &);

    private slots:
        void updateConfTime();

    private:
        QString startedSince(double time) const;

        static QList<QString> m_headers;
        QList<ConferenceListItem> m_conflist_item;
};

#endif

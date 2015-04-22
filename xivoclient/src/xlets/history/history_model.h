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

#ifndef __HISTORY_MODEL_H__
#define __HISTORY_MODEL_H__

#include <QAbstractTableModel>
#include <QDateTime>
#include <QModelIndex>
#include <QString>
#include <QVariant>
#include <QWidget>

#include <ipbxlistener.h>

enum HistoryMode {
    OUTCALLS = 0,
    INCALLS,
    MISSEDCALLS,
    DEFAULT
};

//const QString DATE_PATTERN = "yyyy-MM-ddThh:mm:ss";

/*! \brief cdr model
 */
class HistoryModel : public QAbstractTableModel, public IPBXListener
{
    Q_OBJECT

    public:
        HistoryModel(QWidget * parent = NULL);
        void parseCommand(const QVariantMap &);

    protected:
        virtual int rowCount(const QModelIndex&) const;
        virtual int columnCount(const QModelIndex&) const;
        virtual QVariant data(const QModelIndex&, int) const;
        virtual QVariant headerData(int , Qt::Orientation, int) const;

    public slots:
        void missedCallMode();
        void receivedCallMode();
        void sentCallMode();
        void updateHistory(const QVariantMap &p);

    private slots:
        void requestHistory(HistoryMode mode = DEFAULT, QString xuserid = "");

    private:
        QVariantList m_history;
        HistoryMode m_mode;
};

#endif

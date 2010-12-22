/* XiVO Client
 * Copyright (C) 2007-2010, Proformatique
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

/* $Revision$
 * $Date$
 */

#ifndef _ETVNG_H_
#define _ETVNG_H_

#include <QLabel>
#include <QTimer>
#include <QVBoxLayout>
#include <QTableView>
#include <QModelIndex>
#include <QVariant>
#include <QAbstractTableModel>
#include <QHeaderView>
#include <QMenu>
#include <QAction>
#include <QMouseEvent>

#include <baseengine.h>

#include "xlet.h"

class ETVListProperties
{
    public:
        ETVListProperties();

        int displayOptionShowGrid() const;
        QString displayOptionStyleSheet() const;
        int ncolumns() const;
        QString treebase() const;

        QString title(int) const;
        QString eventfield(int) const;
        QVariant::Type qttype(int) const;

        void addProperty(const QString &,
                         const QString &,
                         const QVariant::Type &,
                         const QString &);
    private:
        QVariantMap m_properties;
};

class ETVListModel : public QAbstractTableModel
{
    Q_OBJECT

    public:
        ETVListModel(const ETVListProperties * const);
        int displayOptionShowGrid() const;
        QString displayOptionStyleSheet() const;

    private slots:
        void mylistChange(const QString &path, DStoreEvent event);
    protected:
        void timerEvent(QTimerEvent *event);
    private:
        void sort(int, Qt::SortOrder);
        int rowCount(const QModelIndex&) const;
        int columnCount(const QModelIndex&) const;
        QVariant data(const QModelIndex&, int) const;
        QVariant headerData(int, Qt::Orientation, int) const;
        Qt::ItemFlags flags(const QModelIndex &) const;

        QMap<int, QString> m_row2id;
        QVariantMap m_myList;
        const ETVListProperties * m_fieldoptions;
};

class ETVListView : public QTableView
{
    Q_OBJECT

    public:
        ETVListView(QWidget * parent,
                    XLet * parentxlet,
                    ETVListModel * model);
    signals:
        void pevent(QMouseEvent *);
    protected:
        virtual void selectionChanged(const QItemSelection &,
                                      const QItemSelection &);
        virtual void mousePressEvent(QMouseEvent *event);
};


class ETVListWidget : public QWidget
{
    Q_OBJECT

    public:
        ETVListWidget(const ETVListProperties * const,
                      XLet * parent);
};

#endif

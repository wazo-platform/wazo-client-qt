/* XiVO Client
 * Copyright (C) 2007-2011, Proformatique
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

#ifndef _COMMONTABLE_H_
#define _COMMONTABLE_H_

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

class CommonTableProperties
{
    public:
        CommonTableProperties(const QString &);
        ~CommonTableProperties() {};

        int displayOptionShowGrid() const;
        QString displayOptionStyleSheet() const;
        int ncolumns() const;
        QString treebase() const;

        QString title(int) const;
        QString eventfield(int) const;
        QVariant::Type qttype(int) const;
        QString xivotype(int) const;

        void setTooltipCallBack(QString (*cb)(const QModelIndex &, void *),
                                void *);
        QString tooltip(const QModelIndex &) const;

        void addColumn(const QString &,
                       const QString &,
                       const QVariant::Type &,
                       const QString &);
    private:
        QVariantMap m_properties;
        QString (* m_tooltip_cb)(const QModelIndex &, void *);
        void * m_tooltip_xlet;
};

class CommonTableModel : public QAbstractTableModel
{
    Q_OBJECT

    public:
        CommonTableModel(const CommonTableProperties * const);
        ~CommonTableModel() {};

        int displayOptionShowGrid() const;
        QString displayOptionStyleSheet() const;

    signals:
        void setDataEdit(const QModelIndex &, const QVariant &);
    private slots:
        void mylistChange(const QString &, DStoreEvent);
    protected:
        void timerEvent(QTimerEvent *event);
    private:
        void sort(int, Qt::SortOrder);
        int rowCount(const QModelIndex &) const;
        int columnCount(const QModelIndex &) const;
        QString tooltip(const QModelIndex &) const;
        bool setData(const QModelIndex &, const QVariant &, int);
        QVariant data(const QModelIndex &, int) const;
        QVariant headerData(int, Qt::Orientation, int) const;
        Qt::ItemFlags flags(const QModelIndex &) const;

        QMap<int, QString> m_row2id;
        QStringList m_myList;
        const CommonTableProperties * m_fieldoptions;
};

class CommonTableView : public QTableView
{
    Q_OBJECT

    public:
        CommonTableView(QWidget *, XLet *, CommonTableModel *);
        ~CommonTableView() {};
    signals:
        void signalMousePressEvent(QMouseEvent *);
    protected:
        void selectionChanged(const QItemSelection &,
                              const QItemSelection &);
        void mousePressEvent(QMouseEvent *event);
    private:
        QList<QModelIndex> m_modelindexlist;
};

class CommonTableWidget : public QWidget
{
    Q_OBJECT

    public:
        CommonTableWidget(const CommonTableProperties * const,
                          XLet * parent);
        ~CommonTableWidget() {};
};

#endif

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

#ifndef __LOGWIDGET_H__
#define __LOGWIDGET_H__

#include <QDateTime>
#include <QAbstractTableModel>
#include <QTableView>
#include <QVariant>
#include <QHeaderView>
#include <QList>
#include <QDebug>
#include <QMouseEvent>
#include <QHBoxLayout>
#include <QLabel>
#include <QLayoutItem>
#include <QGroupBox>
#include <QRadioButton>
#include <QScrollArea>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QApplication>
#include <QMenu>
#include <QCursor>

#include "xlet.h"
#include <baseengine.h>

class QVBoxLayout;
class QRadioButton;

class UserInfo;
class LogWidgetModel;


/*! \brief cdr model
 */
class LogWidgetModel : public QAbstractTableModel
{
    Q_OBJECT

    public:
        LogWidgetModel(int);
        void updateHistory(const QVariantMap &p);
        static void updateHistory_t(const QVariantMap &p, void *udata) {
            return ((LogWidgetModel*)udata)->updateHistory(p);
        };

    protected:
        virtual int rowCount(const QModelIndex&) const;
        virtual int columnCount(const QModelIndex&) const;
        virtual QVariant data(const QModelIndex&, int) const;
        virtual Qt::ItemFlags flags(const QModelIndex &) const;
        virtual void sort(int, Qt::SortOrder);
        virtual QVariant headerData(int , Qt::Orientation, int) const;

    public slots:
        void changeMode(bool);

    private:
        static int ascendingOrderByDuration(const QVariant &a, const QVariant &b) {
            return a.toMap()["duration"].toInt() <  b.toMap()["duration"].toInt();
        }
        static int descendingOrderByDuration(const QVariant &a, const QVariant &b) {
            return a.toMap()["duration"].toInt() >  b.toMap()["duration"].toInt();
        }
        static int ascendingOrderByNumber(const QVariant &a, const QVariant &b) {
            return a.toMap()["fullname"].toString() <  b.toMap()["fullname"].toString();
        }
        static int descendingOrderByNumber(const QVariant &a, const QVariant &b) {
            return a.toMap()["fullname"].toString() >  b.toMap()["fullname"].toString();
        }
        static int ascendingOrderByDate(const QVariant &a, const QVariant &b) {
            return 
                QDateTime::fromString(a.toMap()["ts"].toString(),"yyyy-MM-dd hh:mm:ss").toTime_t() <
                QDateTime::fromString(b.toMap()["ts"].toString(),"yyyy-MM-dd hh:mm:ss").toTime_t();
        }
        static int descendingOrderByDate(const QVariant &a, const QVariant &b) {
            return 
                QDateTime::fromString(a.toMap()["ts"].toString(),"yyyy-MM-dd hh:mm:ss").toTime_t() >
                QDateTime::fromString(b.toMap()["ts"].toString(),"yyyy-MM-dd hh:mm:ss").toTime_t();
        }

        void requestHistory(const QString &, int, const QDateTime &since = QDateTime(), int f = 0);

        QVariantList history;
        int mode;
};

/* class reason:
 *   You can't know which mouse button caused the onViewClick to be called
 *   through QApplication::mouseButtons or through filtering event from
 *   QTableView
 */
class LogTableView : public QTableView
{
    Q_OBJECT

    public:
        LogTableView(QWidget *parent, LogWidgetModel *model);

    private slots:
        void onViewClick(const QModelIndex &);
        void callOnClick(bool);

    protected:
        virtual void mousePressEvent(QMouseEvent *event);

    private:
        int lastPressed;
};


/*! \brief Call Log display widget
 */
class LogWidget : public XLet
{
    Q_OBJECT

    public:
        LogWidget(QWidget *parent=0);

    private:
        LogTableView *m_view;
};

#endif

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

/* $Revision$
 * $Date$
 */

#ifndef __LOGWIDGET_H__
#define __LOGWIDGET_H__

#include <QtGui>

#include <xletinterface.h>
#include <xlet.h>
#include <ipbxlistener.h>

enum HistoryMode {
    OUTCALLS = 0,
    INCALLS,
    MISSEDCALLS,
    DEFAULT
};

const QString DATE_PATTERN = "yyyy-MM-ddThh:mm:ss";

/*! \brief cdr model
 */
class LogWidgetModel : public QAbstractTableModel, public IPBXListener
{
    Q_OBJECT

    public:
        LogWidgetModel(int, QWidget * parent = NULL);
        void parseCommand(const QVariantMap &);

    protected:
        virtual int rowCount(const QModelIndex&) const;
        virtual int columnCount(const QModelIndex&) const;
        virtual QVariant data(const QModelIndex&, int) const;
        virtual Qt::ItemFlags flags(const QModelIndex &) const;
        virtual void sort(int, Qt::SortOrder);
        virtual QVariant headerData(int , Qt::Orientation, int) const;

    public slots:
        void changeMode(bool);
        void updateHistory(const QVariantMap &p);

    private slots:
        void requestHistory(HistoryMode mode = DEFAULT, QString xuserid = "");

    private:
        bool m_sorted;
        int m_sorted_column;
        Qt::SortOrder m_sort_order;

        static QDateTime dateFromString(const QString &string) {
            QString date = string.split(".")[0];
            return QDateTime::fromString(date, DATE_PATTERN);
        }
        static int ascendingOrderByDuration(const QVariant &a, const QVariant &b) {
                return a.toMap().value("duration").toInt() <
                        b.toMap().value("duration").toInt();
        }
        static int descendingOrderByDuration(const QVariant &a, const QVariant &b) {
                return a.toMap().value("duration").toInt() >
                        b.toMap().value("duration").toInt();
        }
        static int ascendingOrderByNumber(const QVariant &a, const QVariant &b) {
                return a.toMap().value("fullname").toString() <
                        b.toMap().value("fullname").toString();
        }
        static int descendingOrderByNumber(const QVariant &a, const QVariant &b) {
                return a.toMap().value("fullname").toString() >
                        b.toMap().value("fullname").toString();
        }
        static int ascendingOrderByDate(const QVariant &a, const QVariant &b) {
            QDateTime first_date = dateFromString(a.toMap()["calldate"].toString());
            QDateTime second_date = dateFromString(b.toMap()["calldate"].toString());
            return first_date.toTime_t() < second_date.toTime_t();
        }
        static int descendingOrderByDate(const QVariant &a, const QVariant &b) {
            QDateTime first_date = dateFromString(a.toMap()["calldate"].toString());
            QDateTime second_date = dateFromString(b.toMap()["calldate"].toString());
            return first_date.toTime_t() > second_date.toTime_t();
        }

        QVariantList m_history;
        HistoryMode m_mode;
};

/* class reason:
 *   You can't know which mouse button caused the onViewClick to be called.
 *   QApplication::mouseButtons and filtering event from QTableView doesn't
 *   look to work
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
        int m_lastPressed;
};


/*! \brief Call Log display widget
 */
class LogWidget : public XLet
{
    Q_OBJECT

    public:
        LogWidget(QWidget *parent=0);
    private:
        LogTableView * m_view;
        LogWidgetModel *m_history_model;
};

class XLetHistoryPlugin : public QObject, XLetInterface
{
    Q_OBJECT
    Q_INTERFACES(XLetInterface)

    public:
        XLet* newXLetInstance(QWidget *parent=0);
};

#endif

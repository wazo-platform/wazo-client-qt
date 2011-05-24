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
#ifndef _CONFERENCE2_CONFROOM_H_
#define _CONFERENCE2_CONFROOM_H_
#include <QWidget>
#include <QDebug>
#include <QLabel>
#include <QTimer>
#include <QAbstractTableModel>
#include <QTableView>
#include <QModelIndex>
#include <QPushButton>
#include <QVBoxLayout>
#include <QMouseEvent>
#include <QMap>
#include <QHeaderView>

#include "conference.h"
#include "baseengine.h"
class ConfRoomView;
class ConfTab;

class ConfRoomModel : public QAbstractTableModel
{
    Q_OBJECT

    public:
        ConfRoomModel(ConfTab *t, QWidget *parent, const QString &);
        ~ConfRoomModel();
        void setView(ConfRoomView *m_view);
        QString id() const;
        QString row2participantId(int) const;
        int isAdmin() { return m_admin; };
        int isAuthed() { return m_authed; };
    private slots:
        void confRoomChange(const QString &path, DStoreEvent event);
        void extractRow2IdMap();
    protected:
        void timerEvent(QTimerEvent *event);
    private:
        void updateView();
        void sort(int, Qt::SortOrder);
        int rowCount(const QModelIndex&) const;
        int columnCount(const QModelIndex&) const;
        QVariant data(const QModelIndex&, int) const;
        QVariant headerData(int, Qt::Orientation, int) const;
        Qt::ItemFlags flags(const QModelIndex &) const;
        ConfTab *m_tab;
        QWidget *m_parent;
        bool m_admin;
        bool m_authed;
        QString m_id;
        ConfRoomView *m_view;
        QMap<int, QString> m_row2id;
        QVariantMap m_pplInRoom;

};

class ConfRoomView : public QTableView
{
    Q_OBJECT

    public:
        ConfRoomView(QWidget *parent, ConfRoomModel *model);
    private slots:
        void onViewClick(const QModelIndex &);
        void sectionHeaderClicked(int);
    protected:
        virtual void mousePressEvent(QMouseEvent *event);
    private:
        int lastPressed;
};

class ConfRoom : public QWidget
{
    Q_OBJECT

    public:
        ConfRoom(QWidget *parent, ConfTab *tab, const QString &id);
    public slots:
        void pauseConf();
        void allowedIn();
    private:
        QString m_id;
        ConfRoomModel *m_model;
        QLabel *m_moderatedRoom;
};

#endif

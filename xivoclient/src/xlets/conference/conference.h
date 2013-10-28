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

#ifndef __XLET_CONFERENCE
#define __XLET_CONFERENCE

#include <QDebug>
#include <QWidget>
#include <QtPlugin>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QPushButton>

#include <xletlib/xletinterface.h>
#include <xletlib/xlet.h>

#include <xletlib/functests.h>

class XletConference;

#include "conflist.h"
#include "confroom.h"

Q_DECLARE_METATYPE(QWidget*);

class ConfTab : public QTabWidget
{
    Q_OBJECT

    public:
        ConfTab(QWidget *parent);
        int addClosableTab(QWidget *w, const QString &title);
        void showConfRoom(const QString &number, const QVariantMap &members);
        int indexOf(QWidget *w) { return QTabWidget::indexOf(w); };
        int indexOf(const QString &id);

    public slots:
        void closeTab(QWidget *w=0);

};


class XletConference : public XLet
{
    Q_OBJECT
    FUNCTESTED

    public:
        XletConference(QWidget *parent=0);

    public slots:
        void openConfRoom(const QString &number, const QVariantMap &members);
    private:
        void registerMeetmeUpdate() const;
        ConfTab *m_tab;

};

class XLetConferencePlugin : public QObject, XLetInterface
{
    Q_OBJECT
    Q_INTERFACES(XLetInterface)

    public:
        XLet *newXLetInstance(QWidget *parent=0);
};

Q_EXPORT_PLUGIN2(xletconferenceplugin, XLetConferencePlugin);

#endif

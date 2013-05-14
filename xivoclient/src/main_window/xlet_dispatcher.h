/* XiVO Client
 * Copyright (C) 2013, Avencall
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

#ifndef __XLET_DISPATCHER_H__
#define __XLET_DISPATCHER_H__

#include <QWidget>
#include <QtGui>
#include <QList>

#include <xletlib/functests.h>

#include "main_widget.h"
#include "main_window.h"

class XLet;
class MainWindow;
class MainWidget;

class XletDispatcher : public QObject
{
    Q_OBJECT
    FUNCTESTED

    public:
        XletDispatcher(MainWindow *main_window, MainWidget *main_widget, QObject *parent=NULL);
        ~XletDispatcher();

    public slots:
        void resetState();

    private slots:
        void setStatusNotLogged();
        void setStatusLogged();
        void showWidgetOnTop(QWidget *);

    private:
        void setAppearance(const QVariantList &);
        void clearAppearance();
        void addPanel(const QString &, const QString &, QWidget *);
        void removePanel(const QString &, QWidget *);
        QDockWidget* createDockXlet(const QString& name,
                                    const QString& title,
                                    QDockWidget::DockWidgetFeatures features,
                                    QWidget *widget);

        MainWindow *m_main_window;
        MainWidget *m_main_widget;

        // Widgets for Xlets
        QDockWidget *m_resizingHelper;
        QVBoxLayout *m_vL;
        QByteArray m_defaultState;
        QTabWidget *m_tabwidget;
        QHash<QString, XLet *> m_xletlist;
        QHash<QString, QString> m_dockoptions;
        QHash<QString, QList<QDockWidget *>* > m_docks;
        QStringList m_docknames;
        QStringList m_gridnames;
        QStringList m_tabnames;
        QStringList m_allnames;
};

#endif

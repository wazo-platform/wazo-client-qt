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

#include <QDebug>
#include <QObject>

#include <baseengine.h>
#include <xletfactory.h>

#include "xlet_dispatcher.h"


XletDispatcher::XletDispatcher(MainWindow *main_window, MainWidget *main_widget, QObject *parent)
    : QObject(parent),
      m_main_window(main_window),
      m_main_widget(main_widget),
      m_resizingHelper(0),
      m_vL(new QVBoxLayout(main_window)),
      m_tabwidget(NULL)
{
}

XletDispatcher::~XletDispatcher()
{
}

void XletDispatcher::setStatusLogged()
{
    qDebug() << Q_FUNC_INFO;
    this->setAppearance(b_engine->getCapaXlets());

    this->m_tabwidget = new QTabWidget(this->m_main_window);
    this->m_tabwidget->hide();

    if (this->m_docknames.contains("tabber")) {
        this->m_tabwidget->show();
        this->addPanel("tabber", tr("Tabs"), this->m_tabwidget);
    }
    if (this->m_gridnames.contains("tabber")) {
        this->m_tabwidget->show();
        this->m_vL->addWidget(m_tabwidget);
    }

    foreach (QString xletid, this->m_allnames) {
        if (! QStringList("tabber").contains(xletid)) {
            bool withscrollbar = this->m_dockoptions[xletid].contains("s");
            XLet *xlet = XLetFactory::spawn(xletid, this->m_main_window);
            if (xlet) {
                this->m_xletlist.insert(xletid, xlet);
                xlet->doGUIConnects(this->m_main_window);
                if (withscrollbar) {
                    QScrollArea *sa_ag = new QScrollArea(this->m_main_window);
                    sa_ag->setWidget(xlet);
                    sa_ag->setWidgetResizable(true);
                    this->addPanel(xletid, xlet->title(), sa_ag);
                } else {
                    this->addPanel(xletid, xlet->title(), xlet);
                }
            } else {
                qDebug() << Q_FUNC_INFO << "cannot instantiate XLet" << xletid;
            }
        }
    }

    this->m_tabwidget->setCurrentIndex(b_engine->getSettings()->value("display/lastfocusedtab").toInt());

    foreach (QString name, this->m_docks.keys()) {
        for(QList<QDockWidget *>::iterator i = this->m_docks[name]->begin(); i != this->m_docks[name]->end(); i++) {
            (*i)->show();
        }
    }
    this->m_defaultState = this->m_main_window->saveState();
    // restore the saved state AFTER showing the docks
    this->m_main_window->restoreState(b_engine->getSettings()->value("display/mainwindowstate").toByteArray());

    if ((this->m_resizingHelper == 0) && (this->m_docks.size())) {
        // we gonna resize this widget in resizeEvent
        // to force the mainWindow dockArea expand instead of the centralWidget
        this->m_resizingHelper = new QDockWidget(this->m_main_window);
        this->m_resizingHelper->setFixedWidth(1);
        this->m_main_window->addDockWidget(Qt::BottomDockWidgetArea, this->m_resizingHelper);
        this->m_resizingHelper->show(); // not a no-op, show is needed, to force Qt to calc
        this->m_resizingHelper->hide(); // the widget size!
        this->m_main_window->removeDockWidget(this->m_resizingHelper);
    }
}

void XletDispatcher::setStatusNotLogged()
{
    qDebug() << Q_FUNC_INFO;
    b_engine->getSettings()->setValue("display/mainwindowstate", this->m_main_window->saveState());
    if (this->m_tabwidget->currentIndex() > -1) {
        b_engine->getSettings()->setValue("display/lastfocusedtab", this->m_tabwidget->currentIndex());
    }

    foreach (QString dname, this->m_docknames) {
      if(this->m_docks.contains(dname)) {
        for(QList<QDockWidget *>::iterator i = this->m_docks[dname]->begin(); i != this->m_docks[dname]->end(); i++)
          removePanel(dname, *i);

        delete this->m_docks[dname];
        this->m_docks.remove(dname);
      }
    }

    // delete all xlets
    foreach (XLet *xlet, this->m_xletlist) {
        xlet->deleteLater();
    }
    this->m_xletlist.clear();

    if (this->m_docknames.contains("tabber")) {
        removePanel("tabber", this->m_tabwidget);
    }
    if (this->m_gridnames.contains("tabber")) {
        this->m_vL->removeWidget(this->m_tabwidget);
        this->m_tabwidget->deleteLater();
    }
}

void XletDispatcher::resetState()
{
    this->m_main_window->restoreState(this->m_defaultState);
}

QDockWidget* XletDispatcher::createDockXlet(const QString& name,
                                        const QString& title,
                                        QDockWidget::DockWidgetFeatures features,
                                        QWidget *widget)
{
    QDockWidget* tmpDockWidget = new QDockWidget(title);
    tmpDockWidget->setFeatures(features);
    tmpDockWidget->setAllowedAreas(Qt::BottomDockWidgetArea);
    tmpDockWidget->setObjectName(name);
    this->m_main_window->addDockWidget(Qt::BottomDockWidgetArea, tmpDockWidget);
    tmpDockWidget->hide();
    tmpDockWidget->setWidget(widget);
    return tmpDockWidget;
}

void XletDispatcher::addPanel(const QString &name, const QString &title, QWidget *widget)
{
    if (this->m_docknames.contains(name)) {
        QDockWidget::DockWidgetFeatures features = QDockWidget::NoDockWidgetFeatures;
        if (this->m_dockoptions[name].contains("c"))
            features |= QDockWidget::DockWidgetClosable;
        if (this->m_dockoptions[name].contains("f"))
            features |= QDockWidget::DockWidgetFloatable;
        if (this->m_dockoptions[name].contains("m"))
            features |= QDockWidget::DockWidgetMovable;
        if(! this->m_docks.contains(name)) {
            this->m_docks[name] = new QList<QDockWidget *>();
        }
        this->m_docks[name]->prepend(createDockXlet(name, title, features, widget));
    } else if (this->m_gridnames.contains(name)) {
        this->m_vL->insertWidget(this->m_dockoptions[name].toInt(), widget);
    } else if (this->m_tabnames.contains(name)) {
        QString tabTitle = "  " + title + "  ";
        if (this->m_dockoptions[name].size() > 0) {
            this->m_tabwidget->insertTab(this->m_dockoptions[name].toInt(), widget, tabTitle);
        } else {
            this->m_tabwidget->addTab(widget, tabTitle);
        }
    }
}

void XletDispatcher::removePanel(const QString & name, QWidget * widget)
{
    if (this->m_docknames.contains(name)) {
      for(QList<QDockWidget *>::iterator i = this->m_docks[name]->begin(); i != this->m_docks[name]->end(); i++) {
          this->m_main_window->removeDockWidget(*i);
        (*i)->deleteLater();
      }
    }
    if (this->m_tabnames.contains(name)) {
        int thisindex = this->m_tabwidget->indexOf(widget);
        if (thisindex > -1) {
            qDebug() << Q_FUNC_INFO << "removing tab" << name << thisindex;
            this->m_tabwidget->removeTab(thisindex);
        }
    }
    if (this->m_gridnames.contains(name)) {
        //this->m_gridlayout->removeWidget(widget);
        //delete widget;
        //widget->deleteLater();
    }
}

/*! \brief show this XLet on top of others
 *
 * This slot Works when tabbed view is selected.
 */
void XletDispatcher::showWidgetOnTop(QWidget * widget)
{
    if (this->m_tabwidget)
        this->m_tabwidget->setCurrentWidget(widget);
}

void XletDispatcher::setAppearance(const QVariantList & dockoptions)
{
    foreach (QVariant dproperties, dockoptions) {
        QStringList dopt = dproperties.toStringList();
        if (dopt.size() > 0) {
            QString wname = dopt[0];
            if ((wname == "customerinfo") && (! b_engine->checkedFunction(wname)))
                continue;
            this->m_allnames.append(wname);
            this->m_dockoptions[wname] = "";
            if (dopt.size() > 1) {
                if (dopt[1] == "dock") {
                    this->m_docknames.append(wname);
                } else if (dopt[1] == "grid")
                    this->m_gridnames.append(wname);
                else if (dopt[1] == "tab")
                    this->m_tabnames.append(wname);
                if (dopt.size() > 2)
                    this->m_dockoptions[wname] = dopt[2];
            } else {
                this->m_docknames.append(dopt[0]);
            }
        }
    }
}

void XletDispatcher::clearAppearance()
{
    this->m_docknames.clear();
    this->m_gridnames.clear();
    this->m_tabnames.clear();
    this->m_allnames.clear();
}

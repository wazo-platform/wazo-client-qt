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
      m_dock_container(NULL),
      m_xlets_dock_widget(),
      m_xlets_dock(),
      m_grid_container(NULL),
      m_xlets_grid_widget(),
      m_xlets_grid(),
      m_tab_container(NULL),
      m_xlets_tab_widget(),
      m_xlets_tab()
{
    this->connect(b_engine, SIGNAL(logged()), SLOT(setStatusLogged()));
    this->connect(b_engine, SIGNAL(delogged()), SLOT(setStatusNotLogged()));
}

XletDispatcher::~XletDispatcher()
{
}

void XletDispatcher::setStatusLogged()
{
    this->prepareAppearance();
    this->prepareXletsGrid();
    this->prepareXletsDock();
}

void XletDispatcher::setStatusNotLogged()
{
    this->m_main_window->saveState();

    this->cleanXletsGrid();
    this->cleanXletsDock();
    this->clearAppearance();
}

bool XletDispatcher::has_widget()
{
    if (this->m_xlets_grid.size() == 0) {
        return false;
    }

    return true;
}

void XletDispatcher::prepareXletsGrid()
{
    if (this->m_xlets_grid.size() == 0) {
        return;
    }

    this->m_grid_container = new QVBoxLayout(this->m_main_widget);

    foreach (QString xlet_id, this->m_xlets_grid.keys()) {
        QString options = this->m_xlets_grid.value(xlet_id);
        if (xlet_id == "tabber") {
            this->prepareXletsTab();
            this->m_grid_container->insertWidget(options.toInt(), this->m_tab_container);
        } else {
            XLet *xlet = XLetFactory::spawn(xlet_id, this->m_main_widget);
            if (xlet) {
                xlet->doGUIConnects(this->m_main_window);
                this->m_grid_container->insertWidget(options.toInt(), xlet);
                this->m_xlets_grid_widget.insert(xlet_id, xlet);
            }
        }
    }
}

void XletDispatcher::cleanXletsGrid()
{
    if (this->m_xlets_grid.size() == 0) {
        return;
    }

    foreach (QWidget *widget, this->m_xlets_grid_widget.values()) {
        this->m_grid_container->removeWidget(widget);
        widget->deleteLater();
    }
    if (m_xlets_grid.contains("tabber")) {
        this->m_grid_container->removeWidget(this->m_tab_container);
        this->cleanXletsTab();
    }
    this->m_grid_container->deleteLater();
    this->m_xlets_grid_widget.clear();
}

void XletDispatcher::prepareXletsTab()
{
    if (this->m_xlets_tab.size() == 0) {
        return;
    }

    this->m_tab_container = new QTabWidget(this->m_main_widget);

    foreach (QString xlet_id, this->m_xlets_tab.keys()) {
        QString options = this->m_xlets_tab.value(xlet_id);
        XLet *xlet = XLetFactory::spawn(xlet_id, this->m_main_widget);
        if (xlet) {
            xlet->doGUIConnects(this->m_main_window);
            QString tabTitle = "  " + xlet->title() + "  ";
            this->m_tab_container->addTab(xlet, tabTitle);
            this->m_xlets_tab_widget.insert(xlet_id, xlet);
        }
    }

    this->m_tab_container->setCurrentIndex(b_engine->getSettings()->value("display/lastfocusedtab").toInt());
}

void XletDispatcher::cleanXletsTab()
{
    if (this->m_xlets_tab.size() == 0) {
        return;
    }

    b_engine->getSettings()->setValue("display/lastfocusedtab", this->m_tab_container->currentIndex());
    foreach (QWidget *widget, this->m_xlets_tab_widget.values()) {
        widget->deleteLater();
    }
    this->m_tab_container->deleteLater();
    this->m_xlets_tab_widget.clear();
}

void XletDispatcher::prepareXletsDock()
{
    if (this->m_xlets_dock.size() == 0) {
        return;
    }

    foreach (QString xlet_id, this->m_xlets_dock.keys()) {
        QString options = this->m_xlets_dock.value(xlet_id);
        QDockWidget::DockWidgetFeatures features = this->getXletsDockFeatures(options);
        XLet *xlet = XLetFactory::spawn(xlet_id, this->m_main_widget);
        if (xlet) {
            xlet->doGUIConnects(this->m_main_window);
            QDockWidget *dockWidget = new QDockWidget(xlet->title(), this->m_main_widget);
            dockWidget->setFeatures(features);
            dockWidget->setAllowedAreas(Qt::BottomDockWidgetArea);
            dockWidget->setObjectName(xlet_id);
            if (options.contains("s")) { // with scrollbar ?
                QScrollArea *dockWidgetContents = new QScrollArea(this->m_main_widget);
                dockWidgetContents->setWidget(xlet);
                dockWidgetContents->setWidgetResizable(true);
                dockWidget->setWidget(dockWidgetContents);
            } else {
                dockWidget->setWidget(xlet);
            }
            this->m_main_window->addDockWidget(Qt::BottomDockWidgetArea, dockWidget);
            this->m_xlets_dock_widget.insert(xlet_id, dockWidget);
        }
    }
}

QDockWidget::DockWidgetFeatures XletDispatcher::getXletsDockFeatures(QString &options)
{
    QDockWidget::DockWidgetFeatures features = QDockWidget::NoDockWidgetFeatures;
    if (options.contains("c")) {
        features |= QDockWidget::DockWidgetClosable;
    }
    if (options.contains("f")) {
        features |= QDockWidget::DockWidgetFloatable;
    }
    if (options.contains("m")) {
        features |= QDockWidget::DockWidgetMovable;
    }
    return features;
}

void XletDispatcher::cleanXletsDock()
{
    foreach (QDockWidget *widget, this->m_xlets_dock_widget.values()) {
        this->m_main_window->removeDockWidget(widget);
        widget->deleteLater();
    }
    this->m_xlets_dock_widget.clear();
}

/*! \brief show this XLet on top of others
 *
 * This slot Works when tabbed view is selected.
 */
void XletDispatcher::showWidgetOnTop(QWidget *widget)
{
    if (this->m_tab_container)
        this->m_tab_container->setCurrentWidget(widget);
}

void XletDispatcher::prepareAppearance()
{
    QVariantList xlets_infos = b_engine->getCapaXlets();
    foreach (QVariant qvariant_xlet_infos, xlets_infos) {
        QStringList xlet_infos_raw = qvariant_xlet_infos.toStringList();
        if (xlet_infos_raw.size() > 0) {
            QString name = xlet_infos_raw[0];
            if ((name == "customerinfo") && (! b_engine->checkedFunction(name)))
                continue;

            if (xlet_infos_raw.size() > 1) {
                QString otions = "";
                if (xlet_infos_raw.size() > 2) {
                    otions = xlet_infos_raw[2];
                }
                QString type = xlet_infos_raw[1];
                if (type == "dock") {
                    this->m_xlets_dock.insert(name, otions);
                } else if (type == "grid") {
                    this->m_xlets_grid.insert(name, otions);
                } else if (type == "tab") {
                    this->m_xlets_tab.insert(name, otions);
                }
            }
        }
    }
}

void XletDispatcher::clearAppearance()
{
    m_xlets_dock.clear();
    m_xlets_grid.clear();
    m_xlets_tab.clear();
}

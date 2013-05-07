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

#include <baseengine.h>
#include <xivoconsts.h>
#include <xletfactory.h>
#include <application_status_icon.h>

#include "main_widget.h"
#include "menu_availability.h"


MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
      m_config_widget(NULL),
      m_appliname(tr("Client %1").arg(XC_VERSION))
{
    b_engine->setParent(this);
    this->ui.setupUi(this);
    this->updateAppliName();

    QSettings *qsettings = b_engine->getSettings();
    restoreGeometry(qsettings->value("display/mainwingeometry").toByteArray());

    b_engine->logAction("application started on " + b_engine->osname());

    this->connect(b_engine, SIGNAL(logged()), SLOT(engineStarted()));
    this->connect(b_engine, SIGNAL(delogged()), SLOT(engineStopped()));
    this->connect(b_engine, SIGNAL(settingsChanged()), SLOT(confUpdated()));
    this->connect(this->ui.action_configure, SIGNAL(triggered()), SLOT(showConfDialog()));
    qApp->connect(this->ui.action_quit, SIGNAL(triggered()), SLOT(quit()));
    b_engine->connect(this->ui.action_quit, SIGNAL(triggered()), SLOT(stop()));
    b_engine->connect(this->ui.action_connect, SIGNAL(triggered()), SLOT(start()));
    b_engine->connect(this->ui.action_disconnect, SIGNAL(triggered()), SLOT(stop()));

    this->m_menu_availability = new MenuAvailability(this->ui.menu_availability);
    this->m_menu_statusbar = new Statusbar(this->ui.statusbar);
    this->m_login_widget = new LoginWidget(this->ui.stacked_widget);
    this->m_main_widget = new QWidget(this->ui.stacked_widget);

    this->ui.stacked_widget->addWidget(this->m_login_widget);
    this->ui.stacked_widget->addWidget(this->m_main_widget);

    this->m_login_widget->setConfig();
    this->showLogin();
}

MainWindow::~MainWindow()
{
    b_engine->getSettings()->setValue("display/mainwingeometry", saveGeometry());
    b_engine->logAction("application quit");
}

void MainWindow::updateAppliName()
{
    setWindowTitle(QString("XiVO %1").arg(this->m_appliname));
    //if (m_withsystray) {
    //    m_systrayIcon.setToolTip(QString("XiVO %1").arg(m_appliname));
    //}
}

void MainWindow::showMessageBox(const QString & message)
{
    QMessageBox::critical(NULL, tr("XiVO CTI Error"), message);
}

void MainWindow::showLogin()
{
    this->ui.stacked_widget->setCurrentWidget(this->m_login_widget);
}

void MainWindow::hideLogin()
{
    this->ui.stacked_widget->setCurrentWidget(this->m_main_widget);
}

void MainWindow::showConfDialog()
{
    this->m_login_widget->saveConfig();
    this->m_config_widget = new ConfigWidget(this->ui.stacked_widget);
    this->m_config_widget->show();
    this->connect(this->m_config_widget, SIGNAL(finished(int)), SLOT(cleanConfDialog()));
}

void MainWindow::cleanConfDialog()
{
    this->disconnect(this->m_config_widget, SIGNAL(finished(int)));
    this->m_config_widget = NULL;
}

void MainWindow::confUpdated()
{
    this->m_login_widget->setAgentLoginWidgetsVisible();
    this->m_menu_statusbar->confUpdated();

}

void MainWindow::engineStarted()
{
    qDebug() << Q_FUNC_INFO;
    this->hideLogin();
    this->connectionStateChanged();
}

void MainWindow::engineStopped()
{
    qDebug() << Q_FUNC_INFO;
    this->connectionStateChanged();
    b_engine->getSettings()->setValue("display/mainwindowstate", saveState());
    this->m_menu_availability->clearPresence();
    this->showLogin();
}

void MainWindow::connectionStateChanged()
{
    if (b_engine->state() == BaseEngine::ELogged) {
        statusBar()->showMessage(tr("Connected"));
        b_engine->logAction("connection started");
        this->m_menu_availability->setMenuAvailabilityEnabled(true);
        this->ui.action_connect->setVisible(false);
        this->ui.action_disconnect->setVisible(true);
        this->ui.action_disconnect->setEnabled(true);

    } else if (b_engine->state() == BaseEngine::ENotLogged) {
        statusBar()->showMessage(tr("Disconnected"));
        this->m_menu_availability->setMenuAvailabilityEnabled(false);
        this->ui.action_connect->setVisible(true);
        this->ui.action_disconnect->setVisible(false);
        b_engine->logAction("connection stopped");
    }
}

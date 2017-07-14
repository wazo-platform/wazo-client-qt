/*
 * Copyright 2013-2017 The Wazo Authors  (see the AUTHORS file)
 *
 * This program is free software: you can redistribute it and/or modify
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
 * This programm is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <baseengine.h>
#include <xivoconsts.h>
#include <xletfactory.h>
#include <assembler.h>

#include "main_window.h"
#include "menu_availability.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      m_config_widget(NULL),
      m_default_state(NULL),
      m_launch_date_time(QDateTime::currentDateTime()),
      m_folded(false)
{
    this->ui->setupUi(this);
    b_engine->setParent(this);
    b_engine->logAction("application started on " + b_engine->osname());

    QFontDatabase fontDB;
    fontDB.addApplicationFont(":/fonts/Dyno-Regular.ttf");
    fontDB.addApplicationFont(":/fonts/LiberationSans-Regular.ttf");
    QFile qssFile(QString(":/default.qss"));
    if(qssFile.open(QIODevice::ReadOnly)) {
        this->setStyleSheet(qssFile.readAll());
    }

    this->createTrayIcon();
    m_unfolded_size = this->size();

    connect(b_engine, SIGNAL(logged()),
            this, SLOT(setStatusLogged()));
    connect(b_engine, SIGNAL(delogged()),
            this, SLOT(setStatusNotLogged()));
    connect(b_engine, SIGNAL(emitMessageBox(const QString &)),
            this, SLOT(showMessageBox(const QString &)), Qt::QueuedConnection);
    connect(this->ui->action_configure, SIGNAL(triggered()),
            this, SLOT(showConfDialog()));
    connect(this->ui->action_to_systray, SIGNAL(triggered()),
            this, SLOT(hideWindow()));
    connect(this->ui->action_show_window, SIGNAL(triggered()),
            this, SLOT(showWindow()));
    connect(this->ui->action_about_client, SIGNAL(triggered()),
            this, SLOT(about()));
    connect(this->ui->action_credits, SIGNAL(triggered()),
            this, SLOT(showCredits()));
    connect(this->ui->action_about_qt, SIGNAL(triggered()),
            qApp, SLOT(aboutQt()));
    connect(this->ui->action_quit, SIGNAL(triggered()),
            qApp, SLOT(quit()));
    connect(this->ui->action_quit, SIGNAL(triggered()),
            b_engine, SLOT(stop()));
    connect(this->ui->action_connect, SIGNAL(triggered()),
            b_engine, SLOT(start()));
    connect(this->ui->action_disconnect, SIGNAL(triggered()),
            b_engine, SLOT(stop()));

    connect(m_tray_icon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(systrayActivated(QSystemTrayIcon::ActivationReason)));
    connect(m_tray_icon, SIGNAL(messageClicked()),
            this, SLOT(showWindow()));

}

MainWindow::~MainWindow()
{
    b_engine->getSettings()->setValue("display/mainwingeometry", QMainWindow::saveGeometry());
    b_engine->logAction("application quit");
    delete this->ui;
    this->ui = NULL;
}

void MainWindow::initialize()
{
    bool start_minimized = b_engine->getConfig("systrayed").toBool();

    this->m_config_widget = assembler->configWidget();
    QMainWindow::restoreGeometry(b_engine->getSettings()->value("display/mainwingeometry").toByteArray());
    this->setAppIcon("default");
    this->setTitle(tr("Client %1").arg(XC_VERSION));
    if (! start_minimized) {
        this->show();
    }
    this->setFocusPolicy(Qt::StrongFocus);
    m_tray_icon->show();
    emit this->initialized();
}

void MainWindow::setAppIcon(const QString & def)
{
    QIcon icon;
    if (def == "xivo-transp") { // connected
        icon = QIcon(":/images/xivoicon-orange.png");
    } else if (def == "xivo-red") { // agent_paused
        icon = QIcon(":/images/xivoicon-red.png");
    } else if (def == "xivo-green") { // agent_logged
        icon = QIcon(":/images/xivoicon-green.png");
    } else { // disconnected
        icon = QIcon(":/images/xivoicon-black.png");
    }
    this->setWindowIcon(icon);
    m_tray_icon->setIcon(icon);
}

void MainWindow::setTitle(const QString & app_title)
{
    QString title = QString("XiVO %1").arg(app_title);
    this->setWindowTitle(title);
    m_tray_icon->setToolTip(title);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    event->ignore();
    this->hideWindow();
}

void MainWindow::showWindow()
{
    this->setVisible(true);
    this->showNormal();
    this->activateWindow();
    this->ui->action_show_window->setVisible(false);
    this->ui->action_to_systray->setVisible(true);
}

void MainWindow::hideWindow()
{
    if(QSystemTrayIcon::isSystemTrayAvailable()) {
        this->setVisible(false);
    } else {
        this->minimizeWindow();
    }
    this->ui->action_show_window->setEnabled(true);
    this->ui->action_show_window->setVisible(true);
    this->ui->action_to_systray->setVisible(false);
}

void MainWindow::minimizeWindow()
{
    this->showMinimized();
}

void MainWindow::showMessageBox(const QString & message)
{
    QMessageBox::critical(NULL, tr("XiVO CTI Error"), message);
}

void MainWindow::showConfDialog()
{
    this->m_config_widget->show();
    connect(this->m_config_widget, SIGNAL(finished(int)),
            this, SLOT(cleanConfDialog()));
}

void MainWindow::cleanConfDialog()
{
    this->disconnect(this->m_config_widget, SIGNAL(finished(int)));
    this->m_config_widget->hide();
}

void MainWindow::setStatusLogged()
{
    QString app_title = tr("Client %1 (%2 profile)")
        .arg(XC_VERSION)
        .arg(b_engine->getCapaApplication());

    this->setTitle(app_title);
    this->setAppIcon("xivo-transp");
    this->ui->action_connect->setVisible(false);
    this->ui->action_disconnect->setVisible(true);
    this->ui->action_disconnect->setEnabled(true);
    b_engine->logAction("connection started");
}

void MainWindow::setStatusNotLogged()
{
    this->ui->action_connect->setVisible(true);
    this->ui->action_disconnect->setVisible(false);
    b_engine->logAction("connection stopped");
    this->setAppIcon("xivo-black");
    this->setTitle(tr("Client %1").arg(XC_VERSION));
}

void MainWindow::prepareState()
{
    this->saveDefaultState();
    this->restoreStateFromConfigFile();
}

void MainWindow::saveDefaultState()
{
    this->m_default_state = QMainWindow::saveState();
}

void MainWindow::restoreDefaultState()
{
    QMainWindow::restoreState(this->m_default_state);
}

void MainWindow::saveStateToConfigFile()
{
    b_engine->getSettings()->setValue("display/mainwindowstate", QMainWindow::saveState());
}

void MainWindow::restoreStateFromConfigFile()
{
    QMainWindow::restoreState(b_engine->getSettings()->value("display/mainwindowstate").toByteArray());
}

bool MainWindow::isFolded()
{
    return m_folded;
}

void MainWindow::setFolded(const QSize size)
{
    if (m_folded) {
        return;
    }
    m_folded = true;

    m_minimum_height = this->minimumHeight();
    if (! (this->isMaximized() || this->isFullScreen())) {
        m_unfolded_size = this->size();
    }

    Qt::WindowStates state = this->windowState();
    // Disable WindowMaximized and/or WindowFullScreen
    // setWindowState resolve glitch when fold/unfold while maximized
    // BUG WHEN PREVIOUSLY MAXIMIZED/FULLSCREEN: 
    // resizeEvent is only executed after this function, 
    // so this->size() return the old (maximized) size
    this->setWindowState(state & ~Qt::WindowMaximized & ~Qt::WindowFullScreen);

    // setFixedHeight probably takes this->size().width() for not changing width
    this->setFixedHeight(size.height()
                         + this->statusBar()->height()
                         + this->menuBar()->height()
                        );

    this->setWindowState(state);

}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    // we need to keep the size before maximizing, because of "bug" setWindowState
    // The only resize event with (isMiximized == true) is when you maximized, so the
    // oldSize will be the minimized size
    if ((this->isMaximized() || this->isFullScreen())) {
        if (m_folded) {
            m_unfolded_size.setWidth(event->oldSize().width());
        } else {
            m_unfolded_size = event->oldSize();
        }
    }
}

void MainWindow::restoreFolded()
{
    if (! m_folded) {
        return;
    }

    if (! (this->isMaximized() || this->isFullScreen())) {
        m_unfolded_size.setWidth(this->size().width());
    }

    Qt::WindowStates state = this->windowState();
    this->setWindowState(state & ~Qt::WindowMaximized & ~Qt::WindowFullScreen);

    this->setMaximumHeight(QWIDGETSIZE_MAX);
    this->setMinimumHeight(m_minimum_height);

    this->resize(m_unfolded_size);

    this->setWindowState(state);

    m_folded = false;
}

void MainWindow::createTrayIcon()
{

    m_tray_menu = new QMenu(QString("SystrayMenu"), this);
    m_menu_availability = new MenuAvailability(tr("&Availability"), m_tray_menu);

    m_tray_menu->addAction(this->ui->action_configure);
    m_tray_menu->addSeparator();
    #ifndef Q_OS_LINUX
    m_tray_menu->addMenu(m_menu_availability);
    #endif
    m_tray_menu->addSeparator();
    m_tray_menu->addAction(this->ui->action_connect);
    m_tray_menu->addAction(this->ui->action_disconnect);
    m_tray_menu->addSeparator();
    m_tray_menu->addAction(this->ui->action_show_window);
    m_tray_menu->addSeparator();
    m_tray_menu->addAction(this->ui->action_to_systray);
    m_tray_menu->addSeparator();
    m_tray_menu->addAction(this->ui->action_quit);

    m_tray_icon = new QSystemTrayIcon(this);
    m_tray_icon->setContextMenu(m_tray_menu);
}

void MainWindow::systrayActivated(QSystemTrayIcon::ActivationReason reason)
{
    if (reason == QSystemTrayIcon::Trigger) {
        #ifndef Q_OS_MAC
        if(this->isVisible()) {
            this->hideWindow();
        } else {
            this->showWindow();
        }
        #endif
    }
}

void MainWindow::customerInfoPopup(const QString & msgtitle,
                                   const QHash<QString, QString> & msgs,
                                   const QString & options)
{
    qDebug() << Q_FUNC_INFO;
    // systray popup
    // to be customisable (yes or no)
    if (options.contains("s") && (msgtitle.size() > 0)) {
        QStringList todisp;
        QStringList orders = msgs.keys();
        orders.sort();
        foreach (QString order, orders) {
            QString linetodisp = msgs[order];
            if (! linetodisp.isEmpty())
                todisp.append(linetodisp);
        }
        m_tray_icon->showMessage(msgtitle,
                                 todisp.join("\n"),
                                 QSystemTrayIcon::Information,
                                 5000);
    }

    // to be customisable, if the user wants the window to popup
    if (options.contains("p")) {
        this->showWindow();
    }
}

/*! \brief Displays the about box
 *
 * use QMessageBox::about() to display
 * the version and informations about the application
 */
void MainWindow::about()
{
    QString applicationVersion(XC_VERSION);
    QString fetchlastone = (QString("<a href=%1>%2</a>")
                            .arg("http://mirror.xivo.io/iso/xivo-current")
                            .arg(tr("Fetch the last one")));
    QString datebuild(QDateTime::fromString(__datebuild_client__, Qt::ISODate).toString());
    QString gitdate_qs(__git_date__);
    QString gitdate(QDateTime::fromTime_t(gitdate_qs.toUInt()).toString());

    // might be useful to display whether QSystemTrayIcon::isSystemTrayAvailable() is true
    QMessageBox::about(this,
                       tr("About XiVO Client"),
                       "<h3>XiVO " + tr("Client %1").arg(XC_VERSION) + "</h3>" +
                       tr("The XiVO CTI applications, once connected to the proper server, "
                          "complete the XiVO telephony system, by providing computer tools, "
                          "such as customer information display, directory searches, or various "
                          "supervision 'X-lets' for call centers.") +
                       "<p>" +
                       tr("<u>Version Information</u>") + "<br>" +
                       tr("Major : <b>%1</b> (XiVO branch)").arg(applicationVersion) + "<br>" +
                       tr("Minor : git hash <b>%1</b> from : %2").arg(__git_hash__).arg(gitdate) + "<br>" +
                       fetchlastone + "<br>" +
                       "<br>" +
                       tr("(Application Built on : %1)").arg(datebuild) + "<br>" +
                       tr("(Application Launched on : %1)").arg(this->m_launch_date_time.toString()) + "<br>" +
                       tr("(Config File Location : %1)").arg(b_engine->getSettings()->fileName()) + "<hr>" +
                       "Copyright (C) 2007-2016 <a href=http://www.avencall.com><b>Avencall</b></a>"
                       "<b>" + tr("E-mail : ") + "</b><a href=mailto:contact@avencall.com>contact@avencall.com</a><br>"
                       "<b>" + tr("Phone : ") + "</b>(+33 / 0) 1.41.38.99.60<br>" +
                       "<b>" + tr("Authors : ") + "</b>" + tr("Avencall Development Team") +
                       "<hr>"
                       "<b>" + tr("License : ") + "</b>" +
                       "<a href=http://www.gnu.org/licenses/gpl-3.0-standalone.html>GNU General Public License v3</a><br>"
                       + tr("with a special exception allowing you to link it with OpenSSL, under some conditions."));
}

void MainWindow::showCredits()
{
    QMessageBox::about(this,
                       tr("Credits"),
                       "<h3>Design</h3>"
                       " by <a href=\"http://www.kerosine.fr/\">Kerosine</a>."
                       "<h3>Dyno Sans Font</h3>"
                       " by <a href=\"http://grandchaos9000.deviantart.com\">Grandos Plex</a>"
                       " is licensed under a <a href=\"http://creativecommons.org/licenses/by-sa/4.0/\">Creative Commons Attribution-ShareAlike 4.0 International License</a>."
                       "<h3>Liberation Sans</h3>"
                       " by Ascender Corp."
                       " is licensed under a <a href=\"http://scripts.sil.org/OFL_web\">SIL Open Font License</a>."
                       "<h3>Font Awesome</h3>"
                       " by <a href=\"http://fontawesome.io\">Dave Gandy</a>"
                       " is licensed under a <a href=\"http://scripts.sil.org/OFL_web\">SIL Open Font License</a>."
                       "<h3>Font Awesome SVG-PNG</h3>"
                       " by <a href=\"https://github.com/encharm/Font-Awesome-SVG-PNG\">Code Charm Ltd</a>"
                       " is licensed under the <a href=\"http://opensource.org/licenses/MIT\">MIT license</a>."
                       );
}

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
#include <assembler.h>

#include "main_window.h"
#include "menu_availability.h"
#include "central_widget.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      m_config_widget(NULL),
      m_clipboard(NULL),
      m_default_state(NULL),
      m_launch_date_time(QDateTime::currentDateTime())
{
    this->ui->setupUi(this);
    b_engine->setParent(this);
    b_engine->logAction("application started on " + b_engine->osname());

    this->connect(b_engine, SIGNAL(logged()), SLOT(setStatusLogged()));
    this->connect(b_engine, SIGNAL(delogged()), SLOT(setStatusNotLogged()));
    this->connect(b_engine, SIGNAL(settingsChanged()), SLOT(confUpdated()));
    this->connect(b_engine, SIGNAL(emitMessageBox(const QString &)), SLOT(showMessageBox(const QString &)), Qt::QueuedConnection);
    this->connect(this->ui->action_configure, SIGNAL(triggered()), SLOT(showConfDialog()));
    this->connect(this->ui->action_to_systray, SIGNAL(triggered()), SLOT(hideWindow()));
    this->connect(this->ui->action_show_window, SIGNAL(triggered()), SLOT(showWindow()));
    this->connect(this->ui->action_about_client, SIGNAL(triggered()), SLOT(about()));
    this->connect(this->ui->action_credits, SIGNAL(triggered()), SLOT(showCredits()));
    qApp->connect(this->ui->action_about_qt, SIGNAL(triggered()), SLOT(aboutQt()));
    qApp->connect(this->ui->action_quit, SIGNAL(triggered()), SLOT(quit()));
    b_engine->connect(this->ui->action_quit, SIGNAL(triggered()), SLOT(stop()));
    b_engine->connect(this->ui->action_connect, SIGNAL(triggered()), SLOT(start()));
    b_engine->connect(this->ui->action_disconnect, SIGNAL(triggered()), SLOT(stop()));
}

MainWindow::~MainWindow()
{
    b_engine->getSettings()->setValue("display/mainwingeometry", saveGeometry());
    b_engine->logAction("application quit");
}

void MainWindow::initialize()
{
    bool start_minimized = b_engine->getConfig("systrayed").toBool();

    this->m_config_widget = assembler->configWidget();
    this->restoreGeometry(b_engine->getSettings()->value("display/mainwingeometry").toByteArray());
    this->setAppIcon("default");
    this->confUpdated();
    this->setTitle(tr("Client %1").arg(XC_VERSION));
    if (! start_minimized) {
        this->show();
    }
    this->setFocusPolicy(Qt::StrongFocus);
    emit this->initialized();
}

void MainWindow::setAppIcon(const QString & def)
{
    QIcon icon;
    if (def == "xivo-transp") { // connected
        icon = QIcon(":/images/xivo-login.png");
    } else if (def == "xivo-red") { // agent_paused
        icon = QIcon(":/images/xivoicon-red.png");
    } else if (def == "xivo-green") { // agent_logged
        icon = QIcon(":/images/xivoicon-green.png");
    } else { // disconnected
        icon = QIcon(":/images/xivoicon-black.png");
    }
    this->setWindowIcon(icon);
    emit this->iconUpdated(icon);
}

void MainWindow::setTitle(const QString & app_title)
{
    QString title = QString("XiVO %1").arg(app_title);
    this->setWindowTitle(title);
    emit this->titleUpdated(title);
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

void MainWindow::clipselection()
{
    b_engine->pasteToDial(this->m_clipboard->text(QClipboard::Selection));
}

void MainWindow::clipdata()
{
    b_engine->pasteToDial(this->m_clipboard->text(QClipboard::Clipboard));
}

void MainWindow::showMessageBox(const QString & message)
{
    QMessageBox::critical(NULL, tr("XiVO CTI Error"), message);
}

void MainWindow::showConfDialog()
{
    this->m_config_widget->show();
    this->connect(this->m_config_widget, SIGNAL(finished(int)), SLOT(cleanConfDialog()));
}

void MainWindow::cleanConfDialog()
{
    this->disconnect(this->m_config_widget, SIGNAL(finished(int)));
    this->m_config_widget->hide();
}

void MainWindow::confUpdated()
{
    if (b_engine->getConfig("enableclipboard").toBool()) {
        this->m_clipboard = QApplication::clipboard();
        this->connect(this->m_clipboard, SIGNAL(selectionChanged()), SLOT(clipselection()));
        this->connect(this->m_clipboard, SIGNAL(dataChanged()), SLOT(clipdata()));
        this->m_clipboard->setText("", QClipboard::Selection);
    }
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
    this->restoreState();
}

void MainWindow::saveDefaultState()
{
    this->m_default_state = QMainWindow::saveState();
}

void MainWindow::restoreDefaultState()
{
    QMainWindow::restoreState(this->m_default_state);
}

QByteArray MainWindow::saveState()
{
    b_engine->getSettings()->setValue("display/mainwindowstate", QMainWindow::saveState());
    return QMainWindow::saveState();
}

void MainWindow::restoreState()
{
    QMainWindow::restoreState(b_engine->getSettings()->value("display/mainwindowstate").toByteArray());
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
        assembler->systemTrayIcon()->showMessage(msgtitle,
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
    QString fetchlastone = QString("<a href=http://mirror.xivo.fr/iso>"
                                   "%1"
                                   "</a>").arg(tr("Fetch the last one"));
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
                       "Copyright (C) 2007-2013 <a href=http://www.avencall.com><b>Avencall</b></a>"
                       "<br>"
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
                       "<h3>JsonQt</h3>"
                       "<p>Copyright (c) 2008, Frederick Emmott <mail@fredemmott.co.uk></p>"
                       "<p></p>"
                       "<p>Permission to use, copy, modify, and/or distribute this software for any "
                       "purpose with or without fee is hereby granted, provided that the above "
                       "copyright notice and this permission notice appear in all copies.</p>"
                       "<p></p>"
                       "<p>THE SOFTWARE IS PROVIDED \"AS IS\" AND THE AUTHOR DISCLAIMS ALL WARRANTIES "
                       "WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF "
                       "MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR "
                       "ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES "
                       "WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN "
                       "ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF "
                       "OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.</p>"
                       );
}

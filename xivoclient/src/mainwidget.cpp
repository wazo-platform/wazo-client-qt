/* XiVO Client
 * Copyright (C) 2007-2011, Avencall
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

#include "mainwidget.h"
#include "baseengine.h"
#include "configwidget.h"
#include "xivoconsts.h"
#include "xletfactory.h"

/*! \brief Constructor
 *
 * Construct the Widget with all subwidgets.
 * The geometry is restored from settings.
 * engine object ownership is taken
 */
MainWidget::MainWidget()
    : QMainWindow(NULL),
      m_systrayIcon(0),
      m_icon_transp(":/images/xivo-login.png"),
      m_icon_red(":/images/xivoicon-red.png"),
      m_icon_green(":/images/xivoicon-green.png"),
      m_icon_black(":/images/xivoicon-black.png"),
      m_pixmap_disconnected(QPixmap(":/images/disconnected.png")
                            .scaledToHeight(18, Qt::SmoothTransformation)),
      m_pixmap_connected(QPixmap(":/images/connected.png")
                         .scaledToHeight(18, Qt::SmoothTransformation)),
      m_withsystray(true),
      m_centralWidget(new QStackedWidget(this)),
      m_resizingHelper(0),
      m_configwindow(NULL),
      m_clipboard(NULL)
{
    b_engine->setParent(this); // take ownership of the engine object
    
    fetchConfig();
    
    m_appliname = tr("Client %1").arg(XC_VERSION);

    m_profilename = new QLabel(this);
    bool displayprofile = b_engine->getConfig("displayprofile").toBool();
    if (m_profilename && displayprofile)
        m_profilename->show();
    else
        m_profilename->hide();
    statusBar()->addPermanentWidget(m_profilename);

    m_padlock = new QLabel(this);
    QPixmap padlock_pixmap = QPixmap(":/images/padlock.png")
                             .scaledToHeight(18, Qt::SmoothTransformation);
    m_padlock->setPixmap(padlock_pixmap);
    m_padlock->hide();
    statusBar()->addPermanentWidget(m_padlock);

    m_status = new QLabel(this);
    m_status->setPixmap(m_pixmap_disconnected);
    statusBar()->addPermanentWidget(m_status);

    setWindowTitle(QString("XiVO %1").arg(m_appliname));
    setDockOptions(QMainWindow::AllowNestedDocks);
    setAnimated(false);

    createActions();
    createMenus();

    if (m_withsystray && QSystemTrayIcon::isSystemTrayAvailable())
        createSystrayIcon();

    connect(b_engine, SIGNAL(logged()),
            this, SLOT(engineStarted()));
    connect(b_engine, SIGNAL(delogged()),
            this, SLOT(engineStopped()));
    connect(b_engine, SIGNAL(emitTextMessage(const QString &)),
            statusBar(), SLOT(showMessage(const QString &)));
    connect(b_engine, SIGNAL(emitMessageBox(const QString &)),
            this, SLOT(showMessageBox(const QString &)),
            Qt::QueuedConnection);
    connect(b_engine, SIGNAL(settingsChanged()),
            this, SLOT(confUpdated()));
    connect(b_engine, SIGNAL(localUserInfoDefined()), this, SLOT(updatePresence()));
    connect(b_engine, SIGNAL(updateUserStatus(const QString &)),
            this, SLOT(updateUserStatus(const QString &)));

    bool enableclipboard =  b_engine->getConfig("enableclipboard").toBool();
    if (enableclipboard) {
        m_clipboard = QApplication::clipboard();
        connect(m_clipboard, SIGNAL(selectionChanged()),
                this, SLOT(clipselection()));
        connect(m_clipboard, SIGNAL(dataChanged()),
                this, SLOT(clipdata()));
        m_clipboard->setText("", QClipboard::Selection); // see comment in MainWidget::clipselection()
    }

    resize(500, 440);
    QSettings *qsettings = b_engine->getSettings();
    restoreGeometry(qsettings->value("display/mainwingeometry").toByteArray());

    b_engine->logAction("application started on " + b_engine->osname());

    setCentralWidget(m_centralWidget);

    m_wid = new QWidget(m_centralWidget);
    m_centralWidget->addWidget(m_wid);
    m_vL = new QVBoxLayout(m_wid);

    m_launchDateTime = QDateTime::currentDateTime();

    makeLoginWidget();
    showLogin();
    
    bool systrayed = b_engine->getConfig("systrayed").toBool();
    if ((m_withsystray && ( systrayed == false)) || (! m_withsystray)) {
        show();
    }
    setFocusPolicy(Qt::StrongFocus);
}

/*! \brief Destructor
 *
 * The Geometry settings are saved for use by the new instance
 */
MainWidget::~MainWidget()
{
    b_engine->getSettings()->setValue("display/mainwingeometry", saveGeometry());
    b_engine->logAction("application quit");
}

void MainWidget::makeLoginWidget()
{
    m_loginWidget = new QWidget(m_centralWidget);
    m_loginWidget->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    m_centralWidget->addWidget(m_loginWidget);
    QGridLayout *loginL = new QGridLayout(m_loginWidget);
    loginL->setRowStretch(0, 1);
    loginL->setColumnStretch(0, 1);
    loginL->setColumnStretch(2, 1);
    loginL->setRowStretch(6, 1);

    QLabel *xivoBg = new QLabel();
    xivoBg->setPixmap(QPixmap(":/images/xivoicon.png"));
    xivoBg->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    loginL->addWidget(xivoBg, 1, 1, Qt::AlignHCenter | Qt::AlignVCenter);

    m_lab1 = new QLabel(tr("Login"));
    loginL->addWidget(m_lab1, 2, 0, Qt::AlignRight);
    m_lab2 = new QLabel(tr("Password"));
    loginL->addWidget(m_lab2, 3, 0, Qt::AlignRight);
    m_lab3 = new QLabel(tr("Phone"));
    loginL->addWidget(m_lab3, 4, 0, Qt::AlignRight);

    m_qlab1 = new QLineEdit();
    m_qlab1->setText(m_config["userlogin"].toString());
    loginL->addWidget(m_qlab1, 2, 1);
    m_qlab2 = new QLineEdit();
    m_qlab2->setText(m_config["password"].toString());
    m_qlab2->setEchoMode(QLineEdit::Password);
    loginL->addWidget(m_qlab2, 3, 1);
    m_qlab3 = new QLineEdit();
    m_qlab3->setText(m_config["agentphonenumber"].toString());
    loginL->addWidget(m_qlab3, 4, 1);

    m_ack = new QPushButton("OK");
    loginL->addWidget(m_ack, 2, 2, Qt::AlignLeft);
    m_kpass = new QCheckBox(tr("Keep Password"));
    m_kpass->setChecked(m_config["keeppass"].toBool());
    loginL->addWidget(m_kpass, 3, 2, Qt::AlignLeft);
    m_loginkind = new QComboBox();
    m_loginkind->addItem(QString(tr("No Agent")));
    m_loginkind->addItem(QString(tr("Agent (unlogged)")));
    m_loginkind->addItem(QString(tr("Agent (logged)")));
    m_loginkind->setCurrentIndex(m_config["guioptions.loginkind"].toInt());
    loginL->addWidget(m_loginkind, 4, 2, Qt::AlignLeft);

    setAgentLoginWidgetsVisible();
    m_qlab1->setFocus();

    connect(m_qlab1, SIGNAL(returnPressed()),
             this, SLOT(setConfigAndStart()));
    connect(m_qlab2, SIGNAL(returnPressed()),
             this, SLOT(setConfigAndStart()));
    connect(m_qlab3, SIGNAL(returnPressed()),
             this, SLOT(setConfigAndStart()));
    connect(m_ack, SIGNAL(pressed()),
             this, SLOT(setConfigAndStart()));
    connect(m_loginkind, SIGNAL(currentIndexChanged(int)),
             this, SLOT(syncAgentLoginWidgets()));
}

void MainWidget::clipselection()
{
    QString selected = m_clipboard->text(QClipboard::Selection);
    b_engine->pasteToDial(selected);

    // X11 : when a pattern is selected on (seemingly) any KDE(QT) application on Linux
    // X11 (non-KDE) : we don't get the signal, but the data can be retrieved anyway (the question "when ?" remains)

    // X11 (non-KDE) : force a selection to be owned, so that the next not-owned one will be catched
    // if(selected.size() > 0) // avoid infinite loop, however
    // m_clipboard->setText("", QClipboard::Selection);

    // might be boring anyway, since the selected texts disappear at once wherever you are
    // the following does not fix it :
    // if((selected.size() > 0) && (! m_clipboard->ownsSelection()))
    // m_clipboard->setText(selected, QClipboard::Selection);
}

void MainWidget::clipdata()
{
    b_engine->pasteToDial(m_clipboard->text(QClipboard::Clipboard));

    // WIN : we fall here in any Ctrl-C/Ctrl-X/"copy"/... action
    // X11 : same actions, on (seemingly) any KDE(QT) application
    // X11 (non-KDE) : we don't get the signal, but the data can
    //                 be retrieved anyway (the question "when ?" remains)
    //                 however, the xclipboard application seems to be
    //                 able to catch such signals ...
}

void MainWidget::setAppearance(const QVariantList & dockoptions)
{
    foreach (QVariant dproperties, dockoptions) {
        QStringList dopt = dproperties.toStringList();
        if (dopt.size() > 0) {
            QString wname = dopt[0];
            if ((wname == "customerinfo") && (! b_engine->checkedFunction(wname)))
                continue;
            m_allnames.append(wname);
            m_dockoptions[wname] = "";
            if (dopt.size() > 1) {
                if (dopt[1] == "dock") {
                    m_docknames.append(wname);
                } else if (dopt[1] == "grid")
                    m_gridnames.append(wname);
                else if (dopt[1] == "tab")
                    m_tabnames.append(wname);
                if (dopt.size() > 2)
                    m_dockoptions[wname] = dopt[2];
            } else {
                m_docknames.append(dopt[0]);
            }
        }
    }

    qDebug() << "dock : " << m_docknames;
    qDebug() << "grid : " << m_gridnames;
    qDebug() << "tab  : " << m_tabnames;
    qDebug() << "all  : " << m_allnames;
}

void MainWidget::clearAppearance()
{
    m_docknames.clear();
    m_gridnames.clear();
    m_tabnames.clear();
    m_allnames.clear();
}

void MainWidget::setConfig()
{
    m_config["userlogin"] = m_qlab1->text();
    m_config["password"] = m_qlab2->text();
    m_config["agentphonenumber"] = m_qlab3->text();
    m_config["keeppass"] = m_kpass->isChecked();
    m_config["guioptions.loginkind"] = m_loginkind->currentIndex();
    b_engine->setConfig(m_config);
}

void MainWidget::setConfigAndStart()
{
    setConfig();
    b_engine->start();
}

/*! \brief Displays the agent login widgets or not, according to the config
 */

void MainWidget::setAgentLoginWidgetsVisible() {
    bool showagselect = b_engine->getConfig("showagselect").toBool();
    if (showagselect) {
        m_loginkind->show();
        syncAgentLoginWidgets();
    } else {
        m_lab3->hide();
        m_qlab3->hide();
        m_loginkind->hide();
    }
}

/*! \brief Displays the right agent login widgets, according to m_loginkind
 */
void MainWidget::syncAgentLoginWidgets()
{
    int index = m_loginkind->currentIndex();
    if (index == 0) {
        m_lab3->hide();
        m_qlab3->hide();
    } else if (index > 0) {
        m_lab3->show();
        m_qlab3->show();
    }
    m_loginkind->show();
}

/*! \brief hide "main" window and show login widget
 */
void MainWidget::showLogin()
{
    m_centralWidget->setCurrentWidget(m_loginWidget);
    m_resizingHelper = 0;
}

/*! \brief hide login widget and show "Main" window
 */
void MainWidget::hideLogin()
{
    m_centralWidget->setCurrentWidget(m_wid);
}

void MainWidget::createActions()
{
    m_cfgact = new QAction(tr("Confi&gure"), this);
    m_cfgact->setMenuRole(QAction::PreferencesRole);
    m_cfgact->setStatusTip(tr("Configure account and connection options"));
    connect(m_cfgact, SIGNAL(triggered()),
            this, SLOT(showConfDialog()));

    m_quitact = new QAction(tr("&Quit"), this);
    m_quitact->setProperty("stopper", "quit");
    m_quitact->setStatusTip(tr("Close the application"));
    m_quitact->setShortcut(QKeySequence(tr("Ctrl+Q")));
    connect(m_quitact, SIGNAL(triggered()),
            b_engine, SLOT(stop()));
    connect(m_quitact, SIGNAL(triggered()),
            qApp, SLOT(quit()));

    if (m_withsystray) {
        m_systraymin = new QAction(tr("To S&ystray"), this);
        m_systraymin->setStatusTip(tr("Enter the system tray"));
        connect(m_systraymin, SIGNAL(triggered()),
                 this, SLOT(hide()));
        m_systraymin->setEnabled(QSystemTrayIcon::isSystemTrayAvailable());

        m_systraymax = new QAction(tr("&Show window"), this);
        m_systraymax->setStatusTip(tr("Leave the system tray"));
        connect(m_systraymax, SIGNAL(triggered()),
                this, SLOT(showNormal()));
        connect(m_systraymax, SIGNAL(triggered()),
                this, SLOT(show()));
        connect(m_systraymax, SIGNAL(triggered()),
                this, SLOT(raise()));
        m_systraymax->setEnabled(QSystemTrayIcon::isSystemTrayAvailable());
    }

    m_connectact = new QAction(tr("&Connect"), this);
    m_connectact->setStatusTip(tr("Connect to the server"));
    connect(m_connectact, SIGNAL(triggered()),
            b_engine, SLOT(start()));

    m_disconnectact = new QAction(tr("&Disconnect"), this);
    m_disconnectact->setProperty("stopper", "disconnect");
    m_disconnectact->setStatusTip(tr("Disconnect from the server"));
    connect(m_disconnectact, SIGNAL(triggered()),
            b_engine, SLOT(stop()));

    m_connectact->setEnabled(true);
    m_disconnectact->setEnabled(false);

    // Availability actions :
    m_availgrp = new QActionGroup(this);
    m_availgrp->setExclusive(true);
}

/*!
 * Change the selected state in the availability menu
 *
 * If this method is called without while the connections between the
 * m_avact[state] and b_engine are up, this will also update the state
 * on the server
 */
void MainWidget::checksAvailState()
{
    if (const UserInfo * u = b_engine->getXivoClientUser()) {
        const QString & state = u->availstate();
        if (! state.isEmpty() && m_avact.contains(state)) {
            setEnabledMenus(state);
            m_avact[state]->setChecked(true);
        }
    }
}

/*!
 * Enable or disable menu item according to the state
 * \param state The new state
 */
void MainWidget::setEnabledMenus(const QString & state)
{
    const QVariantMap & states = b_engine->getOptionsUserStatus();
    if (states.contains(state)) {
        const QStringList & allowed = states.value(state).toMap()
            .value("allowed").toStringList();
        foreach (const QString & presence, m_avact.keys()) {
            bool enabled = allowed.contains(presence);
            m_avact[presence]->setCheckable(enabled);
            m_avact[presence]->setEnabled(enabled);
        }
    }
}

/*!
 * Update the presence of the current user
 * \param userxid The user's XiVO id
 */
void MainWidget::updateUserStatus(const QString & userxid)
{
    if (b_engine->getFullId() == userxid) {
        const UserInfo * u = b_engine->getXivoClientUser();
        if (u && b_engine->getOptionsUserStatus().size()) {
            syncPresence();
        }
    }
}

/*!
 * Disconnect signals for the availability menu and select the current state
 */
void MainWidget::syncPresence()
{
    if (const UserInfo * u = b_engine->getXivoClientUser()) {
        const QString & state = u->availstate();
        if (m_avact.contains(state)) {
            checksAvailState();
        }
    }
}

void MainWidget::createMenus()
{
    m_filemenu = menuBar()->addMenu("&XiVO Client"); // + m_appliname too heavy
    m_filemenu->addAction(m_cfgact);
    if (m_withsystray)
        m_filemenu->addAction(m_systraymin);
    m_filemenu->addSeparator();
    m_filemenu->addAction(m_connectact);
    m_filemenu->addAction(m_disconnectact);
    m_filemenu->addSeparator();
    m_filemenu->addAction(m_quitact);

    m_avail = menuBar()->addMenu(tr("&Availability"));
    m_avail->setEnabled(false);

    m_helpmenu = menuBar()->addMenu(tr("&Help"));
    m_helpmenu->addAction(tr("&About XiVO Client"), this, SLOT(about()));
    m_helpmenu->addAction(tr("About &Qt"), qApp, SLOT(aboutQt()));
    m_helpmenu->addAction(tr("&Credits"), this, SLOT(showCredits()));
}

void MainWidget::updateAppliName()
{
    setWindowTitle(QString("XiVO %1").arg(m_appliname));
    if (m_withsystray && m_systrayIcon) {
        m_systrayIcon->setToolTip(QString("XiVO %1").arg(m_appliname));
    }
}

/*! \brief create and show the system tray icon
 *
 * Create the system tray icon, show it and connect its
 * activated() signal to some slot
 */
void MainWidget::createSystrayIcon()
{
    m_systrayIcon = new QSystemTrayIcon(this);
    setSystrayIcon("xivo-black");
    m_systrayIcon->setToolTip(QString("XiVO %1").arg(m_appliname));
    QMenu *menu = new QMenu(QString("SystrayMenu"), this);
    menu->addAction(m_cfgact);
    menu->addSeparator();
    menu->addMenu(m_avail);
    menu->addSeparator();
    menu->addAction(m_connectact);
    menu->addAction(m_disconnectact);
    menu->addSeparator();
#ifdef Q_WS_MAC
    menu->addAction(m_systraymax);
    menu->addSeparator();
#endif
    menu->addAction(m_quitact);
    m_systrayIcon->setContextMenu(menu);
    m_systrayIcon->show();
    //connect(m_systrayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
    //         this, SLOT(show()));
    connect(m_systrayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
             this, SLOT(systrayActivated(QSystemTrayIcon::ActivationReason)));
    connect(m_systrayIcon, SIGNAL(messageClicked()),
             this, SLOT(systrayMsgClicked()));
    // QSystemTrayIcon::ActivationReason
    // qDebug() << Q_FUNC_INFO << "QSystemTrayIcon::supportsMessages() = "
    //          << QSystemTrayIcon::supportsMessages();
}

/*! \brief show the Configuration Dialog
 *
 * create and execute a new ConfigWidget
 */
void MainWidget::showConfDialog()
{
    setConfig();
    m_configwindow = new ConfigWidget(this);
    m_configwindow->setModal(true);
    m_configwindow->show();
    connect(m_configwindow, SIGNAL(finished(int)),
            this, SLOT(cleanConfDialog()));
}

void MainWidget::cleanConfDialog()
{
    disconnect(m_configwindow, SIGNAL(finished(int)),
               this, SLOT(cleanConfDialog()));
    m_configwindow = NULL;
}

void MainWidget::fetchConfig()
{
    foreach (QString key, QStringList() << "userlogin"
                                        << "password"
                                        << "agentphonenumber"
                                        << "keeppass"
                                        << "guioptions.loginkind") {
        m_config[key] = b_engine->getConfig(key);
    }
}

void MainWidget::confUpdated()
{
    // qDebug() << Q_FUNC_INFO;
    fetchConfig();
    m_qlab1->setText(m_config["userlogin"].toString());
    m_qlab2->setText(m_config["password"].toString());
    m_qlab3->setText(m_config["agentphonenumber"].toString());
    m_kpass->setChecked(m_config["keeppass"].toBool());
    m_loginkind->setCurrentIndex(m_config["guioptions.loginkind"].toInt());
    
    bool displayprofile = b_engine->getConfig("displayprofile").toBool();
    m_profilename->setVisible(displayprofile);
    
    setMenuAvailabilityEnabled(true);
    setAgentLoginWidgetsVisible();
}

/*! \brief process clicks to the systray icon
 *
 * This slot is connected to the activated() signal of the
 * System Tray icon. It currently toggle the visibility
 * of the MainWidget on a simple left click. */
void MainWidget::systrayActivated(QSystemTrayIcon::ActivationReason reason)
{
    qDebug() << Q_FUNC_INFO
             << "reason =" << reason
             << "isMinimized =" << isMinimized()
             << "isVisible =" << isVisible()
             << "isActiveWindow =" << isActiveWindow();
    // QSystemTrayIcon::DoubleClick
    // QSystemTrayIcon::Trigger
    if (reason == QSystemTrayIcon::Trigger) {
#ifdef Q_WS_MAC
        // try to reduce potential problems under MacOS X
        if (isVisible())
            setVisible(false);
#else
        if (isVisible() && !isActiveWindow()) {
            showNormal();
            activateWindow();
            raise();
        } else {
            // Toggle visibility
            setVisible(!isVisible());
            if (isVisible()) {
                showNormal();
                activateWindow();
                raise();
            }
        }
#endif
    }
}

void MainWidget::showMessageBox(const QString & message)
{
    QMessageBox::critical(NULL, tr("XiVO CTI Error"), message);
}

/*!
 * This slot implementation show, activate (and raise) the
 * window.
 */
void MainWidget::systrayMsgClicked()
{
    qDebug() << Q_FUNC_INFO;
    setVisible(true);
    activateWindow();
    raise();
}

/*! \brief show this XLet on top of others
 *
 * This slot Works when tabbed view is selected.
 */
void MainWidget::showWidgetOnTop(QWidget * widget)
{
    if (m_tabwidget)
        m_tabwidget->setCurrentWidget(widget);
}

void MainWidget::addPanel(const QString &name, const QString &title, QWidget *widget)
{
    if (m_docknames.contains(name)) {
        qDebug() << Q_FUNC_INFO << "(dock)" << name << m_dockoptions[name];
        QDockWidget::DockWidgetFeatures features = QDockWidget::NoDockWidgetFeatures;
        if (m_dockoptions[name].contains("c"))
            features |= QDockWidget::DockWidgetClosable;
        if (m_dockoptions[name].contains("f"))
            features |= QDockWidget::DockWidgetFloatable;
        if (m_dockoptions[name].contains("m"))
            features |= QDockWidget::DockWidgetMovable;
        m_docks[name] = new QDockWidget(title);
        m_docks[name]->setFeatures(features);
        m_docks[name]->setAllowedAreas(Qt::BottomDockWidgetArea); // restrain the area to Bottom region
        m_docks[name]->setObjectName(name); // compulsory to allow a proper state's saving
        addDockWidget(Qt::BottomDockWidgetArea, m_docks[name]);
        m_docks[name]->hide();
        m_docks[name]->setWidget(widget);
    } else if (m_gridnames.contains(name)) {
        qDebug() << Q_FUNC_INFO << "(grid)" << name << m_dockoptions[name] << title << m_dockoptions[name].toInt();
        qDebug() << Q_FUNC_INFO << "inserting" << m_dockoptions[name].toInt();
        m_vL->insertWidget(m_dockoptions[name].toInt(), widget);
    } else if (m_tabnames.contains(name)) {
        qDebug() << Q_FUNC_INFO << "(tab) " << name << m_dockoptions[name] << m_tabwidget->count() << title;
        QString tabTitle = "  " + title + "  ";
        if (m_dockoptions[name].size() > 0) {
            m_tabwidget->insertTab(m_dockoptions[name].toInt(), widget, tabTitle);
        } else {
            m_tabwidget->addTab(widget, tabTitle);
        }
    }
}

/*!
 * Fills the list of available availability states and select the current state
 */
void MainWidget::updatePresence()
{
    const QVariantMap & presencemap = b_engine->getOptionsUserStatus();
    foreach (const QString & presencestate, presencemap.keys()) {
        const QVariantMap & pdetails = presencemap.value(presencestate).toMap();
        const QString & longname = pdetails.value("longname").toString();
            if (! m_avact.contains(presencestate)) {
                m_avact[presencestate] = new QAction(longname, this);
                m_avact[presencestate]->setProperty("availstate", presencestate);
                connect(m_avact[presencestate], SIGNAL(triggered()),
                        this, SLOT(setAvailability()));
                m_availgrp->addAction(m_avact[presencestate]);
            }
        }
        m_avail->addActions(m_availgrp->actions());
    syncPresence();
}

void MainWidget::clearPresence()
{
    foreach (QAction *action, m_avact) {
        disconnect(action, SIGNAL(triggered()),
                   this, SLOT(setAvailability()));
        m_availgrp->removeAction(action);
        delete action;
    }
    m_avact.clear();
    m_avail->clear();
}

/*!
 * enables the "Disconnect" action and disables the "Connect" Action.
 * sets the Green indicator
 * \sa engineStopped()
 */
void MainWidget::engineStarted()
{
    setAppearance(b_engine->getCapaXlets());
    m_appliname = tr("Client %1 (%2 profile)")
        .arg(XC_VERSION)
        .arg(b_engine->getCapaApplication());

    updateAppliName();
    hideLogin();

    m_tabwidget = new QTabWidget(this);
    m_tabwidget->hide();

    if (m_docknames.contains("tabber")) {
        m_tabwidget->show();
        addPanel("tabber", tr("Tabs"), m_tabwidget);
    }
    if (m_gridnames.contains("tabber")) {
        m_tabwidget->show();
        m_vL->addWidget(m_tabwidget);
    }

    foreach (QString xletid, m_allnames) {
        if (! QStringList("tabber").contains(xletid)) {
            bool withscrollbar = m_dockoptions[xletid].contains("s");
            XLet *xlet = XLetFactory::spawn(xletid, this);
            if (xlet) {
                m_xletlist.insert(xletid, xlet);
                xlet->doGUIConnects(this);
                if (withscrollbar) {
                    QScrollArea *sa_ag = new QScrollArea(this);
                    sa_ag->setWidget(xlet);
                    sa_ag->setWidgetResizable(true);
                    addPanel(xletid, xlet->title(), sa_ag);
                } else {
                    addPanel(xletid, xlet->title(), xlet);
                }
            } else {
                qDebug() << Q_FUNC_INFO << "cannot instantiate XLet" << xletid;
            }
        }
    }

    qDebug() << Q_FUNC_INFO << "the xlets have been created";
    m_tabwidget->setCurrentIndex(b_engine->getSettings()->value("display/lastfocusedtab").toInt());

    foreach (QString name, m_docks.keys())
        m_docks[name]->show();

    m_defaultState = saveState();
    // restore the saved state AFTER showing the docks
    restoreState(b_engine->getSettings()->value("display/mainwindowstate").toByteArray());

    if ((m_resizingHelper == 0)&&(m_docks.size())) {
        // we gonna resize this widget in resizeEvent
        // to force the mainWindow dockArea expand instead of the centralWidget
        m_resizingHelper =  new QDockWidget(this);
        m_resizingHelper->setFixedWidth(1);
        addDockWidget(Qt::BottomDockWidgetArea, m_resizingHelper);
        m_resizingHelper->show(); // not a no-op, show is needed, to force Qt to calc
        m_resizingHelper->hide(); // the widget size!
        removeDockWidget(m_resizingHelper);
    }

    if (m_withsystray && m_systrayIcon)
        setSystrayIcon("xivo-transp");

    connectionStateChanged();
}

void MainWidget::setSystrayIcon(const QString & def)
{
    QIcon icon;
    if (def == "xivo-transp") {
        icon = m_icon_transp;
    } else if (def == "xivo-red") {
        icon = m_icon_red;
    } else if (def == "xivo-green") {
        icon = m_icon_green;
    } else if (def == "xivo-black") {
        icon = m_icon_black;
    } else {
        int psize = 16;
        QPixmap square(psize, psize);
        square.fill(def);
        icon = QIcon(square);
    }

    if (m_systrayIcon) {
        m_systrayIcon->setIcon(icon);
    }
    setWindowIcon(icon);
}

void MainWidget::resetState() {
    // qDebug() << Q_FUNC_INFO;
    restoreState(m_defaultState);
}

void MainWidget::removePanel(const QString & name, QWidget * widget)
{
//    qDebug() << Q_FUNC_INFO << name << widget;
    if (m_docknames.contains(name)) {
        removeDockWidget(m_docks[name]);
        m_docks[name]->deleteLater();
        m_docks.remove(name);
    }
    if (m_tabnames.contains(name)) {
        int thisindex = m_tabwidget->indexOf(widget);
        if (thisindex > -1) {
            qDebug() << Q_FUNC_INFO << "removing tab" << name << thisindex;
            m_tabwidget->removeTab(thisindex);
        }
    }
    if (m_gridnames.contains(name)) {
        //m_gridlayout->removeWidget(widget);
        //delete widget;
        //widget->deleteLater();
    }
}

void MainWidget::connectionStateChanged()
{
    if (b_engine->state() == BaseEngine::ELogged) {
        statusBar()->showMessage(tr("Connected"));
        m_connectact->setEnabled(false);
        m_disconnectact->setEnabled(true);
        m_status->setPixmap(m_pixmap_connected);
        m_padlock->setVisible(b_engine->getConfig("cti_encrypt").toBool());
        m_profilename->setText(b_engine->getConfig("profilename").toString());
        setMenuAvailabilityEnabled(true);
        b_engine->logAction("connection started");
    } else if (b_engine->state() == BaseEngine::ENotLogged) {
        statusBar()->showMessage(tr("Disconnected"));
        m_connectact->setEnabled(true);
        m_disconnectact->setEnabled(false);
        m_status->setPixmap(m_pixmap_disconnected);
        m_padlock->hide();
        m_profilename->setText("");
        setMenuAvailabilityEnabled(false);
        b_engine->logAction("connection stopped");
    }
}

/*!
 * enables or disables the Availability menu
 */
void MainWidget::setMenuAvailabilityEnabled(bool enabled) {
    if (enabled) {
        bool presence_enabled = b_engine->getConfig("checked_function.presence").toBool();
        // qDebug() << Q_FUNC_INFO << presence_enabled;
        m_avail->setEnabled(presence_enabled);
    } else {
        m_avail->setEnabled(false);
    }
}

/*! \brief send the availability action clicked in the menu */
void MainWidget::setAvailability()
{
    QString availstate = sender()->property("availstate").toString();
    b_engine->setAvailState(availstate, false);
}

/*!
 * disables the "Disconnect" action and enables the "Connect" Action.
 * sets the Red indicator
 * \sa engineStarted()
 */
void MainWidget::engineStopped()
{
    // qDebug() << Q_FUNC_INFO;
    connectionStateChanged();
    b_engine->getSettings()->setValue("display/mainwindowstate", saveState());
    if (m_tabwidget->currentIndex() > -1) {
        b_engine->getSettings()->setValue("display/lastfocusedtab", m_tabwidget->currentIndex());
    }

    foreach (QString dname, m_docknames) {
        removePanel(dname, m_docks.value(dname));
    }
    clearPresence();

    // delete all xlets
    foreach (XLet *xlet, m_xletlist) {
        xlet->deleteLater();
    }
    m_xletlist.clear();

    if (m_docknames.contains("tabber")) {
        removePanel("tabber", m_tabwidget);
    }
    if (m_gridnames.contains("tabber")) {
        m_vL->removeWidget(m_tabwidget);
        m_tabwidget->deleteLater();
    }

    showLogin();

    if (m_withsystray && m_systrayIcon)
        setSystrayIcon("xivo-black");

    clearAppearance();
    m_appliname = tr("Client %1").arg(XC_VERSION);
    updateAppliName();
}

void MainWidget::resizeEvent(QResizeEvent *ev)
{
    int oldWH = ev->oldSize().height();
    int wh = ev->size().height();
    if (m_resizingHelper) {
        int diff  = wh - oldWH;
        // the window is getting bigger
        if (oldWH < wh) {
            addDockWidget(Qt::BottomDockWidgetArea, m_resizingHelper);
            m_resizingHelper->show();
            int newH = m_resizingHelper->size().height() + diff ;
            m_resizingHelper->setFixedHeight(newH);
            resize(ev->size());
            m_resizingHelper->hide();
            removeDockWidget(m_resizingHelper);
        } else if (oldWH > wh) {
            int newH = m_resizingHelper->size().height() + diff;
            m_resizingHelper->setFixedHeight(newH);
        }
    }
}

void MainWidget::customerInfoPopup(const QString & msgtitle,
                                   const QHash<QString, QString> & msgs,
                                   const QString & options)
{
    qDebug() << Q_FUNC_INFO;
    // systray popup
    // to be customisable (yes or no)
    if (m_withsystray && m_systrayIcon && options.contains("s") && (msgtitle.size() > 0)) {
        QStringList todisp;
        QStringList orders = msgs.keys();
        orders.sort();
        foreach (QString order, orders) {
            QString linetodisp = msgs[order];
            if (! linetodisp.isEmpty())
                todisp.append(linetodisp);
        }
        m_systrayIcon->showMessage(msgtitle,
                                   todisp.join("\n"),
                                   QSystemTrayIcon::Information,
                                   5000);
    }

    // to be customisable, if the user wants the window to popup
    if (options.contains("p")) {
        setVisible(true);
        activateWindow();
        raise();
    }
}

void MainWidget::hideEvent(QHideEvent *event)
{
    // called when minimized
    // if systray available
    // qDebug() << Q_FUNC_INFO;
    // << "spontaneous =" << event->spontaneous()
    // << "isMinimized =" << isMinimized()
    // << "isVisible ="   << isVisible()
    // << "isActiveWindow =" << isActiveWindow();

#ifdef Q_WS_MAC
    if (event->spontaneous()) {
        event->ignore();
    }
    setVisible(false);
#else
    if (event->spontaneous()) {
        event->ignore();
    } else {
        event->accept();
    }
#endif
}

/*! \brief Catch the Close event
 *
 * This method is called when the user click the upper right X of the
 * Window.
 * We ignore the event but hide the window (to minimize it to systray)
 */
void MainWidget::closeEvent(QCloseEvent *event)
{
    qDebug() << Q_FUNC_INFO
             << "spontaneous =" << event->spontaneous()
             << "type =" << event->type();
    // << "isMinimized =" << isMinimized()
    // << "isVisible ="   << isVisible()
    // << "isActiveWindow =" << isActiveWindow();
    if (m_withsystray == false)
        return;

#ifdef Q_WS_MAC
    setVisible(false);
#else
    if (QSystemTrayIcon::isSystemTrayAvailable())
        setVisible(false);
    else
        showMinimized();
#endif
    event->ignore();
}

void MainWidget::keyPressEvent(QKeyEvent * event)
{
    functionKeyPressed(event->key());
}


/*! \brief Displays the about box
 *
 * use QMessageBox::about() to display
 * the version and informations about the application
 */
void MainWidget::about()
{
    QString applicationVersion(XC_VERSION);
    QString fetchlastone = QString("<a href=http://downloads.xivo.fr/xivo_cti_client/") +
#if defined(Q_WS_X11)
        "linux/debian"
#elif defined(Q_WS_WIN)
        "win32"
#elif defined(Q_WS_MAC)
        "macos"
#endif
        "/" + __xivo_version__ + ">" + tr("Fetch the last one") + "</a>";
    QString datebuild(QDateTime::fromString(__datebuild_client__, Qt::ISODate).toString());
    QString gitdate_qs(__git_date__);
    QString gitdate(QDateTime::fromTime_t(gitdate_qs.toUInt()).toString());

    // might be useful to display whether QSystemTrayIcon::isSystemTrayAvailable() is true
    QMessageBox::about(this,
                       tr("About XiVO Client"),
                       "<h3>XiVO " + m_appliname + "</h3>" +
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
                       tr("(Application Launched on : %1)").arg(m_launchDateTime.toString()) + "<br>" +
                       tr("(Config File Location : %1)").arg(b_engine->getSettings()->fileName()) + "<hr>" +
                       "Copyright (C) 2007-2011 <a href=http://www.proformatique.com><b>Avencall</b></a>"
                       "<br>"
                       "10 bis rue Lucien Voilin - 92800 Puteaux - FRANCE"
                       "<p>"
                       "<b>" + tr("E-mail : ") + "</b><a href=mailto:technique@proformatique.com>technique@proformatique.com</a><br>"
                       "<b>" + tr("Phone : ") + "</b>(+33 / 0) 1.41.38.99.60<br>" +
                       "<b>" + tr("Authors : ") + "</b>" + tr("Avencall Development Team") +
                       "<hr>"
                       "<b>" + tr("License : ") + "</b>" +
                       "<a href=http://www.gnu.org/licenses/gpl-3.0-standalone.html>GNU General Public License v3</a><br>"
                       + tr("with a special exception allowing you to link it with OpenSSL, under some conditions."));
}

void MainWidget::showCredits()
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
                       "<h3>Outlook Xlet</h3>"
                       "<p>Fr&eacute;d&eacute;ric Bor <a href=mailto:frederic.bor@grouperocca.fr>frederic.bor@grouperocca.fr</p>"
                       );
}

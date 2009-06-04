/* XiVO Client
 * Copyright (C) 2007-2009, Proformatique
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

#include <QAction>
#include <QApplication>
#include <QCheckBox>
#include <QClipboard>
#include <QCloseEvent>
#include <QComboBox>
#include <QDateTime>
#include <QDebug>
#include <QDockWidget>
#include <QGridLayout>
#include <QHideEvent>
#include <QLabel>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QScrollArea>
#include <QSettings>
#include <QShowEvent>
#include <QStatusBar>
#include <QSystemTrayIcon>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QStackedWidget>
#include <QLineEdit>
#include <QPushButton>

#include "mainwidget.h"
#include "baseengine.h"
#include "configwidget.h"
#include "xivoconsts.h"
#include "xletfactory.h"

const QString extraspace("  ");

/*! \brief Constructor
 *
 * Construct the Widget with all subwidgets.
 * The geometry is restored from settings.
 * engine object ownership is taken
 */
//        : QMainWindow(parent, Qt::FramelessWindowHint),
MainWidget::MainWidget(BaseEngine * engine,
                       const QString & osname,
                       QWidget * parent)
    : QMainWindow(parent),
      m_engine(engine), m_systrayIcon(0),
      m_icon_transp(":/images/xivo-login.png"),
      m_icon_red(":/images/xivoicon-red.png"),
      m_icon_green(":/images/xivoicon-green.png"),
      m_icon_black(":/images/xivoicon-black.png")
{
    m_xletfactory = new XLetFactory(m_engine, this);
    m_engine->setParent( this ); // take ownership of the engine object
    m_appliname = "Client";
    m_engine->setOSInfos(osname);
    m_withsystray = true;
    
    m_settings = m_engine->getSettings();
    QPixmap redsquare(":/images/disconnected.png");
    statusBar();        // This creates the status bar.
    m_status = new QLabel( this );
    m_status->setPixmap(redsquare);
    statusBar()->addPermanentWidget(m_status);
    statusBar()->clearMessage();
    
    setWindowTitle("XIVO " + m_appliname);
    setDockOptions(QMainWindow::AllowNestedDocks);
    //setWindowFlags(Qt::Dialog);
    //layout->setSizeConstraint(QLayout::SetFixedSize);        // remove minimize and maximize button
    setAnimated(false);
    
    createActions();
    createMenus();
    if ( m_withsystray && QSystemTrayIcon::isSystemTrayAvailable() )
        createSystrayIcon();
    
    connect( m_engine, SIGNAL(logged()),
             this, SLOT(engineStarted()) );
    connect( m_engine, SIGNAL(delogged()),
             this, SLOT(engineStopped()) );
    connect( m_engine, SIGNAL(emitTextMessage(const QString &)),
             statusBar(), SLOT(showMessage(const QString &)) );
    connect( m_engine, SIGNAL(emitMessageBox(const QString &)),
             this, SLOT(showMessageBox(const QString &)),
             Qt::QueuedConnection );
    
#ifndef Q_WS_WIN
    m_clipboard = QApplication::clipboard();
    connect(m_clipboard, SIGNAL(selectionChanged()),
            this, SLOT(clipselection()) );
    connect(m_clipboard, SIGNAL(dataChanged()),
            this, SLOT(clipdata()) );
    m_clipboard->setText("", QClipboard::Selection); // see comment in MainWidget::clipselection()
#endif
    
    // to be better defined
    // resize(500, 400);
    restoreGeometry(m_settings->value("display/mainwingeometry").toByteArray());
    
    m_central_widget = new QStackedWidget( this );
    setCentralWidget( m_central_widget );
    
    m_wid = new QWidget( m_central_widget );
    m_central_widget->addWidget( m_wid );
    m_gridlayout = new QGridLayout(m_wid);
    
    m_login_widget = new QWidget( m_central_widget );
    m_login_widget->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Ignored );
    m_central_widget->addWidget( m_login_widget );
    m_login_layout = new QGridLayout( m_login_widget );
    m_login_layout->setRowStretch(0, 1);
    m_login_layout->setColumnStretch(0, 1);
    m_login_layout->setColumnStretch(2, 1);
    m_login_layout->setRowStretch(6, 1);
    
    if(m_settings->value("display/logtofile", false).toBool())
        m_engine->setLogFile(m_settings->value("display/logfilename", "XIVO_Client.log").toString());
    m_engine->logAction("application started on " + osname);
    
    m_xivobg = new QLabel();
    m_xivobg->setPixmap(QPixmap(":/images/xivoicon.png"));
    m_xivobg->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    m_login_layout->addWidget(m_xivobg, 1, 1, Qt::AlignHCenter | Qt::AlignVCenter);
    
    m_lab1 = new QLabel(tr("Login"));
    m_login_layout->addWidget(m_lab1, 2, 0, Qt::AlignRight);
    m_lab2 = new QLabel(tr("Password"));
    m_login_layout->addWidget(m_lab2, 3, 0, Qt::AlignRight);
    m_lab3 = new QLabel(tr("Phone"));
    m_login_layout->addWidget(m_lab3, 4, 0, Qt::AlignRight);
    
    m_qlab1 = new QLineEdit();
    m_qlab1->setText(m_engine->userId());
    m_login_layout->addWidget(m_qlab1, 2, 1);
    m_qlab2 = new QLineEdit();
    m_qlab2->setText(m_engine->password());
    m_qlab2->setEchoMode(QLineEdit::Password);
    m_login_layout->addWidget(m_qlab2, 3, 1);
    m_qlab3 = new QLineEdit();
    m_qlab3->setText(m_engine->agentphonenumber());
    m_login_layout->addWidget(m_qlab3, 4, 1);
    
    m_ack = new QPushButton("OK");
    m_login_layout->addWidget(m_ack, 2, 2, Qt::AlignLeft);
    m_kpass = new QCheckBox(tr("Keep Password"));
    m_kpass->setCheckState((m_engine->keeppass() == 2) ? Qt::Checked : Qt::Unchecked);
    m_login_layout->addWidget(m_kpass, 3, 2, Qt::AlignLeft);
    m_loginkind = new QComboBox();
    m_loginkind->addItem(QString(tr("No Agent")));
    m_loginkind->addItem(QString(tr("Agent (unlogged)")));
    m_loginkind->addItem(QString(tr("Agent (logged)")));
    m_loginkind->setCurrentIndex(m_engine->loginkind());
    m_login_layout->addWidget(m_loginkind, 4, 2, Qt::AlignLeft);
    
    loginKindChanged(m_loginkind->currentIndex());
    m_qlab1->setFocus();
    
    connect( m_qlab1, SIGNAL(returnPressed()),
             this, SLOT(config_and_start()) );
    connect( m_qlab2, SIGNAL(returnPressed()),
             this, SLOT(config_and_start()) );
    connect( m_qlab3, SIGNAL(returnPressed()),
             this, SLOT(config_and_start()) );
    connect( m_ack, SIGNAL(pressed()),
             this, SLOT(config_and_start()) );
    connect( m_loginkind, SIGNAL(currentIndexChanged(int)),
             this, SLOT(loginKindChanged(int)) );
    m_launchDateTime = QDateTime::currentDateTime();
    
    showLogin();
    if((m_withsystray && (m_engine->systrayed() == false)) || (! m_withsystray))
        show();
    setFocusPolicy(Qt::StrongFocus);

    connect( this, SIGNAL(pasteToDialPanel(const QString &)),
             m_engine, SIGNAL(pasteToDialPanel(const QString &)) );
}

/*! \brief Destructor
 *
 * The Geometry settings are saved for use by the new instance
 */
MainWidget::~MainWidget()
{
    // qDebug() << "MainWidget::~MainWidget()";
    savePositions();
    m_engine->logAction("application quit");
}

#ifndef Q_WS_WIN
void MainWidget::clipselection()
{
    //qDebug() << "BaseEngine::clipselection()" << m_clipboard->text(QClipboard::Selection);
    // statusBar()->showMessage("selected : " + m_clipboard->text(QClipboard::Selection));
    
    QString selected = m_clipboard->text(QClipboard::Selection);
    pasteToDialPanel(selected);
    
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
    //qDebug() << "BaseEngine::clipdata()" << m_clipboard->text(QClipboard::Clipboard);
    // statusBar()->showMessage("data : " + m_clipboard->text(QClipboard::Clipboard));
    
    pasteToDialPanel(m_clipboard->text(QClipboard::Clipboard));
    
    // WIN : we fall here in any Ctrl-C/Ctrl-X/"copy"/... action
    // X11 : same actions, on (seemingly) any KDE(QT) application
    // X11 (non-KDE) : we don't get the signal, but the data can be retrieved anyway (the question "when ?" remains)
    // X11 (non-KDE) : however, the xclipboard application seems to be able to catch such signals ...
}
#endif

void MainWidget::setAppearance(const QStringList & dockoptions)
{
    qDebug() << "MainWidget::setAppearance()" << dockoptions;
    foreach (QString dname, dockoptions) {
        if(dname.size() > 0) {
            QStringList dopt = dname.split("-");
            QString wname = dopt[0];
            if((wname == "customerinfo") && (! m_engine->checkedFunction(wname)))
                continue;
            m_allnames.append(wname);
            m_dockoptions[wname] = "";
            if(dopt.size() > 1) {
                if(dopt[1] == "dock") {
                    m_docknames.append(wname);
                } else if(dopt[1] == "grid")
                    m_gridnames.append(wname);
                else if(dopt[1] == "tab")
                    m_tabnames.append(wname);
                if(dopt.size() > 2)
                    m_dockoptions[wname] = dopt[2];
            } else {
                m_docknames.append(dname);
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

void MainWidget::config_and_start()
{
    m_engine->setKeepPass(m_kpass->checkState());
    m_engine->config_and_start(m_qlab1->text(),
                               m_qlab2->text(),
                               m_qlab3->text());
}

void MainWidget::logintextChanged(const QString & logintext)
{
    m_qlab3->setText(logintext);
}

void MainWidget::loginKindChanged(int index)
{
    // qDebug() << "MainWidget::loginKindChanged()" << index;
    m_engine->setLoginKind(index);
    if(index == 0) {
        m_lab3->hide();
        m_qlab3->hide();
    }
    
    if(m_engine->showagselect()) {
        if(index > 0) {
            m_lab3->show();
            m_qlab3->show();
        }
        m_loginkind->show();
    } else {
        m_lab3->hide();
        m_qlab3->hide();
        m_loginkind->hide();
    }
}

/*! \brief hide "main" window and show login widget
 */
void MainWidget::showLogin()
{
    m_central_widget->setCurrentWidget( m_login_widget );
}

/*! \brief hide login widget and show "Main" window
 */
void MainWidget::hideLogin()
{
    m_central_widget->setCurrentWidget( m_wid );
}

void MainWidget::createActions()
{
    m_cfgact = new QAction(tr("Confi&gure"), this);
    m_cfgact->setMenuRole(QAction::PreferencesRole);
    m_cfgact->setStatusTip(tr("Configure account and connection options"));
    connect( m_cfgact, SIGNAL(triggered()),
             this, SLOT(showConfDialog()) );
    
    m_quitact = new QAction(tr("&Quit"), this);
    m_quitact->setProperty("stopper", "quit");
    m_quitact->setStatusTip(tr("Close the application"));
    connect( m_quitact, SIGNAL(triggered()),
             m_engine, SLOT(stop()) );
    connect( m_quitact, SIGNAL(triggered()),
             qApp, SLOT(quit()) );
    
    if(m_withsystray) {
        m_systraymin = new QAction(tr("To S&ystray"), this);
        m_systraymin->setStatusTip(tr("Enter the system tray"));
        connect( m_systraymin, SIGNAL(triggered()),
                 this, SLOT(hide()) );
        m_systraymin->setEnabled( QSystemTrayIcon::isSystemTrayAvailable() );
        
        m_systraymax = new QAction(tr("&Show window"), this);
        m_systraymax->setStatusTip(tr("Leave the system tray"));
        connect( m_systraymax, SIGNAL(triggered()),
                 this, SLOT(showNormal()) );
        connect( m_systraymax, SIGNAL(triggered()),
                 this, SLOT(show()) );
        connect( m_systraymax, SIGNAL(triggered()),
                 this, SLOT(raise()) );
        m_systraymax->setEnabled( QSystemTrayIcon::isSystemTrayAvailable() );
    }
    
    m_connectact = new QAction(tr("&Connect"), this);
    m_connectact->setStatusTip(tr("Connect to the server"));
    connect( m_connectact, SIGNAL(triggered()),
             m_engine, SLOT(start()) );
    
    m_disconnectact = new QAction(tr("&Disconnect"), this);
    m_disconnectact->setProperty("stopper", "disconnect");
    m_disconnectact->setStatusTip(tr("Disconnect from the server"));
    connect( m_disconnectact, SIGNAL(triggered()),
             m_engine, SLOT(stop()) );
    
    m_connectact->setEnabled(true);
    m_disconnectact->setEnabled(false);
    
    // Availability actions :
    m_availgrp = new QActionGroup( this );
    m_availgrp->setExclusive(true);
    
    connect( m_engine, SIGNAL(changesAvailChecks()),
             this, SLOT(checksAvailState()) );
    
    checksAvailState();
}

void MainWidget::checksAvailState()
{
    if(m_avact.contains(m_engine->getAvailState()))
        m_avact[m_engine->getAvailState()]->setChecked( true );
}

void MainWidget::createMenus()
{
    m_filemenu = menuBar()->addMenu("&XIVO Client"); // + m_appliname too heavy
    m_filemenu->addAction( m_cfgact );
    if(m_withsystray)
        m_filemenu->addAction( m_systraymin );
    m_filemenu->addSeparator();
    m_filemenu->addAction( m_connectact );
    m_filemenu->addAction( m_disconnectact );
    m_filemenu->addSeparator();
    m_filemenu->addAction( m_quitact );

    m_avail = menuBar()->addMenu(tr("&Availability"));
    // m_avail->addActions( m_availgrp->actions() );
    m_avail->setEnabled( false );
    connect( m_engine, SIGNAL(availAllowChanged(bool)),
             m_avail, SLOT(setEnabled(bool)) );

    m_helpmenu = menuBar()->addMenu(tr("&Help"));
    m_helpmenu->addAction(tr("&About XIVO Client"), this, SLOT(about()));
    m_helpmenu->addAction(tr("About &Qt"), qApp, SLOT(aboutQt()));
    m_helpmenu->addAction(tr("&Credits"), this, SLOT(showCredits()));
}

void MainWidget::updateAppliName()
{
    setWindowTitle("XIVO " + m_appliname);
    if(m_withsystray && m_systrayIcon)
        m_systrayIcon->setToolTip("XIVO " + m_appliname);
    // m_filemenu->setTitle("&XIVO Client");
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
    m_systrayIcon->setToolTip("XIVO " + m_appliname);
    QMenu * menu = new QMenu(QString("SystrayMenu"), this);
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
    m_systrayIcon->setContextMenu( menu );
    m_systrayIcon->show();
    //connect( m_systrayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
    //         this, SLOT(show()) );
    connect( m_systrayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
             this, SLOT(systrayActivated(QSystemTrayIcon::ActivationReason)) );
    connect( m_systrayIcon, SIGNAL(messageClicked()),
             this, SLOT(systrayMsgClicked()) );
    // QSystemTrayIcon::ActivationReason
    // qDebug() << "QSystemTrayIcon::supportsMessages() = "
    //          << QSystemTrayIcon::supportsMessages();
}

/*! \brief show the Configuration Dialog
 *
 * create and execute a new ConfigWidget
 */
void MainWidget::showConfDialog()
{
    ConfigWidget config(m_engine);
    connect( &config, SIGNAL(confUpdated()),
             this, SLOT(confUpdated()) );
    config.exec();
}

void MainWidget::confUpdated()
{
    // qDebug() << "MainWidget::confUpdated()";
    m_qlab1->setText(m_engine->userId());
    m_qlab2->setText(m_engine->password());
    m_qlab3->setText(m_engine->agentphonenumber());
    m_kpass->setCheckState((m_engine->keeppass() == 2) ? Qt::Checked : Qt::Unchecked);
    m_loginkind->setCurrentIndex(m_engine->loginkind());
    loginKindChanged(m_loginkind->currentIndex()); // Hide or Show the phone number
}

/*! \brief process clicks to the systray icon
 *
 * This slot is connected to the activated() signal of the
 * System Tray icon. It currently toggle the visibility
 * of the MainWidget on a simple left click. */
void MainWidget::systrayActivated(QSystemTrayIcon::ActivationReason reason)
{
    qDebug() << "MainWidget::systrayActivated()"
             << "reason =" << reason
             << "isMinimized =" << isMinimized()
             << "isVisible =" << isVisible()
             << "isActiveWindow =" << isActiveWindow();
    // QSystemTrayIcon::DoubleClick
    // QSystemTrayIcon::Trigger
    if (reason == QSystemTrayIcon::Trigger) {
#ifdef Q_WS_MAC
        // try to reduce potential problems under MacOS X
        if ( isVisible() )
            setVisible(false);
#else
        if ( isVisible() && !isActiveWindow() ) {
            showNormal();
            activateWindow();
            raise();
        } else {
            // Toggle visibility
            setVisible(!isVisible());
            if ( isVisible() ) {
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
    QMessageBox::critical(NULL, tr("XIVO CTI Error"), message);
}

/*!
 * This slot implementation show, activate (and raise) the
 * window.
 */
void MainWidget::systrayMsgClicked()
{
    qDebug() << "MainWidget::systrayMsgClicked()";
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
    if(m_tabwidget)
        m_tabwidget->setCurrentWidget(widget);
}

void MainWidget::addPanel(const QString & name, const QString & title, QWidget * widget)
{
    if(m_docknames.contains(name)) {
        qDebug() << "MainWidget::addPanel() (dock)" << name << m_dockoptions[name];
        QDockWidget::DockWidgetFeatures features = QDockWidget::NoDockWidgetFeatures;
        if(m_dockoptions[name].contains("c"))
            features |= QDockWidget::DockWidgetClosable;
        if(m_dockoptions[name].contains("f"))
            features |= QDockWidget::DockWidgetFloatable;
        if(m_dockoptions[name].contains("m"))
            features |= QDockWidget::DockWidgetMovable;
        m_docks[name] = new QDockWidget(title);
        m_docks[name]->setFeatures(features);
        m_docks[name]->setAllowedAreas(Qt::BottomDockWidgetArea); // restrain the area to Bottom region
        m_docks[name]->setObjectName(name); // compulsory to allow a proper state's saving
        addDockWidget(Qt::BottomDockWidgetArea, m_docks[name]);
        m_docks[name]->setWidget(widget);
        m_docks[name]->hide();
    } else if(m_gridnames.contains(name)) {
        qDebug() << "MainWidget::addPanel() (grid)" << name << m_dockoptions[name];
        m_gridlayout->addWidget(widget, m_dockoptions[name].toInt(), 0);
    } else if(m_tabnames.contains(name)) {
        qDebug() << "MainWidget::addPanel() (tab) " << name << m_dockoptions[name] << m_tabwidget->count();
        if(m_dockoptions[name].size() > 0) 
            m_tabwidget->insertTab(m_dockoptions[name].toInt(), widget, extraspace + title + extraspace);
        else
            m_tabwidget->addTab(widget, extraspace + title + extraspace);
    }
    qDebug() << "adding" << name << title;
}

void MainWidget::updatePresence(const QVariant & presence)
{
    // qDebug() << "MainWidget::updatePresence()" << presence;
    QVariantMap presencemap = presence.toMap();
    if(presencemap.contains("names")) {
        foreach (QString avstate, presencemap["names"].toMap().keys()) {
            QString name = presencemap["names"].toMap()[avstate].toMap()["longname"].toString();
            if(! m_avact.contains(avstate)) {
                m_avact[avstate] = new QAction(name, this);
                m_avact[avstate]->setCheckable(false);
                m_avact[avstate]->setProperty("availstate", avstate);
                m_avact[avstate]->setEnabled(false);
                connect( m_avact[avstate], SIGNAL(triggered()),
                         m_engine, SLOT(setAvailability()) );
                m_availgrp->addAction( m_avact[avstate] );
            }
        }
        m_avail->addActions( m_availgrp->actions() );
    }
    if(presencemap.contains("allowed")) {
        QMapIterator<QString, QVariant> capapres(presencemap["allowed"].toMap());
        while (capapres.hasNext()) {
            capapres.next();
            QString avstate = capapres.key();
            bool allow = capapres.value().toBool();
            if(m_avact.contains(avstate)) {
                m_avact[avstate]->setCheckable(allow);
                m_avact[avstate]->setEnabled(allow);
            }
        }
    }
    if(presencemap.contains("state")) {
        m_engine->setAvailState(presencemap["state"].toMap()["stateid"].toString(), true);
    }
}

void MainWidget::clearPresence()
{
    QVariantMap presence = m_engine->getCapaPresence();
    if(presence.contains("names")) {
        QMapIterator<QString, QVariant> capapres(presence["names"].toMap());
        while (capapres.hasNext()) {
            capapres.next();
            QString avstate = capapres.key();
            if(m_avact.contains(avstate)) {
                disconnect( m_avact[avstate], SIGNAL(triggered()),
                            m_engine, SLOT(setAvailability()) );
                m_availgrp->removeAction( m_avact[avstate] );
                delete m_avact[avstate];
            }
        }
        m_avact.clear();
        m_avail->clear();
    }
}

/*!
 * enables the "Disconnect" action and disables the "Connect" Action.
 * sets the Green indicator
 * \sa engineStopped()
 */
void MainWidget::engineStarted()
{
    setForceTabs(false);
    setAppearance(m_engine->getCapaXlets());
    
    m_appliname = tr("Client (%1 profile)").arg(m_engine->getCapaApplication());
    
    connect( m_engine, SIGNAL(updatePresence(const QVariant &)),
             this, SLOT(updatePresence(const QVariant &)) );
    updateAppliName();
    hideLogin();
    
    m_tabwidget = new QTabWidget(this);
    m_tabwidget->hide();
    
    if(m_docknames.contains("tabber")) {
        m_tabwidget->show();
        addPanel("tabber", tr("Tabs"), m_tabwidget);
    }
    if(m_gridnames.contains("tabber")) {
        m_tabwidget->show();
        m_gridlayout->addWidget(m_tabwidget, 1, 0);
    }
    
        foreach(QString xletid, m_allnames) {
            bool withscrollbar = m_dockoptions[xletid].contains("s");
            XLet * xlet = m_xletfactory->newXLet(xletid, this);
            if(xlet) {
                m_xletlist.insert(xlet);
                xlet->doGUIConnects( this );
                if (withscrollbar) {
                    QScrollArea * sa_ag = new QScrollArea(this);
                    sa_ag->setWidget(xlet);
                    sa_ag->setWidgetResizable(true);
                    addPanel(xletid, xlet->title(), sa_ag);
                } else {
                    addPanel(xletid, xlet->title(), xlet);
                }
            } else {
                qDebug() << "cannot instanciate XLet" << xletid;
            }
        }
    
    qDebug() << "MainWidget::engineStarted() : the xlets have been created";
    m_tabwidget->setCurrentIndex(m_settings->value("display/lastfocusedtab").toInt());
    
    foreach (QString dname, m_docknames) {
        if(m_docks.value(dname))
            m_docks.value(dname)->show();
        else
            qDebug() << "dock" << dname << "not there";
    }
    
    // restore settings, especially for Docks' positions
    restoreState(m_settings->value("display/mainwindowstate").toByteArray());
    
    if(m_withsystray && m_systrayIcon)
        setSystrayIcon("xivo-transp");
    
    statusBar()->showMessage(tr("Connected"));
    m_connectact->setEnabled(false);
    m_disconnectact->setEnabled(true);
    // set status icon to green
    QPixmap greensquare(":/images/connected.png");
    m_status->setPixmap(greensquare);
    m_engine->logAction("connection started");
}

void MainWidget::setSystrayIcon(const QString & def)
{
    QIcon icon;
    if(def == "xivo-transp")
        icon = m_icon_transp;
    else if(def == "xivo-red")
        icon = m_icon_red;
    else if(def == "xivo-green")
        icon = m_icon_green;
    else if(def == "xivo-black")
        icon = m_icon_black;
    else {
        int psize = 16;
        QPixmap square(psize, psize);
        square.fill(def);
        icon = QIcon(square);
    }
    
    if(m_systrayIcon)
        m_systrayIcon->setIcon(icon);
    setWindowIcon(icon);
}

void MainWidget::removePanel(const QString & name, QWidget * widget)
{
    if(m_docknames.contains(name)) {
        removeDockWidget(m_docks[name]);
        //delete widget;
        //delete m_docks[name]; // seems to "delete widget", also
        //widget->deleteLater();
        m_docks[name]->deleteLater();
        m_docks.remove(name);
    }
    if(m_tabnames.contains(name)) {
        int thisindex = m_tabwidget->indexOf(widget);
        if (thisindex > -1) {
            qDebug() << "removing tab" << name << thisindex;
            m_tabwidget->removeTab(thisindex);
        }
        //widget->deleteLater();
        //delete widget;
    }
    if(m_gridnames.contains(name)) {
        //m_gridlayout->removeWidget(widget);
        //delete widget;
        //widget->deleteLater();
    }
}

/*!
 * disables the "Disconnect" action and enables the "Connect" Action.
 * sets the Red indicator
 * \sa engineStarted()
 */
void MainWidget::engineStopped()
{
    // qDebug() << "MainWidget::engineStopped()";
    m_settings->setValue("display/mainwindowstate", saveState());
    if (m_tabwidget->currentIndex() > -1) {
        // qDebug() << m_tabwidget->currentIndex();
        m_settings->setValue("display/lastfocusedtab", m_tabwidget->currentIndex());
        // qDebug() << m_tabwidget->tabText(m_tabwidget->currentIndex());
    }
    
    foreach (QString dname, m_docknames) {
        if(m_docks.contains(dname))
            m_docks.value(dname)->hide();
    }
    clearPresence();
    
    // delete all xlets
    QSetIterator<XLet *> i(m_xletlist);
    while (i.hasNext()) {
        i.next()->deleteLater();
    }
    m_xletlist.clear();
    
    if(m_docknames.contains("tabber")) {
        removePanel("tabber", m_tabwidget);
    }
    if(m_gridnames.contains("tabber")) {
        m_gridlayout->removeWidget(m_tabwidget);
        m_tabwidget->deleteLater();
    }
    
    showLogin();
    
    if(m_withsystray && m_systrayIcon)
        setSystrayIcon("xivo-black");
    
    statusBar()->showMessage(tr("Disconnected"));
    m_connectact->setEnabled(true);
    m_disconnectact->setEnabled(false);
    // set status icon to red
    QPixmap redsquare(":/images/disconnected.png");
    m_status->setPixmap(redsquare);

    clearAppearance();
    m_appliname = "Client";
    updateAppliName();
    m_engine->logAction("connection stopped");
}

void MainWidget::setForceTabs(bool force)
{
    m_forcetabs = force;
}

void MainWidget::savePositions() const
{
    // qDebug() << "MainWidget::savePositions()";
    m_settings->setValue("display/mainwingeometry", saveGeometry());
}

/*!
 * does nothing
 */
void MainWidget::dispurl(const QUrl &url)
{
    qDebug() << "MainWidget::dispurl()" << url;
}

void MainWidget::customerInfoPopup(const QString & msgtitle,
                                   const QHash<QString, QString> & msgs,
                                   const QString & options)
{
    qDebug() << "MainWidget::customerInfoPopup()";
    // systray popup
    // to be customisable (yes or no)
    if(m_withsystray && m_systrayIcon && options.contains("s") && (msgtitle.size() > 0)) {
        QStringList todisp;
        QStringList orders = msgs.keys();
        orders.sort();
        foreach (QString order, orders) {
            QString linetodisp = msgs[order];
            if(! linetodisp.isEmpty())
                todisp.append(linetodisp);
        }
        m_systrayIcon->showMessage(msgtitle,
                                   todisp.join("\n"),
                                   QSystemTrayIcon::Information,
                                   5000);
    }
    
    // focus on the customerinfo tab
//    if(m_tabnames.contains("customerinfo") && m_engine->checkedFunction("customerinfo") && options.contains("f"))
//        if (m_cinfo_index > -1)
//            m_tabwidget->setCurrentIndex(m_cinfo_index);
    
    // to be customisable, if the user wants the window to popup
    if(options.contains("p")) {
        setVisible(true);
        activateWindow();
        raise();
    }
}

void MainWidget::showEvent(QShowEvent *event)
{
    // qDebug() << "MainWidget::showEvent()";
    event->accept();
    // << "spontaneous =" << event->spontaneous()
    // << "isMinimized =" << isMinimized()
    // << "isVisible ="   << isVisible()
    // << "isActiveWindow =" << isActiveWindow();
}

void MainWidget::hideEvent(QHideEvent *event)
{
    // called when minimized
    // if systray available
    // qDebug() << "MainWidget::hideEvent()";
    // << "spontaneous =" << event->spontaneous()
    // << "isMinimized =" << isMinimized()
    // << "isVisible ="   << isVisible()
    // << "isActiveWindow =" << isActiveWindow();

#ifdef Q_WS_MAC
    if (event->spontaneous())
        event->ignore();
    setVisible(false);
#else
    if (event->spontaneous())
        event->ignore();
    else
        event->accept();
#endif

    // #ifndef Q_WS_MAC
    //          if ( QSystemTrayIcon::isSystemTrayAvailable() ) {
    //                 qDebug() << "MainWidget::hideEvent() setVisible(false)";
    //                  setVisible(false);
    //         }
    // #endif
}

/*! \brief Catch the Close event
 *
 * This method is called when the user click the upper right X of the
 * Window.
 * We ignore the event but hide the window (to minimize it to systray)
 */
void MainWidget::closeEvent(QCloseEvent *event)
{
    qDebug() << "MainWidget::closeEvent()"
    << "spontaneous =" << event->spontaneous()
    << "type =" << event->type();
    // << "isMinimized =" << isMinimized()
    // << "isVisible ="   << isVisible()
    // << "isActiveWindow =" << isActiveWindow();
    if(m_withsystray == false)
        return;

#ifdef Q_WS_MAC
    setVisible(false);
#else
    if ( QSystemTrayIcon::isSystemTrayAvailable() )
        setVisible( false );
    else
        showMinimized();
#endif
    event->ignore();
}

void MainWidget::changeEvent(QEvent * /* event */)
{
    // qDebug() << "MainWidget::changeEvent()";
    //qDebug() << "MainWidget::changeEvent() eventtype=" << event->type();
    //if (event->type() == 105)
    //        event->accept();
    //event->ignore();
}

void MainWidget::keyPressEvent(QKeyEvent * event)
{
    if((event->key() >= Qt::Key_F1) && (event->key() <= Qt::Key_F35))
        qDebug() << QString("MainWidget::keyPressEvent() F%1").arg(event->key() - Qt::Key_F1 + 1);
    else if(event->key() == Qt::Key_Up)
        qDebug() << "MainWidget::keyPressEvent() Up";
    else if(event->key() == Qt::Key_Down)
        qDebug() << "MainWidget::keyPressEvent() Down";
    if(((event->key() >= Qt::Key_F1) && (event->key() <= Qt::Key_F35))
       || (event->key() == Qt::Key_Up)
       || (event->key() == Qt::Key_Down))
        functionKeyPressed(event->key());
}

#if 0
bool MainWidget::event(QEvent *event)
{
    qDebug() << "MainWidget::event() eventtype=" << event->type();
    event->accept();
    //if (event->type() == 105)
    //        event->accept();
    //event->ignore();
    return true;
}
#endif

/*! \brief Displays the about box
 *
 * use QMessageBox::about() to display
 * the version and informations about the application
 */
void MainWidget::about()
{
    QString applicationVersion(XIVOVER);
    QString fetchlastone = "<a href=http://downloads.xivo.fr/xivo_cti_client/"
#if defined(Q_WS_X11)
        "linux/debian"
#elif defined(Q_WS_WIN)
        "win32"
#elif defined(Q_WS_MAC)
        "macos"
#endif
        "/" + __xivo_version__ + ">" + tr("last one") + "</a>";
    QString datebuild(QDateTime::fromString(__datebuild_client__, Qt::ISODate).toString());
    
    // might be useful to display whether QSystemTrayIcon::isSystemTrayAvailable() is true
    QMessageBox::about(this,
                       tr("About XIVO Client"),
                       "<h3>XIVO " + m_appliname + "</h3>" +
                       tr("The XIVO CTI applications, once connected to the proper server, "
                          "complete the XIVO telephony system, by providing computer tools, "
                          "such as customer information display, directory searches, or various "
                          "supervision 'X-lets' for call centers.") +
                       "<p>"
                       "<b>" + tr("Version : ") + QString("</b>%1 (").arg(applicationVersion) +
                       "<b>svn : " + QString("</b>%1 - %2)<br>").arg(__current_client_version__,
                                                                     fetchlastone) +
                       "(" + tr("Advised Server Version : ") + __required_server_version__ + ")"
                       "<br>" +
                       "(" + tr("Application Built on : ") + datebuild + ")"
                       "<br>" +
                       "(" + tr("Application Launched on : ") + m_launchDateTime.toString() + ")"
                       "<br>" +
                       "(" + tr("Config File Location : ") + m_settings->fileName() + ")" +
                       "<hr>"
                       "Copyright (C) 2007, 2008, 2009  <a href=http://www.proformatique.com><b>Proformatique</b></a>"
                       "<br>"
                       "67 rue Voltaire 92800 Puteaux FRANCE"
                       "<p>"
                       "<b>" + tr("E-mail : ") + "</b><a href=mailto:technique@proformatique.com>technique@proformatique.com</a><br>"
                       "<b>" + tr("Phone : ") + "</b>(+33 / 0) 1.41.38.99.60<br>" +
                       "<b>" + tr("Authors : ") + "</b>Thomas Bernard, Corentin Le Gall" +
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

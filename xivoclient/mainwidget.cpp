/* XIVO CTI Clients
 * Copyright (C) 2007, 2008  Proformatique
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License version 2 for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Linking the Licensed Program statically or dynamically with other
 * modules is making a combined work based on the Licensed Program. Thus,
 * the terms and conditions of the GNU General Public License version 2
 * cover the whole combination.
 *
 * In addition, as a special exception, the copyright holders of the
 * Licensed Program give you permission to combine the Licensed Program
 * with free software programs or libraries that are released under the
 * GNU Library General Public License version 2.0 or GNU Lesser General
 * Public License version 2.1 or any later version of the GNU Lesser
 * General Public License, and with code included in the standard release
 * of OpenSSL under a version of the OpenSSL license (with original SSLeay
 * license) which is identical to the one that was published in year 2003,
 * or modified versions of such code, with unchanged license. You may copy
 * and distribute such a system following the terms of the GNU GPL
 * version 2 for the Licensed Program and the licenses of the other code
 * concerned, provided that you include the source code of that other code
 * when and as the GNU GPL version 2 requires distribution of source code.
*/

/* $Revision$
 * $Date$
 */

#include <QAction>
#include <QApplication>
#include <QClipboard>
#include <QCloseEvent>
#include <QComboBox>
#include <QDateTime>
#include <QDebug>
#include <QDockWidget>
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
#include <QTime>
#include <QVBoxLayout>

#include "agentspanel.h"
#include "agentdetailspanel.h"
#include "baseengine.h"
#include "callcampaignpanel.h"
#include "callstackwidget.h"
#include "conferencepanel.h"
#include "configwidget.h"
#include "customerinfopanel.h"
#include "datetimepanel.h"
#include "dialpanel.h"
#include "directorypanel.h"
#include "displaymessages.h"
#include "identitydisplay.h"
#include "faxpanel.h"
#include "logwidget.h"
#include "mainwidget.h"
#include "mylocaldirpanel.h"
#include "parkingpanel.h"
#include "popup.h"
#include "queuespanel.h"
#include "queuedetailspanel.h"
#include "queueentrydetailspanel.h"
#include "searchpanel.h"
#include "servercommand.h"
#include "servicepanel.h"
#include "statuspanel.h"
#include "switchboardwindow.h"
#include "videopanel.h"
#include "xletprotopanel.h"
#include "xivoconsts.h"
#ifdef USE_OUTLOOK
#include "outlookpanel.h"
#endif /* USE_OUTLOOK */

const QString extraspace("  ");

/*! \brief Widget containing the CallStackWidget and a Title QLabel
 */
LeftPanel::LeftPanel(QWidget * bottomWidget, QWidget * parent)
        : QWidget(parent)
{
	QVBoxLayout * layout = new QVBoxLayout(this);
	layout->setMargin(0);
	m_titleLabel = new QLabel("                     ", this);
        m_titleLabel->setObjectName("monitored");
	layout->addWidget(m_titleLabel, 0, Qt::AlignCenter);
	layout->addWidget(bottomWidget, 1);
}

QLabel * LeftPanel::titleLabel()
{
	return m_titleLabel;
}

/*! \brief Constructor
 *
 * Construct the Widget with all subwidgets.
 * The geometry is restored from settings.
 */
//        : QMainWindow(parent, Qt::FramelessWindowHint),
MainWidget::MainWidget(BaseEngine * engine,
                       const QString & osname,
                       QWidget * parent)
        : QMainWindow(parent),
          m_engine(engine), m_systrayIcon(0),
          m_icon(":/images/xivoicon.png"), m_icongrey(":/images/xivoicon-grey.png")
{
        m_appliname = "Clients";
        m_engine->setOSInfos(osname);
        m_withsystray = true;
        m_loginfirst = true;

        m_settings = m_engine->getSettings();
	QPixmap redsquare(":/images/disconnected.png");
        statusBar();	// This creates the status bar.
        m_status = new QLabel();
        m_status->setPixmap(redsquare);
        statusBar()->addPermanentWidget(m_status);
        statusBar()->clearMessage();

	setWindowIcon(QIcon(":/images/xivoicon.png"));
	setWindowTitle("XIVO " + m_appliname);
        setDockOptions(QMainWindow::AllowNestedDocks);
	//setWindowFlags(Qt::Dialog);
	//layout->setSizeConstraint(QLayout::SetFixedSize);	// remove minimize and maximize button
        setAnimated(false);

        createActions();
        createMenus();
        if ( m_withsystray && QSystemTrayIcon::isSystemTrayAvailable() )
                createSystrayIcon();

	connect( m_engine, SIGNAL(logged()),
	         this, SLOT(engineStarted()));
	connect( m_engine, SIGNAL(delogged()),
                 this, SLOT(engineStopped()));
        connect( m_engine, SIGNAL(emitTextMessage(const QString &)),
                 statusBar(), SLOT(showMessage(const QString &)));
        
        m_clipboard = QApplication::clipboard();
        connect(m_clipboard, SIGNAL(selectionChanged()),
                this, SLOT(clipselection()));
        connect(m_clipboard, SIGNAL(dataChanged()),
                this, SLOT(clipdata()));
        m_clipboard->setText("", QClipboard::Selection); // see comment in MainWidget::clipselection()
        
        // to be better defined
        // resize(500, 400);
        restoreGeometry(m_settings->value("display/mainwingeometry").toByteArray());
	m_wid = new QWidget();
	m_gridlayout = new QGridLayout(m_wid);
        
        m_xivobg = new QLabel();
        m_xivobg->setPixmap(QPixmap(":/images/xivoicon.png"));
        m_xivobg->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        
        if(m_loginfirst) {
                m_lab1 = new QLabel(tr("Login"));
                m_lab2 = new QLabel(tr("Password"));
                m_lab3 = new QLabel(tr("Phone"));
                m_qlab1 = new QLineEdit();
                m_qlab2 = new QLineEdit();
                m_qlab3 = new QLineEdit();
                m_ack = new QPushButton("OK");
                m_kpass = new QCheckBox(tr("Keep Password"));
                m_loginkind = new QComboBox(this);
                m_loginkind->addItem(QString(tr("No Agent")));
                m_loginkind->addItem(QString(tr("Agent (unlogged)")));
                m_loginkind->addItem(QString(tr("Agent (logged)")));
                
                connect( m_qlab1, SIGNAL(returnPressed()),
                         this, SLOT(config_and_start()) );
                // connect( m_qlab1, SIGNAL(textChanged(const QString &)),
                // this, SLOT(logintextChanged(const QString &)) );
                connect( m_qlab2, SIGNAL(returnPressed()),
                         this, SLOT(config_and_start()) );
                connect( m_qlab3, SIGNAL(returnPressed()),
                         this, SLOT(config_and_start()) );
                connect( m_ack, SIGNAL(pressed()),
                         this, SLOT(config_and_start()) );
                connect( m_loginkind, SIGNAL(currentIndexChanged(int)),
                         this, SLOT(loginKindChanged(int)) );
                m_qlab2->setEchoMode(QLineEdit::Password);
        }
        m_launchDateTime = QDateTime::currentDateTime();

	setCentralWidget(m_wid);
        showLogin();
        if((m_withsystray && (m_engine->systrayed() == false)) || (! m_withsystray))
                this->show();
}


/*! \brief Destructor
 *
 * The Geometry settings are saved for use by the new instance
 */
MainWidget::~MainWidget()
{
        // qDebug() << "MainWidget::~MainWidget()";
        savePositions();
        delete m_settings;
}

void MainWidget::clipselection()
{
        // qDebug() << "BaseEngine::clipselection()" << m_clipboard->text(QClipboard::Selection);
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
        // qDebug() << "BaseEngine::clipdata()" << m_clipboard->text(QClipboard::Clipboard);
        // statusBar()->showMessage("data : " + m_clipboard->text(QClipboard::Clipboard));
        
        pasteToDialPanel(m_clipboard->text(QClipboard::Clipboard));
        
        // WIN : we fall here in any Ctrl-C/Ctrl-X/"copy"/... action
        // X11 : same actions, on (seemingly) any KDE(QT) application
        // X11 (non-KDE) : we don't get the signal, but the data can be retrieved anyway (the question "when ?" remains)
        // X11 (non-KDE) : however, the xclipboard application seems to be able to catch such signals ...
}

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
        m_engine->setLoginKind(index);
        if(index == 0) {
                m_lab3->hide();
                m_qlab3->hide();
        } else {
                if(m_engine->showagselect()) {
                        m_lab3->show();
                        m_qlab3->show();
                        m_loginkind->show();
                } else {
                        m_lab3->hide();
                        m_qlab3->hide();
                        m_loginkind->hide();
                }
        }
}

void MainWidget::showLogin()
{
        m_gridlayout->addWidget(m_xivobg, 1, 1, Qt::AlignHCenter | Qt::AlignVCenter);
        m_gridlayout->setRowStretch(0, 1);
        m_gridlayout->setColumnStretch(0, 1);
        m_gridlayout->setColumnStretch(2, 1);
        m_gridlayout->setRowStretch(6, 1);
        if(m_loginfirst) {
                m_gridlayout->addWidget(m_lab1, 2, 0, Qt::AlignRight);
                m_gridlayout->addWidget(m_qlab1, 2, 1);
                m_gridlayout->addWidget(m_ack, 2, 2, Qt::AlignLeft);
                
                m_gridlayout->addWidget(m_lab2, 3, 0, Qt::AlignRight);
                m_gridlayout->addWidget(m_qlab2, 3, 1);
                m_gridlayout->addWidget(m_kpass, 3, 2, Qt::AlignLeft);
                
                m_gridlayout->addWidget(m_lab3, 4, 0, Qt::AlignRight);
                m_gridlayout->addWidget(m_qlab3, 4, 1);
                m_gridlayout->addWidget(m_loginkind, 4, 2, Qt::AlignLeft);
                
                // show widgets after they have been put in the layout, in order for
                // temporary windows not to be opened
                m_lab1->show();
                m_lab2->show();
                m_qlab1->show();
                m_qlab2->show();
                m_ack->show();
                m_kpass->show();
                m_loginkind->show();
                
                m_qlab1->setText(m_engine->userId());
                m_qlab2->setText(m_engine->password());
                m_qlab3->setText(m_engine->phonenumber());
                m_kpass->setCheckState((m_engine->keeppass() == 2) ? Qt::Checked : Qt::Unchecked);
                m_loginkind->setCurrentIndex(m_engine->loginkind());
                
                loginKindChanged(m_loginkind->currentIndex());
                m_qlab1->setFocus();
        }
        m_xivobg->show();
}

void MainWidget::hideLogin()
{
        m_xivobg->hide();
        m_gridlayout->setRowStretch(0, 0);
        m_gridlayout->setColumnStretch(0, 0);
        m_gridlayout->setColumnStretch(2, 0);
        m_gridlayout->setRowStretch(6, 0);
        if(m_loginfirst) {
                m_lab1->hide();
                m_lab2->hide();
                m_lab3->hide();
                m_qlab1->hide();
                m_qlab2->hide();
                m_qlab3->hide();
                m_ack->hide();
                m_kpass->hide();
                m_loginkind->hide();

                m_gridlayout->removeWidget(m_ack);
                m_gridlayout->removeWidget(m_lab1);
                m_gridlayout->removeWidget(m_lab2);
                m_gridlayout->removeWidget(m_lab3);
                m_gridlayout->removeWidget(m_qlab1);
                m_gridlayout->removeWidget(m_qlab2);
                m_gridlayout->removeWidget(m_qlab3);
                m_gridlayout->removeWidget(m_kpass);
                m_gridlayout->removeWidget(m_loginkind);
        }
        m_gridlayout->removeWidget(m_xivobg);
}

void MainWidget::affTextChanged()
{
        // 	QString txt = m_xlet["instantmessaging"]->text();
        // 	txt.replace(" ", "_");
        // 	m_engine->sendMessage(txt.toUtf8());
        // 	m_xlet["instantmessaging"]->setText("");
}

void MainWidget::createActions()
{
	m_cfgact = new QAction(tr("Confi&gure"), this);
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
	m_filemenu = menuBar()->addMenu("&XIVO " + m_appliname);
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
	m_helpmenu->addAction(tr("&About XIVO %1").arg(m_appliname), this, SLOT(about()));
	m_helpmenu->addAction(tr("About &Qt"), qApp, SLOT(aboutQt()));
}

void MainWidget::updateAppliName()
{
	setWindowTitle("XIVO " + m_appliname);
        if(m_withsystray && m_systrayIcon)
                m_systrayIcon->setToolTip("XIVO " + m_appliname);
	m_filemenu->setTitle("&XIVO " + m_appliname);
}

/*! \brief create and show the system tray icon
 *
 * Create the system tray icon, show it and connect its
 * activated() signal to some slot 
 */
void MainWidget::createSystrayIcon()
{
	m_systrayIcon = new QSystemTrayIcon(m_icongrey, this);
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
        m_config = new ConfigWidget(m_engine, this);
	connect( m_config, SIGNAL(confUpdated()),
	         this, SLOT(confUpdated()));
	m_config->exec();
}

void MainWidget::confUpdated()
{
        // qDebug() << "MainWidget::confUpdated()";
        m_qlab1->setText(m_engine->userId());
        m_qlab2->setText(m_engine->password());
        m_qlab3->setText(m_engine->phonenumber());
        m_kpass->setCheckState((m_engine->keeppass() == 2) ? Qt::Checked : Qt::Unchecked);
        m_loginkind->setCurrentIndex(m_engine->loginkind());
        if(m_engine->showagselect()) {
                m_lab3->show();
                m_qlab3->show();
                m_loginkind->show();
        } else {
                m_lab3->hide();
                m_qlab3->hide();
                m_loginkind->hide();
        }
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
        connect( m_engine, SIGNAL(setGuiOptions(const QVariant &)),
                 m_xlet[name], SLOT(setGuiOptions(const QVariant &)));
        connect( m_engine, SIGNAL(localUserInfoDefined(const UserInfo *)),
                 m_xlet[name], SLOT(setUserInfo(const UserInfo *)));
}


void MainWidget::connectDials(QWidget * widget)
{
        connect( widget, SIGNAL(pickUp(const UserInfo *)),
                 m_engine, SLOT(pickUp(const UserInfo *)) );
        connect( widget, SIGNAL(originateCall(const QString &, const QString &)),
                 m_engine, SLOT(originateCall(const QString &, const QString &)) );
        connect( widget, SIGNAL(transferCall(const QString &, const QString &)),
                 m_engine, SLOT(transferCall(const QString &, const QString &)) );
        connect( widget, SIGNAL(atxferCall(const QString &, const QString &)),
                 m_engine, SLOT(atxferCall(const QString &, const QString &)) );
        connect( widget, SIGNAL(simplehangupCall(const UserInfo *, const QString &)),
                 m_engine, SLOT(simplehangupCall(const UserInfo *, const QString &)) );
        connect( widget, SIGNAL(hangupCall(const UserInfo *, const QString &)),
                 m_engine, SLOT(hangupCall(const UserInfo *, const QString &)) );
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
                m_engine->setAvailState(presencemap["state"].toString(), true);
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
        
        m_appliname = m_engine->getCapaApplication();
        QVariantMap opt_map;
        opt_map.unite(m_engine->getGuiOptions("server").toMap());
        opt_map.unite(m_engine->getGuiOptions("user").toMap());
        m_options = QVariant(opt_map);
        
        connect( m_engine, SIGNAL(updatePresence(const QVariant &)),
                 this, SLOT(updatePresence(const QVariant &)) );
        updateAppliName();
        hideLogin();
        
        m_tabwidget = new QTabWidget(this);
        
        if(m_docknames.contains("tabber")) {
                addPanel("tabber", tr("Tabs"), m_tabwidget);
        }
        if(m_gridnames.contains("tabber")) {
                m_gridlayout->addWidget(m_tabwidget, 1, 0);
        }

        for(int j = 0; j < XletList.size(); j++) {
		QString dc = XletList[j];
 		if (m_forcetabs || m_allnames.contains(dc)) {
                        bool withscrollbar = m_dockoptions[dc].contains("s");
                        if (dc == "history") {
                                m_xlet[dc] = new LogWidget(m_engine, m_options, this);
                                addPanel("history", tr("History"), m_xlet[dc]);
                                connect( m_engine, SIGNAL(updateLogEntry(const QDateTime &, int,
                                                                         const QString &, const QString &, const QString &)),
                                         m_xlet[dc], SLOT(addLogEntry(const QDateTime &, int,
                                                                      const QString &, const QString &, const QString &)) );
                                connect( m_xlet[dc], SIGNAL(askHistory(const QString &, int)),
                                         m_engine, SLOT(requestHistory(const QString &, int)) );
                                connect( m_engine, SIGNAL(delogged()),
                                         m_xlet[dc], SLOT(clear()) );
                                connect( m_engine, SIGNAL(monitorPeer(UserInfo *)),
                                         m_xlet[dc], SLOT(monitorPeer(UserInfo *)) );
                                
			} else if (dc == QString("identity")) {
                                m_xlet[dc] = new IdentityDisplay(m_engine, m_options);
                                addPanel("identity", tr("&Identity"), m_xlet[dc]);
                                
                                connect( m_engine, SIGNAL(updatePeer(UserInfo *,
                                                                     const QString &,
                                                                     const QHash<QString, QStringList> &)),
                                         m_xlet[dc], SLOT(updatePeer(UserInfo *,
                                                                     const QString &,
                                                                     const QHash<QString, QStringList> &)) );
                                connect( m_engine, SIGNAL(updatePeerAgent(const QString &, const QString &, const QStringList &)),
                                         m_xlet[dc], SLOT(updatePeerAgent(const QString &, const QString &, const QStringList &)));
                                connect( m_engine, SIGNAL(newAgentList(const QVariant &)),
                                         m_xlet[dc], SLOT(setAgentList(const QVariant &)));
                                connect( m_engine, SIGNAL(updatePresence(const QVariant &)),
                                         m_xlet[dc], SLOT(updatePresence(const QVariant &)) );
                                connect( m_xlet[dc], SIGNAL(setAvailState(const QString &, bool)),
                                         m_engine, SLOT(setAvailState(const QString &, bool)) );
                                connect( m_xlet[dc], SIGNAL(changeWatchedAgent(const QString &, bool)),
                                         m_engine, SLOT(changeWatchedAgentSlot(const QString &, bool)));
                                
                                connect( m_engine, SIGNAL(setQueueStatus(const QStringList &)),
                                         m_xlet[dc], SLOT(setQueueStatus(const QStringList &)));
                                connect( m_engine, SIGNAL(newQueueList(bool, const QVariant &)),
                                         m_xlet[dc], SLOT(setQueueList(bool, const QVariant &)));
                                connect( m_xlet[dc], SIGNAL(agentAction(const QString &)),
                                         m_engine, SLOT(agentAction(const QString &)));
                                
			} else if (dc == QString("agents")) {
                                m_xlet[dc] = new AgentsPanel(m_options);
                                if (withscrollbar) {
                                        QScrollArea * sa_ag = new QScrollArea(this);
                                        sa_ag->setWidget(m_xlet[dc]);
                                        sa_ag->setWidgetResizable(true);
                                        addPanel("agents", tr("Agents' List"), sa_ag);
                                } else
                                        addPanel("agents", tr("Agents' List"), m_xlet[dc]);
                                
                                connect( m_engine, SIGNAL(newAgentList(const QVariant &)),
                                         m_xlet[dc], SLOT(setAgentList(const QVariant &)));
                                connect( m_xlet[dc], SIGNAL(changeWatchedAgent(const QString &, bool)),
                                         m_engine, SLOT(changeWatchedAgentSlot(const QString &, bool)));
				connect( m_engine, SIGNAL(updatePeerAgent(const QString &, const QString &, const QStringList &)),
					 m_xlet[dc], SLOT(updatePeerAgent(const QString &, const QString &, const QStringList &)) );
				connect( m_engine, SIGNAL(updateAgentPresence(const QString &, const QString &, const QColor &)),
					 m_xlet[dc], SLOT(updateAgentPresence(const QString &, const QString &, const QColor &)) );
                                connect( m_xlet[dc], SIGNAL(agentAction(const QString &)),
                                         m_engine, SLOT(agentAction(const QString &)));

			} else if (dc == QString("agentdetails")) {
                                m_xlet[dc] = new AgentdetailsPanel();
                                if (withscrollbar) {
                                        QScrollArea * sa_ad = new QScrollArea(this);
                                        sa_ad->setWidget(m_xlet[dc]);
                                        sa_ad->setWidgetResizable(true);
                                        addPanel("agentdetails", tr("Agent Details"), sa_ad);
                                } else
                                        addPanel("agentdetails", tr("Agent Details"), m_xlet[dc]);
                                
                                connect( m_engine, SIGNAL(changeWatchedAgentSignal(const QString &, const QString &, const QVariant &)),
                                         m_xlet[dc], SLOT(newAgent(const QString &, const QString &, const QVariant &)));
                                connect( m_xlet[dc], SIGNAL(changeWatchedQueue(const QString &)),
                                         m_engine, SLOT(changeWatchedQueueSlot(const QString &)));
				connect( m_engine, SIGNAL(updatePeerAgent(const QString &, const QString &, const QStringList &)),
					 m_xlet[dc], SLOT(updatePeerAgent(const QString &, const QString &, const QStringList &)) );
                                connect( m_xlet[dc], SIGNAL(agentAction(const QString &)),
                                         m_engine, SLOT(agentAction(const QString &)));
                                connect( m_engine, SIGNAL(serverFileList(const QStringList &)),
                                         m_xlet[dc], SLOT(serverFileList(const QStringList &)));
                                connect( m_engine, SIGNAL(fileReceived()),
                                         m_xlet[dc], SLOT(saveToFile()));
                                connect( m_xlet[dc], SIGNAL(setFileName(const QString &)),
                                         m_engine, SLOT(saveToFile(const QString &)));
                                connect( m_engine, SIGNAL(statusRecord(const QString &, const QString &)),
                                         m_xlet[dc], SLOT(statusRecord(const QString &, const QString &)));
                                
			} else if (dc == QString("conference")) {
                                m_xlet[dc] = new ConferencePanel();
                                addPanel(dc, tr("Conference"), m_xlet[dc]);

                                connect( m_engine, SIGNAL(meetmeEvent(const QStringList &)),
                                         m_xlet[dc], SLOT(meetmeEvent(const QStringList &)));
                                connect( m_xlet[dc], SIGNAL(meetmeAction(const QString &)),
                                         m_engine, SLOT(meetmeAction(const QString &)));

			} else if (dc == QString("queues")) {
                                m_xlet[dc] = new QueuesPanel(m_options);
                                if (withscrollbar) {
                                        QScrollArea * sa_qu = new QScrollArea(this);
                                        sa_qu->setWidget(m_xlet[dc]);
                                        sa_qu->setWidgetResizable(true);
                                        addPanel("queues", tr("Queues' List"), sa_qu);
                                } else
                                        addPanel("queues", tr("Queues' List"), m_xlet[dc]);
                                
                                connect( m_engine, SIGNAL(updateCounter(const QVariant &)),
                                         m_xlet[dc], SLOT(updateCounter(const QVariant &)));
                                connect( m_engine, SIGNAL(setQueueStatus(const QStringList &)),
                                         m_xlet[dc], SLOT(setQueueStatus(const QStringList &)));
                                connect( m_engine, SIGNAL(newQueueList(bool, const QVariant &)),
                                         m_xlet[dc], SLOT(setQueueList(bool, const QVariant &)));
                                connect( m_engine, SIGNAL(removeQueues(const QString &, const QStringList &)),
                                         m_xlet[dc], SLOT(removeQueues(const QString &, const QStringList &)));
                                connect( m_xlet[dc], SIGNAL(changeWatchedQueue(const QString &)),
                                         m_engine, SLOT(changeWatchedQueueSlot(const QString &)));
				connect( m_engine, SIGNAL(updatePeerAgent(const QString &, const QString &, const QStringList &)),
					 m_xlet[dc], SLOT(updatePeerAgent(const QString &, const QString &, const QStringList &)) );

			} else if (dc == QString("queuedetails")) {
                                m_xlet[dc] = new QueuedetailsPanel();
                                if (withscrollbar) {
                                        QScrollArea * sa_qd = new QScrollArea(this);
                                        sa_qd->setWidget(m_xlet[dc]);
                                        sa_qd->setWidgetResizable(true);
                                        addPanel("queuedetails", tr("Agents of a Queue"), sa_qd);
                                } else
                                        addPanel("queuedetails", tr("Agents of a Queue"), m_xlet[dc]);
                                
                                connect( m_engine, SIGNAL(changeWatchedQueueSignal(const QString &, const QString &, const QVariant &)),
                                         m_xlet[dc], SLOT(newQueue(const QString &, const QString &, const QVariant &)));
                                connect( m_xlet[dc], SIGNAL(changeWatchedAgent(const QString &, bool)),
                                         m_engine, SLOT(changeWatchedAgentSlot(const QString &, bool)));
				connect( m_engine, SIGNAL(updatePeerAgent(const QString &, const QString &, const QStringList &)),
					 m_xlet[dc], SLOT(updatePeerAgent(const QString &, const QString &, const QStringList &)) );
                                
			} else if (dc == QString("queueentrydetails")) {
                                m_xlet[dc] = new QueueentrydetailsPanel();
                                if (withscrollbar) {
                                        QScrollArea * sa_qd = new QScrollArea(this);
                                        sa_qd->setWidget(m_xlet[dc]);
                                        sa_qd->setWidgetResizable(true);
                                        addPanel(dc, tr("Calls of a Queue"), sa_qd);
                                } else
                                        addPanel(dc, tr("Calls of a Queue"), m_xlet[dc]);
                                
                                connect( m_engine, SIGNAL(changeWatchedQueueSignal(const QString &, const QString &, const QVariant &)),
                                         m_xlet[dc], SLOT(newQueue(const QString &, const QString &, const QVariant &)));
                                
			} else if (dc == QString("datetime")) {
				m_xlet[dc] = new DatetimePanel();
                                addPanel(dc, tr("Date and Time"), m_xlet[dc]);

			} else if (dc == QString("dial")) {
				m_xlet[dc] = new DialPanel(m_options);
                                addPanel("dial", tr("Dial"), m_xlet[dc]);

                                connectDials(m_xlet[dc]);
                                connect( this, SIGNAL(pasteToDialPanel(const QString &)),
                                         m_xlet[dc], SLOT(setNumberToDial(const QString &)) );
                                connect( m_engine, SIGNAL(pasteToDialPanel(const QString &)),
                                         m_xlet[dc], SLOT(setNumberToDial(const QString &)) );
                                connect( m_xlet[dc], SIGNAL(textEdited(const QString &)),
                                         m_engine, SLOT(textEdited(const QString &)) );
                                
                        } else if (dc == QString("video")) {
                                m_xlet[dc] = new PlayerWidget(this);
                                addPanel("video", tr("Video"), m_xlet[dc]);

                        } else if (dc == QString("operator")) {
                                m_xlet[dc] = new StatusPanel(this);
                                addPanel("operator", tr("Operator"), m_xlet[dc]);
                                
                                connectDials(m_xlet[dc]);
                                connect( this, SIGNAL(functionKeyPressed(int)),
                                         m_xlet[dc], SLOT(functionKeyPressed(int)));
                                connect( m_engine, SIGNAL(updatePeer(UserInfo *,
                                                                     const QString &,
                                                                     const QHash<QString, QStringList> &)),
                                         m_xlet[dc], SLOT(updatePeer(UserInfo *,
                                                                     const QString &,
                                                                     const QHash<QString, QStringList> &)) );
                                
                        } else if (dc == QString("messages")) {
                                m_xlet[dc] = new DisplayMessagesPanel();
                                addPanel("messages", tr("Messages"), m_xlet[dc]);
                                
                                connect( m_engine, SIGNAL(emitTextMessage(const QString &)),
                                         m_xlet[dc], SLOT(addMessage(const QString &)));
                                
                        } else if (dc == QString("calls")) {
                                m_areaCalls = new QScrollArea(this);
                                m_areaCalls->setWidgetResizable(true);
                                m_leftpanel = new LeftPanel(m_areaCalls);
                                m_calls = new CallStackWidget(this);
                                m_areaCalls->setWidget(m_calls);
                                addPanel("calls", tr("Calls"), m_leftpanel);
                                
                                // connectDials(m_calls);
                                connect( m_engine, SIGNAL(updatePeer(UserInfo *,
                                                                     const QString &,
                                                                     const QHash<QString, QStringList> &)),
                                         m_calls, SLOT(updatePeer(UserInfo *,
                                                                  const QString &,
                                                                  const QHash<QString, QStringList> &)) );
                                connect( m_calls, SIGNAL(changeTitle(const QString &)),
                                         m_leftpanel->titleLabel(), SLOT(setText(const QString &)) );
                                connect( m_engine, SIGNAL(callsUpdated()),
                                         m_calls, SLOT(updateDisplay()) );
                                connect( m_engine, SIGNAL(delogged()),
                                         m_calls, SLOT(reset()) );

                                connect( m_calls, SIGNAL(monitorPeerRequest(const QString &)),
                                         m_engine, SLOT(monitorPeerRequest(const QString &)) );
                                connect( m_engine, SIGNAL(monitorPeer(UserInfo *)),
                                         m_calls, SLOT(monitorPeer(UserInfo *)) );

                                connect( m_calls, SIGNAL(transferToNumber(const QString &)),
                                         m_engine, SLOT(transferToNumber(const QString &)) );
                                connect( m_calls, SIGNAL(parkCall(const QString &)),
                                         m_engine, SLOT(parkCall(const QString &)) );
                                
                        } else if (dc == QString("switchboard")) {
                                m_xlet[dc] = new SwitchBoardWindow(m_engine, this);
                                m_engine->addRemovable(m_xlet[dc]->metaObject());
                                QScrollArea * sa_sb = new QScrollArea(this);
                                sa_sb->setWidget(m_xlet[dc]);
                                sa_sb->setWidgetResizable(true);
                                addPanel(dc, tr("Switchboard"), sa_sb);
                                
                                connect( m_engine, SIGNAL(updatePeer(UserInfo *,
                                                                     const QString &,
                                                                     const QHash<QString, QStringList> &)),
                                         m_xlet[dc], SLOT(updatePeer(UserInfo *,
                                                                     const QString &,
                                                                     const QHash<QString, QStringList> &)) );
                                connect( m_engine, SIGNAL(newUser(UserInfo *)),
                                         m_xlet[dc], SLOT(newUser(UserInfo *)));
				connect( m_engine, SIGNAL(updatePeerAgent(const QString &, const QString &, const QStringList &)),
					 m_xlet[dc], SLOT(updatePeerAgent(const QString &, const QString &, const QStringList &)) );
                                connect( m_engine, SIGNAL(delogged()),
                                         m_xlet[dc], SLOT(removePeers()) );
                                connect( m_engine, SIGNAL(removePeer(const QString &)),
                                         m_xlet[dc], SLOT(removePeer(const QString &)) );
                                
                        } else if (dc == QString("parking")) {
                                m_xlet[dc] = new ParkingPanel();
                                addPanel(dc, tr("Parking"), m_xlet[dc]);
                                
                                connect( m_engine, SIGNAL(parkingEvent(const QString &)),
                                         m_xlet[dc], SLOT(parkingEvent(const QString &)));
                                connect( m_xlet[dc], SIGNAL(copyNumber(const QString &)),
                                         m_engine, SLOT(copyNumber(const QString &)) );
                                connect( m_xlet[dc], SIGNAL(newParkEvent()),
                                         this, SLOT(newParkEvent()) );
                                connectDials(m_xlet[dc]);
                                
                        } else if (dc == QString("fax")) {
				m_xlet[dc] = new FaxPanel(m_engine, m_options, this);
                                addPanel("fax", tr("Fax"), m_xlet[dc]);
                                
                                connect( m_xlet[dc], SIGNAL(faxSend(const QString &, const QString &, Qt::CheckState)),
                                         m_engine, SLOT(sendFaxCommand(const QString &, const QString &, Qt::CheckState)) );
                                connect( m_engine, SIGNAL(ackFax(const QString &, const QString &)),
                                         m_xlet[dc], SLOT(popupMsg(const QString &, const QString &)) );
                                
			} else if ((dc == "customerinfo") && m_engine->checkedFunction(dc)) {
                                m_xlet[dc] = new CustomerInfoPanel(m_options);
                                addPanel("customerinfo", tr("Sheets"), m_xlet[dc]);
                                
                                connect( m_engine, SIGNAL(displayFiche(const QString &, bool)),
                                         m_xlet[dc], SLOT(displayFiche(const QString &, bool)) );
                                connect( m_xlet[dc], SIGNAL(newPopup(const QString &, const QHash<QString, QString> &, const QString &)),
                                         this, SLOT(customerInfoPopup(const QString &, const QHash<QString, QString> &, const QString &)) );
                                connect( m_xlet[dc], SIGNAL(actionFromFiche(const QStringList &)),
                                         m_engine, SLOT(actionFromFiche(const QStringList &)) );

			} else if (dc == QString("search")) {
				m_xlet[dc] = new SearchPanel(m_engine, m_options);
                                addPanel(dc, tr("Contacts"), m_xlet[dc]);
                                
                                connect( m_engine, SIGNAL(updatePeer(UserInfo *,
                                                                     const QString &,
                                                                     const QHash<QString, QStringList> &)),
                                         m_xlet[dc], SLOT(updatePeer(UserInfo *,
                                                                     const QString &,
                                                                     const QHash<QString, QStringList> &)) );
                                connect( m_engine, SIGNAL(newUser(UserInfo *)),
                                         m_xlet[dc], SLOT(newUser(UserInfo *)));
				connect( m_engine, SIGNAL(updatePeerAgent(const QString &, const QString &, const QStringList &)),
					 m_xlet[dc], SLOT(updatePeerAgent(const QString &, const QString &, const QStringList &)) );
				connect( m_engine, SIGNAL(peersReceived()),
					 m_xlet[dc], SLOT(callsUpdated()) );
				connect( m_xlet[dc], SIGNAL(askCallerIds()),
					 m_engine, SLOT(askCallerIds()) );
                                connect( m_engine, SIGNAL(delogged()),
                                         m_xlet[dc], SLOT(removePeers()) );
                                connect( m_engine, SIGNAL(removePeer(const QString &)),
                                         m_xlet[dc], SLOT(removePeer(const QString &)) );
                                
                        } else if (dc == QString("features")) {
                                m_xlet[dc] = new ServicePanel(m_options);
                                addPanel("features", tr("Services"), m_xlet[dc]);
                                
                                connect( m_xlet[dc], SIGNAL(askFeatures()),
                                         m_engine, SLOT(askFeatures()) );
                                connect( m_engine, SIGNAL(monitorPeer(UserInfo *)),
                                         m_xlet[dc], SLOT(monitorPeer(UserInfo *)) );
                                
                                connect( m_engine, SIGNAL(disconnectFeatures()),
                                         m_xlet[dc], SLOT(DisConnect()) );
                                connect( m_engine, SIGNAL(connectFeatures()),
                                         m_xlet[dc], SLOT(Connect()) );
                                connect( m_engine, SIGNAL(resetFeatures()),
                                         m_xlet[dc], SLOT(Reset()) );
                                connect( m_engine, SIGNAL(featurePutIsKO()),
                                         m_xlet[dc], SLOT(getRecordedStatus()) );
                                connect( m_engine, SIGNAL(featurePutIsOK()),
                                         m_xlet[dc], SLOT(setRecordedStatus()) );
                                
                                connect( m_xlet[dc], SIGNAL(chkoptChanged(const QString &, bool)),
                                         m_engine, SLOT(featurePutOpt(const QString &, bool)) );
                                
                                connect( m_engine, SIGNAL(optChanged(const QString &, bool)),
                                         m_xlet[dc], SLOT(setOpt(const QString &, bool)) );                                
                                connect( m_xlet[dc], SIGNAL(forwardChanged(const QString &, bool, const QString &)),
                                         m_engine, SLOT(featurePutForward(const QString &, bool, const QString &)) );
                                connect( m_engine, SIGNAL(forwardUpdated(const QString &, bool, const QString &)),
                                         m_xlet[dc], SLOT(setForward(const QString &, bool, const QString &)) );
                                
			} else if (dc == QString("directory")) {
                                m_xlet[dc] = new DirectoryPanel(m_options);
                                addPanel("directory", tr("Directory"), m_xlet[dc]);
                                m_xlet[dc]->setFocus();
                                
                                connectDials(m_xlet[dc]);
                                connect( m_engine, SIGNAL(updatePeer(UserInfo *,
                                                                     const QString &,
                                                                     const QHash<QString, QStringList> &)),
                                         m_xlet[dc], SLOT(updatePeer(UserInfo *,
                                                                     const QString &,
                                                                     const QHash<QString, QStringList> &)) );
				connect( m_xlet[dc], SIGNAL(searchDirectory(const QString &)),
					 m_engine, SLOT(searchDirectory(const QString &)) );
				connect( m_engine, SIGNAL(directoryResponse(const QString &)),
					 m_xlet[dc], SLOT(setSearchResponse(const QString &)) );
				connect( m_xlet[dc], SIGNAL(copyNumber(const QString &)),
					 m_engine, SLOT(copyNumber(const QString &)) );
                                connect( m_engine, SIGNAL(delogged()),
                                         m_xlet[dc], SLOT(stop()) );
                                
#ifdef USE_OUTLOOK
			} else if ((dc == QString("outlook")) ) {
                                m_xlet[dc] = new OutlookPanel(this);
                                addPanel("outlook", tr("Outlook"), m_xlet[dc]);
                                m_xlet[dc]->setFocus();
                                
				connect( m_xlet[dc], SIGNAL(searchOutlook(const QString &)),
					 m_engine, SLOT(searchOutlook(const QString &)) );
				connect( m_xlet[dc], SIGNAL(emitDial(const QString &)),
					 m_engine, SLOT(dialFullChannel(const QString &)) );
				connect( m_xlet[dc], SIGNAL(copyNumber(const QString &)),
					 m_engine, SLOT(copyNumber(const QString &)) );
				connect( m_engine, SIGNAL(outlookResponse(const QString &)),
					 m_xlet[dc], SLOT(setSearchResponse(const QString &)) );
#endif /* USE_OUTLOOK */

			} else if (dc == QString("instantmessaging")) {
                                m_xlet[dc] = new QLineEdit();
                                addPanel("instantmessaging", tr("Messages"), m_xlet[dc]);
                                
                                connect( m_xlet[dc], SIGNAL(returnPressed()),
                                         this, SLOT(affTextChanged()) );
                                
                        } else if (dc == QString("callcampaign")) {
                                m_xlet[dc] = new CallCampaignPanel();
                                addPanel("callcampaign", tr("Call Campaigns"), m_xlet[dc]);
                                connect( m_xlet[dc], SIGNAL(requestFileList(const QString &)),
                                         m_engine, SLOT(requestFileList(const QString &)));
                                connect( m_engine, SIGNAL(requestFileListResult(const QString &)),
                                         m_xlet[dc], SLOT(requestFileListResult(const QString &)));

                        } else if (dc == QString("mylocaldir")) {
                                m_xlet[dc] = new MyLocalDirPanel(m_engine);
                                addPanel("mylocaldir", tr("Personal Directory"), m_xlet[dc]);
                                
                        } else if (dc == QString("xletproto")) {
                                m_xlet[dc] = new XletprotoPanel(m_engine);
                                addPanel("xletproto", tr("Xlet Prototype"), m_xlet[dc]);
                        }
                }
        }
        
        qDebug() << "MainWidget::engineStarted() : the xlets have been created";
        m_tabwidget->setCurrentIndex(m_settings->value("display/lastfocusedtab").toInt());
        
        foreach (QString dname, m_docknames)
                m_docks[dname]->show();
        
	// restore settings, especially for Docks' positions
        restoreState(m_settings->value("display/mainwindowstate").toByteArray());

        if(m_tabnames.contains("customerinfo") && m_engine->checkedFunction("customerinfo")) {
                m_cinfo_index = m_tabwidget->indexOf(m_xlet["customerinfo"]);
                qDebug() << "the index of customer-info widget is" << m_cinfo_index;
        }

        if(m_withsystray && m_systrayIcon)
                m_systrayIcon->setIcon(m_icon);

        statusBar()->showMessage(tr("Connected"));
        m_connectact->setEnabled(false);
        m_disconnectact->setEnabled(true);
        // set status icon to green
        QPixmap greensquare(":/images/connected.png");
        m_status->setPixmap(greensquare);
}


void MainWidget::removePanel(const QString & name, QWidget * widget)
{
        disconnect( m_engine, SIGNAL(setGuiOptions(const QVariant &)),
                    m_xlet[name], SLOT(setGuiOptions(const QVariant &)));
        disconnect( m_engine, SIGNAL(localUserInfoDefined(const UserInfo *)),
                    m_xlet[name], SLOT(setUserInfo(const UserInfo *)));
        if(m_docknames.contains(name)) {
                removeDockWidget(m_docks[name]);
                delete widget;
                delete m_docks[name]; // seems to "delete widget", also
                m_docks.remove(name);
        }
        if(m_tabnames.contains(name)) {
                int thisindex = m_tabwidget->indexOf(widget);
                if (thisindex > -1) {
                        qDebug() << "removing" << name << thisindex;
                        m_tabwidget->removeTab(thisindex);
                }
                delete widget;
        }
        if(m_gridnames.contains(name)) {
                m_gridlayout->removeWidget(widget);
                delete widget;
        }
}

/*!
 * disables the "Disconnect" action and enables the "Connect" Action.
 * sets the Red indicator
 * \sa engineStarted()
 */
void MainWidget::engineStopped()
{
        qDebug() << "MainWidget::engineStopped()";
        m_settings->setValue("display/mainwindowstate", saveState());
        if (m_tabwidget->currentIndex() > -1) {
                // qDebug() << m_tabwidget->currentIndex();
                m_settings->setValue("display/lastfocusedtab", m_tabwidget->currentIndex());
                // qDebug() << m_tabwidget->tabText(m_tabwidget->currentIndex());
        }
        
        foreach (QString dname, m_docknames)
                m_docks[dname]->hide();
        clearPresence();
        
	for(int j = 0; j < XletList.size(); j++) {
                QString dc = XletList[j];
 		if (m_forcetabs || m_allnames.contains(dc)) {
                        if ((dc == QString("customerinfo")) && m_engine->checkedFunction("customerinfo")) {
                                removePanel(dc, m_xlet[dc]);
                        } else if (dc == QString("calls")) {
                                removePanel("calls", m_leftpanel);
                                //delete m_calls;
                                //delete m_areaCalls;
                                //delete m_leftpanel;
                        } else
                                removePanel(dc, m_xlet[dc]);
                }
        }
        
        if(m_docknames.contains("tabber")) {
                removePanel("tabber", m_tabwidget);
        }
        if(m_gridnames.contains("tabber")) {
                m_gridlayout->removeWidget(m_tabwidget);
                delete m_tabwidget;
        }

        showLogin();
        
        if(m_withsystray && m_systrayIcon)
                m_systrayIcon->setIcon(m_icongrey);
        
        statusBar()->showMessage(tr("Disconnected"));
        m_connectact->setEnabled(true);
        m_disconnectact->setEnabled(false);
        // set status icon to red
        QPixmap redsquare(":/images/disconnected.png");
        m_status->setPixmap(redsquare);

        clearAppearance();
        m_appliname = "Clients";
        updateAppliName();
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

void MainWidget::showEvent(QShowEvent *event)
{
        // qDebug() << "MainWidget::showEvent()";
        event->accept();
        // << "spontaneous =" << event->spontaneous()
        // << "isMinimized =" << isMinimized()
        // << "isVisible ="   << isVisible()
        // << "isActiveWindow =" << isActiveWindow();
}

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
                foreach (QString order, orders)
                        todisp.append(msgs[order]);
                m_systrayIcon->showMessage(msgtitle,
                                           todisp.join("\n"),
                                           QSystemTrayIcon::Information,
                                           5000);
        }
        
        // focus on the customerinfo tab
        if(m_tabnames.contains("customerinfo") && m_engine->checkedFunction("customerinfo") && options.contains("f"))
                if (m_cinfo_index > -1)
                        m_tabwidget->setCurrentIndex(m_cinfo_index);
        
        // to be customisable, if the user wants the window to popup
        if(options.contains("p")) {
                setVisible(true);
                activateWindow();
                raise();
        }
}

void MainWidget::newParkEvent()
{
        qDebug() << "MainWidget::newParkEvent()";

        int index_parking = m_tabwidget->indexOf(m_xlet["parking"]);
        if(index_parking > -1)
                m_tabwidget->setCurrentIndex(index_parking);
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
        //  	if ( QSystemTrayIcon::isSystemTrayAvailable() ) {
        //                 qDebug() << "MainWidget::hideEvent() setVisible(false)";
        //  		setVisible(false);
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
        // qDebug() << "MainWidget::closeEvent()";
        // << "spontaneous =" << event->spontaneous()
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
	//	event->accept();
		//event->ignore();
}

void MainWidget::keyPressEvent(QKeyEvent * event)
{
        if((event->key() >= Qt::Key_F1) && (event->key() <= Qt::Key_F35))
                qDebug() << "MainWidget::keyPressEvent F<n>" << (event->key() - Qt::Key_F1 + 1);
        else if(event->key() == Qt::Key_Up)
                qDebug() << "MainWidget::keyPressEvent Up";
        else if(event->key() == Qt::Key_Down)
                qDebug() << "MainWidget::keyPressEvent Down";
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
	//	event->accept();
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
        QString fetchlastone = "<a href=http://www.xivo.fr/download/xivo_cti_client/"
#if defined(Q_WS_X11)
                "linux"
#elif defined(Q_WS_WIN)
                "win32"
#elif defined(Q_WS_MAC)
                "macos"
#endif
                ">" + tr("last one") + "</a>";
        
        // might be useful to display whether QSystemTrayIcon::isSystemTrayAvailable() is true
        QMessageBox::about(this,
                           tr("About XIVO %1").arg(m_appliname),
                           "<h3>XIVO " + m_appliname + "</h3>" +
                           tr("<p>The XIVO CTI applications, once connected to the proper server, "
                              "complete the XIVO telephony system, by providing computer tools, "
                              "such as customer information display, directory searches, or various "
                              "supervision 'X-lets' for call centers.") +
			   "<p><b>" + tr("Version : ") + QString("</b>%1 (").arg(applicationVersion) +
			   "<b>svn : " + QString("</b>%1 - %2)</p>").arg(__current_client_version__,
                                                                         fetchlastone) +
                           "<p>(" + tr("Advised Server Version : ") + __required_server_version__ + ")</p>" +
                           "<p>(" + tr("Application Launched on : ") + m_launchDateTime.toString() + ")</p>" +
			   "<hr><p>Copyright (C) 2007, 2008  <a href=http://www.proformatique.com><b>Proformatique</b></a></p>"
			   "<p>67 rue Voltaire 92800 Puteaux FRANCE</p>"
			   "<p><b>E-mail : </b><a href=mailto:technique@proformatique.com>technique@proformatique.com</p>"
			   "<p>(+33 / 0) 1.41.38.99.60</p>" +
			   "<p><b>" + tr("Authors : ") + "</b>Thomas Bernard, Corentin Le Gall</p>" +
			   "<hr><p><b>" + tr("License : ") + "</b>" +
			   "<a href=http://www.gnu.org/licenses/gpl-2.0.txt>GNU General Public License v2</a></p>"
			   "<p>" + tr("with a special exception allowing you to link it with OpenSSL, under some conditions.") + "</p>");
}

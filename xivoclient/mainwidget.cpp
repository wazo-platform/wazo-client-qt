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
#include "parkingpanel.h"
#include "popup.h"
#include "queuespanel.h"
#include "queuedetailspanel.h"
#include "queueentrydetailspanel.h"
#include "searchpanel.h"
#include "servicepanel.h"
#include "statuspanel.h"
#include "switchboardwindow.h"
#include "videopanel.h"
#include "xivoconsts.h"

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
                m_loginkind = new QCheckBox(tr("Login as Agent"));

                connect( m_qlab1, SIGNAL(returnPressed()),
                         this, SLOT(config_and_start()) );
                connect( m_qlab1, SIGNAL(textChanged(const QString &)),
                         this, SLOT(logintextChanged(const QString &)) );
                connect( m_qlab2, SIGNAL(returnPressed()),
                         this, SLOT(config_and_start()) );
                connect( m_qlab3, SIGNAL(returnPressed()),
                         this, SLOT(config_and_start()) );
                connect( m_ack, SIGNAL(pressed()),
                         this, SLOT(config_and_start()) );
                connect( m_loginkind, SIGNAL(stateChanged(int)),
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
        // X11 : when a pattern is selected on (seemingly) any KDE(QT) application on Linux
        // X11 (non-KDE) : we don't get the signal, but the data can be retrieved anyway (the question "when ?" remains)
        
        // qDebug() << "BaseEngine::clipselection()" << m_clipboard->text(QClipboard::Selection);
        // statusBar()->showMessage("selected : " + m_clipboard->text(QClipboard::Selection));
}

void MainWidget::clipdata()
{
        // WIN : we fall here in any Ctrl-C/Ctrl-X/"copy"/... action
        // X11 : same actions, on (seemingly) any KDE(QT) application
        // X11 (non-KDE) : we don't get the signal, but the data can be retrieved anyway (the question "when ?" remains)
        // X11 (non-KDE) : however, the xclipboard application seems to be able to catch such signals ...
        
        // qDebug() << "BaseEngine::clipdata()" << m_clipboard->text(QClipboard::Clipboard);
        // statusBar()->showMessage("data : " + m_clipboard->text(QClipboard::Clipboard));
}

void MainWidget::setAppearance(const QStringList & dockoptions)
{
        qDebug() << "MainWidget::setAppearance()" << dockoptions;
        foreach (QString dname, dockoptions) {
                if(dname.size() > 0) {
                        QStringList dopt = dname.split(":");
                        QString wname = dopt[0];
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
                m_lab3->show();
                m_qlab3->show();
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
                m_gridlayout->addWidget(m_kpass, 3, 2);

                m_gridlayout->addWidget(m_lab3, 4, 0, Qt::AlignRight);
                m_gridlayout->addWidget(m_qlab3, 4, 1);
                m_gridlayout->addWidget(m_loginkind, 4, 2);

                // show widgets after they have been put in the layout, in order for
                // temporary windows not to be opened
                m_lab1->show();
                m_lab2->show();
                m_qlab1->show();
                m_qlab2->show();
                m_ack->show();
                m_kpass->show();
                m_loginkind->show();
                loginKindChanged(m_loginkind->checkState());

                m_qlab1->setText(m_engine->userId());
                m_qlab2->setText(m_engine->password());
                m_qlab3->setText(m_engine->phonenumber());
                m_kpass->setCheckState((m_engine->keeppass() == 2) ? Qt::Checked : Qt::Unchecked);
                m_loginkind->setCheckState((m_engine->loginkind() == 2) ? Qt::Checked : Qt::Unchecked);
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
	QString txt = m_messagetosend->text();
	txt.replace(" ", "_");
	m_engine->sendMessage(txt.toUtf8());
	m_messagetosend->setText("");
}

void MainWidget::createActions()
{
	m_cfgact = new QAction(tr("Confi&gure"), this);
	m_cfgact->setStatusTip(tr("Configure account and connection options"));
	connect( m_cfgact, SIGNAL(triggered()),
		 this, SLOT(showConfDialog()) );

	m_quitact = new QAction(tr("&Quit"), this);
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
	m_disconnectact->setStatusTip(tr("Disconnect from the server"));
	connect( m_disconnectact, SIGNAL(triggered()),
		 m_engine, SLOT(stop()) );

	m_connectact->setEnabled(true);
	m_disconnectact->setEnabled(false);

	// Availability actions :
	m_availgrp = new QActionGroup( this );
	m_availgrp->setExclusive(true);

	m_avact_avail = new QAction( tr("&Available"), this );
	m_avact_avail->setCheckable(true);
	connect( m_avact_avail, SIGNAL(triggered()),
	         m_engine, SLOT(setAvailable()) );
	m_availgrp->addAction( m_avact_avail );
	m_avact_away = new QAction( tr("A&way"), this );
	m_avact_away->setCheckable(true);
	connect( m_avact_away, SIGNAL(triggered()),
	         m_engine, SLOT(setAway()) );
	m_availgrp->addAction( m_avact_away );
	m_avact_brb = new QAction( tr("&Be Right Back"), this );
	m_avact_brb->setCheckable(true);
	connect( m_avact_brb, SIGNAL(triggered()),
	         m_engine, SLOT(setBeRightBack()) );
	m_availgrp->addAction( m_avact_brb );
	m_avact_otl = new QAction( tr("&Out To Lunch"), this );
	m_avact_otl->setCheckable(true);
	connect( m_avact_otl, SIGNAL(triggered()),
	         m_engine, SLOT(setOutToLunch()) );
	m_availgrp->addAction( m_avact_otl );
	m_avact_dnd = new QAction( tr("&Do not disturb"), this );
	m_avact_dnd->setCheckable(true);
	connect( m_avact_dnd, SIGNAL(triggered()),
	         m_engine, SLOT(setDoNotDisturb()) );
	m_availgrp->addAction( m_avact_dnd );
        
        connect( m_engine, SIGNAL(changesAvailChecks()),
                 this, SLOT(checksAvailState()) );
        
        checksAvailState();
}

void MainWidget::checksAvailState()
{
	if (m_engine->getAvailState() == QString("berightback"))
		m_avact_brb->setChecked( true );
	else if (m_engine->getAvailState() == QString("donotdisturb"))
		m_avact_dnd->setChecked( true );
	else if (m_engine->getAvailState() == QString("away"))
		m_avact_away->setChecked( true );
	else if (m_engine->getAvailState() == QString("outtolunch"))
		m_avact_otl->setChecked( true );
	else
		m_avact_avail->setChecked( true );
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
	m_avail->addActions( m_availgrp->actions() );
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
        qDebug() << "MainWidget::confUpdated";
        m_qlab1->setText(m_engine->userId());
        m_qlab2->setText(m_engine->password());
        m_qlab3->setText(m_engine->phonenumber());
        m_kpass->setCheckState((m_engine->keeppass() == 2) ? Qt::Checked : Qt::Unchecked);
        m_loginkind->setCheckState((m_engine->loginkind() == 2) ? Qt::Checked : Qt::Unchecked);
}

/*! \brief process clicks to the systray icon
 *
 * This slot is connected to the activated() signal of the
 * System Tray icon. It currently toggle the visibility
 * of the MainWidget on a simple left click. */
void MainWidget::systrayActivated(QSystemTrayIcon::ActivationReason reason)
{
        qDebug() << "MainWidget::systrayActivated()";
        // << "reason =" << reason
        // << "isMinimized =" << isMinimized()
        // << "isVisible =" << isVisible()
        // << "isActiveWindow =" << isActiveWindow();
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
                qDebug() << "MainWidget::addPanel() dock : " << name;

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
                qDebug() << "MainWidget::addPanel() grid : " << m_dockoptions[name];
                m_gridlayout->addWidget(widget, m_dockoptions[name].toInt(), 0);
        } else if(m_tabnames.contains(name))
                m_tabwidget->addTab(widget, extraspace + title + extraspace);
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
                        if (dc == QString("history")) {
                                m_historypanel = new LogWidget(m_engine, this);
                                addPanel("history", tr("History"), m_historypanel);
                                connect( m_engine, SIGNAL(updateLogEntry(const QDateTime &, int,
                                                                         const QString &, const QString &, const QString &)),
                                         m_historypanel, SLOT(addLogEntry(const QDateTime &, int,
                                                                          const QString &, const QString &, const QString &)) );
                                connect( m_historypanel, SIGNAL(askHistory(const QString &, int)),
                                         m_engine, SLOT(requestHistory(const QString &, int)) );
                                connect( m_engine, SIGNAL(delogged()),
                                         m_historypanel, SLOT(clear()) );
                                connect( m_engine, SIGNAL(setPeerToDisplay(const QString &)),
                                         m_historypanel, SLOT(setPeerToDisplay(const QString &)) );
                                connect( m_engine, SIGNAL(localUserInfoDefined(const UserInfo *)),
                                         m_historypanel, SLOT(setUserInfo(const UserInfo *)));

			} else if (dc == QString("identity")) {
                                m_infowidget = new IdentityDisplay();
                                m_infowidget->setEngine(m_engine);
                                addPanel("identity", tr("&Identity"), m_infowidget);
                                connect( m_engine, SIGNAL(localUserInfoDefined(const UserInfo *)),
                                         m_infowidget, SLOT(setUserInfo(const UserInfo *)));
				connect( m_engine, SIGNAL(updatePeer(UserInfo *,
								     const QString &,
								     const QStringList &, const QStringList &,
								     const QStringList &, const QStringList &)),
					 m_infowidget, SLOT(updatePeer(UserInfo *,
                                                                       const QString &,
                                                                       const QStringList &, const QStringList &,
                                                                       const QStringList &, const QStringList &)) );
                                connect( m_engine, SIGNAL(updatePeerAgent(const QString &, const QString &, const QStringList &)),
                                         m_infowidget, SLOT(updatePeerAgent(const QString &, const QString &, const QStringList &)));
                                connect( m_engine, SIGNAL(newAgentList(const QString &)),
                                         m_infowidget, SLOT(setAgentList(const QString &)));
                                
                                connect( m_engine, SIGNAL(setQueueStatus(const QString &)),
                                         m_infowidget, SLOT(setQueueStatus(const QString &)));
                                connect( m_engine, SIGNAL(newQueueList(bool, const QString &)),
                                         m_infowidget, SLOT(setQueueList(bool, const QString &)));
                                connect( m_infowidget, SIGNAL(agentAction(const QString &)),
                                         m_engine, SLOT(agentAction(const QString &)));

			} else if (dc == QString("agents")) {
                                m_agentspanel = new AgentsPanel();
                                if (withscrollbar) {
                                        QScrollArea * sa_ag = new QScrollArea(this);
                                        sa_ag->setWidget(m_agentspanel);
                                        sa_ag->setWidgetResizable(true);
                                        addPanel("agents", tr("Agents' List"), sa_ag);
                                } else
                                        addPanel("agents", tr("Agents' List"), m_agentspanel);
                                
                                connect( m_engine, SIGNAL(localUserInfoDefined(const UserInfo *)),
                                         m_agentspanel, SLOT(setUserInfo(const UserInfo *)));
                                connect( m_engine, SIGNAL(newAgentList(const QString &)),
                                         m_agentspanel, SLOT(setAgentList(const QString &)));
                                connect( m_agentspanel, SIGNAL(changeWatchedAgent(const QString &)),
                                         m_engine, SLOT(changeWatchedAgentSlot(const QString &)));
				connect( m_engine, SIGNAL(updatePeerAgent(const QString &, const QString &, const QStringList &)),
					 m_agentspanel, SLOT(updatePeerAgent(const QString &, const QString &, const QStringList &)) );
				connect( m_engine, SIGNAL(updateAgentPresence(const QString &, const QString &)),
					 m_agentspanel, SLOT(updateAgentPresence(const QString &, const QString &)) );
                                connect( m_agentspanel, SIGNAL(agentAction(const QString &)),
                                         m_engine, SLOT(agentAction(const QString &)));

			} else if (dc == QString("agentdetails")) {
                                m_agentdetailspanel = new AgentdetailsPanel();
                                if (withscrollbar) {
                                        QScrollArea * sa_ad = new QScrollArea(this);
                                        sa_ad->setWidget(m_agentdetailspanel);
                                        sa_ad->setWidgetResizable(true);
                                        addPanel("agentdetails", tr("Agent Details"), sa_ad);
                                } else
                                        addPanel("agentdetails", tr("Agent Details"), m_agentdetailspanel);
                                
                                connect( m_engine, SIGNAL(changeWatchedAgentSignal(const QStringList &)),
                                         m_agentdetailspanel, SLOT(newAgent(const QStringList &)));
                                connect( m_agentdetailspanel, SIGNAL(changeWatchedQueue(const QString &)),
                                         m_engine, SLOT(changeWatchedQueueSlot(const QString &)));
				connect( m_engine, SIGNAL(updatePeerAgent(const QString &, const QString &, const QStringList &)),
					 m_agentdetailspanel, SLOT(updatePeerAgent(const QString &, const QString &, const QStringList &)) );
                                connect( m_agentdetailspanel, SIGNAL(agentAction(const QString &)),
                                         m_engine, SLOT(agentAction(const QString &)));

			} else if (dc == QString("conference")) {
                                m_conferencepanel = new ConferencePanel();
                                addPanel("conference", tr("Conference"), m_conferencepanel);

			} else if (dc == QString("queues")) {
                                m_queuespanel = new QueuesPanel();
				m_queuespanel->setEngine(m_engine);
                                if (withscrollbar) {
                                        QScrollArea * sa_qu = new QScrollArea(this);
                                        sa_qu->setWidget(m_queuespanel);
                                        sa_qu->setWidgetResizable(true);
                                        addPanel("queues", tr("Queues' List"), sa_qu);
                                } else
                                        addPanel("queues", tr("Queues' List"), m_queuespanel);
                                
                                connect( m_engine, SIGNAL(setQueueStatus(const QString &)),
                                         m_queuespanel, SLOT(setQueueStatus(const QString &)));
                                connect( m_engine, SIGNAL(newQueueList(bool, const QString &)),
                                         m_queuespanel, SLOT(setQueueList(bool, const QString &)));
                                connect( m_queuespanel, SIGNAL(changeWatchedQueue(const QString &)),
                                         m_engine, SLOT(changeWatchedQueueSlot(const QString &)));
				connect( m_engine, SIGNAL(updatePeerAgent(const QString &, const QString &, const QStringList &)),
					 m_queuespanel, SLOT(updatePeerAgent(const QString &, const QString &, const QStringList &)) );

			} else if (dc == QString("queuedetails")) {
                                m_queuedetailspanel = new QueuedetailsPanel();
                                if (withscrollbar) {
                                        QScrollArea * sa_qd = new QScrollArea(this);
                                        sa_qd->setWidget(m_queuedetailspanel);
                                        sa_qd->setWidgetResizable(true);
                                        addPanel("queuedetails", tr("Agents of a Queue"), sa_qd);
                                } else
                                        addPanel("queuedetails", tr("Agents of a Queue"), m_queuedetailspanel);
                                
                                connect( m_engine, SIGNAL(changeWatchedQueueSignal(const QStringList &)),
                                         m_queuedetailspanel, SLOT(newQueue(const QStringList &)));
                                connect( m_queuedetailspanel, SIGNAL(changeWatchedAgent(const QString &)),
                                         m_engine, SLOT(changeWatchedAgentSlot(const QString &)));
				connect( m_engine, SIGNAL(updatePeerAgent(const QString &, const QString &, const QStringList &)),
					 m_queuedetailspanel, SLOT(updatePeerAgent(const QString &, const QString &, const QStringList &)) );

			} else if (dc == QString("queueentrydetails")) {
                                m_queueentrydetailspanel = new QueueentrydetailsPanel();
                                if (withscrollbar) {
                                        QScrollArea * sa_qd = new QScrollArea(this);
                                        sa_qd->setWidget(m_queueentrydetailspanel);
                                        sa_qd->setWidgetResizable(true);
                                        addPanel("queueentrydetails", tr("Calls of a Queue"), sa_qd);
                                } else
                                        addPanel("queueentrydetails", tr("Calls of a Queue"), m_queueentrydetailspanel);
                                
                                connect( m_engine, SIGNAL(changeWatchedQueueSignal(const QStringList &)),
                                         m_queueentrydetailspanel, SLOT(newQueue(const QStringList &)));

			} else if (dc == QString("datetime")) {
				m_datetimepanel = new DatetimePanel();
                                addPanel("datetime", tr("Date and Time"), m_datetimepanel);

			} else if (dc == QString("dial")) {
				m_dialpanel = new DialPanel();
				m_dialpanel->setEngine(m_engine);
                                addPanel("dial", tr("Dial"), m_dialpanel);

                                connect( m_engine, SIGNAL(localUserInfoDefined(const UserInfo *)),
                                         m_dialpanel, SLOT(setUserInfo(const UserInfo *)));
                                connectDials(m_dialpanel);
                                connect( m_engine, SIGNAL(pasteToDialPanel(const QString &)),
                                         m_dialpanel, SLOT(setNumberToDial(const QString &)) );
                                connect( m_dialpanel, SIGNAL(textEdited(const QString &)),
                                         m_engine, SLOT(textEdited(const QString &)) );
                                
                        } else if (dc == QString("video")) {
                                m_videopanel = new PlayerWidget(this);
                                addPanel("video", tr("Video"), m_videopanel);

                        } else if (dc == QString("operator")) {
                                m_statuspanel = new StatusPanel(this);
                                addPanel("operator", tr("Operator"), m_statuspanel);

                                connect( m_engine, SIGNAL(localUserInfoDefined(const UserInfo *)),
                                         m_statuspanel, SLOT(setUserInfo(const UserInfo *)));
                                connectDials(m_statuspanel);
                                connect( this, SIGNAL(functionKeyPressed(int)),
                                         m_statuspanel, SLOT(functionKeyPressed(int)));
                                connect( m_engine, SIGNAL(updatePeer(UserInfo *,
                                                                     const QString &,
                                                                     const QStringList &, const QStringList &,
                                                                     const QStringList &, const QStringList &)),
                                         m_statuspanel, SLOT(updatePeer(UserInfo *,
                                                                        const QString &,
                                                                        const QStringList &, const QStringList &,
                                                                        const QStringList &, const QStringList &)) );
                                
                        } else if (dc == QString("messages")) {
                                m_messages_widget = new DisplayMessagesPanel();
                                addPanel("messages", tr("Messages"), m_messages_widget);
                                
                                connect( m_engine, SIGNAL(emitTextMessage(const QString &)),
                                         m_messages_widget, SLOT(addMessage(const QString &)));
                                
                        } else if (dc == QString("calls")) {
                                m_areaCalls = new QScrollArea(this);
                                m_areaCalls->setWidgetResizable(true);
                                m_leftpanel = new LeftPanel(m_areaCalls);
                                m_calls = new CallStackWidget(this, m_engine);
                                m_areaCalls->setWidget(m_calls);
                                addPanel("calls", tr("Calls"), m_leftpanel);
                                
                                // connectDials(m_calls);
                                connect( m_calls, SIGNAL(changeTitle(const QString &)),
                                         m_leftpanel->titleLabel(), SLOT(setText(const QString &)) );
                                connect( m_calls, SIGNAL(monitoredPeerChanged(const QString &)),
                                         m_engine, SLOT(monitoredPeerChanged(const QString &)) );
                                connect( m_engine, SIGNAL(updateCall(UserInfo *,
                                                                     const QString &, const QString &,
                                                                     int, const QString &,
                                                                     const QString &, const QString &)),
                                         m_calls, SLOT(addCall(UserInfo *,
                                                               const QString &, const QString &,
                                                               int, const QString &,
                                                               const QString &, const QString &)) );
                                connect( m_engine, SIGNAL(callsUpdated()),
                                         m_calls, SLOT(updateDisplay()) );
                                connect( m_engine, SIGNAL(delogged()),
                                         m_calls, SLOT(reset()) );
                                connect( m_engine, SIGNAL(monitorPeer(const QString &, const QString &)),
                                         m_calls, SLOT(monitorPeer(const QString &, const QString &)) );
                                connect( m_calls, SIGNAL(transferToNumber(const QString &)),
                                         m_engine, SLOT(transferToNumber(const QString &)) );
                                connect( m_calls, SIGNAL(parkCall(const QString &)),
                                         m_engine, SLOT(parkCall(const QString &)) );
                                
                        } else if (dc == QString("switchboard")) {
                                m_sbwidget = new SwitchBoardWindow(this);
                                m_sbwidget->setEngine(m_engine);
                                m_engine->addRemovable(m_sbwidget->metaObject());
                                QScrollArea * sa_sb = new QScrollArea(this);
                                sa_sb->setWidget(m_sbwidget);
                                sa_sb->setWidgetResizable(true);
                                addPanel("switchboard", tr("Switchboard"), sa_sb);
                                
                                connect( m_engine, SIGNAL(updatePeer(UserInfo *,
                                                                     const QString &,
                                                                     const QStringList &, const QStringList &,
                                                                     const QStringList &, const QStringList &)),
                                         m_sbwidget, SLOT(updatePeer(UserInfo *,
                                                                     const QString &,
                                                                     const QStringList &, const QStringList &,
                                                                     const QStringList &, const QStringList &)) );
                                connect( m_engine, SIGNAL(newUser(UserInfo *)),
                                         m_sbwidget, SLOT(newUser(UserInfo *)));
				connect( m_engine, SIGNAL(updatePeerAgent(const QString &, const QString &, const QStringList &)),
					 m_sbwidget, SLOT(updatePeerAgent(const QString &, const QString &, const QStringList &)) );
                                connect( m_engine, SIGNAL(delogged()),
                                         m_sbwidget, SLOT(removePeers()) );
                                connect( m_engine, SIGNAL(removePeer(const QString &)),
                                         m_sbwidget, SLOT(removePeer(const QString &)) );
                        } else if (dc == QString("parking")) {
                                m_parkingpanel = new ParkingPanel();
                                addPanel("parking", tr("Parking"), m_parkingpanel);
                                
                                connect( m_engine, SIGNAL(localUserInfoDefined(const UserInfo *)),
                                         m_parkingpanel, SLOT(setUserInfo(const UserInfo *)));
                                connect( m_engine, SIGNAL(parkingEvent(const QString &, const QString &)),
                                         m_parkingpanel, SLOT(parkingEvent(const QString &, const QString &)));
                                connect( m_parkingpanel, SIGNAL(copyNumber(const QString &)),
                                         m_engine, SLOT(copyNumber(const QString &)) );
                                connect( m_parkingpanel, SIGNAL(newParkEvent()),
                                         this, SLOT(newParkEvent()) );
                                connectDials(m_parkingpanel);
                        } else if (dc == QString("fax")) {
				m_faxwidget = new FaxPanel(m_engine, this);
                                addPanel("fax", tr("Fax"), m_faxwidget);
                                
                                connect( m_faxwidget, SIGNAL(faxSend(const QString &, const QString &, Qt::CheckState)),
                                         m_engine, SLOT(sendFaxCommand(const QString &, const QString &, Qt::CheckState)) );
                                connect( m_engine, SIGNAL(ackFax(const QString &)),
                                         m_faxwidget, SLOT(popupMsg(const QString &)) );
                                
			} else if ((dc == QString("customerinfo")) && (m_engine->checkedCInfo())) {
                                m_customerinfopanel = new CustomerInfoPanel();
                                m_customerinfopanel->setEngine(m_engine);
                                addPanel("customerinfo", tr("Sheets"), m_customerinfopanel);
                                
                                connect( m_engine, SIGNAL(localUserInfoDefined(const UserInfo *)),
                                         m_customerinfopanel, SLOT(setUserInfo(const UserInfo *)));
                                connect( m_engine, SIGNAL(displayFiche(const QString &, bool)),
                                         m_customerinfopanel, SLOT(displayFiche(const QString &, bool)) );
                                connect( m_customerinfopanel, SIGNAL(newPopup()),
                                         this, SLOT(customerInfoPopup()) );

			} else if (dc == QString("search")) {
				m_searchpanel = new SearchPanel();
                                addPanel("search", tr("Contacts"), m_searchpanel);
                                
				connect( m_engine, SIGNAL(updatePeer(UserInfo *,
								     const QString &,
								     const QStringList &, const QStringList &,
								     const QStringList &, const QStringList &)),
					 m_searchpanel, SLOT(updatePeer(UserInfo *,
									const QString &,
									const QStringList &, const QStringList &,
									const QStringList &, const QStringList &)) );
                                connect( m_engine, SIGNAL(newUser(UserInfo *)),
                                         m_searchpanel, SLOT(newUser(UserInfo *)));
				connect( m_engine, SIGNAL(updatePeerAgent(const QString &, const QString &, const QStringList &)),
					 m_searchpanel, SLOT(updatePeerAgent(const QString &, const QString &, const QStringList &)) );
				connect( m_engine, SIGNAL(peersReceived()),
					 m_searchpanel, SLOT(callsUpdated()) );
				connect( m_searchpanel, SIGNAL(askCallerIds()),
					 m_engine, SLOT(askCallerIds()) );
                                connect( m_engine, SIGNAL(delogged()),
                                         m_searchpanel, SLOT(removePeers()) );
                                connect( m_engine, SIGNAL(removePeer(const QString &)),
                                         m_searchpanel, SLOT(removePeer(const QString &)) );

				m_searchpanel->setEngine(m_engine);
                        } else if (dc == QString("features")) {
                                m_featureswidget = new ServicePanel(m_engine->getCapaFeatures());
                                addPanel("features", tr("Services"), m_featureswidget);
                                
                                connect( m_featureswidget, SIGNAL(askFeatures()),
                                         m_engine, SLOT(askFeatures()) );
                                connect( m_engine, SIGNAL(setPeerToDisplay(const QString &)),
                                         m_featureswidget, SLOT(setPeerToDisplay(const QString &)) );
                                
                                connect( m_engine, SIGNAL(disconnectFeatures()),
                                         m_featureswidget, SLOT(DisConnect()) );
                                connect( m_engine, SIGNAL(connectFeatures()),
                                         m_featureswidget, SLOT(Connect()) );
                                connect( m_engine, SIGNAL(resetFeatures()),
                                         m_featureswidget, SLOT(Reset()) );
                                connect( m_engine, SIGNAL(featurePutIsKO()),
                                         m_featureswidget, SLOT(getRecordedStatus()) );
                                connect( m_engine, SIGNAL(featurePutIsOK()),
                                         m_featureswidget, SLOT(setRecordedStatus()) );
                                
                                connect( m_featureswidget, SIGNAL(voiceMailToggled(bool)),
                                         m_engine, SLOT(featurePutVoiceMail(bool)) );
                                connect( m_engine, SIGNAL(voiceMailChanged(bool)),
                                         m_featureswidget, SLOT(setVoiceMail(bool)) );
                                
                                connect( m_featureswidget, SIGNAL(callRecordingToggled(bool)),
                                         m_engine, SLOT(featurePutCallRecording(bool)) );
                                connect( m_engine, SIGNAL(callRecordingChanged(bool)),
                                         m_featureswidget, SLOT(setCallRecording(bool)) );
                                
                                connect( m_featureswidget, SIGNAL(callFilteringToggled(bool)),
                                         m_engine, SLOT(featurePutCallFiltering(bool)) );
                                connect( m_engine, SIGNAL(callFilteringChanged(bool)),
                                         m_featureswidget, SLOT(setCallFiltering(bool)) );
                                
                                connect( m_featureswidget, SIGNAL(dndToggled(bool)),
                                         m_engine, SLOT(featurePutDnd(bool)) );
                                connect( m_engine, SIGNAL(dndChanged(bool)),
                                         m_featureswidget, SLOT(setDnd(bool)) );
                                
                                connect( m_featureswidget, SIGNAL(uncondForwardChanged(bool, const QString &)),
                                         m_engine, SLOT(featurePutUncondForward(bool, const QString &)) );
                                connect( m_engine, SIGNAL(uncondForwardUpdated(bool, const QString &)),
                                         m_featureswidget, SLOT(setUncondForward(bool, const QString &)) );
                                connect( m_engine, SIGNAL(uncondForwardUpdated(bool)),
                                         m_featureswidget, SLOT(setUncondForward(bool)) );
                                connect( m_engine, SIGNAL(uncondForwardUpdated(const QString &)),
                                         m_featureswidget, SLOT(setUncondForward(const QString &)) );
                                
                                connect( m_featureswidget, SIGNAL(forwardOnBusyChanged(bool, const QString &)),
                                         m_engine, SLOT(featurePutForwardOnBusy(bool, const QString &)) );
                                connect( m_engine, SIGNAL(forwardOnBusyUpdated(bool, const QString &)),
                                         m_featureswidget, SLOT(setForwardOnBusy(bool, const QString &)) );
                                connect( m_engine, SIGNAL(forwardOnBusyUpdated(bool)),
                                         m_featureswidget, SLOT(setForwardOnBusy(bool)) );
                                connect( m_engine, SIGNAL(forwardOnBusyUpdated(const QString &)),
                                         m_featureswidget, SLOT(setForwardOnBusy(const QString &)) );
                                
                                connect( m_featureswidget, SIGNAL(forwardOnUnavailableChanged(bool, const QString &)),
                                         m_engine, SLOT(featurePutForwardOnUnavailable(bool, const QString &)) );
                                connect( m_engine, SIGNAL(forwardOnUnavailableUpdated(bool, const QString &)),
                                         m_featureswidget, SLOT(setForwardOnUnavailable(bool, const QString &)) );
                                connect( m_engine, SIGNAL(forwardOnUnavailableUpdated(bool)),
                                         m_featureswidget, SLOT(setForwardOnUnavailable(bool)) );
                                connect( m_engine, SIGNAL(forwardOnUnavailableUpdated(const QString &)),
                                         m_featureswidget, SLOT(setForwardOnUnavailable(const QString &)) );

			} else if (dc == QString("directory")) {
				m_dirpanel = new DirectoryPanel(this);
                                addPanel("directory", tr("Directory"), m_dirpanel);
                                m_dirpanel->myfocus();

                                connectDials(m_dirpanel);
				connect( m_dirpanel, SIGNAL(searchDirectory(const QString &)),
					 m_engine, SLOT(searchDirectory(const QString &)) );
				connect( m_engine, SIGNAL(directoryResponse(const QString &)),
					 m_dirpanel, SLOT(setSearchResponse(const QString &)) );
				connect( m_dirpanel, SIGNAL(copyNumber(const QString &)),
					 m_engine, SLOT(copyNumber(const QString &)) );
				connect( m_engine, SIGNAL(updatePeer(UserInfo *,
								     const QString &,
								     const QStringList &, const QStringList &,
								     const QStringList &, const QStringList &)),
					 m_dirpanel, SLOT(updatePeer(UserInfo *,
                                                                     const QString &,
                                                                     const QStringList &, const QStringList &,
                                                                     const QStringList &, const QStringList &)) );
                                connect( m_engine, SIGNAL(delogged()),
                                         m_dirpanel, SLOT(stop()) );
			} else if (dc == QString("instantmessaging")) {
                                m_messagetosend = new QLineEdit();
                                addPanel("instantmessaging", tr("Messages"), m_messagetosend);
                                
                                connect( m_messagetosend, SIGNAL(returnPressed()),
                                         this, SLOT(affTextChanged()) );
                        }
                }
        }
        
        m_tabwidget->setCurrentIndex(m_settings->value("display/lastfocusedtab").toInt());
        
        foreach (QString dname, m_docknames)
                m_docks[dname]->show();

	// restore settings, especially for Docks' positions
        restoreState(m_settings->value("display/mainwindowstate").toByteArray());

        if(m_tabnames.contains("customerinfo")) {
                m_cinfo_index = m_tabwidget->indexOf(m_customerinfopanel);
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

	for(int j = 0; j < XletList.size(); j++) {
                QString dc = XletList[j];
 		if (m_forcetabs || m_allnames.contains(dc)) {
                        if (dc == QString("features")) {
                                removePanel("features", m_featureswidget);
			} else if ((dc == QString("customerinfo")) && (m_engine->checkedCInfo())) {
                                removePanel("customerinfo", m_customerinfopanel);
                        } else if (dc == QString("calls")) {
                                removePanel("calls", m_leftpanel);
                                //delete m_calls;
                                //delete m_areaCalls;
                                //delete m_leftpanel;
                        } else if (dc == QString("switchboard")) {
                                removePanel("switchboard", m_sbwidget);
			} else if (dc == QString("directory")) {
                                removePanel("directory", m_dirpanel);
                        } else if (dc == QString("instantmessaging")) {
                                removePanel("instantmessaging", m_messagetosend);
                        } else if (dc == QString("video")) {
                                removePanel("video", m_videopanel);
                        } else if (dc == QString("operator")) {
                                removePanel("operator", m_statuspanel);
                        } else if (dc == QString("messages")) {
                                removePanel("messages", m_messages_widget);
			} else if (dc == QString("parking")) {
                                removePanel("parking", m_parkingpanel);
			} else if (dc == QString("history")) {
                                removePanel("history", m_historypanel);
			} else if (dc == QString("search")) {
                                removePanel("search", m_searchpanel);
			} else if (dc == QString("fax")) {
                                removePanel("fax", m_faxwidget);
			} else if (dc == QString("dial")) {
                                removePanel("dial", m_dialpanel);
			} else if (dc == QString("identity")) {
                                removePanel("identity", m_infowidget);
			} else if (dc == QString("queues")) {
                                removePanel("queues", m_queuespanel);
			} else if (dc == QString("queuedetails")) {
                                removePanel("queuedetails", m_queuedetailspanel);
			} else if (dc == QString("queueentrydetails")) {
                                removePanel("queueentrydetails", m_queueentrydetailspanel);
			} else if (dc == QString("agents")) {
                                removePanel("agents", m_agentspanel);
			} else if (dc == QString("agentdetails")) {
                                removePanel("agentdetails", m_agentdetailspanel);
			} else if (dc == QString("datetime")) {
                                removePanel("datetime", m_datetimepanel);
			} else if (dc == QString("conference")) {
                                removePanel("conference", m_conferencepanel);
                        }
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

void MainWidget::customerInfoPopup()
{
        qDebug() << "MainWidget::customerInfoPopup()";
        QString currentTimeStr = QTime::currentTime().toString("hh:mm:ss");
        
        // systray popup
        // to be customisable (yes or no)
        if(m_withsystray && m_systrayIcon) {
                m_systrayIcon->showMessage(tr("Call"),
                                           currentTimeStr + "\nhello");
        }
        
        // focus on the customerinfo tab
        if(m_tabnames.contains("customerinfo"))
                if (m_cinfo_index > -1)
                        m_tabwidget->setCurrentIndex(m_cinfo_index);
        
        // to be customisable, if the user wants the window to popup
        setVisible(true);
        activateWindow();
        raise();
}

void MainWidget::newParkEvent()
{
        qDebug() << "MainWidget::newParkEvent()";

        int index_parking = m_tabwidget->indexOf(m_parkingpanel);
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
			   "<p>" + tr("with a special exception allowing you to link it with OpenSSL, under some conditions") + "</p>");
}

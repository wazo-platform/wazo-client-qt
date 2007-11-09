/*
XIVO switchboard : 
Copyright (C) 2007  Proformatique

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
*/

/* $Revision$
 * $Date$
 */

#include <QAction>
#include <QApplication>
#include <QCloseEvent>
#include <QDateTime>
#include <QDebug>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QScrollArea>
#include <QSettings>
#include <QSplitter>
#include <QStatusBar>
#include <QTabWidget>
#include <QVBoxLayout>

#include "baseengine.h"
#include "callstackwidget.h"
#include "confwidget.h"
#include "dialpanel.h"
#include "directorypanel.h"
#include "displaymessages.h"
#include "identitydisplay.h"
#include "logwidget.h"
#include "mainwidget.h"
#include "parkingpanel.h"
#include "popup.h"
#include "searchpanel.h"
#include "servicepanel.h"
#include "switchboardwindow.h"
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




/*!
 * Construct the Widget with all subwidgets : a left panel for
 * displaying calls and a right panel for peers.
 * The geometry is restored from settings.
 */
//        : QMainWindow(parent, Qt::FramelessWindowHint),
MainWidget::MainWidget(BaseEngine * engine, QWidget * parent)
        : QMainWindow(parent),
          m_engine(engine)
{
	QSettings settings;
	QPixmap redsquare(":/images/disconnected.png");
	statusBar();	// This creates the status bar.
	m_status = new QLabel();
	m_status->setPixmap(redsquare);
	statusBar()->addPermanentWidget(m_status);
	statusBar()->clearMessage();
	setWindowIcon(QIcon(":/images/xivoicon.png"));
	setWindowTitle("XIVO Switchboard");

	createActions();
	createMenus();

	connect( m_engine, SIGNAL(logged()),
	         this, SLOT(engineStarted()));
	connect( m_engine, SIGNAL(delogged()),
                 this, SLOT(engineStopped()));
	connect( m_engine, SIGNAL(newProfile(Popup *)),
	         this, SLOT(showNewProfile(Popup *)) );
        connect( m_engine, SIGNAL(emitTextMessage(const QString &)),
                 statusBar(), SLOT(showMessage(const QString &)));

	restoreGeometry(settings.value("display/mainwingeometry").toByteArray());
	
	m_wid = new QWidget();
	m_mainlayout = new QVBoxLayout(m_wid);
        m_xivobg = new QLabel();
        m_xivobg->setPixmap(QPixmap(":/images/xivoicon.png"));
        m_mainlayout->addWidget(m_xivobg, 0, Qt::AlignHCenter | Qt::AlignVCenter);
	setCentralWidget(m_wid);
	m_tablimit = settings.value("display/tablimit", 5).toInt();
        m_launchDateTime = QDateTime::currentDateTime();
}

void MainWidget::buildSplitters()
{
	QSettings settings;
        m_infowidget = new IdentityDisplay(this);
	m_splitter = new QSplitter(this);

        m_mainlayout->removeWidget(m_xivobg);
        delete m_xivobg;
        m_mainlayout->addWidget(m_infowidget, 0);
        m_mainlayout->addWidget(m_splitter, 1);

	m_leftSplitter = new QSplitter(Qt::Vertical, m_splitter);
	m_middleSplitter = new QSplitter( Qt::Vertical, m_splitter);
	m_rightSplitter = new QSplitter(Qt::Vertical, m_splitter);

        // Left Splitter Definitions
        m_areaCalls = new QScrollArea(m_leftSplitter);
	m_areaCalls->setWidgetResizable(true);
        m_leftpanel = new LeftPanel(m_areaCalls, m_leftSplitter);
	m_calls = new CallStackWidget(m_areaCalls);
 	m_areaCalls->setWidget(m_calls);
	m_svc_tabwidget = new QTabWidget(m_leftSplitter);
        m_parkingpanel = new ParkingPanel(m_svc_tabwidget);
        m_parkingpanel->setEngine(m_engine);
	m_svc_tabwidget->addTab(m_parkingpanel, extraspace + tr("Parking") + extraspace);
	m_messages_widget = new DisplayMessagesPanel(m_svc_tabwidget);
        m_svc_tabwidget->addTab(m_messages_widget, extraspace + tr("Messages") + extraspace);
	m_leftSplitter->restoreState(settings.value("display/leftSplitterSizes").toByteArray());

	// Middle Splitter Definitions
        m_areaPeers = new QScrollArea(m_middleSplitter);
        m_areaPeers->setWidgetResizable(true);
 	m_sbwidget = new SwitchBoardWindow(m_areaPeers);
 	m_sbwidget->setEngine(m_engine);
	m_engine->addRemovable(m_sbwidget->metaObject());
 	m_areaPeers->setWidget(m_sbwidget);
	m_dirpanel = new DirectoryPanel(m_middleSplitter);
	m_middleSplitter->restoreState(settings.value("display/middleSplitterSizes").toByteArray());

        // Right Splitter Definitions
        m_searchpanel = new SearchPanel(m_rightSplitter);
        m_searchpanel->setEngine(m_engine);
	m_cinfo_tabwidget = new QTabWidget(m_rightSplitter);
        m_cinfo_tabwidget->setObjectName("cinfo");
        m_dialpanel = new DialPanel(m_rightSplitter);
	m_rightSplitter->restoreState(settings.value("display/rightSplitterSizes").toByteArray());

        //	setCentralWidget(m_splitter);

	// restore splitter settings
        m_splitter->restoreState(settings.value("display/splitterSizes").toByteArray());

        connect( m_engine, SIGNAL(localUserDefined(const QString &)),
                 m_infowidget, SLOT(setUser(const QString &)));
	connect( m_calls, SIGNAL(changeTitle(const QString &)),
	         m_leftpanel->titleLabel(), SLOT(setText(const QString &)) );
	connect( m_engine, SIGNAL(emitTextMessage(const QString &)),
                 m_messages_widget, SLOT(addMessage(const QString &)));

        /* Parking Signals */
	connect( m_engine, SIGNAL(parkingEvent(const QString &, const QString &)),
                 m_parkingpanel, SLOT(parkingEvent(const QString &, const QString &)));
	connect( m_parkingpanel, SIGNAL(copyNumber(const QString &)),
	         m_engine, SLOT(copyNumber(const QString &)) );
	connect( m_parkingpanel, SIGNAL(emitDial(const QString &)),
	         m_engine, SLOT(dialFullChannel(const QString &)) );
	connect( m_parkingpanel, SIGNAL(transferCall(const QString &, const QString &)),
	         m_engine, SLOT(transferCall(const QString &, const QString &)) );
	connect( m_parkingpanel, SIGNAL(originateCall(const QString &, const QString &)),
	         m_engine, SLOT(originateCallGoodAsterisk(const QString &, const QString &)) );
	connect( m_parkingpanel, SIGNAL(newParkEvent()),
	         this, SLOT(newParkEvent()) );

	connect( m_engine, SIGNAL(updateCall(const QString &, const QString &, int, const QString &,
					     const QString &, const QString &, const QString &)),
		 m_calls, SLOT(addCall(const QString &, const QString &, int, const QString &,
                                       const QString &, const QString &, const QString &)) );
	connect( m_engine, SIGNAL(callsUpdated()),
	         m_calls, SLOT(updateDisplay()) );
	connect( m_engine, SIGNAL(delogged()),
	         m_calls, SLOT(reset()) );
	connect( m_engine, SIGNAL(monitorPeer(const QString &, const QString &)),
	         m_calls, SLOT(monitorPeer(const QString &, const QString &)) );
	connect( m_calls, SIGNAL(hangUp(const QString &)),
		 m_engine, SLOT(hangUp(const QString &)) );
	connect( m_calls, SIGNAL(transferToNumber(const QString &)),
		 m_engine, SLOT(transferToNumber(const QString &)) );
	connect( m_calls, SIGNAL(parkCall(const QString &)),
		 m_engine, SLOT(parkCall(const QString &)) );
	connect( m_engine, SIGNAL(updatePeer(const QString &, const QString &,
                                             const QString &, const QString &,
                                             const QString &, const QString &,
                                             const QStringList &, const QStringList &,
                                             const QStringList &)),
	         m_sbwidget, SLOT(updatePeer(const QString &, const QString &,
                                             const QString &, const QString &,
                                             const QString &, const QString &,
                                             const QStringList &, const QStringList &,
                                             const QStringList &)) );
	connect( m_engine, SIGNAL(delogged()),
	         m_sbwidget, SLOT(removePeers()) );
	connect( m_engine, SIGNAL(removePeer(const QString &)),
	         m_sbwidget, SLOT(removePeer(const QString &)) );

        /* Directory Signals */
	connect( m_dirpanel, SIGNAL(searchDirectory(const QString &)),
	         m_engine, SLOT(searchDirectory(const QString &)) );
	connect( m_engine, SIGNAL(directoryResponse(const QString &)),
	         m_dirpanel, SLOT(setSearchResponse(const QString &)) );

        connect( m_dirpanel, SIGNAL(copyNumber(const QString &)),
                 m_engine, SLOT(copyNumber(const QString &)) );
	connect( m_dirpanel, SIGNAL(emitDial(const QString &)),
	         m_engine, SLOT(dialFullChannel(const QString &)) );
	connect( m_dirpanel, SIGNAL(transferCall(const QString &, const QString &)),
	         m_engine, SLOT(transferCall(const QString &, const QString &)) );
	connect( m_dirpanel, SIGNAL(originateCall(const QString &, const QString &)),
	         m_engine, SLOT(originateCallGoodAsterisk(const QString &, const QString &)) );
	connect( m_engine, SIGNAL(updateMyCalls(const QStringList &, const QStringList &, const QStringList &)),
	         m_dirpanel, SLOT(updateMyCalls(const QStringList &, const QStringList &, const QStringList &)) );
	connect( m_engine, SIGNAL(delogged()),
	         m_dirpanel, SLOT(stop()) );
        
	connect( m_engine, SIGNAL(updatePeer(const QString &, const QString &,
                                             const QString &, const QString &,
                                             const QString &, const QString &,
                                             const QStringList &, const QStringList &,
                                             const QStringList &)),
	         m_searchpanel, SLOT(updatePeer(const QString &, const QString &,
                                                const QString &, const QString &,
                                                const QString &, const QString &,
                                                const QStringList &, const QStringList &,
                                                const QStringList &)) );
        connect( m_searchpanel, SIGNAL(askCallerIds()),
                 m_engine, SLOT(askCallerIds()) );
	connect( m_engine, SIGNAL(delogged()),
	         m_searchpanel, SLOT(removePeers()) );
	connect( m_engine, SIGNAL(removePeer(const QString &)),
	         m_searchpanel, SLOT(removePeer(const QString &)) );
	connect( m_dialpanel, SIGNAL(emitDial(const QString &)),
	         m_engine, SLOT(dialFullChannel(const QString &)) );
        connect( m_dialpanel, SIGNAL(originateCall(const QString&, const QString&)),
	         m_engine, SLOT(originateCallGoodAsterisk(const QString&, const QString&)) );
	connect( m_dialpanel, SIGNAL(textEdited(const QString &)),
                 m_engine, SLOT(textEdited(const QString &)) );
        connect( m_engine, SIGNAL(pasteToDialPanel(const QString &)),
                 m_dialpanel, SLOT(setNumberToDial(const QString &)) );
}


void MainWidget::newParkEvent()
{
        qDebug() << "MainWidget::newParkEvent()";

        int index_parking = m_svc_tabwidget->indexOf(m_parkingpanel);
        if(index_parking > -1)
                m_svc_tabwidget->setCurrentIndex(index_parking);
}

void MainWidget::removeSplitters()
{
	QSettings settings;
        // Left Splitter Definitions
	delete m_messages_widget;
	delete m_parkingpanel;
	delete m_svc_tabwidget;
        delete m_calls;
        delete m_areaCalls;
        delete m_leftpanel;

	// Middle Splitter Definitions
	delete m_dirpanel;
 	delete m_sbwidget;
        delete m_areaPeers;

        // Right Splitter Definitions
        delete m_dialpanel;
	delete m_cinfo_tabwidget;
        delete m_searchpanel;

        // Splitters
        delete m_rightSplitter;
        delete m_leftSplitter;
        delete m_middleSplitter;
        delete m_splitter;
}


/*! \brief Destructor
 *
 * The Geometry settings are saved for use by the new instance
 */
MainWidget::~MainWidget()
{
        //        qDebug() << "MainWidget::~MainWidget()";
	QSettings settings;
	settings.setValue("display/mainwingeometry", saveGeometry());
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
	QMenu * filemenu = menuBar()->addMenu("&XIVO Switchboard");
	filemenu->addAction( m_cfgact );

	filemenu->addSeparator();
	filemenu->addAction( m_connectact );
	filemenu->addAction( m_disconnectact );
	filemenu->addSeparator();
	filemenu->addAction( m_quitact );

	m_avail = menuBar()->addMenu(tr("&Availability"));
	m_avail->addActions( m_availgrp->actions() );
	m_avail->setEnabled( false );
	connect( m_engine, SIGNAL(availAllowChanged(bool)),
	         m_avail, SLOT(setEnabled(bool)) );

	QMenu * helpmenu = menuBar()->addMenu(tr("&Help"));
	helpmenu->addAction(tr("&About XIVO Switchboard"), this, SLOT(about()));
	helpmenu->addAction(tr("About &Qt"), qApp, SLOT(aboutQt()));
}

/*!
 * tablimit property defines the maximum
 * number of profile that can be displayed in the Tabbed
 * widget.
 *
 * \sa setTablimit
 * \sa m_tablimit
 */
int MainWidget::tablimit() const
{
	return m_tablimit;
}

/*!
 * \sa tablimit
 * \sa m_tablimit
 */
void MainWidget::setTablimit(int tablimit)
{
	QSettings settings;
	m_tablimit = tablimit;
	settings.setValue("display/tablimit", m_tablimit);
}

/*! \brief show the Configuration Dialog
 *
 * create and execute a new ConfWidget
 */
void MainWidget::showConfDialog()
{
        m_conf = new ConfWidget(m_engine, this);
	m_conf->exec();
}


/*!
 * enables the "Disconnect" action and disables the "Connect" Action.
 * sets the Green indicator
 * \sa engineStopped()
 */
void MainWidget::engineStarted()
{
        QSettings settings;
	QStringList display_capas = QString("customerinfo,features,history,directory,peers,dial,presence").split(",");
	QStringList allowed_capas = m_engine->getCapabilities();
        settings.setValue("display/capas", allowed_capas.join(","));

        buildSplitters();

        for (int j = 0; j < display_capas.size(); j++) {
		QString dc = display_capas[j];
		if (allowed_capas.contains(dc)) {
			if (dc == QString("history")) {
                                m_logwidget = new LogWidget(m_engine, m_svc_tabwidget);
                                m_svc_tabwidget->insertTab(0, m_logwidget, extraspace + tr("History") + extraspace);

                                connect( m_engine, SIGNAL(updateLogEntry(const QDateTime &, int, const QString &, int)),
                                         m_logwidget, SLOT(addLogEntry(const QDateTime &, int, const QString &, int)) );
                                connect( m_logwidget, SIGNAL(askHistory(const QString &, int)),
                                         m_engine, SLOT(requestHistory(const QString &, int)) );
                                connect( m_engine, SIGNAL(delogged()),
                                         m_logwidget, SLOT(clear()) );
                                connect( m_calls, SIGNAL(monitoredPeerChanged(const QString &)),
                                         m_logwidget, SLOT(setPeerToDisplay(const QString &)) );
                        } else if (dc == QString("features")) {

                                m_featureswidget = new ServicePanel(m_svc_tabwidget);
                                m_svc_tabwidget->insertTab(0, m_featureswidget, extraspace + tr("Services") + extraspace);

                                connect( m_featureswidget, SIGNAL(askFeatures(const QString &)),
                                         m_engine, SLOT(askFeatures(const QString &)) );
                                connect( m_calls, SIGNAL(monitoredPeerChanged(const QString &)),
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
                                m_engine->askFeatures("peer/to/define");
                        }
                }
        }

	m_svc_tabwidget->setCurrentIndex(0);

	statusBar()->showMessage(tr("Connected"));
	m_connectact->setEnabled(false);
	m_disconnectact->setEnabled(true);
	// set status icon to green
	QPixmap greensquare(":/images/connected.png");
	m_status->setPixmap(greensquare);
}

/*!
 * disables the "Disconnect" action and enables the "Connect" Action.
 * sets the Red indicator
 * \sa engineStarted()
 */
void MainWidget::engineStopped()
{
        QSettings settings;
	QStringList display_capas = QString("customerinfo,features,history,directory,peers,dial,presence").split(",");
	QStringList allowed_capas = m_engine->getCapabilities();

	settings.setValue("display/splitterSizes", m_splitter->saveState());
	settings.setValue("display/leftSplitterSizes", m_leftSplitter->saveState());
	settings.setValue("display/middleSplitterSizes", m_middleSplitter->saveState());
	settings.setValue("display/rightSplitterSizes", m_rightSplitter->saveState());

	for(int j = 0; j < display_capas.size(); j++) {
		QString dc = display_capas[j];
		if (allowed_capas.contains(dc)) {
                        if (dc == QString("features")) {
                                int index_features = m_svc_tabwidget->indexOf(m_featureswidget);
                                if (index_features > -1) {
                                        m_svc_tabwidget->removeTab(index_features);
                                        delete m_featureswidget;
                                }
                        } else if (dc == QString("history")) {
                                int index_logs = m_svc_tabwidget->indexOf(m_logwidget);
                                if (index_logs > -1) {
                                        m_svc_tabwidget->removeTab(index_logs);
                                        delete m_logwidget;
                                }
                        }
                }
        }

        m_cinfo_tabwidget->clear();
        removeSplitters();
 	m_wid = new QWidget();
 	m_mainlayout = new QVBoxLayout(m_wid);
        m_xivobg = new QLabel();
        m_xivobg->setPixmap(QPixmap(":/images/xivoicon.png"));
        m_mainlayout->addWidget(m_xivobg, 0, Qt::AlignHCenter | Qt::AlignVCenter);
 	setCentralWidget(m_wid);

	statusBar()->showMessage(tr("Disconnected"));
	m_connectact->setEnabled(true);
	m_disconnectact->setEnabled(false);
	// set status icon to red
	QPixmap redsquare(":/images/disconnected.png");
	m_status->setPixmap(redsquare);
}

/*!
 * Display the new profile in the tabbed area
 * and show a message with the systray icon
 */
void MainWidget::showNewProfile(Popup * popup)
{
	QTime currentTime = QTime::currentTime();
	QString currentTimeStr = currentTime.toString("hh:mm:ss");
	/*	if (m_systrayIcon)
	  {
	  m_systrayIcon->showMessage(tr("Incoming call"),
	  currentTimeStr + "\n"
	  + popup->message() );
	  }*/
	if (m_cinfo_tabwidget)
	{
		int index = m_cinfo_tabwidget->addTab(popup, extraspace + currentTimeStr + extraspace);
		qDebug() << "added tab" << index;
		m_cinfo_tabwidget->setCurrentIndex(index);
		if (index >= m_tablimit)
		{
			// close the first widget
			m_cinfo_tabwidget->widget(0)->close();
		}
                connect( popup, SIGNAL(emitDial(const QString &)),
                         m_engine, SLOT(dialFullChannel(const QString &)) );
		// show the window and give it the focus.
		setVisible(true);
		activateWindow();
		raise();
	}
	else
	{
		popup->show();
	}
}

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
        
	QMessageBox::about(this,
			   tr("About XIVO SwitchBoard"),
			   "<h3>XIVO Switchboard</h3>" +
			   tr("<p>This application displays the status of the"
			      " ongoing phone calls.</p>") +
			   "<p><b>" + tr("Version : ") + QString("</b>%1 (").arg(applicationVersion) +
			   "<b>svn : " + QString("</b>%1 - %2)</p>").arg(__current_client_version__,
                                                                         fetchlastone) +
                           "<p>(" + tr("Advised Server Version : ") + __required_server_version__ + ")</p>" +
                           "<p>(" + tr("Application Launched on : ") + m_launchDateTime.toString() + ")</p>" +
			   "<hr><p>(C) 2007 <a href=http://www.proformatique.com><b>Proformatique</b></a></p>"
			   "<p>67 rue Voltaire 92800 Puteaux FRANCE</p>"
			   "<p><b>E-mail : </b><a href=mailto:technique@proformatique.com>technique@proformatique.com</p>"
			   "<p>(+33 / 0) 1.41.38.99.60</p>" +
			   "<p><b>" + tr("Authors : ") + "</b>Thomas Bernard, Corentin Le Gall</p>" +
			   "<hr><p><b>" + tr("License : ") + "</b>" +
			   "<a href=http://www.gnu.org/licenses/gpl-2.0.txt>GNU General Public Licence v.2</a></p>");
}

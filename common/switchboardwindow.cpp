/* XIVO CTI clients
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

#include <QDebug>
#include <QGridLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QPoint>
#include <QPushButton>
#include <QSettings>
#include <QToolTip>

#include "baseengine.h"
#include "peerwidget.h"
#include "peerslayout.h"
#include "switchboardwindow.h"
#include "xivoconsts.h"

SwitchBoardWindow::SwitchBoardWindow(QWidget * parent)
	: QWidget(parent), m_engine(0)
{
	m_layout = new PeersLayout(this);
        setObjectName("scroller"); // in order for the style settings to be set accordingly
	setAcceptDrops(true);

        m_persons["red"] = QPixmap(":/images/personal-red.png");
        m_persons["blue"] = QPixmap(":/images/personal-blue.png");
        m_persons["green"] = QPixmap(":/images/personal-green.png");
        m_persons["grey"] = QPixmap(":/images/personal-grey.png");
        m_persons["orange"] = QPixmap(":/images/personal-orange.png");
        m_persons["yellow"] = QPixmap(":/images/personal-yellow.png");

        m_phones["red"] = QPixmap(":/images/phone-red.png");
        m_phones["blue"] = QPixmap(":/images/phone-blue.png");
        m_phones["green"] = QPixmap(":/images/phone-green.png");
        m_phones["grey"] = QPixmap(":/images/phone-grey.png");
        m_phones["orange"] = QPixmap(":/images/phone-orange.png");
        m_phones["yellow"] = QPixmap(":/images/phone-yellow.png");

        m_agents["red"] = QPixmap(":/images/agent-red.png");
        m_agents["blue"] = QPixmap(":/images/agent-blue.png");
        m_agents["green"] = QPixmap(":/images/agent-green.png");
        m_agents["grey"] = QPixmap(":/images/agent-grey.png");
        m_agents["orange"] = QPixmap(":/images/agent-orange.png");
        m_agents["yellow"] = QPixmap(":/images/agent-yellow.png");
}

/*!
 * Save the positions in the grid of the peer widgets.
 */
SwitchBoardWindow::~SwitchBoardWindow()
{
	// qDebug() << "SwitchBoardWindow::~SwitchBoardWindow()";
        savePositions();
        removePeers();
}

/*! \brief setter for m_engine
 *
 * set BaseEngine object to be used to connect to
 * peer object slot/signals.
 */
void SwitchBoardWindow::setEngine(BaseEngine * engine)
{
	m_engine = engine;
}

/*! \brief update or add a peer
 *
 * The peer with the ext extension is updated or added
 * to the list if it is not present.
 * The placement of the PeerWidget is restored from the settings.
 * 
 * \arg ext phone extension
 * \arg name name
 *
 * \sa removePeer
 */
void SwitchBoardWindow::updatePeer(const QString & ext,
                                   const QString & name,
                                   const QString & imavail,
                                   const QString & sipstatus,
                                   const QString & vmstatus,
                                   const QString & agentstatus,
				   const QStringList & chanIds,
				   const QStringList & chanStates,
				   const QStringList & chanOthers,
                                   const QStringList &)
{
	// first search in the peerhash
        if(m_peerhash.contains(ext)) {
                Peer * peeritem = m_peerhash.value(ext);
                peeritem->updateStatus(imavail, sipstatus, vmstatus, agentstatus);
                peeritem->updateChans(chanIds, chanStates, chanOthers);
                peeritem->updateName(name);
                return;
        }

        // if the name (i.e. full callerid info) has not been received yet, do not add as a peer
        if(name.isEmpty())
                return;
        
	Peer * peeritem = new Peer(ext, name);
        peeritem->updateStatus(imavail, sipstatus,
                               vmstatus, agentstatus);
        peeritem->updateChans(chanIds, chanStates, chanOthers);
        m_peerhash.insert(ext, peeritem);

	// if not found in the peerhash, create a new Peer
	QPoint pos = m_engine->getSettings()->value("layout/" + ext, QPoint(-1, -1) ).toPoint();
	if(pos.x() >= 0) {
                PeerWidget * peerwidget = new PeerWidget(ext,
                                                         name,
                                                         m_persons,
                                                         m_phones,
                                                         m_agents);
                peerwidget->setEngine(m_engine);
             	connect( peerwidget, SIGNAL(originateCall(const QString&, const QString&)),
                         m_engine, SLOT(originateCall(const QString&, const QString&)) );
                connect( peerwidget, SIGNAL(transferCall(const QString&, const QString&)),
                         m_engine, SLOT(transferCall(const QString&, const QString&)) );
                connect( peerwidget, SIGNAL(atxferCall(const QString&, const QString&)),
                         m_engine, SLOT(atxferCall(const QString&, const QString&)) );
                connect( peerwidget, SIGNAL(hangUpChan(const QString &)),
                         m_engine, SLOT(hangUp(const QString &)) );
                connect( peerwidget, SIGNAL(emitDial(const QString &, bool)),
                         m_engine, SLOT(dialFullChannel(const QString &, bool)) );
                connect( peerwidget, SIGNAL(interceptChan(const QString &)),
                         m_engine, SLOT(interceptCall(const QString &)) );
                connect( peerwidget, SIGNAL(doRemoveFromPanel(const QString &)),
                         this, SLOT(removePeerFromLayout(const QString &)) );
                connect( m_engine, SIGNAL(updateMyCalls(const QStringList &, const QStringList &, const QStringList &)),
                         peerwidget, SLOT(updateMyCalls(const QStringList &, const QStringList &, const QStringList &)) );
                m_layout->addWidget( peerwidget, pos );
                m_peerlist << peeritem;
                peeritem->setWidget(peerwidget);
                peeritem->updateDisplayedStatus();
                peeritem->updateDisplayedChans();
                peeritem->updateDisplayedName();
        }
}


void SwitchBoardWindow::updatePeerAgent(const QString & ext,
                                        const QString & agentstatus)
{
        // qDebug() << "SwitchBoardWindow::updatePeerAgent()";
        if(m_peerhash.contains(ext)) {
                Peer * peeritem = m_peerhash.value(ext);
                peeritem->updateAgentStatus(agentstatus);
        }
        return;
}


/*! \brief remove the peer from the layout
 *
 * the peer with extension is moved to position (-1, -1) so it
 * wont be displayed anymore
 *
 * \sa dropEvent()
 */
void SwitchBoardWindow::removePeerFromLayout(const QString & ext)
{
	for(int i = 0; i < m_peerlist.count(); i++) {
		if(ext == m_peerlist[i]->ext()) {
                        m_layout->setItemPosition(i, QPoint(-1, -1));
			savePositions();
                        Peer * peeritem = m_peerhash[ext];
                        QString name = peeritem->name();
                        PeerWidget * peerwidget = peeritem->getWidget();
                        m_layout->removeWidget( peerwidget );
                        // this disconnect() step takes time, whether explicitly or implicitly,
                        // so we should be careful to avoid too much connect's (anyway)
                        disconnect( peerwidget, SIGNAL(originateCall(const QString&, const QString&)),
                                    m_engine, SLOT(originateCall(const QString&, const QString&)) );
                        disconnect( peerwidget, SIGNAL(transferCall(const QString&, const QString&)),
                                    m_engine, SLOT(transferCall(const QString&, const QString&)) );
                        disconnect( peerwidget, SIGNAL(atxferCall(const QString&, const QString&)),
                                    m_engine, SLOT(atxferCall(const QString&, const QString&)) );
                        disconnect( peerwidget, SIGNAL(hangUpChan(const QString &)),
                                    m_engine, SLOT(hangUp(const QString &)) );
                        disconnect( peerwidget, SIGNAL(emitDial(const QString &, bool)),
                                    m_engine, SLOT(dialFullChannel(const QString &, bool)) );
                        disconnect( peerwidget, SIGNAL(interceptChan(const QString &)),
                                    m_engine, SLOT(interceptCall(const QString &)) );
                        disconnect( peerwidget, SIGNAL(doRemoveFromPanel(const QString &)),
                                    this, SLOT(removePeerFromLayout(const QString &)) );
                        disconnect( m_engine, SIGNAL(updateMyCalls(const QStringList &, const QStringList &, const QStringList &)),
                                    peerwidget, SLOT(updateMyCalls(const QStringList &, const QStringList &, const QStringList &)) );
                        m_peerlist.removeAt(i);
                        peerwidget->deleteLater();
                        peeritem->setWidget(NULL);
			return;
		}
	}
}


/*! \brief remove a Peer
 *
 * Find the peer with extension ext and remove it from the list
 * and the widget.
 *
 * \sa updatePeer
 * \sa removePeers
 */
void SwitchBoardWindow::removePeer(const QString & ext)
{
        // qDebug() << "SwitchBoardWindow::removePeer()" << ext;
        if(m_peerhash.contains(ext)) {
                Peer * peeritem = m_peerhash.value(ext);
                PeerWidget * peerwidget = peeritem->getWidget();
                m_layout->removeWidget( peerwidget );
                m_peerhash.remove(ext);
                delete peerwidget; //peerwidget->deleteLater();
                return;
        }
}


/*! \brief remove all peers
 *
 * remove all peers and widget.
 *
 * \sa removePeer
 */
void SwitchBoardWindow::removePeers(void)
{
        // qDebug() << "SwitchBoardWindow::removePeers()";
        QHashIterator<QString, Peer *> peeriter(m_peerhash);
        QTime qtime;
        qtime.start();
        while(peeriter.hasNext()) {
                peeriter.next();
                Peer * peeritem = peeriter.value();
                PeerWidget * peerwidget = peeritem->getWidget();
                if(peerwidget != NULL) {
                        m_layout->removeWidget( peerwidget );
                        // this disconnect() step takes time, whether explicitly or implicitly,
                        // so we should be careful to avoid too much connect's (anyway)
                        disconnect( peerwidget, SIGNAL(originateCall(const QString&, const QString&)),
                                    m_engine, SLOT(originateCall(const QString&, const QString&)) );
                        disconnect( peerwidget, SIGNAL(transferCall(const QString&, const QString&)),
                                    m_engine, SLOT(transferCall(const QString&, const QString&)) );
                        disconnect( peerwidget, SIGNAL(atxferCall(const QString&, const QString&)),
                                    m_engine, SLOT(atxferCall(const QString&, const QString&)) );
                        disconnect( peerwidget, SIGNAL(hangUpChan(const QString &)),
                                    m_engine, SLOT(hangUp(const QString &)) );
                        disconnect( peerwidget, SIGNAL(emitDial(const QString &, bool)),
                                    m_engine, SLOT(dialFullChannel(const QString &, bool)) );
                        disconnect( peerwidget, SIGNAL(interceptChan(const QString &)),
                                    m_engine, SLOT(interceptCall(const QString &)) );
                        disconnect( peerwidget, SIGNAL(doRemoveFromPanel(const QString &)),
                                    this, SLOT(removePeerFromLayout(const QString &)) );
                        disconnect( m_engine, SIGNAL(updateMyCalls(const QStringList &, const QStringList &, const QStringList &)),
                                    peerwidget, SLOT(updateMyCalls(const QStringList &, const QStringList &, const QStringList &)) );
                        peerwidget->deleteLater();
                }
        }
       	m_peerhash.clear();
       	m_peerlist.clear();
        // qDebug() << "SwitchBoardWindow::removePeers : time elapsed" << qtime.elapsed();
}

// void SwitchBoardWindow::mouseMoveEvent(QMouseEvent * event)
// {
// 	qDebug() << "SwitchBoardWindow::mouseMoveEvent()" << event;
// }

// void SwitchBoardWindow::mousePressEvent(QMouseEvent * event)
// {
// 	qDebug() << "SwitchBoardWindow::mousePressEvent()" << event;
// 	qDebug() << "   " << event->x() << event->y() << event->pos();
// 	qDebug() << "   " << event->globalX() << event->globalY() << event->globalPos();
// }

/*!
 * This method accept or reject the drag event.
 *
 * \sa dropEvent()
 */
void SwitchBoardWindow::dragEnterEvent(QDragEnterEvent * event)
{
        // qDebug() << "SwitchBoardWindow::dragEnterEvent()" << event->mimeData()->formats();
	if( event->mimeData()->hasFormat(PEER_MIMETYPE) ||
            event->mimeData()->hasFormat(NUMBER_MIMETYPE) )
		event->acceptProposedAction();
}

/*! \brief Receives drop events
 * 
 * This method recieve drop events. It is currently used to 
 * move PeerWidgets arount :)
 *
 * \sa dragEnterEvent()
 */
void SwitchBoardWindow::dropEvent(QDropEvent * event)
{
	QString ext = event->mimeData()->text();
        // qDebug() << "SwitchBoardWindow::dropEvent()" << event << ext;
	// qDebug() << "  " << event->pos() << m_layout->getPosInGrid(event->pos());
        bool isAlreadyThere = false;

	for(int i=0; i < m_peerlist.count(); i++) {
		if(ext == m_peerlist[i]->ext()) {
                        qDebug() << "   " << i;
			m_layout->setItemPosition(i, m_layout->getPosInGrid(event->pos()));
			updateGeometry();
                        isAlreadyThere = true;
			//update();
		}
	}
        
        if((! isAlreadyThere) && m_peerhash.contains(ext)) {
                Peer * peeritem = m_peerhash[ext];
                QString name = peeritem->name();
                //                PeerWidget * peerwidget = peeritem->getWidget();
                PeerWidget * peerwidget = new PeerWidget(ext,
                                                         name,
                                                         m_persons,
                                                         m_phones,
                                                         m_agents);
                peerwidget->setEngine(m_engine);
                peeritem->setWidget(peerwidget);
                peeritem->updateDisplayedStatus();
                peeritem->updateDisplayedChans();
                peeritem->updateDisplayedName();

             	connect( peerwidget, SIGNAL(originateCall(const QString&, const QString&)),
                         m_engine, SLOT(originateCall(const QString&, const QString&)) );
                connect( peerwidget, SIGNAL(transferCall(const QString&, const QString&)),
                         m_engine, SLOT(transferCall(const QString&, const QString&)) );
                connect( peerwidget, SIGNAL(atxferCall(const QString&, const QString&)),
                         m_engine, SLOT(atxferCall(const QString&, const QString&)) );
                connect( peerwidget, SIGNAL(hangUpChan(const QString &)),
                         m_engine, SLOT(hangUp(const QString &)) );
                connect( peerwidget, SIGNAL(emitDial(const QString &, bool)),
                         m_engine, SLOT(dialFullChannel(const QString &, bool)) );
                connect( peerwidget, SIGNAL(interceptChan(const QString &)),
                         m_engine, SLOT(interceptCall(const QString &)) );
                connect( peerwidget, SIGNAL(doRemoveFromPanel(const QString &)),
                         this, SLOT(removePeerFromLayout(const QString &)) );
                connect( m_engine, SIGNAL(updateMyCalls(const QStringList &, const QStringList &, const QStringList &)),
                         peerwidget, SLOT(updateMyCalls(const QStringList &, const QStringList &, const QStringList &)) );
                m_layout->addWidget( peerwidget, m_layout->getPosInGrid(event->pos()) );
                m_peerlist << peeritem;
       }

	event->acceptProposedAction();
	savePositions();
}


/*! \brief Save the positions of Peer Widgets
 *
 * Save the positions of all Peer Widgets to the settings.
 */
void SwitchBoardWindow::savePositions() const
{
        // qDebug() << "SwitchBoardWindow::savePositions()";
	QSettings * settings = m_engine->getSettings();
	for(int i = 0; i < m_peerlist.size(); i++) {
                QPoint pos = m_layout->getItemPosition(i);
                // qDebug() << m_peerlist[i]->ext() << pos;
                if(pos == QPoint(-1, -1))
                        settings->remove("layout/" + m_peerlist[i]->ext());
                else
                        settings->setValue("layout/" + m_peerlist[i]->ext(),
                                          m_layout->getItemPosition(i));
	}
}

/* XIVO CTI clients
Copyright (C) 2007, 2008  Proformatique

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
version 2 as published by the Free Software Foundation.

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

#include <QVBoxLayout>
#include <QLabel>
#include <QScrollArea>
#include <QDebug>

#include "searchpanel.h"
#include "peerwidget.h"
#include "baseengine.h"
#include "extendedlineedit.h"

SearchPanel::SearchPanel(QWidget * parent)
        : QWidget(parent)
{
	QVBoxLayout * vlayout = new QVBoxLayout(this);
	vlayout->setMargin(0);
	QLabel * lbl = new QLabel( tr("N&ame or number to search :"), this );
	vlayout->addWidget(lbl, 0, Qt::AlignCenter);
	m_input = new ExtendedLineEdit( this );
	lbl->setBuddy(m_input);
	connect( m_input, SIGNAL(textChanged(const QString &)),
	         this, SLOT(affTextChanged(const QString &)) );
	vlayout->addWidget( m_input );
	QScrollArea * scrollarea = new QScrollArea(this);
	scrollarea->setWidgetResizable(true);
	QWidget * widget = new QWidget(scrollarea);
        widget->setObjectName("scroller");
	scrollarea->setWidget(widget);
	QVBoxLayout * scrollarealayout = new QVBoxLayout(widget);
	m_peerlayout = new QVBoxLayout();
	m_qsl = new QStringList();
	scrollarealayout->addLayout( m_peerlayout );
	scrollarealayout->addStretch( 1 );
	vlayout->addWidget(scrollarea);

        m_phone_green  = QPixmap(":/images/phone-green.png");
        m_phone_red    = QPixmap(":/images/phone-red.png");
        m_phone_orange = QPixmap(":/images/phone-orange.png");
        m_phone_yellow = QPixmap(":/images/phone-yellow.png");
        m_phone_grey   = QPixmap(":/images/phone-grey.png");
        m_phone_blue   = QPixmap(":/images/phone-blue.png");
        m_person_green  = QPixmap(":/images/personal-green.png");
        m_person_red    = QPixmap(":/images/personal-red.png");
        m_person_orange = QPixmap(":/images/personal-orange.png");
        m_person_yellow = QPixmap(":/images/personal-yellow.png");
        m_person_grey   = QPixmap(":/images/personal-grey.png");
        m_person_blue   = QPixmap(":/images/personal-blue.png");
        m_maxdisplay = 15;
        m_searchpattern = "";
}

SearchPanel::~SearchPanel()
{
        // qDebug() << "SearchPanel::~SearchPanel()";
        removePeers();
}

void SearchPanel::setEngine(BaseEngine * engine)
{
        // qDebug() << "SearchPanel::setEngine()";
	m_engine = engine;
        m_maxdisplay = m_engine->contactsSize();
        askCallerIds();
}

/*! \brief apply the search
 */
void SearchPanel::callsUpdated()
{
        affTextChanged("");
}


/*! \brief apply the search
 */
void SearchPanel::affTextChanged(const QString & text)
{
        m_searchpattern = text;
        QHashIterator<QString, Peer *> i(m_peerhash);
	// qDebug() << "affTextChanged" << text;
        int naff = 0;
        while(i.hasNext()) {
                i.next();
                Peer * peeritem = i.value();
                PeerWidget * peerwidget = peeritem->getWidget();
                if( peeritem->name().contains(m_searchpattern, Qt::CaseInsensitive) && (naff < m_maxdisplay) ) {
                        if(peerwidget == NULL) {
                                peerwidget = new PeerWidget(peeritem->ext(),
                                                            peeritem->name(),
                                                            &m_phone_green,
                                                            &m_phone_red,
                                                            &m_phone_orange,
                                                            &m_phone_grey,
                                                            &m_phone_yellow,
                                                            &m_phone_blue,
                                                            &m_person_green,
                                                            &m_person_red,
                                                            &m_person_orange,
                                                            &m_person_grey,
                                                            &m_person_yellow,
                                                            &m_person_blue);
                                peerwidget->setEngine(m_engine);
                                peeritem->setWidget(peerwidget);
                                peeritem->updateDisplayedStatus();
                                peeritem->updateDisplayedChans();
                                peeritem->updateDisplayedName();

                                m_qsl->append(peeritem->name());
                                m_qsl->sort();
                                int idof = m_qsl->indexOf(peeritem->name());
                                m_peerlayout->insertWidget(idof, peerwidget);
                                peerwidget->show();
                                if(m_engine->isASwitchboard()) {
                                        connect( peerwidget, SIGNAL(originateCall(const QString&, const QString&)),
                                                 m_engine, SLOT(originateCall(const QString&, const QString&)) );
                                        connect( peerwidget, SIGNAL(transferCall(const QString&, const QString&)),
                                                 m_engine, SLOT(transferCall(const QString&, const QString&)) );
                                        connect( peerwidget, SIGNAL(hangUpChan(const QString &)),
                                                 m_engine, SLOT(hangUp(const QString &)) );
                                        connect( peerwidget, SIGNAL(interceptChan(const QString &)),
                                                 m_engine, SLOT(interceptCall(const QString &)) );
                                        connect( m_engine, SIGNAL(updateMyCalls(const QStringList &, const QStringList &, const QStringList &)),
                                                 peerwidget, SLOT(updateMyCalls(const QStringList &, const QStringList &, const QStringList &)) );
                                }
                                connect( peerwidget, SIGNAL(emitDial(const QString &)),
                                         m_engine, SLOT(dialFullChannel(const QString &)) );
                        }
                        naff ++;
                } else {
                        if ( m_peerlayout->indexOf( peerwidget ) > -1 ) {
                                if(peerwidget != NULL) {
                                        m_qsl->removeAll(peeritem->name());
                                        m_peerlayout->removeWidget( peerwidget );
                                        peerwidget->hide();
                                        if(m_engine->isASwitchboard()) {
                                                disconnect( peerwidget, SIGNAL(originateCall(const QString&, const QString&)),
                                                            m_engine, SLOT(originateCall(const QString&, const QString&)) );
                                                disconnect( peerwidget, SIGNAL(transferCall(const QString&, const QString&)),
                                                            m_engine, SLOT(transferCall(const QString&, const QString&)) );
                                                disconnect( peerwidget, SIGNAL(hangUpChan(const QString &)),
                                                            m_engine, SLOT(hangUp(const QString &)) );
                                                disconnect( peerwidget, SIGNAL(interceptChan(const QString &)),
                                                            m_engine, SLOT(interceptCall(const QString &)) );
                                                disconnect( m_engine, SIGNAL(updateMyCalls(const QStringList &, const QStringList &, const QStringList &)),
                                                            peerwidget, SLOT(updateMyCalls(const QStringList &, const QStringList &, const QStringList &)) );
                                        }
                                        disconnect( peerwidget, SIGNAL(emitDial(const QString &)),
                                                    m_engine, SLOT(dialFullChannel(const QString &)) );
                                        peeritem->setWidget(NULL);
                                        delete peerwidget;
                                }
                        }
                }
 	}
}

/*! \brief update peer
 */
void SearchPanel::updatePeer(const QString & ext,
                             const QString & name,
			     const QString & imavail,
			     const QString & sipstatus,
			     const QString & vmstatus,
			     const QString & queuestatus,
			     const QStringList & chanIds,
			     const QStringList & chanStates,
			     const QStringList & chanOthers)
{
        // qDebug() << "SearchPanel::updatePeer()";
        if(m_peerhash.contains(ext)) {
                Peer * peeritem = m_peerhash.value(ext);
                peeritem->updateStatus(imavail, sipstatus, vmstatus, queuestatus);
                peeritem->updateChans(chanIds, chanStates, chanOthers);
                peeritem->updateName(name);
                return;
        }

        
        // if the name (i.e. full callerid info) has not been received yet, do not add as a peer
        if(name.isEmpty())
                return;
        
	Peer * peer = new Peer(ext, name);
	peer->updateStatus(imavail, sipstatus, vmstatus, queuestatus);
	peer->updateChans(chanIds, chanStates, chanOthers);
        m_peerhash.insert(ext, peer);

        // the peerwidget is not set while its display is not needed, see affTextChanged()
}

/*! \brief remove on peer
 */
void SearchPanel::removePeer(const QString & ext)
{
        // qDebug() << "SearchPanel::removePeer()" << ext;
        if(m_peerhash.contains(ext)) {
                Peer * peeritem = m_peerhash.value(ext);
                PeerWidget * peerwidget = peeritem->getWidget();
                if (m_peerlayout->indexOf( peerwidget ) > -1)
                        m_peerlayout->removeWidget( peerwidget );
                m_peerhash.remove(ext);
                delete peerwidget; // peerwidget->deleteLater();
                return;
        }
}

/*! \brief clear the widget
 */
void SearchPanel::removePeers()
{
        // qDebug() << "SearchPanel::removePeers()";
        QHashIterator<QString, Peer *> i(m_peerhash);
        QTime qtime;
        qtime.start();
        while(i.hasNext()) {
                i.next();
                Peer * peeritem = i.value();
                PeerWidget * peerwidget = peeritem->getWidget();
                // qDebug() << "span" << m_peerlayout->indexOf( peerwidget );
                if (m_peerlayout->indexOf( peerwidget ) > -1)
                        m_peerlayout->removeWidget( peerwidget );
                delete peerwidget;
                //peerwidget->deleteLater();
        }
        // qDebug() << "SearchPanel::removePeers : time elapsed" << qtime.elapsed();
        m_peerhash.clear();
}

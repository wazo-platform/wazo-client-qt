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

#include <QVBoxLayout>
#include <QLabel>
#include <QScrollArea>
#include <QDebug>

#include "searchpanel.h"
#include "peerwidget.h"
#include "baseengine.h"
#include "extendedlineedit.h"
#include "userinfo.h"

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
	m_peerlayout = new QGridLayout();
	scrollarealayout->addLayout( m_peerlayout );
	scrollarealayout->addStretch( 1 );
	vlayout->addWidget(scrollarea);

        QStringList colors = (QStringList() << "red" << "blue" << "green" << "grey" << "orange" << "yellow");
        foreach(QString color, colors) {
                m_persons[color] = QPixmap(":/images/personal-" + color + ".png");
                m_phones[color] = QPixmap(":/images/phone-" + color + ".png");
                m_agents[color] = QPixmap(":/images/agent-" + color + ".png");
        }

        m_maxdisplay = 15;
        m_ncolumns = 4;
        m_searchpattern = "";
}

SearchPanel::~SearchPanel()
{
        // qDebug() << "SearchPanel::~SearchPanel()";
        removePeers();
}

void SearchPanel::setEngine(BaseEngine * engine)
{
        qDebug() << "SearchPanel::setEngine()";
	m_engine = engine;
        m_maxdisplay = m_engine->contactsSize();
        m_ncolumns = m_engine->contactsColumns();
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
        QHashIterator<QString, PeerItem *> i(m_peerhash);
	// qDebug() << "affTextChanged" << text;

        while(i.hasNext()) {
                i.next();
                PeerItem * peeritem = i.value();
                PeerWidget * peerwidget = peeritem->getWidget();

                if ( m_peerlayout->indexOf( peerwidget ) > -1 ) {
                        if(peerwidget != NULL) {
                                m_peerlayout->removeWidget( peerwidget );
                                peerwidget->hide();
                                if(m_engine->hasFunction("switchboard")) {
                                        disconnect( peerwidget, SIGNAL(transferCall(const QString&, const QString&)),
                                                    m_engine, SLOT(transferCall(const QString&, const QString&)) );
                                        disconnect( peerwidget, SIGNAL(hangUpChan(const QString &)),
                                                    m_engine, SLOT(hangUp(const QString &)) );
                                        disconnect( peerwidget, SIGNAL(interceptChan(const QString &)),
                                                    m_engine, SLOT(interceptCall(const QString &)) );
                                        disconnect( m_engine, SIGNAL(updateMyCalls(const QStringList &, const QStringList &, const QStringList &)),
                                                    peerwidget, SLOT(updateMyCalls(const QStringList &, const QStringList &, const QStringList &)) );
                                }
                                disconnect( peerwidget, SIGNAL(originateCall(const QString&, const QString&)),
                                            m_engine, SLOT(originateCall(const QString&, const QString&)) );
                                peeritem->setWidget(NULL);
                                delete peerwidget;
                        }
                }
        }

        int naff = 0;
        i.toFront();
        while(i.hasNext()) {
                i.next();
                PeerItem * peeritem = i.value();
                PeerWidget * peerwidget = peeritem->getWidget();

                if( peeritem->name().contains(m_searchpattern, Qt::CaseInsensitive) && (naff < m_maxdisplay) ) {
                        if(peerwidget == NULL) {
                                peerwidget = new PeerWidget(peeritem->userinfo(),
                                                            m_persons,
                                                            m_phones,
                                                            m_agents);
                                peerwidget->setEngine(m_engine);
                                peeritem->setWidget(peerwidget);
                                peeritem->updateDisplayedStatus();
                                peeritem->updateDisplayedChans();
                                peeritem->updateDisplayedName();

                                m_peerlayout->addWidget(peerwidget, naff / m_ncolumns, naff % m_ncolumns);
                                naff ++;
                                peerwidget->show();
                                if(m_engine->hasFunction("switchboard")) {
                                        connect( peerwidget, SIGNAL(transferCall(const QString&, const QString&)),
                                                 m_engine, SLOT(transferCall(const QString&, const QString&)) );
                                        connect( peerwidget, SIGNAL(hangUpChan(const QString &)),
                                                 m_engine, SLOT(hangUp(const QString &)) );
                                        connect( peerwidget, SIGNAL(interceptChan(const QString &)),
                                                 m_engine, SLOT(interceptCall(const QString &)) );
                                        connect( m_engine, SIGNAL(updateMyCalls(const QStringList &, const QStringList &, const QStringList &)),
                                                 peerwidget, SLOT(updateMyCalls(const QStringList &, const QStringList &, const QStringList &)) );
                                }
                                connect( peerwidget, SIGNAL(originateCall(const QString&, const QString&)),
                                         m_engine, SLOT(originateCall(const QString&, const QString&)) );
                        }
                }
 	}
}

/*! \brief update peer
 */
void SearchPanel::updatePeer(const UserInfo * ui,
			     const QString & sipstatus,
			     const QStringList & chanIds,
			     const QStringList & chanStates,
			     const QStringList & chanOthers,
                             const QStringList &)
{
        QString ext = ui->userid();
        QString name = ui->fullname();
        // qDebug() << "SearchPanel::updatePeer()";
        if(m_peerhash.contains(ext)) {
                PeerItem * peeritem = m_peerhash.value(ext);
                peeritem->updateStatus(sipstatus);
                peeritem->updateChans(chanIds, chanStates, chanOthers);
                peeritem->updateName(name);
                return;
        }

        // if the name (i.e. full callerid info) has not been received yet, do not add as a peer
        if(name.isEmpty())
                return;

	PeerItem * peeritem = new PeerItem(ui);
	peeritem->updateStatus(sipstatus);
	peeritem->updateChans(chanIds, chanStates, chanOthers);
        m_peerhash.insert(ext, peeritem);

        // the peerwidget is not set while its display is not needed, see affTextChanged()
}

void SearchPanel::newUser(const UserInfo * ui)
{
        PeerItem * peeritem = new PeerItem(ui);
        m_peerhash.insert(ui->userid(), peeritem);
}

void SearchPanel::updatePeerAgent(const QString & id,
                                  const QString & what,
                                  const QString & status)
{
        // qDebug() << "SearchPanel::updatePeerAgent()";
        if(m_peerhash.contains(id))
                if(what == "agentstatus") {
                        PeerItem * peeritem = m_peerhash.value(id);
                        peeritem->updateAgentStatus(status);
                } else if(what == "imstatus") {
                        PeerItem * peeritem = m_peerhash.value(id);
                        peeritem->updateIMStatus(status);
                }
        return;
}

/*! \brief remove on peer
 */
void SearchPanel::removePeer(const QString & ext)
{
        // qDebug() << "SearchPanel::removePeer()" << ext;
        if(m_peerhash.contains(ext)) {
                PeerItem * peeritem = m_peerhash.value(ext);
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
        QHashIterator<QString, PeerItem *> i(m_peerhash);
        //         QTime qtime;
        //         qtime.start();
        while(i.hasNext()) {
                i.next();
                PeerItem * peeritem = i.value();
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

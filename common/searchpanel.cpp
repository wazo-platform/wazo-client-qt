/* XIVO CTI clients
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

#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QScrollArea>
#include <QDebug>

#include "searchpanel.h"
#include "peerwidget.h"
#include "baseengine.h"

SearchPanel::SearchPanel(QWidget * parent)
        : QWidget(parent)
{
	QVBoxLayout * vlayout = new QVBoxLayout(this);
	vlayout->setMargin(0);
	QLabel * lbl = new QLabel( tr("N&ame or number to search :"), this );
	vlayout->addWidget(lbl, 0, Qt::AlignCenter);
	m_input = new QLineEdit( this );
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
	scrollarealayout->addLayout( m_peerlayout );
	scrollarealayout->addStretch( 1 );
	vlayout->addWidget(scrollarea);
}

void SearchPanel::setEngine(BaseEngine * engine)
{
        qDebug() << "SearchPanel::setEngine()";
	m_engine = engine;
        askCallerIds();
}

/*! \brief apply the search
 */
void SearchPanel::affTextChanged(const QString & text)
{
	int i;
	//qDebug() << "affTextChanged" << text;
	for(i = 0; i < m_peerlist.count(); i++)
	{
		if( m_peerlist[i].name().contains(text, Qt::CaseInsensitive) )
			m_peerlist[i].getWidget()->show();
		else
			m_peerlist[i].getWidget()->hide();
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
	int i;
       	//qDebug() << "SearchPanel::updatePeer()" << ext << name << imavail << sipstatus << chanIds;
	for(i = 0; i < m_peerlist.count(); i++)
	{
		if(ext == m_peerlist[i].ext())
		{
			m_peerlist[i].updateStatus(imavail, sipstatus, vmstatus, queuestatus);
			m_peerlist[i].updateChans(chanIds, chanStates, chanOthers);
			m_peerlist[i].updateName(name);
			return;
		}
	}
        
        // if the name (i.e. full callerid info) has not been received yet, do not add as a peer
        if(name.isEmpty())
                return;
        
	Peer peer(ext, name);
	PeerWidget * peerwidget = new PeerWidget(ext, name, this);
	peerwidget->setEngine(m_engine);
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
	m_peerlayout->addWidget( peerwidget );
	if( !name.contains(m_input->text(), Qt::CaseInsensitive) )
	{
		peerwidget->hide();
	}
	peer.setWidget(peerwidget);
	peer.updateStatus(imavail, sipstatus, vmstatus, queuestatus);
	peer.updateChans(chanIds, chanStates, chanOthers);
	m_peerlist << peer;
}

/*! \brief remove on peer
 */
void SearchPanel::removePeer(const QString & ext)
{
	int i;
        //qDebug() << "SearchPanel::removePeer()" << ext;
	for(i = 0; i < m_peerlist.size(); i++) {
                //qDebug() << m_peerlist[i].ext();
		if(m_peerlist[i].ext() == ext) {
			PeerWidget * peerwidget = m_peerlist[i].getWidget();
			m_peerlayout->removeWidget( peerwidget );
			m_peerlist.removeAt(i);
			peerwidget->deleteLater();
			return;
		}
	}
}

/*! \brief clear the widget
 */
void SearchPanel::removePeers()
{
	int i;
	//qDebug() << "SearchPanel::removePeers()";
	for(i = 0; i < m_peerlist.size(); i++)
	{
		PeerWidget * peerwidget = m_peerlist[i].getWidget();
		m_peerlayout->removeWidget( peerwidget );
		peerwidget->deleteLater();
	}
	m_peerlist.clear();
}

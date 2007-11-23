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

#include <QDebug>
#include <QDragEnterEvent>
#include <QSettings>
#include <QVBoxLayout>

#include "callstackwidget.h"
#include "callwidget.h"
#include "xivoconsts.h"

/*! \brief Constructor
 */
Call::Call(const QString & channelme)
{
	m_channelme = channelme;
}

/*! \brief Constructor
 */
Call::Call(const QString & channelme,
	   const QString & action,
	   int time,
	   const QString & direction,
	   const QString & channelpeer,
	   const QString & exten,
	   const QString & phonen)
{
	m_channelme   = channelme;
	m_action      = action;
	m_startTime   = QDateTime::currentDateTime().addSecs(-time);
	m_direction   = direction;
	m_channelpeer = channelpeer;
	m_exten       = exten;
	m_phonen      = phonen;
}

/*! \brief Copy constructor */
Call::Call(const Call & call)
//: QObject(call.parent())
{
	m_channelme   = call.m_channelme;
	m_action      = call.m_action;
	m_startTime   = call.m_startTime;
	m_direction   = call.m_direction;
	m_channelpeer = call.m_channelpeer;
	m_exten       = call.m_exten;
	m_phonen      = call.m_phonen;
}

/*! \brief update object properties
 */
void Call::updateCall(const QString & action,
		      int time,
		      const QString & direction,
		      const QString & channelpeer,
		      const QString & exten)
{
	m_action      = action;
	m_startTime   = QDateTime::currentDateTime().addSecs(-time);
	m_direction   = direction;
	m_channelpeer = channelpeer;
	m_exten       = exten;
}


/*! \brief Constructor
 */
CallStackWidget::CallStackWidget(QWidget * parent, BaseEngine * engine)
        : QWidget(parent), m_engine(engine)
{
	m_layout = new QVBoxLayout(this);
	//m_layout->setMargin();
	//m_layout->setSpacing(0);
        setObjectName("scroller");
	setAcceptDrops(true);
	m_layout->addStretch(1);
}

/*! \brief add a call to the list
 */
void CallStackWidget::addCall(const QString & channelme,
                              const QString & action,
			      int time,
			      const QString & direction,
			      const QString & channelpeer,
			      const QString & exten,
			      const QString & phonen)
{
	int found = 0;
        // qDebug() << "CallStackWidget::addCall" << channelme << action << time << direction << channelpeer << exten;

	for(int i = 0; i < m_calllist.count() ; i++) {
		if(channelme == m_calllist[i].getChannelMe()) {
			found = 1;
			if(action == QString("Hangup")) {
				m_calllist.removeAt(i);
			} else {
				m_calllist[i].updateCall(action, time,
							 direction, channelpeer, exten);
			}
		}
	}

	if((found == 0) && (action != QString("Hangup"))) {
		Call call(channelme, action, time, direction, channelpeer, exten, phonen);
		m_calllist.append(call);
	}
}

/*! \brief hang up channel
 */
void CallStackWidget::hupchan(const QString & hangupchan)
{
	hangUp(hangupchan);
}

/*! \brief transfers the channel to a number
 */
void CallStackWidget::transftonumberchan(const QString & chan)
{
        // qDebug() << "CallStackWidget::transftonumberchan()" << chan;
	transferToNumber(chan);
}

/*! \brief transfers the channel to a number
 */
void CallStackWidget::parkcall(const QString & chan)
{
        // qDebug() << "CallStackWidget::parkcall()" << chan;
	parkCall(chan);
}

/*! \brief Reset the Widget */
void CallStackWidget::reset()
{
	// qDebug() << "CallStackWidget::reset()";
	m_calllist.clear();
	QString empty = "";
	monitorPeer(empty, empty);
}

/*!
 * delete all widgets in the list 
 * and empty m_afflist */
void CallStackWidget::emptyList()
{
	// qDebug() << "CallStackWidget::emptyList()";
	// cleaning the calling list displayed
	for(int i = 0; i < m_afflist.count() ; i++) {
		// qDebug() << " Removing" << m_afflist[i]->channel();
		m_layout->removeWidget(m_afflist[i]);
		//m_afflist[i]->deleteLater();
		delete m_afflist[i];
	}
	m_afflist.clear();
	//m_calllist.clear();
}

/*! \brief update display according to call list
 *
 * Read m_calllist and update m_afflist accordingly.
 */
void CallStackWidget::updateDisplay()
{
	int i, j;
	CallWidget * callwidget = NULL;
	//	qDebug() << "CallStackWidget::updateDisplay()"
	//	         << m_afflist.count() << m_calllist.count();
	// building the new calling list
	//CallWidget * callwidget = new CallWidget(callerid, this);
	//m_layout->addWidget(callwidget, 0, Qt::AlignTop);
	//m_afflist.append(callwidget);
	
	for(j = m_afflist.count() - 1; j>= 0; j--)
	{
		for(i = 0; i < m_calllist.count(); i++)
		{
			// qDebug() << "   " << j << m_afflist[j]->channel()
			//         << i << m_calllist[i].getChannelMe();
			if(m_afflist[j]->channel() == m_calllist[i].getChannelMe())
				break;
		}
		if(i == m_calllist.count())
		{
			// qDebug() << " Removing " << m_afflist[j]->channel();
			m_layout->removeWidget(m_afflist[j]);
			//m_afflist.takeAt(j)->deleteLater();
			delete m_afflist.takeAt(j);
		}
	}

	for(i = 0; i < m_calllist.count() ; i++) {
		if(m_monitoredPeer == m_calllist[i].getPhone()) {
			Call c = m_calllist[i];
			for(j = 0; j < m_afflist.count(); j++)
			{
				// qDebug() << j << m_afflist[j]->channel();
				if(m_afflist[j]->channel() == c.getChannelMe())
				{
					m_afflist[j]->updateWidget( c.getAction(),
					                            c.getTime(),
								    c.getDirection(),
								    c.getChannelPeer(),
								    c.getExten() );
					break;
				}
			}
			if(j == m_afflist.count())
			{
			callwidget = new CallWidget(c.getChannelMe(),
								 c.getAction(),
								 c.getTime(),
								 c.getDirection(),
								 c.getChannelPeer(),
								 c.getExten(),
								 this);
			connect( callwidget, SIGNAL(doHangUp(const QString &)),
			         this, SLOT(hupchan(const QString &)) );
			connect( callwidget, SIGNAL(doTransferToNumber(const QString &)),
			         this, SLOT(transftonumberchan(const QString &)) );
			connect( callwidget, SIGNAL(doParkCall(const QString &)),
			         this, SLOT(parkcall(const QString &)) );
			m_afflist.append(callwidget);
			//m_layout->addWidget(callwidget, 0, Qt::AlignTop);
			m_layout->insertWidget(m_layout->count() - 1, callwidget,
			                       0, Qt::AlignTop);
			}
		}
	}
/*	
	callwidget = new CallWidget(this);
	m_layout->addWidget(callwidget, 1, Qt::AlignTop);
	m_afflist.append(callwidget);
*/
}

/*! \brief filter events based on the mimetype
 */
void CallStackWidget::dragEnterEvent(QDragEnterEvent * event)
{
	// qDebug() << "CallStackWidget::dragEnterEvent()" << event->mimeData()->formats();
	if (event->mimeData()->hasFormat(PEER_MIMETYPE))
	{
		event->acceptProposedAction();
	}
}

/*! \brief updates the peer to be monitored
 *
 * can be called from reset(), dropEvent(), or at the beginning
 * of a session
 */
void CallStackWidget::monitorPeer(const QString & monit_peer, const QString & name)
{
	emptyList();
	m_monitoredPeer = monit_peer;
	monitoredPeerChanged(m_monitoredPeer);
	if(name.size() > 0)
		changeTitle(tr("Monitoring : ") + name);
        else
		changeTitle("");
	updateDisplay();
	if(monit_peer.size() > 0)
		m_engine->getSettings()->setValue("monitor/peer", monit_peer);
}

/*! \brief receive drop Events.
 *
 * check if the dropped "text" is a Peer "id"
 * and start to monitor it
 */
void CallStackWidget::dropEvent(QDropEvent * event)
{
	if(!event->mimeData()->hasFormat(PEER_MIMETYPE))
	{
		event->ignore();
		return;
	}
	QString text = event->mimeData()->text();
	QString name = QString::fromUtf8(event->mimeData()->data("name"));
	monitorPeer(text, name);
	event->acceptProposedAction();
}


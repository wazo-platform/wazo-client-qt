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
#include <QDragEnterEvent>
#include <QSettings>
#include <QVBoxLayout>

#include "callstackwidget.h"
#include "callwidget.h"
#include "userinfo.h"
#include "xivoconsts.h"

/*! \brief Constructor
 */
Call::Call(const QString & channelme)
{
	m_channelme = channelme;
}

/*! \brief Constructor
 */
Call::Call(UserInfo * ui,
           const QString & channelme,
	   const QString & action,
	   int time,
	   const QString & direction,
	   const QString & channelpeer,
	   const QString & exten)
{
        m_ui          = ui;
	m_channelme   = channelme;
	m_action      = action;
	m_startTime   = QDateTime::currentDateTime().addSecs(-time);
	m_direction   = direction;
	m_channelpeer = channelpeer;
	m_exten       = exten;
}

/*! \brief Copy constructor */
Call::Call(const Call & call)
//: QObject(call.parent())
{
        m_ui          = call.m_ui;
	m_channelme   = call.m_channelme;
	m_action      = call.m_action;
	m_startTime   = call.m_startTime;
	m_direction   = call.m_direction;
	m_channelpeer = call.m_channelpeer;
	m_exten       = call.m_exten;
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

//! get m_phonen
const QString & Call::getUserId() const
{
        return m_ui->userid();
}

//! get m_channelme
const QString & Call::getChannelMe() const
{
        return m_channelme;
}

//! get m_action
const QString & Call::getAction() const
{
        return m_action;
}









/*! \brief Constructor
 */
CallStackWidget::CallStackWidget(QWidget * parent)
        : QWidget(parent)
{
	m_layout = new QVBoxLayout(this);
	//m_layout->setMargin();
	//m_layout->setSpacing(0);
        setObjectName("scroller");
	setAcceptDrops(true);
	m_layout->addStretch(1);
}

void CallStackWidget::updatePeer(UserInfo * ui,
                                 const QString &,
                                 const QHash<QString, QStringList> & chanlist)
{
        // qDebug() << "CallStackWidget::updatePeer()" << m_calllist.size() << chanlist;
        QHashIterator<QString, QStringList> ccallchannel(chanlist);
        while (ccallchannel.hasNext()) {
                ccallchannel.next();
                QStringList value = ccallchannel.value();
                addCall(ui,
                        ccallchannel.key(),
                        value[0],
                        value[1].toInt(),
                        value[2],
                        value[3],
                        value[4]);
        }
}

/*! \brief add a call to the list
 */
void CallStackWidget::addCall(UserInfo * ui,
                              const QString & channelme,
                              const QString & action,
			      int time,
			      const QString & direction,
			      const QString & channelpeer,
			      const QString & exten)
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
		Call call(ui, channelme, action, time, direction, channelpeer, exten);
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
	// monitorPeer(NULL);
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
        // qDebug() << "CallStackWidget::updateDisplay()"
        // << m_afflist.count() << m_calllist.count();
	// building the new calling list
	// CallWidget * callwidget = new CallWidget(callerid, this);
	// m_layout->addWidget(callwidget, 0, Qt::AlignTop);
	// m_afflist.append(callwidget);
	
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
		if(m_monitored_userid == m_calllist[i].getUserId()) {
			Call c = m_calllist[i];
			for(j = 0; j < m_afflist.count(); j++) {
				// qDebug() << j << m_afflist[j]->channel();
				if(m_afflist[j]->channel() == c.getChannelMe()) {
					m_afflist[j]->updateWidget( c.getAction(),
					                            c.getTime(),
								    c.getDirection(),
								    c.getChannelPeer(),
								    c.getExten() );
					break;
				}
			}
			if(j == m_afflist.count()) {
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
void CallStackWidget::monitorPeer(UserInfo * ui)
{
	emptyList();
	m_monitored_userid = ui->userid();
        changeTitle(tr("Monitoring : ") + ui->fullname());
	updateDisplay();
}

/*! \brief receive drop Events.
 *
 * check if the dropped "text" is a Peer "id"
 * and start to monitor it
 */
void CallStackWidget::dropEvent(QDropEvent * event)
{
	if(!event->mimeData()->hasFormat(PEER_MIMETYPE)) {
		event->ignore();
		return;
	}
	monitorPeerRequest(event->mimeData()->data("userid"));
	event->acceptProposedAction();
}

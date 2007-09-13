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

#include "peeritem.h"
#include "peerwidget.h"

/*! \brief Constructor
 *
 * just sets the properties m_ext and m_name.
 */
Peer::Peer(const QString & ext, const QString & name)
        : m_ext(ext), m_name(name)
{
}

/*! \brief Copy constructor
 */
Peer::Peer(const Peer & peer)
{
	m_ext = peer.m_ext;
	m_name = peer.m_name;
	m_peerwidget = peer.m_peerwidget;
}

/*! \brief update status of the peer
 *
 * Change what is displayed according to new status values.
 */
void Peer::updateStatus(const QString & imavail,
			const QString & sipstatus,
			const QString & vmstatus,
			const QString & queuestatus)
{
	QString display_imavail;
	QString display_sipstatus;

	//qDebug() << imavail << sipstatus;
	if(imavail == "available") {
		m_peerwidget->setGreen(1);
		display_imavail = PeerWidget::tr("Available");
	} else if(imavail == "away") {
		m_peerwidget->setBlue(1);/*setDarkGreen(1);*/
		display_imavail = PeerWidget::tr("Away");
	} else if(imavail == "donotdisturb") {
		m_peerwidget->setRed(1);
		display_imavail = PeerWidget::tr("Do not disturb");
	} else if(imavail == "berightback") {
		m_peerwidget->setOrange(1);
		display_imavail = PeerWidget::tr("Be Right Back");
	} else if(imavail == "outtolunch") {
		m_peerwidget->setYellow(1);
		display_imavail = PeerWidget::tr("Out To Lunch");
	} else if(imavail == "unknown") {
		m_peerwidget->setGray(1);
		display_imavail = PeerWidget::tr("Unknown");
	} else {
		m_peerwidget->setGray(1);
		display_imavail = imavail;
	}


	if(sipstatus == "Ready") {
		m_peerwidget->setGreen(0);
		display_sipstatus = PeerWidget::tr("Ready");
	} else if(sipstatus == "Ringing") {
		m_peerwidget->setBlue(0);/*setCyan(0);*/
		display_sipstatus = PeerWidget::tr("Ringing");
	} else if(sipstatus == "Calling") {
		m_peerwidget->setYellow(0);
		display_sipstatus = PeerWidget::tr("Calling");
	} else if((sipstatus == "On the phone") || (sipstatus == "Up")) {
		m_peerwidget->setRed(0);
		display_sipstatus = PeerWidget::tr("On the phone");
	} else if(sipstatus == "Not online") {
		m_peerwidget->setGray(0);
		display_sipstatus = PeerWidget::tr("Not online");
	} else {
		m_peerwidget->setGray(0);
		display_sipstatus = sipstatus;
	}


	QString fortooltip = PeerWidget::tr("SIP Presence : ") + display_sipstatus + "\n"
		+ PeerWidget::tr("Availability : ") + display_imavail;
	/* + "\n"
	   + "Voicemail Status: " + vmstatus + "\n"
	   + "Queues Status: " + queuestatus;*/

	m_peerwidget->setToolTip(fortooltip);

	//  if(corrname == "")
	//    m_peerwidget->setToolTip(status);
	//  else
	//    m_peerwidget->setToolTip(status + "\n" + corrname);
}

/*! \brief update channel list
 */
void Peer::updateChans(const QStringList & chanIds,
                       const QStringList & chanStates,
                       const QStringList & chanOthers)
{
	if( (chanIds.size() != chanStates.size())
	   || (chanIds.size() != chanOthers.size()) )
	{
		qDebug() << "Peer::updateChans() : bad args";
		return;
	}
	if(m_peerwidget)
	{
		m_peerwidget->clearChanList();
		for(int i = 0; i < chanIds.size(); i++)
		{
			m_peerwidget->addChannel(chanIds[i], chanStates[i], chanOthers[i]);
		}
	}
}

/*! \brief update name if changed
 */
void Peer::updateName(const QString & newname)
{
	if(newname != m_name)
	{
		m_name = newname;
		m_peerwidget->setName(m_name);
	}
}


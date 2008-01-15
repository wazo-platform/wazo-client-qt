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

#include <QDebug>

#include "peerchannel.h"

/*! \brief Constructor
 *
 * Just fill the members m_id, m_state and m_otherPeer.
 */
PeerChannel::PeerChannel(const QString &id, const QString &state,
                         const QString &otherPeer, QObject *parent)
: QObject(parent), m_id(id), m_state(state), m_otherPeer(otherPeer)
{
}

/*! \brief intercept the channel
 */
void PeerChannel::intercept()
{
	//qDebug() << "PeerChanne::intercept()" << m_id;
	// emit a signal to be sent to the engine.
	interceptChan( m_id );
}

/*! \brief hang up the channel
 */
void PeerChannel::hangUp()
{
	//qDebug() << "PeerChannel::hangUp()" << m_id;
	hangUpChan( m_id );
}

/*! \brief transfer this channel
 */
void PeerChannel::transfer()
{
	//qDebug() << "PeerChannel::transfer()" << m_id;
	transferChan( m_id );
}


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

#ifndef __PEERCHANNEL_H__
#define __PEERCHANNEL_H__

#include <QObject>
#include <QString>

/*! \brief Channel associated to a peer
 *
 */
class PeerChannel : public QObject
{
	Q_OBJECT
public:
	PeerChannel(const QString &id, const QString & state,
	            const QString &otherPeer, QObject * parent=0);
	//! get m_otherPeer
	const QString & otherPeer() const { return m_otherPeer; };
public slots:
	void intercept();
	void hangUp();
	void transfer();
signals:
	void interceptChan(const QString &);	//!< intercept
	void hangUpChan(const QString &);		//!< hang up
	void transferChan(const QString &);		//!< transfer
private:
	QString m_id;			//!< Identification string of the channel
	QString m_state;		//!< State of the channel
	QString m_otherPeer;	//!< other side of the channel
};

#endif


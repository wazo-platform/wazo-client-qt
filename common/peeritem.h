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

#ifndef __PEERITEM_H__
#define __PEERITEM_H__

#include <QObject>
#include <QString>
#include <QStringList>

class PeerWidget;

/*! \brief Peer object, linking to a PeerWidget
 */
class Peer// : public QObject
{
public:
	//Peer( const QString & ext, QObject * parent = 0 );
	Peer( const QString & ext, const QString & name );
	Peer( const Peer & peer);
	Peer( );
	//! get m_ext
	const QString & ext() const { return m_ext; };
	//! get m_name
	const QString & name() const { return m_name; };
//	Peer & operator=(const Peer & peer);
	//! set m_peerwidget
	void setWidget(PeerWidget * widget) { m_peerwidget = widget; };
	//! get m_peerwidget
	PeerWidget * getWidget() { return m_peerwidget; };

	void updateStatus(const QString & imavail,
			  const QString & sipstatus,
			  const QString & vmstatus,
			  const QString & queuestatus);
	void updateChans(const QStringList & chanIds,
	                 const QStringList & chanStates,
	                 const QStringList & chanOthers);
	void updateName(const QString & newname);

	void updateDisplayedStatus();
	void updateDisplayedChans();
	void updateDisplayedName();
private:
	QString m_ext;		//!< Extension
	QString m_name;		//!< Person name
	PeerWidget * m_peerwidget;	//!< related PeerWidget

        // Properties of each peer
        QString m_imavail;
        QString m_sipstatus;
        QString m_vmstatus;
        QString m_queuestatus;
        QStringList m_chanIds;
        QStringList m_chanStates;
        QStringList m_chanOthers;
};


#endif

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

#ifndef __SEARCHPANEL_H__
#define __SEARCHPANEL_H__
#include <QWidget>
#include <QList>
#include "peeritem.h"

class QVBoxLayout;
class QLineEdit;
class BaseEngine;

/*! \brief search panel widget
 */
class SearchPanel : public QWidget
{
	Q_OBJECT
public:
	SearchPanel(QWidget * parent = 0);	//!< Constructor
	void setEngine(BaseEngine *);	//!< set m_engine
public slots:
	void affTextChanged(const QString &);
	void updatePeer(const QString & ext,
	                const QString & name,
			const QString & imavail,
			const QString & sipstatus,
			const QString & vmstatus,
			const QString & queuestatus,
			const QStringList & chanIds,
			const QStringList & chanStates,
			const QStringList & chanOthers);
	void removePeer(const QString & ext);
	void removePeers();
signals:
	void askCallerIds();
private:
	BaseEngine * m_engine;	//!< engine object reference
	QList<Peer> m_peerlist;	//!< Peer list
	QVBoxLayout * m_peerlayout;	//!< layout object
	QLineEdit * m_input;	//!< widget for search string input
};

#endif


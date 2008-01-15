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

#ifndef __SWITCHBOARDWINDOW_H__
#define __SWITCHBOARDWINDOW_H__

#include <QHash>
#include <QList>
#include <QWidget>

#include "peerwidget.h"
#include "peerslayout.h"
#include "peeritem.h"

class BaseEngine;
class QGridLayout;
class QMouseEvent;


/*! \brief Widget displaying Peers
 *
 * This widget use a PeersLayout to display Peers in a grid.
 */
class SwitchBoardWindow : public QWidget
{
	Q_OBJECT
public:
	//! Constructor
	SwitchBoardWindow( QWidget * parent = 0);
	//! Destructor
	virtual ~SwitchBoardWindow();
	void setEngine(BaseEngine *);
	void savePositions() const;
protected:
/*         void mousePressEvent(QMouseEvent *);	//!< Catch mouse press events */
/*         void mouseMoveEvent(QMouseEvent *); */
	void dragEnterEvent(QDragEnterEvent *);
	void dropEvent(QDropEvent *);
public slots:
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
	void removePeers(void);
private slots:
	void removePeerFromLayout(const QString &);
private:
	//QGridLayout * m_layout;
	PeersLayout * m_layout;			//!< Grid Layout for displaying peers
	QList<Peer *> m_peerlist;		//!< Peer list
	QHash<QString, Peer *> m_peerhash;	//!< Peer hash

	BaseEngine * m_engine;	//!< engine to connect to peer widgets
        QPixmap m_phone_green;
        QPixmap m_phone_red;
        QPixmap m_phone_orange;
        QPixmap m_phone_grey;
        QPixmap m_phone_yellow;
        QPixmap m_phone_blue;
        QPixmap m_person_green;
        QPixmap m_person_red;
        QPixmap m_person_orange;
        QPixmap m_person_grey;
        QPixmap m_person_yellow;
        QPixmap m_person_blue;
};

#endif


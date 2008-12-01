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

#ifndef __SWITCHBOARDWINDOW_H__
#define __SWITCHBOARDWINDOW_H__

#include <QHash>
#include <QList>
#include <QVariant>
#include <QWidget>

class QGridLayout;
class QMouseEvent;

class BaseEngine;
class PeerItem;
class PeersLayout;
class UserInfo;

/*! \brief Widget displaying PeerItems
 *
 * This widget use a PeersLayout to display PeerItems in a grid.
 */
class SwitchBoardWindow : public QWidget
{
	Q_OBJECT
public:
	SwitchBoardWindow(BaseEngine *,
                          const QVariant &,
                          QWidget * parent = 0);
        ~SwitchBoardWindow();	//! Destructor
	void savePositions() const;
protected:
/*         void mousePressEvent(QMouseEvent *);	//!< Catch mouse press events */
/*         void mouseMoveEvent(QMouseEvent *); */
	void dragEnterEvent(QDragEnterEvent *);
	void dropEvent(QDropEvent *);
        void dragMoveEvent(QDragMoveEvent *);
public slots:
        void setGuiOptions(const QVariant &);
        void setUserInfo(const UserInfo *);
        void updatePeer(UserInfo *,
                        const QString &,
                        const QVariant &);
        void newUser(UserInfo *);
        void updatePeerAgent(const QString &,
                             const QString &,
                             const QVariant &);
	void removePeer(const QString &);
	void removePeers();
private slots:
	void removePeerFromLayout(const QString &);
private:
	BaseEngine * m_engine;	//!< engine to connect to peer widgets
	QHash<QString, PeerItem *> m_peerhash;	//!< PeerItem hash
	QList<PeerItem *> m_peerlist;		//!< PeerItem list
	//QGridLayout * m_layout;
	PeersLayout * m_layout;			//!< Grid Layout for displaying peers
	QHash<QString, QPixmap> m_persons;
	QHash<QString, QPixmap> m_phones;
	QHash<QString, QPixmap> m_agents;
        QVariant m_options;
};

#endif

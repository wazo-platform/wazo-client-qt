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

#ifndef __PEERWIDGET_H__
#define __PEERWIDGET_H__

#include <QHash>
#include <QWidget>
#include <QPoint>

#include "peerchannel.h"

class QLabel;
class QPixmap;

class BaseEngine;
class PeerChannel;
class ExtendedLabel;
class UserInfo;

/*! \brief Widget to display a Peer status
 *
 * Display Icons for the state and the peer name.
 */
class PeerWidget : public QWidget
{
	Q_OBJECT
public:
	PeerWidget(UserInfo *,
                   const QVariant &,
                   const QHash<QString, QPixmap> &,
                   const QHash<QString, QPixmap> &,
                   const QHash<QString, QPixmap> &);
        //	           QWidget * parent = 0/*, int size = 16*/);
	~PeerWidget();
	void clearChanList();
	void addChannel(const QVariant &);
	void setName(const QString &);
	void setEngine(BaseEngine *);
        void setAgentToolTip(const QString &, const QStringList &);
        QString getToolTip(const QString &);
	void setColorAvail(const QString &, const QString &, const QString &);	//! sets a kind of icon in a given color
protected:
	void mouseMoveEvent(QMouseEvent *);
	void mousePressEvent(QMouseEvent *);
        void mouseDoubleClickEvent(QMouseEvent *);
	void dragEnterEvent(QDragEnterEvent *);
	void dragMoveEvent(QDragMoveEvent *);
	void dropEvent(QDropEvent *);
	void contextMenuEvent(QContextMenuEvent *);
signals:
	void actionCall(const UserInfo *, const QString &,
                        const QString &, const QString &);	//! originate, transfer & atxfer signals
	void interceptCall(const UserInfo *, const QString &);	//! intercept signal
	void hangupCall(const UserInfo *, const QString &);	//! hang up signal
	//! hide the widget in the channel
	void doRemoveFromPanel(const QString &);
public slots:
        void updatePeer(UserInfo *,
                        const QString &,
                        const QVariant &);
private slots:
	void transferChan(const QString &);
	void hangupChan(const QString &);
	void interceptChan(const QString &);
	void removeFromPanel();
        void mouseDoubleClickEventLocal(QMouseEvent *);
	void dial();
private:
	BaseEngine * m_engine;  //!< Base Engine reference
        UserInfo * m_ui;
        
	QHash<QString, QLabel *> m_lblphones;	//!< phone labels
        
	QLabel * m_availlbl;	//!< Peer state display from XIVO CTI Client
	ExtendedLabel * m_agentlbl;
	QLabel * m_voicelbl;
	QLabel * m_fwdlbl;
	QLabel * m_textlbl;		//!< text label : to display peer name
	//QPixmap m_square;		//!< pixmap used to display the states
	QPoint m_dragstartpos;	//!< drag start position
        
	QAction * m_removeAction;	//!< action to remove this peer from the window
	QAction * m_dialAction;		//!< action to dial this number
	
	QList<PeerChannel *> m_channels;	//!< channels associated with THIS peer
	QList<PeerChannel *> m_mychannels;	//!< channels associated with ME
        
	/* TODO : have the Pixmaps as static objects */
	QHash<QString, QPixmap> m_phones;	//!< phone icons
	QHash<QString, QPixmap> m_persons;	//!< person icons
	QHash<QString, QPixmap> m_agents;	//!< agent icons
        QStringList m_functions;
};

#endif

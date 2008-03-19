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

#ifndef __SEARCHPANEL_H__
#define __SEARCHPANEL_H__

#include <QList>
#include <QHash>
#include <QWidget>

#include "peeritem.h"

class QGridLayout;
class QVBoxLayout;

class BaseEngine;
class ExtendedLineEdit;

/*! \brief search panel widget
 */
class SearchPanel : public QWidget
{
	Q_OBJECT
public:
	SearchPanel(QWidget * parent = 0);	//!< Constructor
	~SearchPanel();	//!< Destructor
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
        void callsUpdated();
signals:
	void askCallerIds();
private:
	BaseEngine * m_engine;	//!< engine object reference
	QHash<QString, Peer *> m_peerhash;	//!< Peer list
	QGridLayout * m_peerlayout;	//!< layout object
	ExtendedLineEdit * m_input;	//!< widget for search string input
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
        int m_maxdisplay;	//!< max number of peers displayed on the search panel
        QString m_searchpattern;
};

#endif

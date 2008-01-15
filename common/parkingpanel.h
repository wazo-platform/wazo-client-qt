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

#ifndef __PARKINGPANEL_H__
#define __PARKINGPANEL_H__

#include <QWidget>
#include <QList>

class QContextMenuEvent;
class QTableWidget;
class QTableWidgetItem;

class BaseEngine;
class ExtendedTableWidget;
class PeerChannel;

/*! \brief Widget to display a list of messages
 *
 * dispays the messages in a table with timestamps.
 */
class ParkingPanel : public QWidget
{
	Q_OBJECT
public:
	ParkingPanel(QWidget * parent = 0);
        ~ParkingPanel();
        void setEngine(BaseEngine *);
protected:
	void timerEvent(QTimerEvent *);		//!< receive timer events
public slots:
        void parkingEvent(const QString &, const QString &);
        void contextMenuEvent(QContextMenuEvent *);
private slots:
	void dialNumber();
	void hangUp();
	void itemClicked(QTableWidgetItem *);
	void itemDoubleClicked(QTableWidgetItem *);
signals:
	void copyNumber(const QString &);
	void emitDial(const QString &);
	void transferCall(const QString &, const QString &);
	void originateCall(const QString &, const QString &);
        void newParkEvent();
private:
	BaseEngine * m_engine;	//!< engine object reference
	ExtendedTableWidget * m_table;	//! Table
        int m_timerid;
        int m_deltasec;
	QString m_astid;	//!< asterisk id selected
	QString m_placenum;	//!< number to call to retrieve the parked call
	QString m_parkedpeer;	//!< parked peer
	QList<PeerChannel *> m_mychannels;	//!< "my channels" list for transfer menu
};

#endif

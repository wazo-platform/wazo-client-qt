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

#ifndef __PARKINGPANEL_H__
#define __PARKINGPANEL_H__

#include <QWidget>
#include <QList>
#include <QLabel>
#include "peeritem.h"

class QVBoxLayout;
class QLineEdit;
class QTableWidget;
class QTableWidgetItem;

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
protected:
	void timerEvent(QTimerEvent *);		//!< receive timer events
public slots:
        void parkingEvent(const QString &, const QString &);
private slots:
	void itemClicked(QTableWidgetItem *);
	void itemDoubleClicked(QTableWidgetItem *);
signals:
	void copyNumber(const QString &);
	void emitDial(const QString &);
private:
 	//QLabel * m_text;
	//! Table
	QTableWidget * m_table;
        int m_timerid;
        int m_deltasec;
};

#endif

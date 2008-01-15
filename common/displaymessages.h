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

#ifndef __DISPLAYMESSAGES_H__
#define __DISPLAYMESSAGES_H__

#include <QWidget>
#include <QList>
#include <QLabel>

#include "peeritem.h"

class QVBoxLayout;
class QLineEdit;
class QTableWidget;

/*! \brief Widget to display a list of messages
 *
 * dispays the messages in a table with timestamps.
 */
class DisplayMessagesPanel : public QWidget
{
	Q_OBJECT
public:
	DisplayMessagesPanel(QWidget * parent = 0);
public slots:
/* 	void affTextChanged(); */
	void addMessage(const QString &);
/* signals: */
/* 	void emitDial(const QString &); */
private:
 	//QLabel * m_text;
	//! Table
	QTableWidget * m_table;
};

#endif


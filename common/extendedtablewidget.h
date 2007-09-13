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

#ifndef __EXTENDEDTABLEWIDGET_H__
#define __EXTENDEDTABLEWIDGET_H__

#include <QTableWidget>
#include <QList>
#include "peerchannel.h"

class QContextMenuEvent;

/*! \brief Table class inheriting QTableWidget with contextMenu added.
 */
class ExtendedTableWidget : public QTableWidget
{
	Q_OBJECT
public:
	ExtendedTableWidget(QWidget * parent = 0);
	ExtendedTableWidget(int rows, int columns, QWidget * parent = 0);
protected:
	void contextMenuEvent(QContextMenuEvent * event);
	void dragEnterEvent(QDragEnterEvent *event);
	void dragMoveEvent(QDragMoveEvent *event);
	void dropEvent(QDropEvent *event);
signals:
	void emitDial(const QString & number);	//!< dial
	void transferCall(const QString &, const QString &);	//!< transfer a call
	void originateCall(const QString &, const QString &);	//!< originate a call
public slots:
	void updateMyCalls(const QStringList &, const QStringList &, const QStringList &);
private slots:
	void dialNumber();
        void sendMail();
	void transferChan(const QString &);
private:
	QString m_numberToDial;		//!< used to store number to dial or to transfer to
	QString m_mailAddr;		//!< used to store email address
	QList<PeerChannel *> m_mychannels;	//!< "my channels" list for transfer menu
};

#endif


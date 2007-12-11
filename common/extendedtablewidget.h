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

class QContextMenuEvent;
class QMouseEvent;

/*! \brief Table class inheriting QTableWidget with contextMenu added.
 */
class ExtendedTableWidget : public QTableWidget
{
	Q_OBJECT
public:
	ExtendedTableWidget(QWidget * parent = 0);
	ExtendedTableWidget(int rows, int columns, QWidget * parent = 0);
protected:
	void contextMenuEvent(QContextMenuEvent *);
	void dragEnterEvent(QDragEnterEvent *);
	void dragMoveEvent(QDragMoveEvent *);
	void dropEvent(QDropEvent *);
        void mouseMoveEvent(QMouseEvent *);
signals:
	void emitDial(const QString & number);	//!< dial
	void transferCall(const QString &, const QString &);	//!< transfer a call
	void originateCall(const QString &, const QString &);	//!< originate a call
	void ContextMenuEvent(QContextMenuEvent *);
};

#endif

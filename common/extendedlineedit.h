/* XIVO CTI clients
Copyright (C) 2007, 2008  Proformatique

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

#ifndef __EXTENDEDLINEEDIT_H__
#define __EXTENDEDLINEEDIT_H__

#include <QLineEdit>

class QDropEvent;
class QLineEdit;

/*! \brief Table class inheriting QLineEdit with contextMenu added.
 */
class ExtendedLineEdit : public QLineEdit
{
	Q_OBJECT
public:
	ExtendedLineEdit(QWidget * parent = 0);
	ExtendedLineEdit(int rows, int columns, QWidget * parent = 0);
protected:
	void dropEvent(QDropEvent *event);
};

#endif

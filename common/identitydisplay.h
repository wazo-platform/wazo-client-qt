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

#ifndef __IDENTITYDISPLAY_H__
#define __IDENTITYDISPLAY_H__

#include <QObject>
#include <QWidget>
#include <QList>
#include <QLabel>

/*! \brief Identity Display
 */
class IdentityDisplay : public QWidget
{
	Q_OBJECT
public:
	IdentityDisplay(QWidget * parent = 0);
protected:
public slots:
	void setUser(const QString &);
private:
        QLabel * m_user;
};


class SizeableLabel : public QLabel
{
        Q_OBJECT
public:
        SizeableLabel(const QString &text, const QSize &size, QWidget *parent = 0);
        QSize sizeHint() const;
private:
        QSize m_size;
};

#endif

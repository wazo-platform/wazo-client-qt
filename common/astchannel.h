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

#ifndef __ASTCHANNEL_H__
#define __ASTCHANNEL_H__
#include <QObject>

/*! \brief Object to store Asterisk channel parameters
 *
 *
 */
class AstChannel : public QObject
{
	Q_OBJECT
public:
	AstChannel(const QString & id, QObject * parent = 0);
	//AstChannel(const AstChannel &);
	const QString & id() const;
	const QString & extension() const;
	void setExtension(const QString &);
	//const QString & state() const;
	//void setState(const QString &);
private:
	QString m_id;	//!< id of the asterisk channel
	QString m_extension;	//!< extension
	//QString m_state;		//!< state
	enum { TO, FROM } m_direction;	//!< direction
	QString m_linkedchannel;	//!< linked channel
	QString m_linkedextension;	//!< linked extension
};

#endif


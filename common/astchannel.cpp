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

#include "astchannel.h"

/*! \brief constructor
 *
 * Just initialize the m_id property.
 */
AstChannel::AstChannel(const QString & id, QObject * parent)
: QObject(parent), m_id(id)
{
}

/*! \brief get m_id
 */
const QString & AstChannel::id() const
{
	return m_id;
}

/*! \brief get m_extension
 */
const QString & AstChannel::extension() const
{
	return m_extension;
}

/*! \brief set m_extension
 */
void AstChannel::setExtension(const QString & ext)
{
	m_extension = ext;
}


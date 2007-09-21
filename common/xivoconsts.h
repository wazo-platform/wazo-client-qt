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

#ifndef __XIVOCONSTS_H__
#define __XIVOCONSTS_H__

#define CHANNEL_MIMETYPE	"XIVO_ASTERISK_CHANNEL"
#define PEER_MIMETYPE		"XIVO_ASTERISK_PEER"

const int REQUIRED_SERVER_VERSION = 1538;
const QString __required_server_version__ = QString::number(REQUIRED_SERVER_VERSION);
const QString __current_client_version__  = SVNVER;

#endif

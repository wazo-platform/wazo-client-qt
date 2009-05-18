/* XiVO Client
 * Copyright (C) 2007-2009, Proformatique
 *
 * This file is part of XiVO Client.
 *
 * XiVO Client is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version, with a Section 7 Additional
 * Permission as follows:
 *   This notice constitutes a grant of such permission as is necessary
 *   to combine or link this software, or a modified version of it, with
 *   the OpenSSL project's "OpenSSL" library, or a derivative work of it,
 *   and to copy, modify, and distribute the resulting work. This is an
 *   extension of the special permission given by Trolltech to link the
 *   Qt code with the OpenSSL library (see
 *   <http://doc.trolltech.com/4.4/gpl.html>). The OpenSSL library is
 *   licensed under a dual license: the OpenSSL License and the original
 *   SSLeay license.
 *
 * XiVO Client is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with XiVO Client.  If not, see <http://www.gnu.org/licenses/>.
 */

/* $Revision$
 * $Date$
 */

#ifndef __XIVOCONSTS_H__
#define __XIVOCONSTS_H__

#define CHANNEL_MIMETYPE  "XIVO_ASTERISK_CHANNEL"
#define PEER_MIMETYPE     "XIVO_ASTERISK_PEER"
#define NUMBER_MIMETYPE   "XIVO_ASTERISK_NUMBER"
#define USERID_MIMETYPE   "XIVO_ASTERISK_USERID"
#define NAME_MIMETYPE     "XIVO_ASTERISK_NAME"

#define XIVO_COMMAND_ROOT "XIVO_COMMAND_ROOT"

#define CHAN_STATUS_READY "ready"
#define CHAN_STATUS_HANGUP "hangup"
#define CHAN_STATUS_CALLING "calling"
#define CHAN_STATUS_RINGING "ringing"
#define CHAN_STATUS_LINKED_CALLER "linked-caller"
#define CHAN_STATUS_LINKED_CALLED "linked-called"
#define CHAN_STATUS_UNLINKED_CALLER "unlinked-caller"
#define CHAN_STATUS_UNLINKED_CALLED "unlinked-called"

const int REQUIRED_SERVER_VERSION = 5650;

const QString __required_server_version__ = QString::number(REQUIRED_SERVER_VERSION);
const QString __current_client_version__  = SVNVER;
const QString __datebuild_client__  = DATEBUILD;
const QString __xivo_version__  = "1.1";
const QString __nopresence__ = "nopresence";
const QStringList XletList = (QStringList() << "void" << "customerinfo" << "features" << "history"
                              << "directory" << "search" << "fax" << "dial"
                              << "operator" << "parking" << "calls" << "switchboard"
                              << "messages" << "identity" << "datetime" << "tabber" << "conference" << "xletproto" << "callcampaign" << "mylocaldir"
#ifdef USE_WEBKIT
                              << "xletweb"
#endif /* USE_WEBKIT */
#ifdef USE_OUTLOOK
                              << "outlook"
#endif /* USE_OUTLOOK */
                              << "agents" << "agentsnext" << "agentdetails" << "queues" << "queuedetails" << "queueentrydetails");
const QStringList CheckFunctions = (QStringList() << "presence" << "customerinfo");

#endif /* __XIVOCONSTS_H__ */

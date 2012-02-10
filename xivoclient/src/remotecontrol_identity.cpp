/* XiVO Client
 * Copyright (C) 2007-2011, Avencall
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

#ifdef FUNCTESTS

#include "xlets/identity/identity.h"

#include "remotecontrol.h"

bool RemoteControl::then_the_xlet_identity_shows_name_as_1_2(const QStringList &args)
{
    IdentityDisplay *xlet = static_cast<IdentityDisplay*>(m_exec_obj.win->m_xletlist.value("identity"));
    QLabel *fullname = xlet->findChild<QLabel*>(QString("fullname"));
    QString args_fullname = args.join(" ");
    return fullname->text() == args_fullname;
}

bool RemoteControl::then_the_xlet_identity_shows_server_name_as_field_1_modified(const QStringList &args)
{
    IdentityDisplay *xlet = static_cast<IdentityDisplay*>(m_exec_obj.win->m_xletlist.value("identity"));
    QLabel *fullname = xlet->findChild<QLabel*>(QString("fullname"));
    QString hostname = args[0];
    return fullname->toolTip().endsWith(hostname);
}

bool RemoteControl::then_the_xlet_identity_shows_phone_number_as_1(const QStringList &args)
{
    IdentityDisplay *xlet = static_cast<IdentityDisplay*>(m_exec_obj.win->m_xletlist.value("identity"));
    QLabel *phonenum = xlet->findChild<QLabel*>(QString("phonenum"));
    QString args_phonenum = args[0];
    return phonenum->text() == args_phonenum;
}

#endif

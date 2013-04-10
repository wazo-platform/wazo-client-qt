/* XiVO Client
 * Copyright (C) 2007-2013, Avencall
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

#include "remote_control.h"

QVariantMap RemoteControl::get_identity_infos()
{
    IdentityDisplay *xlet = static_cast<IdentityDisplay*>(m_exec_obj.win->m_xletlist.value("identity"));
    QLabel *fullname = xlet->findChild<QLabel*>(QString("fullname"));
    QLabel *phonenum = xlet->findChild<QLabel*>(QString("phonenum"));
    QLabel *vm_num = xlet->findChild<QLabel*>(QString("voicemail_num"));
    QPushButton * vm_button = xlet->findChild<QPushButton*>(QString("voicemail_button"));
    QIcon vm_icon = vm_button->icon();
    QLabel *agent_number_label = xlet->findChild<QLabel*>(QString("agent_number"));

    QVariantMap args;
    args["fullname"] = fullname->text();
    args["phonenum"] = phonenum->text();
    args["voicemail_num"] = vm_num->text();
    args["voicemail_button"] = !vm_icon.isNull();
    args["agent_number"] = agent_number_label->text();

    return args;
}

#endif

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

void RemoteControl::then_the_xlet_identity_shows_name_as_1_2(const QVariantList &args)
{
    const QString & expected_firstname = args[0].toString();
    const QString & expected_lastname = args[1].toString();
    QString expected_fullname = QString("%1 %2").arg(expected_firstname).arg(expected_lastname);

    IdentityDisplay *xlet = static_cast<IdentityDisplay*>(m_exec_obj.win->m_xletlist.value("identity"));
    QLabel *fullname = xlet->findChild<QLabel*>(QString("fullname"));
    this->assert(expected_fullname == fullname->text());
}

void RemoteControl::then_the_xlet_identity_shows_server_name_as_field_1_modified(const QVariantList &args)
{
    const QString & expected_hostname = args[0].toString();

    IdentityDisplay *xlet = static_cast<IdentityDisplay*>(m_exec_obj.win->m_xletlist.value("identity"));
    QLabel *fullname = xlet->findChild<QLabel*>(QString("fullname"));
    this->assert(fullname->toolTip().endsWith(expected_hostname));
}

void RemoteControl::then_the_xlet_identity_shows_phone_number_as_1(const QVariantList &args)
{
    const QString & expected_phone_number = args[0].toString();

    IdentityDisplay *xlet = static_cast<IdentityDisplay*>(m_exec_obj.win->m_xletlist.value("identity"));
    QLabel *phonenum = xlet->findChild<QLabel*>(QString("phonenum"));
    this->assert(expected_phone_number == phonenum->text());
}

void RemoteControl::then_the_xlet_identity_shows_a_voicemail_1(const QVariantList &args)
{
    const QString & expected_voicemail_number = args[0].toString();

    IdentityDisplay *xlet = static_cast<IdentityDisplay*>(m_exec_obj.win->m_xletlist.value("identity"));
    this->assert(xlet != NULL, "xlet null");

    QLabel *vm_num = xlet->findChild<QLabel*>(QString("voicemail_num"));
    this->assert(vm_num != NULL, "no vm num label");

    this->assert(vm_num->text().endsWith(expected_voicemail_number), QString("vm num is %1").arg(vm_num->text()));

    QPushButton * vm_button = xlet->findChild<QPushButton*>(QString("voicemail_button"));
    QIcon vm_icon = vm_button->icon();
    this->assert(!vm_icon.isNull(), "no vm icon");
}

void RemoteControl::then_the_xlet_identity_shows_an_agent_1(const QVariantList &args)
{
    QString expected_agent_number = args[0].toString();

    IdentityDisplay *xlet = static_cast<IdentityDisplay*>(m_exec_obj.win->m_xletlist.value("identity"));
    this->assert(xlet != NULL, "xlet null");

    QLabel *agent_number_label = xlet->findChild<QLabel*>(QString("agent_number"));
    this->assert(agent_number_label->text().endsWith(expected_agent_number), QString("could not match agent num %1").arg(agent_number_label->text()));
}

void RemoteControl::then_the_xlet_identity_does_not_show_any_agent()
{
    IdentityDisplay *xlet = static_cast<IdentityDisplay*>(m_exec_obj.win->m_xletlist.value("identity"));
    this->assert(xlet != 0);

    QLabel *agent_number_label = xlet->findChild<QLabel*>(QString("agent_number"));
    this->assert(agent_number_label->text().isEmpty());
}

#endif

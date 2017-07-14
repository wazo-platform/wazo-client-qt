/*
 * Copyright 2007-2017 The Wazo Authors  (see the AUTHORS file)
 *
 * This program is free software: you can redistribute it and/or modify
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
 * This programm is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifdef FUNCTESTS

#include "xlets/identity/identity.h"

#include "remote_control.h"

QVariantMap RemoteControl::get_identity_infos()
{
    QVariantMap args;
    IdentityDisplay *xlet = this->get_xlet<IdentityDisplay>("identity");
    if (xlet == NULL)
        return args;

    QLabel *fullname = xlet->findChild<QLabel*>(QString("name"));
    QLabel *vm_num = xlet->findChild<QLabel*>(QString("voicemail_number"));
    QPushButton * vm_button = xlet->findChild<QPushButton*>(QString("voicemail_button"));
    QToolButton * agent_button = xlet->findChild<QToolButton*>("agent_button");

    args["fullname"] = fullname->text();
    args["voicemail_num"] = vm_num->text();
    args["voicemail_button"] = vm_button->isVisible();
    args["agent_button"] = agent_button->isVisible();

    return args;
}

void RemoteControl::dial(const QVariantList &args)
{
    QString extension = args[0].toString();

    IdentityDisplay *xlet = this->get_xlet<IdentityDisplay>("identity");
    QLineEdit *dial_input = xlet->findChild<QLineEdit *>("dial_input");
    QToolButton *button = xlet->findChild<QToolButton *>("dial_search_button");
    dial_input->setText(extension);
    button->click();
}
#endif

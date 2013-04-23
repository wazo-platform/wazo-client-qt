/* XiVO Client
 * Copyright (C) 2013, Avencall
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

#include "mainwidget.h"
#include "remote_control.h"

QVariantMap RemoteControl::get_login_screen_infos()
{
    QVariantMap args;

    args["login"] = m_exec_obj.win->m_qlab1->text();
    args["password"] = m_exec_obj.win->m_qlab2->text();


    args["show_agent_option"] = true;
    if (m_exec_obj.win->m_lab3->isHidden() && m_exec_obj.win->m_qlab3->isHidden() && m_exec_obj.win->m_loginkind->isHidden()) {
        args["show_agent_option"] = false;
    }

    args["agentphonenumber"] = m_exec_obj.win->m_qlab3->text();
    int current_index = m_exec_obj.win->m_loginkind->currentIndex();
    QString current_agent_option;
    if(current_index == 0)
        current_agent_option = "no";
    if(current_index == 1)
        current_agent_option = "unlogged";
    if(current_index == 2)
        current_agent_option = "logged";

    args["agent_option"] = current_agent_option;


    return args;
}

#endif

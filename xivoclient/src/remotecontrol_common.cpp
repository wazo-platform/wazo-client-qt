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

#include "configwidget.h"
#include "remotecontrol.h"
#include <baseengine.h>

void RemoteControl::i_go_to_the_xivo_client_configuration()
{
    m_exec_obj.win->m_cfgact->trigger();
}

void RemoteControl::i_close_the_xivo_client_configuration()
{
    m_exec_obj.win->m_configwindow->m_btnbox->button(QDialogButtonBox::Ok)->click();
}

void RemoteControl::i_log_in_the_xivo_client_to_host_1_as_2_pass_3(const QStringList &args)
{
    i_go_to_the_xivo_client_configuration();

    m_exec_obj.win->m_configwindow->m_tabwidget->setCurrentIndex(0);
    m_exec_obj.win->m_configwindow->m_main_server_address_input->setText(args[0]);

    m_exec_obj.win->m_configwindow->m_tabwidget->setCurrentIndex(1);
    m_exec_obj.win->m_configwindow->m_userid->setText(args[1]);
    m_exec_obj.win->m_configwindow->m_password->setText(args[2]);

    i_close_the_xivo_client_configuration();
    m_exec_obj.win->m_ack->click();
    pause(1000);
}

void RemoteControl::i_log_in_the_xivo_client_to_host_1_as_2_pass_3_unlogged_agent(const QStringList &args)
{
    i_go_to_the_xivo_client_configuration();

    m_exec_obj.win->m_configwindow->m_tabwidget->setCurrentIndex(0);
    m_exec_obj.win->m_configwindow->m_main_server_address_input->setText(args[0]);

    m_exec_obj.win->m_configwindow->m_tabwidget->setCurrentIndex(1);
    m_exec_obj.win->m_configwindow->m_userid->setText(args[1]);
    m_exec_obj.win->m_configwindow->m_password->setText(args[2]);

    m_exec_obj.win->m_configwindow->m_loginkind->setCurrentIndex(1);

    i_close_the_xivo_client_configuration();
    m_exec_obj.win->m_ack->click();
    pause(1000);
}

void RemoteControl::i_log_out_of_the_xivo_client()
{
    if (m_exec_obj.win->m_configwindow != NULL) {
        m_exec_obj.win->m_configwindow->close();
    }
    m_exec_obj.win->m_disconnectact->trigger();
}

void RemoteControl::i_stop_the_xivo_client()
{
    if (m_exec_obj.win->m_configwindow != NULL) {
        m_exec_obj.win->m_configwindow->close();
    }
    m_exec_obj.win->m_quitact->trigger();
}

void RemoteControl::when_i_enable_screen_pop_up()
{
    i_go_to_the_xivo_client_configuration();

    m_exec_obj.win->m_configwindow->m_tabwidget->setCurrentIndex(3);

    QCheckBox *customer_info_box = m_exec_obj.win->m_configwindow->findChild<QCheckBox*>(QString("enable_customer_info"));
    customer_info_box->setChecked(true);
    i_close_the_xivo_client_configuration();
}


#endif

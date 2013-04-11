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

#include <baseengine.h>
#include <configwidget.h>

#include "remote_control.h"

void RemoteControl::create_signals()
{
    connect(this, SIGNAL(select_queue(const QString &)),
            b_engine, SIGNAL(changeWatchedQueueSignal(const QString &)));
}

void RemoteControl::i_go_to_the_xivo_client_configuration()
{
    m_exec_obj.win->m_cfgact->trigger();
}

void RemoteControl::i_close_the_xivo_client_configuration()
{
    m_exec_obj.win->m_configwindow->m_btnbox->button(QDialogButtonBox::Ok)->click();
}

void RemoteControl::configure(const QVariantList &list)
{
    QVariantMap args = list[0].toMap();

    const QString & xivo_address = args["main_server_address"].toString();
    int xivo_port = args["main_server_port"].toInt();
    const QString & login = args["login"].toString();
    const QString & password = args["password"].toString();
    const QString & agent_option = args["agent_option"].toString();
    bool customerinfo = args["customerinfo"].toBool();
    bool show_agent_option = args["show_agent_option"].toBool();

    i_go_to_the_xivo_client_configuration();

    m_exec_obj.win->m_configwindow->m_tabwidget->setCurrentIndex(0);
    m_exec_obj.win->m_configwindow->m_main_server_address_input->setText(xivo_address);
    m_exec_obj.win->m_configwindow->m_main_server_port_input->setValue(xivo_port);


     m_exec_obj.win->m_configwindow->m_tabwidget->setCurrentIndex(1);
     m_exec_obj.win->m_configwindow->m_userid->setText(login);
     m_exec_obj.win->m_configwindow->m_password->setText(password);

     if(show_agent_option)
         m_exec_obj.win->m_configwindow->m_hide_unlogged_agents->setChecked(true);
     else
         m_exec_obj.win->m_configwindow->m_hide_unlogged_agents->setChecked(false);


    if(agent_option == "no")
        m_exec_obj.win->m_configwindow->m_loginkind->setCurrentIndex(0);
    if(agent_option == "unlogged")
        m_exec_obj.win->m_configwindow->m_loginkind->setCurrentIndex(1);
    if(agent_option == "logged")
        m_exec_obj.win->m_configwindow->m_loginkind->setCurrentIndex(2);


    m_exec_obj.win->m_configwindow->m_tabwidget->setCurrentIndex(3);
    QCheckBox *customer_info_box = m_exec_obj.win->m_configwindow->findChild<QCheckBox*>(QString("enable_customer_info"));
    customer_info_box->setChecked(customerinfo);

    i_close_the_xivo_client_configuration();
}

void RemoteControl::i_log_in_the_xivo_client_to_host_1_as_2_pass_3(const QVariantList &args)
{
    const QString & xivo_address = args[0].toString();
    const QString & login = args[1].toString();
    const QString & password = args[2].toString();

    i_go_to_the_xivo_client_configuration();

    m_exec_obj.win->m_configwindow->m_tabwidget->setCurrentIndex(0);
    m_exec_obj.win->m_configwindow->m_main_server_address_input->setText(xivo_address);

    m_exec_obj.win->m_configwindow->m_tabwidget->setCurrentIndex(1);
    m_exec_obj.win->m_configwindow->m_userid->setText(login);
    m_exec_obj.win->m_configwindow->m_password->setText(password);

    i_close_the_xivo_client_configuration();
    m_exec_obj.win->m_ack->click();
    pause(1000);
}

void RemoteControl::i_log_in_the_xivo_client_to_host_1_as_2_pass_3_unlogged_agent(const QVariantList &args)
{
    const QString & xivo_address = args[0].toString();
    const QString & login = args[1].toString();
    const QString & password = args[2].toString();

    i_go_to_the_xivo_client_configuration();

    m_exec_obj.win->m_configwindow->m_tabwidget->setCurrentIndex(0);
    m_exec_obj.win->m_configwindow->m_main_server_address_input->setText(xivo_address);

    m_exec_obj.win->m_configwindow->m_tabwidget->setCurrentIndex(1);
    m_exec_obj.win->m_configwindow->m_userid->setText(login);
    m_exec_obj.win->m_configwindow->m_password->setText(password);

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


#endif

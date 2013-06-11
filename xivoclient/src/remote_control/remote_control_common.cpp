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
#include <config_widget/config_widget.h>
#include <login_widget/login_widget.h>

#include "remote_control.h"


QVariantMap RemoteControl::is_logged()
{
    QVariantMap args;

    args["logged"] =  (b_engine->state() == b_engine->ELogged);

    return args;
}

void RemoteControl::i_go_to_the_xivo_client_configuration()
{
    this->m_exec_obj.win->ui->action_configure->trigger();
}

void RemoteControl::i_close_the_xivo_client_configuration()
{
    this->m_exec_obj.win->m_config_widget->ui.buttonBox->button(QDialogButtonBox::Ok)->click();
}

QVariantMap RemoteControl::get_status_bar_infos()
{
    QVariantMap args;

    args["profilename_value"] = this->m_statusbar->m_config_profile->text();
    args["profilename_is_hidden"] = this->m_statusbar->m_config_profile->isHidden();

    return args;
}

QVariantMap RemoteControl::get_configuration()
{
    return b_engine->getConfig();
}

void RemoteControl::configure(const QVariantList &list)
{
    QVariantMap args = list[0].toMap();
    this->configureLoginWidget(args);
    this->configureConfigDialog(args);
}

void RemoteControl::configureLoginWidget(const QVariantMap &args)
{
    if (args.find("login") != args.end()) {
        QString login = args["login"].toString();
        this->m_login_widget->ui.userlogin->setText(login);
    }

    if (args.find("password") != args.end()) {
        QString password = args["password"].toString();
        this->m_login_widget->ui.password->setText(password);
    }

    if (args.find("keep_password") != args.end()) {
        bool is_keep_password = args["keep_password"].toBool();
        this->m_login_widget->ui.keep_password->setChecked(is_keep_password);
    }

    if (args.find("agent_option") != args.end()) {
        QString agent_option = args["agent_option"].toString();
        if(agent_option == "no") {
            this->m_login_widget->ui.agent_options->setCurrentIndex(0);
        } else if(agent_option == "unlogged") {
            this->m_login_widget->ui.agent_options->setCurrentIndex(1);
        } else if(agent_option == "logged") {
            this->m_login_widget->ui.agent_options->setCurrentIndex(2);
        }
    }
}

void RemoteControl::configureConfigDialog(const QVariantMap &args)
{
    i_go_to_the_xivo_client_configuration();

    if (args.find("main_server_address") != args.end()) {
        QString xivo_address = args["main_server_address"].toString();
        this->m_exec_obj.win->m_config_widget->ui.server->setText(xivo_address);
    }

    if (args.find("main_server_port") != args.end()) {
        int xivo_port = args["main_server_port"].toInt();
        this->m_exec_obj.win->m_config_widget->ui.port->setValue(xivo_port);
    }

    if (args.find("main_server_encrypted") != args.end()) {
        bool is_main_server_encrypted = args["main_server_encrypted"].toInt();
        this->m_exec_obj.win->m_config_widget->ui.encrypted->setChecked(is_main_server_encrypted);
    }

    if (args.find("backup_server_address") != args.end()) {
        QString backup_server_address = args["backup_server_address"].toString();
        this->m_exec_obj.win->m_config_widget->ui.backup_server->setText(backup_server_address);
    }

    if (args.find("backup_server_port") != args.end()) {
        int backup_server_port = args["backup_server_port"].toInt();
        this->m_exec_obj.win->m_config_widget->ui.backup_port->setValue(backup_server_port);
    }

    if (args.find("backup_server_encrypted") != args.end()) {
        bool is_backup_server_encrypted = args["backup_server_encrypted"].toInt();
        this->m_exec_obj.win->m_config_widget->ui.backup_encrypted->setChecked(is_backup_server_encrypted);
    }

    if (args.find("autoconnect") != args.end()) {
        bool is_autoconnect = args["autoconnect"].toBool();
        this->m_exec_obj.win->m_config_widget->ui.startup_connect->setChecked(is_autoconnect);
    }

    if (args.find("enable_auto_reconnect") != args.end()) {
        bool is_enable_auto_reconnect = args["enable_auto_reconnect"].toBool();
        this->m_exec_obj.win->m_config_widget->ui.auto_reconnect->setChecked(is_enable_auto_reconnect);
    }

    if (args.find("auto_reconnect_interval") != args.end()) {
        int interval = args["auto_reconnect_interval"].toInt();
        this->m_exec_obj.win->m_config_widget->ui.auto_reconnect_interval->setValue(interval);
    }

    if (args.find("show_agent_option") != args.end()) {
        bool is_show_agent_option = args["show_agent_option"].toBool();
        this->m_exec_obj.win->m_config_widget->ui.show_agent_options->setChecked(is_show_agent_option);
    }

    if (args.find("display_profile") != args.end()) {
        bool display_profile = args["display_profile"].toBool();
        this->m_exec_obj.win->m_config_widget->ui.show_displayprofile->setChecked(display_profile);
    }

    if (args.find("enable_presence_reporting") != args.end()) {
        bool is_enable_presence_reporting = args["enable_presence_reporting"].toBool();
        this->m_exec_obj.win->m_config_widget->ui.enable_presence_reporting->setChecked(is_enable_presence_reporting);
    }

    if (args.find("enable_start_systrayed") != args.end()) {
        bool is_enable_start_systrayed = args["enable_start_systrayed"].toBool();
        this->m_exec_obj.win->m_config_widget->ui.start_systrayed->setChecked(is_enable_start_systrayed);
    }

    if (args.find("enable_screen_popup") != args.end()) {
        bool is_enable_screen_popup = args["enable_screen_popup"].toBool();
        this->m_exec_obj.win->m_config_widget->ui.enable_screen_popup->setChecked(is_enable_screen_popup);
    }

    if (args.find("hide_unlogged_agents_for_xlet_queue_members") != args.end()) {
        bool is_hide = args["hide_unlogged_agents_for_xlet_queue_members"].toBool();
        this->m_exec_obj.win->m_config_widget->ui.hide_unlogged_agents->setChecked(is_hide);
    }

    if (args.find("enable_multiple_instances") != args.end()) {
        bool state = args["enable_multiple_instances"].toBool();
        this->m_exec_obj.win->m_config_widget->ui.allow_multiple_instances->setChecked(state);
    }

    i_close_the_xivo_client_configuration();
}

void RemoteControl::i_log_in_the_xivo_client()
{
    this->m_login_widget->ui.buttonBox->click();
    pause(1000);
}

void RemoteControl::i_log_out_of_the_xivo_client()
{
    if (this->m_exec_obj.win != NULL) {
        this->m_exec_obj.win->close();
    }
    this->m_exec_obj.win->ui->action_disconnect->trigger();
}

void RemoteControl::i_stop_the_xivo_client()
{
    if (this->m_exec_obj.win != NULL) {
        this->m_exec_obj.win->close();
    }
    this->m_exec_obj.win->ui->action_quit->trigger();
}

QString RemoteControl::getValueInModel(QAbstractItemModel* model, int row, int column)
{
    QModelIndex index = model->index(row, column);
    QVariant data = model->data(index, Qt::DisplayRole);
    return data.toString();
}

QString RemoteControl::getHeaderValueInModel(QAbstractItemModel* model, int section)
{
    QVariant header_data = model->headerData(section, Qt::Horizontal);
    return header_data.toString();
}

QString RemoteControl::prettyPrintMap(QVariantMap map)
{
    QString prettyprint;
    QDebug debug(&prettyprint);
    debug << map;
    return prettyprint;
}

#endif

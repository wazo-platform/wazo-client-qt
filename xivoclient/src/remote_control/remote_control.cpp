/* XiVO Client
 * Copyright (C) 2007-2014 Avencall
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

#include <QLocalServer>
#include <QLocalSocket>
#include <QFile>
#include <JsonToVariant.h>
#include <VariantToJson.h>

#include "remote_control.h"

TestFailedException::TestFailedException(const QString & message)
{
    this->message = message;
}

TestFailedException::~TestFailedException() throw()
{
}

const char* TestFailedException::what() const throw()
{
    return this->message.toUtf8().data();
}

RemoteControl::RemoteControl(ExecObjects exec_obj, QString &socket)
    : m_exec_obj(exec_obj),
      m_command_found(false),
      m_no_error(true),
      m_socket_name(socket),
      m_server(new QLocalServer),
      m_client_cnx(NULL),
      m_main_window(assembler->mainWindow()),
      m_menu_availability(assembler->menuAvailability()),
      m_login_widget(assembler->loginWidget()),
      m_central_widget(assembler->centralWidget()),
      m_main_widget(assembler->mainWidget()),
      m_statusbar(assembler->statusbar()),
      m_xlet_dispatcher(assembler->xletDispatcher()),
      m_system_tray_icon(assembler->systemTrayIcon())
{
    this->connect(this->m_server, SIGNAL(newConnection()), SLOT(newConnection()));

    QFile::remove(m_socket_name);
    this->m_server->listen(m_socket_name);

    if (! this->m_server->isListening()) {
        qDebug() << "No more sockets available for remote control";
    }

    this->connect(this->m_exec_obj.baseengine, SIGNAL(emitTextMessage(const QString &)), SLOT(on_error(const QString &)));
    disconnect(this->m_exec_obj.baseengine, SIGNAL(emitMessageBox(const QString &)), this->m_exec_obj.win, SLOT(showMessageBox(const QString &)));
}

RemoteControl::~RemoteControl()
{
    this->m_server->close();
}

void RemoteControl::newConnection()
{
    this->m_client_cnx = m_server->nextPendingConnection();
    this->connect(this->m_client_cnx, SIGNAL(readyRead()), SLOT(processCommands()));
}

#define RC_EXECUTE(fct_name) { \
    if (this->commandMatches(command, #fct_name)) \
        fct_name(); \
    }
#define RC_EXECUTE_ARG(fct_name) { \
    if (this->commandMatches(command, #fct_name)) \
        fct_name(command.arguments); \
    }
#define RC_EXECUTE_WITH_RETURN(fct_name) { \
    if (this->commandMatches(command, #fct_name)) \
        return_value = fct_name(); \
    }

bool RemoteControl::commandMatches(RemoteControlCommand command, std::string function_name)
{
    if (command.action == function_name.c_str()) {
        this->m_command_found = true;
        return true;
    }
    return false;
}

void RemoteControl::processCommands()
{
    while (this->m_client_cnx->canReadLine()) {
        QByteArray raw_command = this->m_client_cnx->readLine();
        RemoteControlCommand command = this->parseCommand(raw_command);

        this->m_no_error = true;
        this->m_command_found = false;

        QVariantMap return_value;

        try {
            RC_EXECUTE(i_stop_the_xivo_client);
            RC_EXECUTE(i_log_in_the_xivo_client);
            RC_EXECUTE(i_log_out_of_the_xivo_client);
            RC_EXECUTE(i_go_to_the_xivo_client_configuration);
            RC_EXECUTE(i_close_the_xivo_client_configuration);
            RC_EXECUTE_ARG(configure);
            RC_EXECUTE_WITH_RETURN(get_configuration);
            RC_EXECUTE_WITH_RETURN(get_login_screen_infos);
            RC_EXECUTE_WITH_RETURN(get_status_bar_infos);
            RC_EXECUTE_WITH_RETURN(get_xlets);
            RC_EXECUTE_WITH_RETURN(get_identity_infos);
            RC_EXECUTE_WITH_RETURN(get_queue_members_infos);
            RC_EXECUTE_WITH_RETURN(get_menu_availability_infos);
            RC_EXECUTE_ARG(set_queue_for_queue_members);
            RC_EXECUTE_WITH_RETURN(get_sheet_infos);
            RC_EXECUTE_WITH_RETURN(get_infos_in_custom_sheet);
            RC_EXECUTE_WITH_RETURN(get_conference_room_infos);
            RC_EXECUTE_WITH_RETURN(get_switchboard_infos);
            RC_EXECUTE_ARG(set_search_for_directory);
            RC_EXECUTE_WITH_RETURN(get_remote_directory_infos);
            RC_EXECUTE_ARG(set_search_for_remote_directory);
            RC_EXECUTE_ARG(exec_double_click_on_number_for_name);

            RC_EXECUTE_WITH_RETURN(get_agent_list_infos);
            RC_EXECUTE_WITH_RETURN(get_main_window_infos);
            RC_EXECUTE_WITH_RETURN(is_logged);

            if (this->m_no_error == false) {
                this->sendResponse(TEST_FAILED, command.action, "", return_value);
            } else if (this->m_command_found) {
                this->sendResponse(TEST_PASSED, command.action, "", return_value);
            } else {
                this->sendResponse(TEST_UNKNOWN, command.action);
            }
        } catch (TestFailedException & e) {
            this->sendResponse(TEST_FAILED, command.action, e.what());
        }
    }
}

RemoteControlCommand RemoteControl::parseCommand(const QByteArray & raw_command)
{
    QString encoded_command = QString::fromUtf8(raw_command);

    // remove trailing newline
    encoded_command.chop(1);

    QVariant decoded_command_plain;
    try {
        decoded_command_plain = JsonQt::JsonToVariant::parse(encoded_command);
    } catch(const JsonQt::ParseException &) {
        qDebug() << "Failed to decode args";
    }

    QVariantMap decoded_command_map = decoded_command_plain.toMap();

    RemoteControlCommand return_command;
    return_command.action = decoded_command_map["function_name"].toString();
    return_command.arguments = decoded_command_map["arguments"].toList();
    return return_command;
}

void RemoteControl::sendResponse(RemoteControlResponse test_result,
                                 QString command,
                                 QString message,
                                 QVariantMap return_value)
{
    QVariantMap response;

    switch (test_result) {
        case TEST_FAILED:
            qDebug() << "command failed" << command << ":" << message;
            response["test_result"] = "failed";
            break;
        case TEST_UNKNOWN:
            qDebug() << "command unknown" << command;
            response["test_result"] = "unknown";
            break;
        case TEST_PASSED:
            qDebug() << "command succeeded" << command;
            response["test_result"] = "passed";
            break;
    }

    response["command"] = command;
    response["message"] = message;
    response["return_value"] = return_value;

    QString encoded_command = JsonQt::VariantToJson::parse(response) + '\n';

    this->m_client_cnx->write(encoded_command.toUtf8().data());
    this->m_client_cnx->flush();
}

void RemoteControl::on_error(const QString &error_string)
{
    qDebug() << Q_FUNC_INFO << error_string;
    this->m_no_error = false ;
}

void RemoteControl::pause(unsigned millisec)
{
    QEventLoop loop;
    QTimer timer;

    timer.setSingleShot(true);
    connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));

    timer.start(millisec);
    loop.exec();
}

void RemoteControl::assert(bool condition, const QString& message)
{
    if (!condition) {
        throw TestFailedException(message);
    }
}


#endif

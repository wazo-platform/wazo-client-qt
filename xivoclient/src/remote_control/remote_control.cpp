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

#include <QLocalServer>
#include <QLocalSocket>
#include <QFile>
#include <JsonToVariant.h>
#include <VariantToJson.h>

#include <QDebug>

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

RemoteControl::RemoteControl(ExecObjects exec_obj)
    : m_no_error(true)
{
    m_exec_obj = exec_obj;
    m_server = new QLocalServer;
    connect(m_server, SIGNAL(newConnection()),
            this, SLOT(newConnection()));

    m_socket_name = "/tmp/xivoclient";
    QFile::remove(m_socket_name);
    m_server->listen(m_socket_name);

    if (! m_server->isListening()) {
        qDebug() << "No more sockets available for remote control";
    }

    connect(m_exec_obj.baseengine, SIGNAL(emitTextMessage(const QString &)),
            this, SLOT(on_error(const QString &)));
    disconnect(m_exec_obj.baseengine, SIGNAL(emitMessageBox(const QString &)),
               m_exec_obj.win, SLOT(showMessageBox(const QString &)));

    this->create_signals();
}

RemoteControl::~RemoteControl()
{
    m_server->close();
}

void RemoteControl::newConnection()
{
    m_client_cnx = m_server->nextPendingConnection();
    connect(m_client_cnx, SIGNAL(readyRead()),
            this, SLOT(processCommands()));
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
    while (m_client_cnx->canReadLine()) {
        QByteArray raw_command = m_client_cnx->readLine();
        RemoteControlCommand command = this->parseCommand(raw_command);

        m_no_error = true;
        m_command_found = false;

        QVariantMap return_value;

        try {
            RC_EXECUTE(i_stop_the_xivo_client);

            RC_EXECUTE(i_go_to_the_xivo_client_configuration);
            RC_EXECUTE(i_close_the_xivo_client_configuration);
            RC_EXECUTE_ARG(configure);
            RC_EXECUTE_WITH_RETURN(get_identity_infos);

            RC_EXECUTE_ARG(i_log_in_the_xivo_client_to_host_1_as_2_pass_3);
            RC_EXECUTE_ARG(i_log_in_the_xivo_client_to_host_1_as_2_pass_3_unlogged_agent);
            RC_EXECUTE(i_log_out_of_the_xivo_client);

            RC_EXECUTE(when_i_enable_the_hide_unlogged_agents_option);
            RC_EXECUTE(when_i_disable_the_hide_unlogged_agents_option);
            RC_EXECUTE(then_the_queue_members_xlet_is_empty);
            RC_EXECUTE_ARG(then_the_queue_members_xlet_for_queue_1_is_empty_);
            RC_EXECUTE_ARG(then_the_queue_members_xlet_for_queue_1_displays_agents);

            RC_EXECUTE(when_i_enable_screen_pop_up);
            RC_EXECUTE_ARG(then_i_see_a_sheet_with_variables_and_values);
            RC_EXECUTE(then_i_should_not_see_any_sheet);

            RC_EXECUTE_ARG(when_i_search_for_1_in_the_directory_xlet);
            RC_EXECUTE(then_nothing_shows_up_in_the_directory_xlet);
            RC_EXECUTE_ARG(then_1_shows_up_in_the_directory_xlet);
            RC_EXECUTE_ARG(then_1_does_not_show_up_in_the_directory_xlet);
            RC_EXECUTE_ARG(assert_row_shows_up_in_the_directory_xlet);
            RC_EXECUTE_ARG(when_i_double_click_on_the_phone_number_for_name);

            RC_EXECUTE_ARG(assert_conference_room_1_has_number_2_in_xlet);
            RC_EXECUTE_ARG(assert_conference_room_1_has_pin_code_2_in_xlet);

            RC_EXECUTE_ARG(when_i_search_a_transfer_destination_1);
            RC_EXECUTE_ARG(assert_directory_has_entry);
            RC_EXECUTE(then_i_see_no_transfer_destinations);

            if (this->m_no_error == false) {
                this->sendResponse(TEST_FAILED, "", return_value);
            } else if (this->m_command_found) {
                this->sendResponse(TEST_PASSED, "", return_value);
            } else {
                this->sendResponse(TEST_UNKNOWN, command.action);
            }
        } catch (TestFailedException & e) {
            this->sendResponse(TEST_FAILED, e.what());
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
    } catch(JsonQt::ParseException) {
        qDebug() << "Failed to decode args";
        throw;
    }

    QVariantMap decoded_command_map = decoded_command_plain.toMap();

    RemoteControlCommand return_command;
    return_command.action = decoded_command_map["function_name"].toString();
    return_command.arguments = decoded_command_map["arguments"].toList();
    return return_command;
}

void RemoteControl::sendResponse(RemoteControlResponse test_result, QString message, QVariantMap return_value)
{
    QVariantMap response;

    switch (test_result) {
        case TEST_FAILED:
            response["test_result"] = "failed";
            break;
        case TEST_UNKNOWN:
            response["test_result"] = "unknown";
            break;
        case TEST_PASSED:
            response["test_result"] = "passed";
            break;
    }

    response["message"] = message;
    response["return_value"] = return_value;

    QString encoded_command = JsonQt::VariantToJson::parse(response);

    m_client_cnx->write(encoded_command.toUtf8().data());
    m_client_cnx->flush();
}

void RemoteControl::on_error(const QString &error_string)
{
    qDebug() << Q_FUNC_INFO << error_string;
    m_no_error = false ;
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

void RemoteControl::assert(bool condition, const QString & message)
{
    if (!condition) {
        throw TestFailedException(message);
    }
}


#endif

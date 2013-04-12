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

#ifndef __REMOTE_CONTROL_H__
#define __REMOTE_CONTROL_H__

#ifdef FUNCTESTS

#include <QObject>
#include <QAbstractItemModel>
#include <string>
#include <exception>

#include <main.h>

#include "xlets/conference/conflist.h"

using namespace std;

class QLocalServer;
class QLocalSocket;

struct RemoteControlCommand {
    QString action;
    QVariantList arguments;
};

enum RemoteControlResponse {
    TEST_FAILED,
    TEST_UNKNOWN,
    TEST_PASSED
};

class TestFailedException : public exception {
    public:
        TestFailedException(const QString & message = "");
        ~TestFailedException() throw();

        virtual const char* what() const throw();

    private:
        QString message;
};

class RemoteControl : public QObject
{
    Q_OBJECT

    public:
        RemoteControl(ExecObjects);
        void pause(unsigned);
        ~RemoteControl();

    public:
        void i_stop_the_xivo_client();

        void i_go_to_the_xivo_client_configuration();
        void i_close_the_xivo_client_configuration();
        void configure(const QVariantList &);

        void i_log_in_the_xivo_client();
        void i_log_out_of_the_xivo_client();

        QVariantMap get_identity_infos();
        QVariantMap get_queue_members_infos();
        void set_queue_for_queue_members(const QVariantList &);

        void then_i_see_a_sheet_with_variables_and_values(const QVariantList &);
        void then_i_should_not_see_any_sheet();

        void when_i_search_for_1_in_the_directory_xlet(const QVariantList &);
        void then_nothing_shows_up_in_the_directory_xlet();
        void then_1_shows_up_in_the_directory_xlet(const QVariantList &);
        void then_1_does_not_show_up_in_the_directory_xlet(const QVariantList &);
        void assert_row_shows_up_in_the_directory_xlet(const QVariantList &);

        void assert_conference_room_1_has_number_2_in_xlet(const QVariantList &);
        void assert_conference_room_1_has_pin_code_2_in_xlet(const QVariantList &);

        void when_i_search_a_transfer_destination_1(const QVariantList &);
        void assert_directory_has_entry(const QVariantList &);
        void then_i_see_no_transfer_destinations();
        void when_i_double_click_on_the_phone_number_for_name(const QVariantList &);

    signals:
        void select_queue(const QString & queue_id);

    private:
        void create_signals();
        bool _directory_row_matches(QAbstractItemModel* model,
                                    int row_index,
                                    QVariantMap entry);

    public slots:
        void on_error(const QString &);

    private slots:
        void newConnection();
        void processCommands();

    private:
        RemoteControlCommand parseCommand(const QByteArray & raw_command);
        void sendResponse(RemoteControlResponse response,
              QString command,
              QString message = "",
              QVariantMap return_value = QVariantMap());
        void assert(bool condition, const QString & message = "");
        bool commandMatches(RemoteControlCommand, std::string);

        void assertValueInConferenceXlet(QString roomName, int column, QString value);
        QAbstractItemModel* getConflistModel();
        int findRowWithItem(QAbstractItemModel* model, int column, QString search);
        QString getValueInModel(QAbstractItemModel* model, int row, int column);
        int findColumnForHeader(QAbstractItemModel *model, QString header);
        QString prettyPrintMap(QVariantMap map);

        ExecObjects m_exec_obj;
        bool m_command_found;
        bool m_no_error;

        QString m_socket_name;
        QLocalServer *m_server;
        QLocalSocket *m_client_cnx;
};

#endif /* ifdef FUNCTESTS */

#endif /* ifndef __REMOTE_CONTROL_H__ */

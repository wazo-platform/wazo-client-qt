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

#ifndef __REMOTE_CONTROL_H__
#define __REMOTE_CONTROL_H__

#ifdef FUNCTESTS

#include <QObject>
#include <QAbstractItemModel>
#include <string>
#include <exception>

#include <main.h>
#include <assembler.h>
#include <login_widget/login_widget.h>
#include <config_widget/config_widget.h>
#include <main_window/main_widget.h>
#include <main_window/central_widget.h>
#include <main_window/statusbar.h>
#include <xlets/conference/conflist.h>
#include <xletlib/extendedtablewidget.h>

using namespace std;

class QLocalServer;
class QLocalSocket;
class XLet;

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
        RemoteControl(ExecObjects, QString &socket);
        ~RemoteControl();

    signals:
        void select_queue(const QString & queue_id);
        void itemDoubleClicked(QTableWidgetItem*);

    public slots:
        void on_error(const QString &);

    private slots:
        void newConnection();
        void processCommands();

    private:
        RemoteControlCommand parseCommand(QByteArray raw_command);
        void sendResponse(RemoteControlResponse response,
              QString command,
              QString message = "",
              QVariantMap return_value = QVariantMap());
        bool commandMatches(RemoteControlCommand, std::string);
        void assert(bool condition, const QString& message);

        void assertValueInConferenceXlet(QString roomName, int column, QString value);
        QAbstractItemModel* getConflistModel();
        QAbstractItemModel* getAgentListModel();
        QString getStatusForAgent(QString);

        int findRowWithItem(QAbstractItemModel* model, int column, QString search);
        QString getValueInModel(QAbstractItemModel* model, int row, int column);
        QString getHeaderValueInModel(QAbstractItemModel* model, int section);
        QString prettyPrintMap(QVariantMap map);

        void i_stop_the_xivo_client();
        void i_go_to_the_xivo_client_configuration();
        void i_close_the_xivo_client_configuration();
        void configure(const QVariantList &);
        void configureLoginWidget(const QVariantMap &args);
        void configureConfigDialog(const QVariantMap &args);
        void i_log_in_the_xivo_client();
        void i_log_out_of_the_xivo_client();
        void pause(unsigned);
        void pressEnter(QObject *receiver);
        QVariantMap get_configuration();
        QVariantMap get_login_screen_infos();
        QVariantMap get_status_bar_infos();
        QVariantMap get_identity_infos();
        QVariantMap get_queue_members_infos();
        void set_queue_for_queue_members(const QVariantList &);
        QVariantMap get_sheet_infos();
        QVariantMap get_infos_in_custom_sheet();
        void set_infos_in_custom_sheet(const QVariantList &);
        QVariantMap get_conference_room_infos();
        QVariantMap get_switchboard_infos();
        void set_search_for_directory(const QVariantList &);
        QVariantMap get_remote_directory_infos();
        void set_search_for_remote_directory(const QVariantList &);
        void exec_double_click_on_number_for_name(const QVariantList &);
        void sort_list_for_remote_directory(const QVariantList &);
        QVariantMap get_agent_list_infos();
        QVariantMap get_menu_availability_infos();
        QVariantMap get_main_window_infos();
        QVariantMap is_logged();
        void switchboard_answer_incoming_call(const QVariantList &);
        void switchboard_hang_up();
        QVariantMap get_switchboard_current_call_infos();
        QVariantMap get_switchboard_incoming_calls_infos();
        void dial(const QVariantList &);
        QWidget *_get_current_sheet();

        //Xlets
        template <class T>
        T* get_xlet(const QString &xlet_name);
        QVariantMap get_xlets();

        ExecObjects m_exec_obj;
        bool m_command_found;
        bool m_no_error;

        QString m_socket_name;
        QLocalServer *m_server;
        QLocalSocket *m_client_cnx;

        MainWindow *m_main_window;
        MenuAvailability *m_menu_availability;
        LoginWidget *m_login_widget;
        CentralWidget *m_central_widget;
        MainWidget *m_main_widget;
        Statusbar *m_statusbar;
        XletDispatcher *m_xlet_dispatcher;
        SystemTrayIcon *m_system_tray_icon;
};

template <class T>
T* RemoteControl::get_xlet(const QString &xlet_name)
{
    QHash<QString, XLet*> xlet_list = this->m_xlet_dispatcher->m_xlets;
    if (xlet_list.contains(xlet_name)) {
        return static_cast<T*>(this->m_xlet_dispatcher->m_xlets[xlet_name]);
    } else {
        QString xlet_name_list = QStringList(xlet_list.keys()).join(",");
        throw TestFailedException(QString("could not find xlet %1 in %2").arg(xlet_name, xlet_name_list));
    }
}

#endif /* ifdef FUNCTESTS */

#endif /* ifndef __REMOTE_CONTROL_H__ */

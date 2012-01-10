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

#include <cstdlib>
#include <ctime>

#include <QString>
#include <QStringList>

#include "main.h"

#include "pyxivoclient.h"

PyXiVOClient::PyXiVOClient()
{
}

void PyXiVOClient::launch(char *args)
{
    // Parse args
    QStringList arg_list = QString(args).split(" ");
    arg_list.push_front("xivoclient");
    char **argv = (char **) malloc(arg_list.size() * sizeof(char *));
    for (int i = 0 ; i < arg_list.size() ; i++) {
        argv[i] = (char *) malloc((arg_list.at(i).size() +1) * sizeof(char));
        strcpy(argv[i], arg_list.at(i).toUtf8().data());
    }

    int argc = arg_list.size();

    // m_exec_obj = init_xivoclient(argc, argv);
    // m_exec_obj.win->m_lab1->setText("bouh");
    // qDebug() << m_exec_obj.win->m_lab2->text();

    // int ret = run_xivoclient(m_exec_obj);

    // clean_xivoclient(m_exec_obj);

    m_thread = new PyXiVOClientThread(argc, argv);
    m_thread->start();

    sleep(1);
    m_thread->execObjects().win->hide();
    qDebug() << "show";
    m_thread->execObjects().win->show();

    // m_timer = new QTimer();
    // QObject::connect(m_timer, SIGNAL(timeout()),
    //                  m_thread, SLOT(updateApp()));
    // m_timer->start(100);
}

void PyXiVOClient::exit()
{
    qDebug() << "exit";
    m_thread->execObjects().win->m_quitact->trigger();

    // m_timer->stop();
    // QObject::disconnect(m_timer, SIGNAL(timeout()),
    //            m_thread, SLOT(updateApp));
    while(m_thread->isRunning()) {
        sleep(1);
    }
    delete m_thread;
}

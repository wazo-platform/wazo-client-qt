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

#include <ctime>

#include "pyxivoclientthread.h"

PyXiVOClientThread::PyXiVOClientThread(int argc, char **argv) {
    m_argc = argc;
    m_argv = argv;
}

void PyXiVOClientThread::run()
{
    m_exec_obj = init_xivoclient(m_argc, m_argv);
    m_exec_obj.win->m_lab1->setText("bouh");
    qDebug() << m_exec_obj.win->m_lab2->text();

    exec();
    // int ret = run_xivoclient(m_exec_obj);

    clean_xivoclient(m_exec_obj);

    // while(true) {
    //     qDebug() << "loop";
    //     m_exec_obj.app->syncX();
    //     m_exec_obj.app->processEvents();
    //     m_exec_obj.app->sendPostedEvents();
    //     sleep(1);
    // }
}

ExecObjects PyXiVOClientThread::execObjects()
{
    return m_exec_obj;
}

void PyXiVOClientThread::updateApp()
{
    m_exec_obj.app->processEvents();
    m_exec_obj.app->sendPostedEvents();
}

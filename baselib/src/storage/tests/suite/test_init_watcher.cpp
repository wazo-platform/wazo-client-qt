/* XiVO Client
 * Copyright (C) 2007-2012, Avencall
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

#include <QtTest/QtTest>

#include <init_watcher.h>

#include "test_init_watcher.h"

TestInitWatcher::TestInitWatcher()
{
}

void TestInitWatcher::testWatchList()
{
    QString list_name_1("superlist"), list_name_2("megalist");
    QStringList ids_1 = QStringList() << "1" << "2" << "3";
    QStringList ids_2 = QStringList() << "4" << "5" << "6";

    QHash<QString, QStringList> expected_stack;
    expected_stack.insert(list_name_1, ids_1);
    expected_stack.insert(list_name_2, ids_2);
    InitWatcher init_watcher;

    init_watcher.watchList(list_name_1, ids_1);
    init_watcher.watchList(list_name_2, ids_2);

    QCOMPARE(expected_stack, init_watcher.m_stack);
}

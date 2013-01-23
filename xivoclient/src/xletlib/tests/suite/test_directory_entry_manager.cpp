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

#include <QtTest/QtTest>
#include <QtTest/QSignalSpy>
#include <gmock/gmock.h>
#include <QColor>

#include <dao/phonedao.h>

#include "test_directory_entry_manager.h"

#include <directory_entry_manager.h>

using namespace testing;

class MockPhoneDAO: public PhoneDAO
{
    public:
        MockPhoneDAO() {}
        ~MockPhoneDAO() {}

        MOCK_CONST_METHOD1(getStatusColor, QColor(const PhoneInfo *));
        MOCK_CONST_METHOD1(getStatusName, QString(const PhoneInfo *));
        MOCK_CONST_METHOD1(getPhoneStatusConfig, QVariantMap(const PhoneInfo *));
        MOCK_CONST_METHOD1(findByIdentity, const PhoneInfo *(const QString &));
        MOCK_CONST_METHOD1(findByXId, const PhoneInfo *(const QString &));
};


void TestDirectoryEntryManager::testUpdatePhoneConfig()
{
    QString phone_xid = "123";
    MockPhoneDAO mock_phone_dao;

    DirectoryEntryManager manager(NULL, mock_phone_dao);
    QSignalSpy spy(&manager, SIGNAL(directoryEntryUpdated(int)));

    manager.updatePhoneConfig(phone_xid);

    QCOMPARE(spy.count(), 1);
    const QList<QVariant> &arguments = spy.takeFirst();
    QCOMPARE(arguments.at(0).toInt(), 0);
}

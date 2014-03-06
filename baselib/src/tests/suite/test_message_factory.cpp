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

#include <QtTest/QtTest>
#include <QVariantMap>

#include "test_message_factory.h"

#include "message_factory.h"

void TestMessageFactory::testSubscribeCurrentCalls()
{
    QVariantMap result = MessageFactory::subscribeCurrentCalls();

    QVariantMap expected;
    expected["class"] = "subscribe";
    expected["message"] = "current_calls";

    QCOMPARE(result, expected);
}

void TestMessageFactory::testAnswer()
{
    QVariantMap result = MessageFactory::answer();

    QVariantMap expected;
    expected["class"] = "answer";

    QCOMPARE(result, expected);
}

void TestMessageFactory::testDial()
{
    QString destination = "exten:xivo/1234";
    QVariantMap result = MessageFactory::dial(destination);

    QVariantMap expected;
    expected["class"] = "ipbxcommand";
    expected["command"] = "dial";
    expected["destination"] = destination;

    QCOMPARE(result, expected);
}

void TestMessageFactory::testHangup()
{
    QVariantMap result = MessageFactory::hangup();

    QVariantMap expected;
    expected["class"] = "hangup";

    QCOMPARE(result, expected);
}

void TestMessageFactory::testHoldSwitchboard()
{
    QString queue_name = "my_queue";
    QVariantMap result = MessageFactory::holdSwitchboard(queue_name);

    QVariantMap expected;
    expected["class"] = "hold_switchboard";
    expected["queue_name"] = queue_name;

    QCOMPARE(result, expected);
}

void TestMessageFactory::testResumeSwitchboard()
{
    QString unique_id = "1287634.33";
    QVariantMap result = MessageFactory::resumeSwitchboard(unique_id);

    QVariantMap expected;
    expected["class"] = "resume_switchboard";
    expected["unique_id"] = unique_id;

    QCOMPARE(result, expected);
}

void TestMessageFactory::testAttendedTransfer()
{
    QString number = "1234";
    QVariantMap result = MessageFactory::attendedTransfer(number);

    QVariantMap expected;
    expected["class"] = "attended_transfer";
    expected["number"] = number;

    QCOMPARE(result, expected);
}

void TestMessageFactory::testDirectTransfer()
{
    QString number = "5432";
    QVariantMap result = MessageFactory::directTransfer(number);

    QVariantMap expected;
    expected["class"] = "direct_transfer";
    expected["number"] = number;

    QCOMPARE(result, expected);
}

void TestMessageFactory::testCompleteTransfer()
{
    QVariantMap result = MessageFactory::completeTransfer();

    QVariantMap expected;
    expected["class"] = "complete_transfer";

    QCOMPARE(result, expected);
}

void TestMessageFactory::testCancelTransfer()
{
    QVariantMap result = MessageFactory::cancelTransfer();

    QVariantMap expected;
    expected["class"] = "cancel_transfer";

    QCOMPARE(result, expected);
}

void TestMessageFactory::testDirectorySearch()
{
    QString pattern = "al";
    QVariantMap result = MessageFactory::directorySearch(pattern);

    QVariantMap expected;
    expected["class"] = "directory";
    expected["pattern"] = pattern;

    QCOMPARE(result, expected);
}

void TestMessageFactory::testSwitchboardDirectorySearch()
{
    QString pattern = "al";
    QVariantMap result = MessageFactory::switchboardDirectorySearch(pattern);

    QVariantMap expected;
    expected["class"] = "switchboard_directory_search";
    expected["pattern"] = pattern;

    QCOMPARE(result, expected);
}

void TestMessageFactory::testGetSwitchboardDirectoryHeaders()
{
    QVariantMap result = MessageFactory::getSwitchboardDirectoryHeaders();

    QVariantMap expected;
    expected["class"] = "get_switchboard_directory_headers";

    QCOMPARE(result, expected);
}

void TestMessageFactory::testPauseAgentInQueue()
{
    QString agent_id = "xivo/12", queue_id = "xivo/13";

    QVariantMap result = MessageFactory::pauseAgentInQueue(agent_id, queue_id);

    QVariantMap expected;
    expected["class"] = "ipbxcommand";
    expected["command"] = "queuepause";
    expected["member"] = "agent:" + agent_id;
    expected["queue"] = "queue:" + queue_id;

    QCOMPARE(result, expected);
}

void TestMessageFactory::testUnpauseAgentInQueue()
{
    QString agent_id = "xivo/12", queue_id = "xivo/13";

    QVariantMap result = MessageFactory::unpauseAgentInQueue(agent_id, queue_id);

    QVariantMap expected;
    expected["class"] = "ipbxcommand";
    expected["command"] = "queueunpause";
    expected["member"] = "agent:" + agent_id;
    expected["queue"] = "queue:" + queue_id;

    QCOMPARE(result, expected);
}

void TestMessageFactory::testPauseAgentInAllQueues()
{
    QString agent_id = "xivo/12";
    QString ipbxid = "xivo";

    QVariantMap result = MessageFactory::pauseAgentInAllQueues(agent_id, ipbxid);

    QVariantMap expected;
    expected["class"] = "ipbxcommand";
    expected["command"] = "queuepause";
    expected["member"] = "agent:" + agent_id;
    expected["queue"] = "queue:" + ipbxid + "/all";

    QCOMPARE(result, expected);
}

void TestMessageFactory::testUnpauseAgentInAllQueues()
{
    QString agent_id = "xivo/12";
    QString ipbxid = "xivo";

    QVariantMap result = MessageFactory::unpauseAgentInAllQueues(agent_id, ipbxid);

    QVariantMap expected;
    expected["class"] = "ipbxcommand";
    expected["command"] = "queueunpause";
    expected["member"] = "agent:" + agent_id;
    expected["queue"] = "queue:" + ipbxid + "/all";

    QCOMPARE(result, expected);
}

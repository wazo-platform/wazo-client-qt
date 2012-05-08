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

#include <QtTest/QtTest>

#include <userinfo.h>

#include "mock_baseengine.h"
#include "mock_phoneinfo.h"

#include "test_userinfo.h"

TestUserInfo::TestUserInfo()
{
}

void TestUserInfo::initTestCase()
{
    b_engine = new MockBaseEngine;
    m_ipbxid = QString("testipbx");

    m_linelist.append("23");
    m_phoneidlist.append(QString("%0/%1").arg(m_ipbxid).arg("23"));
    m_linelist.append("42");
    m_phoneidlist.append(QString("%0/%1").arg(m_ipbxid).arg("42"));
    m_phoneidlist.sort();

    m_prop1["fullname"] = "testfullname";
    m_prop1["voicemailid"] = "2345";
    m_prop1["agentid"] = "1234";
    m_prop1["simultcalls"] = 5;
    m_prop1["enableclient"] = true;
    m_prop1["loginclient"] = "testlogin";
    m_prop1["enablevoicemail"] = true;
    m_prop1["incallfilter"] = false;
    m_prop1["enablednd"] = false;
    m_prop1["enableunc"] = false;
    m_prop1["enablerna"] = true;
    m_prop1["destrna"] = "1002";
    m_prop1["enablebusy"] = false;
    m_prop1["mobilephonenumber"] = "4185555555";
    m_prop1["linelist"] = m_linelist;
    m_prop1["availstate"] = "available";
    m_prop1["connection"] = "yes";

    MockPhoneInfo * phoneinfo;
    QVariantMap config;

    phoneinfo = new MockPhoneInfo("asterisk", "p1");
    config.clear();
    config["identity"] = "SIP/sip1";
    config["xchannels"] = "SIP/sip1-01";
    config["number"] = "101";
    phoneinfo->setConfig(config);
    b_engine->setPhone("asterisk/p1", phoneinfo);

    phoneinfo = new MockPhoneInfo("asterisk", "p2");
    config.clear();
    config["identity"] = "SIP/sip2";
    config["xchannels"] = "SIP/sip2-02";
    config["number"] = "102";
    phoneinfo->setConfig(config);
    b_engine->setPhone("asterisk/p2", phoneinfo);

    MockChannelInfo *channelinfo;

    channelinfo = new MockChannelInfo("asterisk", "SIP/sip1-01");
    config.clear();
    config["talkingto_id"] = "SIP/sip2-02";
    channelinfo->setConfig(config);
    b_engine->setChannel("SIP/sip1-01", channelinfo);

    channelinfo = new MockChannelInfo("asterisk", "SIP/sip2-02");
    config.clear();
    config["talkingto_id"] = "SIP/sip1-01";
    channelinfo->setConfig(config);
    b_engine->setChannel("SIP/sip2-02", channelinfo);
}

void TestUserInfo::constructor()
{
    QString ipbxid("testipbx");
    QString id("1234");
    UserInfo u(ipbxid, id);
    QString empty;
    QStringList emptylist;

    QCOMPARE(u.ipbxid(), ipbxid);
    QCOMPARE(u.id(), id);
    QCOMPARE(u.fullname(), empty);
    QCOMPARE(u.voicemailid(), empty);
    QCOMPARE(u.xvoicemailid(), empty);
    QCOMPARE(u.agentid(), empty);
    QCOMPARE(u.xagentid(), empty);
    QCOMPARE(u.simultcalls(), 0);
    QCOMPARE(u.enableclient(), false);
    QCOMPARE(u.ctilogin(), empty);
    QCOMPARE(u.enablevoicemail(), false);
    QCOMPARE(u.incallfilter(), false);
    QCOMPARE(u.enablednd(), false);
    QCOMPARE(u.enableunc(), false);
    QCOMPARE(u.destunc(), empty);
    QCOMPARE(u.enablerna(), false);
    QCOMPARE(u.destrna(), empty);
    QCOMPARE(u.enablebusy(), false);
    QCOMPARE(u.destbusy(), empty);
    QCOMPARE(u.mobileNumber(), empty);

    QCOMPARE(u.phonelist(), emptylist);

    QCOMPARE(u.availstate(), QString(__presence_off__));
    QCOMPARE(u.connected(), false);

    QCOMPARE(u.identitylist(), emptylist);
    QCOMPARE(u.xchannels(), emptylist);
}

void TestUserInfo::xid()
{
    QString ipbxid("test");
    QString id("100");
    UserInfo u(ipbxid, id);
    QCOMPARE(u.xid(), QString("%0/%1").arg(ipbxid).arg(id));
}

void TestUserInfo::updateConfig()
{
    UserInfo u(m_ipbxid, "1");
    {
        // ensure we have at least one member that is different before updateConfig
        QCOMPARE(u.fullname(), QString());

        bool res = u.updateConfig(m_prop1);
        QCOMPARE(res, true);

        QCOMPARE(u.fullname(), m_prop1["fullname"].toString());
        QString vmxid = (QString("%0/%1").arg(m_ipbxid)
                         .arg(m_prop1["voicemailid"].toString()));
        QCOMPARE(u.xvoicemailid(), vmxid);
        QString axid = (QString("%0/%1").arg(m_ipbxid)
                        .arg(m_prop1["agentid"].toString()));
        QCOMPARE(u.xagentid(), axid);
        QCOMPARE(u.simultcalls(), m_prop1["simultcalls"].toInt());
        QCOMPARE(u.enableclient(), m_prop1["enableclient"].toBool());
        QCOMPARE(u.ctilogin(), m_prop1["loginclient"].toString());
        QCOMPARE(u.enablevoicemail(), m_prop1["enablevoicemail"].toBool());
        QCOMPARE(u.incallfilter(), m_prop1["incallfilter"].toBool());
        QCOMPARE(u.enablednd(), m_prop1["enablednd"].toBool());
        QCOMPARE(u.enableunc(), m_prop1["enableunc"].toBool());
        QCOMPARE(u.enablerna(), m_prop1["enablerna"].toBool());
        QCOMPARE(u.destrna(), m_prop1["destrna"].toString());
        QCOMPARE(u.enablebusy(), m_prop1["enablebusy"].toBool());
        QCOMPARE(u.mobileNumber(), m_prop1["mobilephonenumber"].toString());

        QCOMPARE(u.phonelist(), m_phoneidlist);
    }
    {
        bool res = u.updateConfig(m_prop1);
        QCOMPARE(res, false);
    }
    {
        QStringList newlist = QStringList()
            << "23" << "42" << "101";
        QStringList expected = QStringList()
            << QString("%0/%1").arg(m_ipbxid).arg(23)
            << QString("%0/%1").arg(m_ipbxid).arg(42)
            << QString("%0/%1").arg(m_ipbxid).arg(101);
        expected.sort();
        QVariantMap prop;
        prop["linelist"] = newlist;
        bool res = u.updateConfig(prop);
        QCOMPARE(res, true);
        QCOMPARE(u.phonelist(), expected);
    }
}

void TestUserInfo::updateStatus()
{
    UserInfo u(m_ipbxid, "1234");
    {
        QCOMPARE(u.availstate(), QString(__presence_off__));
        bool res = u.updateStatus(m_prop1);
        QCOMPARE(res, true);

        QCOMPARE(u.availstate(), m_prop1["availstate"].toString());
        QCOMPARE(u.connected(), true);
    }
    {
        bool res = u.updateStatus(m_prop1);
        QCOMPARE(res, false);
    }
}

void TestUserInfo::setAvailState()
{
    UserInfo u(m_ipbxid, "1234");
    QCOMPARE(u.availstate(), QString(__presence_off__));
    QString teststate("teststate");
    u.setAvailState(teststate);
    QCOMPARE(u.availstate(), teststate);
}

void TestUserInfo::setPhoneIdList()
{
    UserInfo u(m_ipbxid, "1234");
    QCOMPARE(u.phonelist(), QStringList());
    QStringList testphonelist(QStringList() << "asterisk/p1" << "asterisk/p2");
    u.setPhoneIdList(testphonelist);
    QCOMPARE(u.phonelist(), testphonelist);
}

void TestUserInfo::hasPhoneId()
{
    UserInfo u(m_ipbxid, "1234");
    QString pxid = QString("%0/%1").arg(m_ipbxid).arg("42");
    QCOMPARE(u.hasPhoneId(pxid), false);
    u.updateConfig(m_prop1);
    QCOMPARE(u.hasPhoneId(pxid), true);
    QCOMPARE(u.hasPhoneId(QString("%0/%1").arg(m_ipbxid).arg("133")), false);
}

void TestUserInfo::hasChannelId()
{
    UserInfo u(m_ipbxid, "1234");
    QCOMPARE(u.hasChannelId("SIP/sip1"), false);
    u.setPhoneIdList(QStringList() << "asterisk/p0");
    QCOMPARE(u.hasChannelId("SIP/sip1"), false);
    u.setPhoneIdList(QStringList() << "asterisk/p1");
    QCOMPARE(u.hasChannelId("SIP/sip1"), true);
}

void TestUserInfo::findNumberForXChannel()
{
    UserInfo u(m_ipbxid, "1234");
    QCOMPARE(u.findNumberForXChannel("SIP/sip1"), QString());
    u.setPhoneIdList(QStringList() << "asterisk/p1");
    QCOMPARE(u.findNumberForXChannel("SIP/sip1-01"), QString("101"));
    QCOMPARE(u.findNumberForXChannel("SIP/sip2-02"), QString());
}

void TestUserInfo::xchannels()
{
    UserInfo u(m_ipbxid, "1234");
    QCOMPARE(u.xchannels(), QStringList());
    u.setPhoneIdList(QStringList() << "asterisk/p0");
    QCOMPARE(u.xchannels(), QStringList());
    u.setPhoneIdList(QStringList() << "asterisk/p1");
    QCOMPARE(u.xchannels(), QStringList() << "SIP/sip1-01");
    u.setPhoneIdList(QStringList() << "asterisk/p1" << "asterisk/p2");
    QCOMPARE(u.xchannels(), QStringList() << "SIP/sip1-01" << "SIP/sip2-02");
}

void TestUserInfo::isTalkingTo()
{
    UserInfo u(m_ipbxid, "1234");
    u.setPhoneIdList(QStringList() << "asterisk/p1");
    UserInfo *other = new UserInfo("asterisk", "u2");
    other->setPhoneIdList(QStringList() << "asterisk/p2");

    QCOMPARE(u.isTalkingTo("asterisk/u2"), false);
    b_engine->setUser("asterisk/u2", other);
    QCOMPARE(u.isTalkingTo("asterisk/u2"), true);
}

void TestUserInfo::identitylist()
{
    UserInfo u(m_ipbxid, "1234");
    QCOMPARE(u.identitylist(), QStringList());
    u.setPhoneIdList(QStringList() << "asterisk/p0");
    QCOMPARE(u.identitylist(), QStringList());
    u.setPhoneIdList(QStringList() << "asterisk/p1" << "asterisk/p2");
    QCOMPARE(u.identitylist(), QStringList() << "SIP/sip1" << "SIP/sip2");
}

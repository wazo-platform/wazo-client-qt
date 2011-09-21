#include "test_userinfo.h"

#include <QtTest/QtTest>

#include "userinfo.h"

TestUserInfo::TestUserInfo()
{
    init();
}

void TestUserInfo::init()
{
    m_ipbxid = QString("testipbx");

    m_linelist.append("23");
    m_phoneidlist.append(QString("%0/%1").arg(m_ipbxid).arg("23"));
    m_linelist.append("42");
    m_phoneidlist.append(QString("%0/%1").arg(m_ipbxid).arg("42"));

    m_prop1["loginclient"] = "testlogin";
    m_prop1["fullname"] = "testfullname";
    m_prop1["mobilephonenumber"] = "4185555555";
    m_prop1["context"] = "testctx";
    m_prop1["simultcalls"] = 5;
    m_prop1["agentid"] = "1234";
    m_prop1["voicemailid"] = "2345";
    m_prop1["callrecord"] = false;
    m_prop1["enablednd"] = false;
    m_prop1["enablevoicemail"] = true;
    m_prop1["incallfilter"] = false;
    m_prop1["enablebusy"] = false;
    m_prop1["enablerna"] = true;
    m_prop1["enableune"] = false;
    m_prop1["destrna"] = "1002";
    m_prop1["linelist"] = m_linelist;
}

void TestUserInfo::ctxor()
{
    QString ipbxid("testipbx");
    QString id("1234");
    UserInfo u(ipbxid, id);
    QString empty;
    QStringList emptylist;

    QCOMPARE(u.ipbxid(), ipbxid);
    QCOMPARE(u.id(), id);
    QCOMPARE(u.mobileNumber(), empty);
    QCOMPARE(u.fullname(), empty);
    QCOMPARE(u.ctilogin(), empty);
    QCOMPARE(u.context(), empty);
    QCOMPARE(u.agentid(), empty);
    QCOMPARE(u.xagentid(), empty);
    QCOMPARE(u.voicemailid(), empty);
    QCOMPARE(u.xvoicemailid(), empty);
    QCOMPARE(u.agentNumber(), empty);
    QCOMPARE(u.voicemailNumber(), empty);
    QCOMPARE(u.destbusy(), empty);
    QCOMPARE(u.destrna(), empty);
    QCOMPARE(u.destunc(), empty);
    QCOMPARE(u.availstate(), empty);
    QCOMPARE(u.callrecord(), false);
    QCOMPARE(u.enablednd(), false);
    QCOMPARE(u.enablevoicemail(), false);
    QCOMPARE(u.incallfilter(), false);
    QCOMPARE(u.enablebusy(), false);
    QCOMPARE(u.enablerna(), false);
    QCOMPARE(u.enableunc(), false);
    QCOMPARE(u.simultcalls(), 0);
    QCOMPARE(u.xchannels(), emptylist);
    QCOMPARE(u.phonelist(), emptylist);
    QCOMPARE(u.identitylist(), emptylist);
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
        QCOMPARE(u.fullname(), QString());
        bool res = u.updateConfig(m_prop1);
        QCOMPARE(res, true);
        QCOMPARE(u.phonelist(), m_phoneidlist);
        QCOMPARE(u.fullname(), m_prop1["fullname"].toString());
        QString vmxid = (QString("%0/%1").arg(m_ipbxid)
                         .arg(m_prop1["voicemailid"].toString()));
        QCOMPARE(u.xvoicemailid(), vmxid);
        QString axid = (QString("%0/%1").arg(m_ipbxid)
                        .arg(m_prop1["agentid"].toString()));
        QCOMPARE(u.xagentid(), axid);
        QCOMPARE(u.simultcalls(), m_prop1["simultcalls"].toInt());
        QCOMPARE(u.enablerna(), m_prop1["enablerna"].toBool());
        QCOMPARE(u.destrna(), m_prop1["destrna"].toString());
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
        QVariantMap prop;
        prop["linelist"] = newlist;
        bool res = u.updateConfig(prop);
        QCOMPARE(res, true);
        QCOMPARE(u.phonelist(), expected);
    }
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

void TestUserInfo::availstate()
{
    UserInfo u(m_ipbxid, "1234");
    QCOMPARE(u.availstate(), QString());
    QString teststate("teststate");
    u.setAvailState(teststate);
    QCOMPARE(u.availstate(), teststate);
}

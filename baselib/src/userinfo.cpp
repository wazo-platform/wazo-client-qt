/* XiVO Client
 * Copyright (C) 2007-2011, Proformatique
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

/* $Revision$
 * $Date$
 */

#include <QDebug>
#include "userinfo.h"
#include "phoneinfo.h"

/*! \brief Constructor
 *
 * just set userid
 */
UserInfo::UserInfo(const QString & ipbxid,
                   const QString & id)
    : XInfo(ipbxid, id), m_fullname("")
{
}

bool UserInfo::updateConfig(const QVariantMap & qvm)
{
    bool haschanged = true;
    if (qvm.contains("loginclient"))
        setCtiLogin(qvm.value("loginclient").toString());
    if (qvm.contains("fullname"))
        setFullName(qvm.value("fullname").toString());
    if (qvm.contains("number"))
        setPhoneNumber(qvm.value("number").toString());
    if (qvm.contains("mobilephonenumber"))
        setMobileNumber(qvm.value("mobilephonenumber").toString());
    if (qvm.contains("context"))
        setContext(qvm.value("context").toString());
    if (qvm.contains("simultcalls"))
        setSimultCalls(qvm.value("simultcalls").toInt());
    if (qvm.contains("id")) {
        QStringList lid;
        foreach (QString id, qvm.value("id").toStringList())
            lid << QString("%1/%2").arg(m_ipbxid).arg(id);
        setPhoneIdList(lid);
    }
    if (qvm.contains("agentid"))
        setAgentId(qvm.value("agentid").toString());
    if (qvm.contains("voicemailid"))
        setVoiceMailId(qvm.value("voicemailid").toString());
    return haschanged;
}

bool UserInfo::updateStatus(const QVariantMap & qvm)
{
    bool haschanged = true;
    if (qvm.contains("availstate"))
        setAvailState(qvm.value("availstate").toString());
    return haschanged;
}

/*! \brief set full name */
void UserInfo::setFullName(const QString & fullname)
{
    m_fullname = fullname;
}

/*! \brief set CTI Login */
void UserInfo::setCtiLogin(const QString & ctilogin)
{
    m_ctilogin = ctilogin;
}

void UserInfo::setPhoneNumber(const QString & phonenumber)
{
    m_phonenumber = phonenumber;
}

void UserInfo::setMobileNumber(const QString &mobilenum)
{
    m_mobilenumber = mobilenum;
}

void UserInfo::setSimultCalls(int simultcalls)
{
    m_simultcalls = simultcalls;
}

void UserInfo::setAgentNumber(const QString & agentnumber)
{
    m_agentnumber = agentnumber;
}

void UserInfo::setPhoneIdList(const QStringList & phoneidlist)
{
    m_phoneidlist = phoneidlist;
}

void UserInfo::setAgentId(const QString & agentid)
{
    m_agentid = agentid;
    m_xagentid = QString("%1/%2").arg(m_ipbxid).arg(agentid);
}

void UserInfo::setVoiceMailId(const QString & voicemailid)
{
    m_voicemailid = voicemailid;
}

void UserInfo::setContext(const QString & context)
{
    m_context = context;
}

/*! \brief update availability state */
void UserInfo::setAvailState(const QString & availstate)
{
    m_availstate = availstate;
}

/*! \brief set Message Waiting indicator */
void UserInfo::setMWI(const QStringList & mwi)
{
    m_mwi = mwi;
}

/*! \brief check if this user has this phone */
bool UserInfo::hasPhoneId(const QString & xphoneid) const
{
    return m_phoneidlist.contains(xphoneid);
}

/*! \brief check if this user has this agent */
bool UserInfo::hasAgentNumber(const QString & agentnumber) const
{
    return (m_agentnumber == agentnumber);
}

/*! \brief return m_fullname */
const QString & UserInfo::fullname() const
{
    return m_fullname;
}

/*! \brief return m_phonenumber */
const QString & UserInfo::phoneNumber() const
{
    return m_phonenumber;
}

/*! \brief return m_mobilenumber */
const QString & UserInfo::mobileNumber() const
{
    return m_mobilenumber;
}

const QString & UserInfo::voicemailNumber() const
{
    return m_voicemailnumber;
}

const QString & UserInfo::userid() const
{
    return m_id;
}

const QString & UserInfo::xuserid() const
{
    return m_xid;
}

const QString & UserInfo::ctilogin() const
{
    return m_ctilogin;
}

/*! \brief return Message Waiting Indicator */
const QStringList & UserInfo::mwi() const
{
    return m_mwi;
}

const QString & UserInfo::agentNumber() const
{
    return m_agentnumber;
}

const QString & UserInfo::agentid() const
{
    return m_agentid;
}

const QString & UserInfo::xagentid() const
{
    return m_xagentid;
}

/*! \brief return a list of contexts where this user has its phones
 */
const QString& UserInfo::context() const
{
    return m_context;
}

const QString & UserInfo::availstate() const
{
    return m_availstate;
}

/*! \brief return list of phones identifier */
const QStringList & UserInfo::phonelist() const
{
    return m_phoneidlist;
}

/*! \brief return a String representation of the object
 *
 * useful for debug
 */
QString UserInfo::toString() const
{
    QString str;

    str = "Userid=" + m_id + " company=" + m_company + " fullname=" + m_fullname;
    str += " phonenum=" + m_phonenumber + " mobile=" + m_mobilenumber;
    str += " m_voicemailnum=" + m_voicemailnumber;
    str += " nphones=" + QString::number(m_phoneidlist.size());
    str += " phonesids=" + m_phoneidlist.join(",");

    return str;
}

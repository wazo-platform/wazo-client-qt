/* XiVO Client
 * Copyright (C) 2007-2010, Proformatique
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
UserInfo::UserInfo(const QString &userid)
    : m_fullname("")
{
    m_userid = userid;
}

/*! \brief set full name */
void UserInfo::setFullName(const QString &fullname)
{
    m_fullname = fullname;
}

/*! \brief set CTI Login */
void UserInfo::setCtiLogin(const QString &ctilogin)
{
    m_ctilogin = ctilogin;
}

void UserInfo::setPhoneNumber(const QString &phonenum)
{
    m_phonenumber = phonenum;
}

void UserInfo::setMobileNumber(const QString &mobilenum)
{
    m_mobilenumber = mobilenum;
}

void UserInfo::setSimultCalls(int simultcalls)
{
    m_simultcalls = simultcalls;
}

void UserInfo::setVoiceMailNumber(const QString &voicemailnum)
{
    m_voicemailnumber = voicemailnum;
}

void UserInfo::setContext(const QString &context)
{
    m_context = context;
}

/*! \brief set phone list
 *
 * fill m_phones Hash with pointers to PhoneInfo objects
 */
void UserInfo::setPhones(const QString &astid,
                         const QStringList &termlist,
                         const QHash<QString, PhoneInfo *> &phones)
{
    //qDebug() << "UserInfo::setPhones" << astid << termlist;
    m_astid = astid;
    foreach (const QString term, termlist) {
        PhoneInfo *pi = NULL;
        QString key = astid + "." + term;
        if (phones.contains(key)) {
            pi = phones.value(key);
        }
        m_phones[term] = pi;
    }
}

/*! \brief update availability state */
void UserInfo::setAvailState(const QVariant &availstate)
{
    m_availstate.clear();
    const QMap<QString, QVariant> map = availstate.toMap();
    foreach (const QString key, map.keys()) {
        m_availstate[key] = map.value(key).toString();
    }
}

void UserInfo::setAgentNumber(const QString &agentnumber)
{
    m_agentnumber = agentnumber;
}

void UserInfo::setAgentId(const QString &agentid)
{
    m_agentid = agentid;
}

/*! \brief set Message Waiting indicator */
void UserInfo::setMWI(const QStringList &mwi)
{
    m_mwi = mwi;
}

/*! \brief check if this user has this phone */
bool UserInfo::hasPhone(const QString &astid,
                        const QString &term)
{
    //    qDebug() << "UserInfo::hasPhone" << term << m_phones;
    return ((m_astid == astid) && m_phones.keys().contains(term));
}

/*! \brief check if this user has this agent */
bool UserInfo::hasAgentNumber(const QString &astid,
                              const QString &agentnumber)
{
    return ((m_astid == astid) && (m_agentnumber == agentnumber));
}

/*! \brief return m_fullname */
const QString& UserInfo::fullname() const
{
    return m_fullname;
}

/*! \brief return m_phonenumber */
const QString& UserInfo::phoneNumber() const
{
    return m_phonenumber;
}

/*! \brief return m_mobilenumber */
const QString& UserInfo::mobileNumber() const
{
    return m_mobilenumber;
}

const QString& UserInfo::voicemailNumber() const
{
    return m_voicemailnumber;
}

const QString& UserInfo::userid() const
{
    return m_userid;
}

const QString& UserInfo::ctilogin() const
{
    return m_ctilogin;
}

/*! \brief return Message Waiting Indicator */
const QStringList& UserInfo::mwi() const
{
    return m_mwi;
}

const QString& UserInfo::agentNumber() const
{
    return m_agentnumber;
}

const QString& UserInfo::agentid() const
{
    return m_agentid;
}

/*! \brief return a list of contexts where this user has its phones
 */
const QString& UserInfo::context() const
{
    return m_context;
}

const QString& UserInfo::astid() const
{
    return m_astid;
}

const QHash<QString, QString> & UserInfo::availstate() const
{
    return m_availstate;
}

/*! \brief return list of phones identifier */
QList<QString> UserInfo::phonelist() const
{
    return m_phones.keys();
}

/*! \brief return phone info
 */
const PhoneInfo* UserInfo::getPhoneInfo(const QString & id) const
{
    if (m_phones.contains(id)) {
        return m_phones.value(id);
    }

    QString key = m_astid + "." + id;
    if (m_phones.contains(key)) {
        return m_phones.value(key);
    }

    return NULL;
}

/*! \brief update phone info pointer
 */
void UserInfo::updatePhone(PhoneInfo * pi)
{
    if(pi) {
        QString key = pi->tech() + "." + pi->context() + "." + pi->phoneid() + "." + pi->number();
        m_phones[key] = pi;
    }
}

/*! \brief list channels of this user
 *
 * iterate through the list comms of each phones.
 */
QList<QString> UserInfo::channelList() const
{
    QList<QString> list;
    QMapIterator<QString, PhoneInfo *> it = QMapIterator<QString, PhoneInfo *>(m_phones);

    while (it.hasNext()) {
        it.next();
        if (it.value()) {
            QMapIterator<QString, QVariant> itphone( it.value()->comms() );
            while (itphone.hasNext()) {
                itphone.next();
                QVariantMap qvm = itphone.value().toMap();
                list << qvm.value("thischannel").toString();
            }
        }
    }

    return list;
}

/*! \brief return a String representation of the object
 *
 * useful for debug
 */
QString UserInfo::toString() const
{
    QString str;

    str = "Userid=" + m_userid + " company=" + m_company + " fullname=" + m_fullname;
    str += " phonenum=" + m_phonenumber + " mobile=" + m_mobilenumber;
    str += " m_voicemailnum=" + m_voicemailnumber;
    str += " nphones=" + QString::number(m_phones.size());
    str += " phonesids=" + QStringList(m_phones.keys()).join(",");

    return str;
}

/*! \brief count the number of current communications for this user
 */
int UserInfo::commsCount() const
{
    int c = 0;

    foreach (const QString phone, phonelist()) {
        const PhoneInfo * pi = getPhoneInfo(phone);
        if (pi) {
            c += pi->comms().count();
        }
    }

    return c;
}

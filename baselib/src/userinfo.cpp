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

/*! \brief Constructor
 *
 * just set userid
 */
UserInfo::UserInfo(const QString & ipbxid,
                   const QString & id)
    : XInfo(ipbxid, id), m_fullname(""),
      m_callrecord(false)
{
}

bool UserInfo::updateConfig(const QVariantMap & prop)
{
    bool haschanged = false;
    haschanged |= setIfChangeString(prop, "loginclient", & m_ctilogin);
    haschanged |= setIfChangeString(prop, "fullname", & m_fullname);
    haschanged |= setIfChangeString(prop, "mobilephonenumber", & m_mobilenumber);
    haschanged |= setIfChangeString(prop, "context", & m_context);
    haschanged |= setIfChangeInt(prop, "simultcalls", & m_simultcalls);

    haschanged |= setIfChangeString(prop, "agentid", & m_agentid);
    m_xagentid = QString("%1/%2").arg(m_ipbxid).arg(m_agentid);
    haschanged |= setIfChangeString(prop, "voicemailid", & m_voicemailid);
    m_xvoicemailid = QString("%1/%2").arg(m_ipbxid).arg(m_voicemailid);

    haschanged |= setIfChangeBool(prop, "callrecord", & m_callrecord);
    haschanged |= setIfChangeBool(prop, "enablednd", & m_enablednd);
    haschanged |= setIfChangeBool(prop, "enablevoicemail", & m_enablevoicemail);
    haschanged |= setIfChangeBool(prop, "incallfilter", & m_incallfilter);
    haschanged |= setIfChangeBool(prop, "enablebusy", & m_enablebusy);
    haschanged |= setIfChangeBool(prop, "enablerna", & m_enablerna);
    haschanged |= setIfChangeBool(prop, "enableunc", & m_enableunc);
    haschanged |= setIfChangeString(prop, "destbusy", & m_destbusy);
    haschanged |= setIfChangeString(prop, "destrna", & m_destrna);
    haschanged |= setIfChangeString(prop, "destunc", & m_destunc);

    if (prop.contains("linelist")) {
        QStringList lid;
        foreach (QString id, prop.value("linelist").toStringList())
            lid << QString("%1/%2").arg(m_ipbxid).arg(id);
        setPhoneIdList(lid);
        haschanged = true;
    }
    return haschanged;
}

bool UserInfo::updateStatus(const QVariantMap & prop)
{
    bool haschanged = false;
    haschanged |= setIfChangeString(prop, "availstate", & m_availstate);
    return haschanged;
}

void UserInfo::setPhoneIdList(const QStringList & phoneidlist)
{
    m_phoneidlist = phoneidlist;
}

/*! \brief check if this user has this phone */
bool UserInfo::hasPhoneId(const QString & xphoneid) const
{
    return m_phoneidlist.contains(xphoneid);
}

const QString & UserInfo::availstate() const
{
    return m_availstate;
}

/*! \brief return a String representation of the object
 *
 * useful for debug
 */
QString UserInfo::toString() const
{
    QString str;

    str = "Userid=" + m_id + " company=" + m_company + " fullname=" + m_fullname;
    str += " mobile=" + m_mobilenumber;
    str += " m_voicemailnum=" + m_voicemailnumber;
    str += " nphones=" + QString::number(m_phoneidlist.size());
    str += " phonesids=" + m_phoneidlist.join(",");

    return str;
}

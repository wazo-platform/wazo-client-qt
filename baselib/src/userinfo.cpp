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
#include "baseengine.h"

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

    if (haschanged) { // Clear cached values
        m_identity_list.clear();
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

/*! \brief Returns a phone number for a channel
 *
 * If a channel is owned by the one of the user's phone the phone number
 * is returned.
 *
 * \param xcid The channel's XiVO id
 * \return the phone number or an empty string
 */
QString UserInfo::findNumberForXChannel(const QString & xcid) const
{
    // qDebug() << Q_FUNC_INFO << xcid;
    foreach (const QString & phone_key, phonelist()) {
        const PhoneInfo * p = b_engine->phone(phone_key);
        if (p && p->xchannels().contains(xcid)) {
            return p->number();
        }
    }
    return QString();
}

/*! \brief Returns a list of phone identities for this user
 *
 * If a user has one or many lines a QStringList containing each of the lines
 * identities for that user
 * If the user has no lines an empty list is returned */
const QStringList & UserInfo::identitylist() {
    if (m_identity_list.size() == 0) {
        foreach (const QString & phonexid, m_phoneidlist) {
            const PhoneInfo * p = b_engine->phone(phonexid);
            if (p) {
                m_identity_list << p->identity();
            }
        }
    }
    return m_identity_list;
}

/*! \brief check if this user owns this channel */
bool UserInfo::hasChannelId(const QString & xchannelid) const
{
    foreach (const QString & phoneid, m_phoneidlist) {
        const PhoneInfo * p = b_engine->phone(phoneid);
        if (p && p->xchannels().contains(xchannelid)) {
            return true;
        }
    }
    return false;
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

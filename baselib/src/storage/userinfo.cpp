/* XiVO Client
 * Copyright (C) 2007-2015 Avencall
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

#include <QDebug>

#include "xivoconsts.h"
#include "baseengine.h"
#include "phoneinfo.h"
#include "channelinfo.h"

#include "userinfo.h"

UserInfo::UserInfo(const QString & ipbxid,
                   const QString & id)
  : XInfo(ipbxid, id),
    m_enablevoicemail(false),
    m_incallfilter(false),
    m_enablednd(false),
    m_enableunc(false),
    m_enablerna(false),
    m_enablebusy(false),
    m_enablexfer(false),
    m_availstate(__presence_off__)
{
}

bool UserInfo::updateConfig(const QVariantMap & prop)
{
    bool haschanged = false;
    haschanged |= setIfChangeString(prop, "fullname", & m_fullname);
    haschanged |= setIfChangeString(prop, "voicemailid", & m_voicemailid);
    m_xvoicemailid = QString("%1/%2").arg(m_ipbxid).arg(m_voicemailid);
    haschanged |= setIfChangeString(prop, "agentid", & m_agentid);
    m_xagentid = QString("%1/%2").arg(m_ipbxid).arg(m_agentid);
    haschanged |= setIfChangeString(prop, "mobilephonenumber", & m_mobilenumber);
    haschanged |= setIfChangeBool(prop, "enablevoicemail", & m_enablevoicemail);
    haschanged |= setIfChangeBool(prop, "incallfilter", & m_incallfilter);
    haschanged |= setIfChangeBool(prop, "enablednd", & m_enablednd);
    haschanged |= setIfChangeBool(prop, "enableunc", & m_enableunc);
    haschanged |= setIfChangeString(prop, "destunc", & m_destunc);
    haschanged |= setIfChangeBool(prop, "enablerna", & m_enablerna);
    haschanged |= setIfChangeString(prop, "destrna", & m_destrna);
    haschanged |= setIfChangeBool(prop, "enablebusy", & m_enablebusy);
    haschanged |= setIfChangeBool(prop, "enablexfer", & m_enablexfer);
    haschanged |= setIfChangeString(prop, "destbusy", & m_destbusy);
    haschanged |= setIfChangeString(prop, "firstname", & m_firstname);
    haschanged |= setIfChangeString(prop, "lastname", & m_lastname);
    haschanged |= setIfChangeString(prop, "xivo_uuid", & m_xivo_uuid);

    if (prop.contains("linelist")) {
        QStringList lid;
        foreach (const QString &phone_id, prop.value("linelist").toStringList())
            lid << QString("%1/%2").arg(m_ipbxid).arg(phone_id);
        lid.sort();
        if (lid != m_phoneidlist) {
            haschanged = true;
            setPhoneIdList(lid);
        }
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

/*! \brief returns the list of phone's identities for this user */
const QStringList & UserInfo::identitylist() const
{
    if (m_identitylist.isEmpty()) {
        foreach (const QString & phonexid, m_phoneidlist) {
            const PhoneInfo * p = b_engine->phone(phonexid);
            if (p) {
                m_identitylist.append(p->identity());
            }
        }
    }
    return m_identitylist;
}

const QString & UserInfo::availstate() const
{
    return m_availstate;
}

/*!
 * \brief Retrieves a list of channels for this user
 */
QStringList UserInfo::xchannels() const
{
    QStringList channels;
    foreach (const QString & phonexid, phonelist()) {
        if (const PhoneInfo * p = b_engine->phone(phonexid)) {
            foreach (const QString & channelxid, p->xchannels()) {
                channels << channelxid;
            }
        }
    }
    return channels;
}

/*!
 * \brief Check if we are talking to another user
 * \param rhs Other user
 */
bool UserInfo::isTalkingTo(const QString & rhs) const
{
    if (const UserInfo * u = b_engine->user(rhs)) {
        const QStringList & peers_channel = u->xchannels();
        foreach (const QString & channelxid, peers_channel) {
            if (const ChannelInfo * c = b_engine->channel(channelxid)) {
                QString identity = c->talkingto_id().split("-").value(0);
                if (this->identitylist().contains(identity)) {
                    return true;
                }
            }
        }
    }
    return false;
}

bool UserInfo::hasMobile() const
{
    return ! m_mobilenumber.isEmpty();
}

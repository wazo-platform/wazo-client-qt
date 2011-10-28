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
  : XInfo(ipbxid, id),
    m_simultcalls(0),
    m_enableclient(false),
    m_enablevoicemail(false),
    m_callrecord(false),
    m_incallfilter(false),
    m_enablednd(false),
    m_enableunc(false),
    m_enablerna(false),
    m_enablebusy(false)
{
}

bool UserInfo::updateConfig(const QVariantMap & prop)
{
    bool haschanged = false;
    haschanged |= setIfChangeString(prop, "loginclient", & m_ctilogin);
    haschanged |= setIfChangeString(prop, "fullname", & m_fullname);
    haschanged |= setIfChangeString(prop, "mobilephonenumber", & m_mobilenumber);
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
    haschanged |= setIfChangeBool(prop, "enableclient", & m_enableclient);
    haschanged |= setIfChangeString(prop, "destbusy", & m_destbusy);
    haschanged |= setIfChangeString(prop, "destrna", & m_destrna);
    haschanged |= setIfChangeString(prop, "destunc", & m_destunc);

    if (prop.contains("linelist")) {
        QStringList lid;
        foreach (QString id, prop.value("linelist").toStringList())
            lid << QString("%1/%2").arg(m_ipbxid).arg(id);
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
    haschanged |= setIfChangeString(prop, "connection", & m_connection);
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
 * If a channel is owned by one of the user's phone the phone number
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

/*! \brief check if this user owns this channel */
bool UserInfo::hasChannelId(const QString & xchannelid) const
{
    foreach (const QString & phoneid, m_phoneidlist) {
        const PhoneInfo * p = b_engine->phone(phoneid);
        if (p && xchannelid.contains(p->identity())) {
            return true;
        }
    }
    return false;
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

/*! \brief return a String representation of the object
 *
 * useful for debug
 */
QString UserInfo::toString() const
{
    QString str;

    str  = "Userid=" + m_id + " fullname=" + m_fullname;
    str += " mobile=" + m_mobilenumber;
    str += " nphones=" + QString::number(m_phoneidlist.size());
    str += " phonesids=" + m_phoneidlist.join(",");
    str += " status=" + m_availstate;

    return str;
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
                if (identitylist().contains(identity)) {
                    return true;
                }
            }
        }
    }
    return false;
}

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

#ifndef __USERINFO_H__
#define __USERINFO_H__


#include "baselib_export.h"
#include "xinfo.h"

class BASELIB_EXPORT UserInfo : public XInfo
{
    public:
        UserInfo(const QString &, const QString &);

        const QString & fullname() const { return m_fullname; };
        const QString & firstname() const { return m_firstname; };
        const QString & lastname() const { return m_lastname; };
        const QString & voicemailid() const { return m_voicemailid; };
        const QString & xvoicemailid() const { return m_xvoicemailid; };
        const QString & agentid() const { return m_agentid; };
        const QString & xagentid() const { return m_xagentid; };
        const QString & xivoUuid() const { return m_xivo_uuid; };
        bool enablevoicemail() const { return m_enablevoicemail; };
        bool incallfilter() const { return m_incallfilter; };
        bool enablednd() const { return m_enablednd; };
        bool enableunc() const { return m_enableunc; };
        const QString & destunc() const { return m_destunc; };
        bool enablerna() const { return m_enablerna; };
        const QString & destrna() const { return m_destrna; };
        bool enablebusy() const { return m_enablebusy; };
        bool enablexfer() const { return m_enablexfer; };
        const QString & destbusy() const { return m_destbusy; };
        const QString & mobileNumber() const { return m_mobilenumber; };

        const QStringList & phonelist() const { return m_phoneidlist; };

        const QString & availstate() const;

        bool updateConfig(const QVariantMap &);
        bool updateStatus(const QVariantMap &);

        void setAvailState(const QString & availstate) {m_availstate = availstate;};

        void setPhoneIdList(const QStringList &);

        bool hasMobile() const;
        QStringList xchannels() const;
        bool isTalkingTo(const QString &) const;
        const QStringList & identitylist() const;
    private:

        QString m_xivo_uuid;
        QString m_fullname;
        QString m_firstname;
        QString m_lastname;
        QString m_voicemailid;              //!< voice mail box id
        QString m_xvoicemailid;             //!< ipbxid + voice mail box id
        QString m_agentid;                  //!< agent id
        QString m_xagentid;                 //!< ipbxid + agent id
        bool m_enablevoicemail;
        bool m_incallfilter;
        bool m_enablednd;
        bool m_enableunc;
        QString m_destunc;
        bool m_enablerna;
        QString m_destrna;
        bool m_enablebusy;
        bool m_enablexfer;
        QString m_destbusy;
        QString m_mobilenumber;             //!< mobile phone number
        QStringList m_phoneidlist;          //!< map to phones
        mutable QStringList m_identitylist; //!< Cached identities for this user
        QString m_availstate;               //!< availability state
};

#endif

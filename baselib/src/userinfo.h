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

#ifndef __USERINFO_H__
#define __USERINFO_H__

#include "baselib_export.h"
#include <QStringList>
#include <QVariant>
#include <QHash>
#include <QMap>
#include "xinfo.h"

/*! \brief Store User information
 */
class BASELIB_EXPORT UserInfo : public XInfo
{
    public:
        UserInfo(const QString &, const QString &);
        bool updateConfig(const QVariantMap &);
        bool updateStatus(const QVariantMap &);

        void setAvailState(const QString & availstate) {m_availstate = availstate;};

        void setPhoneIdList(const QStringList &);
        void setAgentNumber(const QString &);

        bool hasPhoneId(const QString &) const;
        bool hasChannelId(const QString &) const;

        const QString & mobileNumber() const { return m_mobilenumber; };
        const QStringList & phonelist() const { return m_phoneidlist; };
        QStringList identitylist() const;
        const QString & fullname() const { return m_fullname; };
        const QString & ctilogin() const { return m_ctilogin; };
        const QString & context() const { return m_context; };
        int simultcalls() const { return m_simultcalls; };
        const QString & agentid() const { return m_agentid; };
        const QString & xagentid() const { return m_xagentid; };
        const QString & voicemailid() const { return m_voicemailid; };
        const QString & xvoicemailid() const { return m_xvoicemailid; };

        // method to fetch config indirectly
        const QString & agentNumber() const { return m_agentnumber; };
        const QString & voicemailNumber() const { return m_voicemailnumber; };

        bool callrecord() const { return m_callrecord; };
        bool enablednd() const { return m_enablednd; };
        bool enablevoicemail() const { return m_enablevoicemail; };
        bool incallfilter() const { return m_incallfilter; };
        bool enablebusy() const { return m_enablebusy; };
        const QString & destbusy() const { return m_destbusy; };
        bool enablerna() const { return m_enablerna; };
        const QString & destrna() const { return m_destrna; };
        bool enableunc() const { return m_enableunc; };
        const QString & destunc() const { return m_destunc; };

        // methods to fetch statuses
        const QString & availstate() const;
        QString toString() const;

        // helper methods
        QString findNumberForXChannel(const QString &) const;

    private:
        QString m_company;  //!< user company
        QString m_ctilogin;  //!< user login on cti
        QString m_fullname;  //!< user full name
        QString m_context;  //!< user context

        int m_simultcalls;

        QString m_mobilenumber;  //!< mobile phone number
        QString m_agentid; //!< agent id
        QString m_xagentid; //!< ipbxid + agent id
        QString m_voicemailid;  //!< voice mail box id
        QString m_xvoicemailid;  //!< ipbxid + voice mail box id
        QStringList m_phoneidlist;  //!< map to phones

        QString m_agentnumber;  //!< agent number
        QString m_voicemailnumber;  //!< voice mail box number

        bool m_callrecord;
        bool m_enablednd;
        bool m_enablevoicemail;
        bool m_incallfilter;
        bool m_enablebusy;
        QString m_destbusy;
        bool m_enablerna;
        QString m_destrna;
        bool m_enableunc;
        QString m_destunc;

        QString m_availstate;   //!< availability state
};

#endif

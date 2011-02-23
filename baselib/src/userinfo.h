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

/*! \brief Store informations on a user
 */
class BASELIB_EXPORT UserInfo
{
    public:
        UserInfo(const QString &, const QString &);

        void setAvailState(const QVariant &);
        void setFullName(const QString &);
        void setPhoneNumber(const QString &);
        void setMobileNumber(const QString &);
        void setSimultCalls(int);
        void setVoiceMailNumber(const QString &);
        void setPhones(const QStringList &);
        void setAgentNumber(const QString &);
        void setAgentId(const QString &);
        void setUserId(const QString &);
        void setCtiLogin(const QString &);
        void setMWI(const QStringList &);
        void setContext(const QString &);

        bool hasPhone(const QString &);
        bool hasAgentNumber(const QString &);

        const QString &phoneNumber() const;
        const QString &mobileNumber() const;
        const QString &voicemailNumber() const;
        const QString &fullname() const;
        const QStringList &mwi() const;
        const QString &userid() const;
        const QString &ctilogin() const;
        const QString &agentNumber() const;
        const QString &agentid() const;
        const QString &ipbxid() const;
        const QString &context() const;
        int simultcalls() const { return m_simultcalls; };

        const QHash<QString, QString>& availstate() const;
        QStringList phonelist() const;
        QString toString() const;

    private:
        QString m_ipbxid;  //!< IPBX id (for main phone)
        QString m_userid;  //!< user id
        QString m_company;  //!< user company
        QString m_ctilogin;  //!< user login on cti
        QString m_fullname;  //!< user full name
        QString m_context;  //!< user context

        QHash<QString, QString> m_availstate;   //!< availability state properties

        QString m_phonenumber;  //!< main phone number
        QString m_mobilenumber;  //!< mobile phone number
        QString m_voicemailnumber;  //!< voice mail box number
        QString m_agentnumber; //!< agent number
        QString m_agentid; //!< agent id
        QString m_agentphonenumber;    //!< agent phone number
        QStringList m_mwi;  //!< message waiting indicator
        int m_simultcalls;
        QStringList m_phones;  //!< map to phones
};

#endif

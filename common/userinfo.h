/* XIVO CTI clients
 * Copyright (C) 2007-2009  Proformatique
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License version 2 for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Linking the Licensed Program statically or dynamically with other
 * modules is making a combined work based on the Licensed Program. Thus,
 * the terms and conditions of the GNU General Public License version 2
 * cover the whole combination.
 *
 * In addition, as a special exception, the copyright holders of the
 * Licensed Program give you permission to combine the Licensed Program
 * with free software programs or libraries that are released under the
 * GNU Library General Public License version 2.0 or GNU Lesser General
 * Public License version 2.1 or any later version of the GNU Lesser
 * General Public License, and with code included in the standard release
 * of OpenSSL under a version of the OpenSSL license (with original SSLeay
 * license) which is identical to the one that was published in year 2003,
 * or modified versions of such code, with unchanged license. You may copy
 * and distribute such a system following the terms of the GNU GPL
 * version 2 for the Licensed Program and the licenses of the other code
 * concerned, provided that you include the source code of that other code
 * when and as the GNU GPL version 2 requires distribution of source code.
 */

/* $Revision$
 * $Date$
 */

#ifndef __USERINFO_H__
#define __USERINFO_H__

#include <QStringList>
#include <QVariant>
#include <QHash>
#include <QMap>

class PhoneInfo;

/*! \brief Store informations on a user
 */
class UserInfo
{
 public:
    UserInfo(const QString &);
    ~UserInfo();
    void setAvailState(const QVariant &);
    void setFullName(const QString &);
    void setPhoneNumber(const QString &);
    void setVoiceMailNumber(const QString &);
    void setPhones(const QString &,
                   const QStringList &,
                   const QHash<QString, PhoneInfo *> & phones);
    void setAgent(const QString &);
    void setUserId(const QString &);
    void setCtiLogin(const QString &);
    void setMWI(const QStringList &);
    bool hasPhone(const QString &,
                  const QString &);
    bool hasAgent(const QString &,
                  const QString &);
    const QString & phonenumber() const;
    const QString & voicemailnumber() const;
    const QString & fullname() const;
    const QStringList & mwi() const;
    const QString & userid() const;
    const QString & ctilogin() const;
    const QString & agentid() const;
    const QString & astid() const;
    const QStringList contexts() const;
    const QHash<QString, QString> & availstate() const;
    //        const QMap<QString, QHash<QString, QString> > & termstatus() const;
    //        void updatePhoneStatus(const QString &,
    //                               const QVariant &);
    QList<QString> phonelist() const;
    const PhoneInfo * getPhoneInfo(const QString &) const;
    void updatePhone( PhoneInfo * );
 private:
    QString m_userid;   //!< user id
    QString m_company;  //!< user company
    QString m_ctilogin; //!< user login on cti
    QString m_fullname; //!< user fulle name
        
    QHash<QString, QString> m_availstate;   //!< availability state properties
        
    QString m_astid;    //!< asterisk id (for main phone)
    QString m_phonenum; //!< main phone number
    QString m_voicemailnum; //!< voice mail box number
    QString m_agentnum; //!< agent number
    QString m_agentphonenum;    //!< agent phone number
    QStringList m_mwi;  //!< message waiting indicator
    //       QMap<QString, QHash<QString, QString> > m_termstatus;
    QMap<QString, PhoneInfo *> m_phones;    //!< map to phones
};

#endif

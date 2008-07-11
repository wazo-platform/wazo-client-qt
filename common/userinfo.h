/* XIVO CTI clients
 * Copyright (C) 2007, 2008  Proformatique
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

#include <QHash>
#include <QStringList>

class UserInfo
{
public:
	UserInfo(const QString &);
        ~UserInfo();
        void setAvailState(const QString &);
        void setFullName(const QString &);
        void setNumber(const QString &);
        void setPhones(const QString &,
                       const QString &,
                       const QString &);
        void setAgent(const QString &);
        void setMWI(const QString &,
                    const QString &,
                    const QString &);
        bool hasPhone(const QString &,
                      const QString &,
                      const QString &);
        bool hasAgent(const QString &,
                      const QString &);
        const QString & phonenum() const;
        const QString & fullname() const;
        const QString & mwi() const;
        const QString & userid() const;
        const QString & agentid() const;
        const QString & astid() const;
        const QString & availstate() const;
        const QStringList & termlist() const;
        const QHash<QString, QString> & termstatus() const;
        void updatePhoneStatus(const QString &,
                               const QString &);
private:
        QString m_userid;
        QString m_fullname;

        QString m_availstate;

        QString m_astid;
        QString m_context;
        QString m_phonenum;
        QStringList m_termlist;
        QString m_agentnum;
        QString m_agentphonenum;
        QString m_mwi;
        QHash<QString, QString> m_termstatus;
};

#endif

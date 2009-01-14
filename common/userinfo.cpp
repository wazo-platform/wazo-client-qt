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

#include <QDebug>

#include "userinfo.h"

UserInfo::UserInfo(const QString & userid)
        : m_fullname("")
{
        m_userid = userid;
}


UserInfo::~UserInfo()
{
}

void UserInfo::setFullName(const QString & fullname)
{
        m_fullname = fullname;
}

void UserInfo::setCtiLogin(const QString & ctilogin)
{
        m_ctilogin = ctilogin;
}

void UserInfo::setPhoneNumber(const QString & phonenum)
{
        m_phonenum = phonenum;
}

void UserInfo::setVoiceMailNumber(const QString & voicemailnum)
{
        m_voicemailnum = voicemailnum;
}

void UserInfo::setPhones(const QString & astid,
                         const QStringList & termlist)
{
        m_astid = astid;
        foreach(QString term, termlist)
                m_termstatus[term] = QVariant();
}

void UserInfo::setAvailState(const QVariant & availstate)
{
        m_availstate = availstate;
}

void UserInfo::setAgent(const QString & agentnum)
{
        m_agentnum = agentnum;
}

void UserInfo::setMWI(const QStringList & mwi)
{
        m_mwi = mwi;
}

void UserInfo::updatePhoneStatus(const QString & term,
                                 const QVariant & status)
{
        m_termstatus[term] = status;
}

bool UserInfo::hasPhone(const QString & astid,
                        const QString & term)
{
        if((m_astid == astid) && m_termstatus.keys().contains(term))
                return true;
        else
                return false;
}

bool UserInfo::hasAgent(const QString & astid,
                        const QString & agentnum)
{
        if((m_astid == astid) && m_agentnum == agentnum)
                return true;
        else
                return false;
}


const QString & UserInfo::fullname() const
{
        return m_fullname;
}

const QString & UserInfo::phonenumber() const
{
        return m_phonenum;
}

const QString & UserInfo::voicemailnumber() const
{
        return m_voicemailnum;
}

const QString & UserInfo::userid() const
{
        return m_userid;
}

const QString & UserInfo::ctilogin() const
{
        return m_ctilogin;
}

const QStringList & UserInfo::mwi() const
{
        return m_mwi;
}

const QString & UserInfo::agentid() const
{
        return m_agentnum;
}

const QString & UserInfo::astid() const
{
        return m_astid;
}

const QVariant & UserInfo::availstate() const
{
        return m_availstate;
}

const QVariantMap & UserInfo::termstatus() const
{
        return m_termstatus;
}

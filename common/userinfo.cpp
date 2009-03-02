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

#include <QDebug>

#include "userinfo.h"
#include "phoneinfo.h"

/*! \brief Constructor
 *
 * just set userid
 */
UserInfo::UserInfo(const QString & userid)
    : m_fullname("")
{
    m_userid = userid;
}


/*! \brief Destructor */
UserInfo::~UserInfo()
{
    //qDebug() << "UserInfo::~UserInfo()" << m_userid;
}

/*! \brief set full name */
void UserInfo::setFullName(const QString & fullname)
{
    m_fullname = fullname;
}

/*! \brief set CTI Login */
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

/*! \brief set phone list
 *
 * fill m_phones Hash with pointers to PhoneInfo objects
 */
void UserInfo::setPhones(const QString & astid,
                         const QStringList & termlist,
                         const QHash<QString, PhoneInfo *> & phones)
{
    //qDebug() << "UserInfo::setPhones" << astid << termlist;
    m_astid = astid;
    foreach(const QString term, termlist)
        {
            PhoneInfo * pi = NULL;
            QString key = astid + "." + term;
            if(phones.contains(key))
                pi = phones[key];
            m_phones[term] = pi;
        }
}

/*! \brief update availability state */
void UserInfo::setAvailState(const QVariant & availstate)
{
    m_availstate.clear();
    const QMap<QString, QVariant> map = availstate.toMap();
    foreach(const QString key, map.keys())
        {
            m_availstate[key] = map[key].toString();
        }
}

void UserInfo::setAgent(const QString & agentnum)
{
    m_agentnum = agentnum;
}

/*! \brief set Message Waiting indicator */
void UserInfo::setMWI(const QStringList & mwi)
{
    m_mwi = mwi;
}

/*! \brief check if this user has this phone */
bool UserInfo::hasPhone(const QString & astid,
                        const QString & term)
{
    //    qDebug() << "UserInfo::hasPhone" << term << m_phones;
    return ((m_astid == astid) && m_phones.keys().contains(term));
}

/*! \brief check if this user has this agent */
bool UserInfo::hasAgent(const QString & astid,
                        const QString & agentnum)
{
    return ((m_astid == astid) && m_agentnum == agentnum);
}

/*! \brief return m_fullname */
const QString & UserInfo::fullname() const
{
    return m_fullname;
}

/*! \brief return m_phonenum */
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

/*! \brief return a list of contexts where this user has its phones
 */
const QStringList UserInfo::contexts() const
{
    QStringList clist;
    //qDebug() << m_phones;
    foreach(const QString key, m_phones.keys())
        {
            //if(m_phones[key])
            //    clist << m_phones[key]->context();
            clist << (key.split(QChar('.')))[1];
        }
    return clist;
}

const QString & UserInfo::astid() const
{
    return m_astid;
}

const QHash<QString, QString> & UserInfo::availstate() const
{
    return m_availstate;
}

/*! \brief return list of phones identifier */
QList<QString> UserInfo::phonelist() const
{
    return m_phones.keys();
}

const PhoneInfo * UserInfo::getPhoneInfo(const QString & id) const
{
    if(m_phones.contains(id))
        return m_phones[id];
    QString key = m_astid + "." + id;
    if(m_phones.contains(key))
        return m_phones[key];
    return NULL;
}

void UserInfo::updatePhone( PhoneInfo * pi )
{
    //qDebug() << "UserInfo::updatePhone before" << m_phones;
    if(pi)
        {
            QString key = pi->tech() + "." + pi->context() + "." + pi->phoneid() + "." + pi->number();
            m_phones[key] = pi;
        }
    //qDebug() << "UserInfo::updatePhone after " << m_phones;
}

/*! \brief list channels of this user
 *
 * iterate through the list comms of each phones.
 */
QList<QString> UserInfo::channelList() const
{
    QList<QString> list;
    QMapIterator<QString, PhoneInfo *> it = QMapIterator<QString, PhoneInfo *>(m_phones);
    while(it.hasNext())
        {
            it.next();
            if(it.value())
                {
                    QMapIterator<QString, QVariant> itphone( it.value()->comms() );
                    while( itphone.hasNext() )
                        {
                            itphone.next();
                            QVariantMap qvm = itphone.value().toMap();
                            list << qvm["thischannel"].toString();
                        }
                }
        }
    return list;
}


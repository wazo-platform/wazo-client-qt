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

#include "agentinfo.h"

AgentInfo::AgentInfo(const QString & astid,
                     const QMap<QString, QVariant> & prop)
    : m_astid(astid)
{
    m_context = prop["context"].toString();
    m_agentnumber = prop["number"].toString();
    QString firstname = prop["firstname"].toString();
    QString lastname = prop["lastname"].toString();
    m_fullname = QString("%1 %2").arg(firstname).arg(lastname);
    update(prop);
}

AgentInfo::~AgentInfo()
{
}

bool AgentInfo::update(const QMap<QString, QVariant> & prop)
{
    if (m_properties == prop)
        return false;
    else {
        m_properties = prop;
        return true;
    }
}

const QString & AgentInfo::astid() const
{
    return m_astid;
}

const QString & AgentInfo::context() const
{
    return m_context;
}

const QString & AgentInfo::agentnumber() const
{
    return m_agentnumber;
}

const QString & AgentInfo::fullname() const
{
    return m_fullname;
}

const QVariantMap & AgentInfo::properties() const
{
    return m_properties;
}

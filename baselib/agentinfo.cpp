/* XiVO Client
 * Copyright (C) 2007-2009, Proformatique
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

#include "agentinfo.h"

AgentInfo::AgentInfo(const QString & astid,
                     const QString & id,
                     const QMap<QString, QVariant> & prop)
    : m_astid(astid), m_id(id)
{
    update(prop);
}

AgentInfo::~AgentInfo()
{
}

bool AgentInfo::update(const QMap<QString, QVariant> & prop)
{
    bool haschanged = false;
    if (m_properties != prop) {
        m_properties = prop;
        haschanged = true;
        
        m_context = prop.value("context").toString();
        m_agentnumber = prop.value("number").toString();
        QString firstname = prop.value("firstname").toString();
        QString lastname = prop.value("lastname").toString();
        m_fullname = QString("%1 %2").arg(firstname).arg(lastname);
    }
    return haschanged;
}

bool AgentInfo::updateQueue(const QMap<QString, QVariant> & prop)
{
    bool haschanged = false;
    QMapIterator<QString, QVariant> it(prop);
    while(it.hasNext()) {
        it.next();
        QString arg = it.key();
        if(! m_properties.contains(arg)) {
            haschanged = true;
            m_properties[arg] = it.value();
        } else if(m_properties.value(arg) != it.value()) {
            haschanged = true;
            QVariantMap tmp = m_properties.value(arg).toMap();
            QMapIterator<QString, QVariant> it2(it.value().toMap());
            while(it2.hasNext()) {
                it2.next();
                if(tmp.value(it2.key()) != it2.value()) {
                    tmp[it2.key()] = it2.value();
                }
            }
            m_properties[arg] = tmp;
        }
    }
    return haschanged;
}

const QString & AgentInfo::astid() const
{
    return m_astid;
}

const QString & AgentInfo::id() const
{
    return m_id;
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

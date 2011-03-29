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
#include "groupinfo.h"

GroupInfo::GroupInfo(const QString & ipbxid,
                     const QString & id)
    : XInfo(ipbxid, id)
{
}

bool GroupInfo::updateConfig(const QVariantMap & prop)
{
    bool haschanged = false;
    m_context = prop.value("context").toString();
    m_groupname = prop.value("name").toString();
    m_groupnumber = prop.value("number").toString();
    if (m_properties != prop) {
        m_properties = prop;
        haschanged = true;
    }
    return haschanged;
}

bool GroupInfo::updateStatus(const QVariantMap &)
{
    bool haschanged = true;
    return haschanged;
}

bool GroupInfo::updateAgent(const QVariantMap & prop)
{
    bool haschanged = false;
    QMapIterator<QString, QVariant> it(prop);
    while (it.hasNext()) {
        it.next();
        QString arg = it.key();
        if (!m_properties.contains(arg)) {
            haschanged = true;
            m_properties[arg] = it.value();
        } else if (m_properties.value(arg) != it.value()) {
            haschanged = true;
            QVariantMap tmp = m_properties.value(arg).toMap();
            QMapIterator<QString, QVariant> it2(it.value().toMap());
            while (it2.hasNext()) {
                it2.next();
                if (tmp.value(it2.key()) != it2.value()) {
                    tmp[it2.key()] = it2.value();
                }
            }
            m_properties[arg] = tmp;
        }
    }
    return haschanged;
}

const QString & GroupInfo::context() const
{
    return m_context;
}

const QVariantMap & GroupInfo::properties() const
{
    return m_properties;
}

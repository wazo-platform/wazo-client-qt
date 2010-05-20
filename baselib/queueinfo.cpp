/* XiVO Client
 * Copyright (C) 2007-2010, Proformatique
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
#include "queueinfo.h"

QueueInfo::QueueInfo(const QString & astid,
                     const QString & id,
                     const QMap<QString, QVariant> & prop)
    : m_astid(astid), m_id(id)
{
    m_context = prop.value("context").toString();
    m_queuename = prop.value("queuename").toString();
    m_number = prop.value("number").toString();
    update(prop);
}

bool QueueInfo::update(const QMap<QString, QVariant> & prop)
{
    bool haschanged = false;
    if (m_properties != prop) {
        m_properties = prop;
        haschanged = true;
    }
    return haschanged;
}

bool QueueInfo::updateAgent(const QMap<QString, QVariant> & prop)
{
    bool haschanged = false;
    QMapIterator<QString, QVariant> it(prop);
    while(it.hasNext()) {
        it.next();
        QString arg = it.key();
        if(!m_properties.contains(arg)) {
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

const QString & QueueInfo::astid() const
{
    return m_astid;
}

const QString & QueueInfo::id() const
{
    return m_id;
}

const QString & QueueInfo::context() const
{
    return m_context;
}

const QString & QueueInfo::queuename() const
{
    return m_queuename;
}

const QVariantMap & QueueInfo::properties() const
{
    return m_properties;
}

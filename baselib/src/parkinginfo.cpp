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

#include "parkinginfo.h"

ParkingInfo::ParkingInfo(const QString & ipbxid, const QString & id)
  :XInfo(ipbxid, id)
{
    // qDebug() << Q_FUNC_INFO << QString("%1/%2").arg(ipbxid).arg(id);
}

// ParkingInfo::ParkingInfo(const ParkingInfo &other)
//   :XInfo(other.ipbxid(), other.id())
// {
//     m_timeout = other.m_timeout;
//     m_parkingtime = other.m_parkingtime;
// }

int ParkingInfo::countParked() const
{
    int count = 0;
    foreach (const QString bay, m_parking_bays.keys()) {
        if (m_parking_bays[bay].toMap().count()) {
            ++count;
        }
    }
    return count;
}

void ParkingInfo::update(const QVariantMap &map)
{
    m_timeout = map.value("timeout").toInt();
    m_parkingtime = map.value("parkingtime").toDouble();
}

bool ParkingInfo::updateConfig(const QVariantMap & prop)
{
    // qDebug() << Q_FUNC_INFO << prop;
    bool haschanged = false;

    haschanged |= setIfChangeString(prop, "context", & m_context);
    haschanged |= setIfChangeString(prop, "extension", & m_exten);
    haschanged |= setIfChangeString(prop, "name", & m_name);
    haschanged |= setIfChangeString(prop, "positions", & m_positions);
    haschanged |= setIfChangeString(prop, "description", & m_description);
    haschanged |= setIfChangeInt(prop, "duration", & m_timeout);

    return haschanged;
}

bool ParkingInfo::updateStatus(const QVariantMap & prop)
{
    // qDebug() << Q_FUNC_INFO << prop;
    bool haschanged = false;
    foreach (const QString id, prop.keys()) {
        if (! m_parking_bays.contains(id) ||
            m_parking_bays[id].toMap() != prop[id].toMap()) {
            m_parking_bays[id] = prop[id];
            haschanged = true;
        }
    }
    return haschanged;
}

/*! \brief returns the string representation of a parkinglot */
QString ParkingInfo::toString() const
{
    return QString("%1 <%2> %3").arg(m_name).arg(m_exten).arg(m_description);
}

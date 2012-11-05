/* XiVO Client
 * Copyright (C) 2007-2011, Avencall
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

#ifndef __PARKINGINFO_H__
#define __PARKINGINFO_H__

#include "baselib_export.h"
#include "xinfo.h"

#include <QHash>
#include <QVariant>

/*! \brief for storing parking (conference room) infos
 *
 * All attributes use implicitly shared data
 * so it would be not too costy to copy/return this
 * class.
 */
class BASELIB_EXPORT ParkingInfo : public XInfo
{
    public:
        ParkingInfo(const QString &, const QString &);
        //! Copy constructor. Just copy all attributes
        // ParkingInfo(const ParkingInfo &);
        const int & timeout() const { return m_timeout; };  //! parking timeout
        int countParked() const; //! Number of parked calls
        void update(const QVariantMap &);
        bool updateConfig(const QVariantMap &); //! update parking properties
        bool updateStatus(const QVariantMap &); //! update parking status
        bool parkedHere(const QString &) const; //! Check if a xchannel is parked
        QString toString() const; //! Returns the string representation of a parkinglot
        const QString & description() const;
        const QString & name() const;
        const QString & number() const;
        const QHash<QString, QVariant> & parkingBays() const;
    private:
        int m_timeout;  //!< parking timeout
        double m_parkingtime;  //!< parking starting time
        QString m_context; //!< parking context
        QString m_description; //!< parking description
        QString m_exten; //!< parking exten
        QString m_name; //!< parking name
        QString m_positions; //!< number of available positions
        QHash<QString, QVariant> m_parking_bays; //!< Parked calls list
};

inline const QString & ParkingInfo::description() const
{
    return m_description;
}

inline const QString & ParkingInfo::name() const
{
    return m_name;
}

inline const QString & ParkingInfo::number() const
{
    return m_exten;
}

inline const QHash<QString, QVariant> & ParkingInfo::parkingBays() const
{
    return m_parking_bays;
}

#endif /* __PARKINGINFO_H__ */

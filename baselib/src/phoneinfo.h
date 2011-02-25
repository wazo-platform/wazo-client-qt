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

#ifndef __PHONEINFO_H__
#define __PHONEINFO_H__

#include "baselib_export.h"
#include <QString>
#include <QStringList>
#include <QVariant>
#include <QVariantMap>

/*! \brief Store Phone information
 */
class BASELIB_EXPORT PhoneInfo
{
    public:
        PhoneInfo(const QString &, const QString &);  //! constructor
        bool updateConfig(const QVariantMap &);  //! update config members
        bool updateStatus(const QVariantMap &);  //! update status members
        const QString & context() const { return m_context; };  //! context this phone belongs to
        const QString & ipbxid() const { return m_ipbxid; };  //! asterisk id
        const QString & number() const { return m_number; };  //! phone number
        const QString & protocol() const { return m_protocol; };  //! phone technology (sip, iax, etc...)
        const QString & phoneid() const { return m_phoneid; };  //! phone id
        int simultcalls() const { return m_simultcalls; };  //! phone simultcalls

        const QStringList & channels() const { return m_channels; };  //! current communications of this phone
        const QString & hintstatus() const { return m_hintstatus; }; //! hint status value

    private:
        QString m_ipbxid;
        QString m_phoneid;

        QString m_protocol;
        QString m_context;
        QString m_number;
        int m_simultcalls;

        bool m_initialized;
        bool m_enable_hint;
        QString m_hintstatus;
        QStringList m_channels;
};

#endif

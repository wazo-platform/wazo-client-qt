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
#include "xinfo.h"

/*! \brief Store Phone information
 */

class BASELIB_EXPORT PhoneInfo : public XInfo
{
    public:
        PhoneInfo(const QString &, const QString &);  //! constructor
        bool updateConfig(const QVariantMap &);  //! update config members
        bool updateStatus(const QVariantMap &);  //! update status members
        const QString & context() const { return m_context; };  //! context this phone belongs to
        const QString & number() const { return m_number; };  //! phone number
        const QString & identity() const { return m_identity; }; //! phone identity (sip/abc123)
        const QString & protocol() const { return m_protocol; };  //! phone technology (sip, iax, etc...)
        const QString & iduserfeatures() const { return m_iduserfeatures; };  //! user relation
        int simultcalls() const { return m_simultcalls; };  //! phone simultcalls

        const QStringList & channels() const { return m_channels; };  //! current communications of this phone
        const QStringList & xchannels() const { return m_xchannels; };  //! current communications of this phone
        const QString & hintstatus() const { return m_hintstatus; }; //! hint status value

        bool enableunc() const { return m_enableunc; };
        QString toString() const;
    private:
        QString m_protocol;
        QString m_context;
        QString m_number;
        QString m_identity; //! The phone's protocol/name
        QString m_iduserfeatures;
        int m_simultcalls;

        bool m_initialized;
        bool m_enable_hint;
        QString m_hintstatus;
        QStringList m_channels;
        QStringList m_xchannels;

        bool m_enablerna;
        bool m_enableunc;
        bool m_enablebusy;
        QString m_destrna;
        QString m_destunc;
        QString m_destbusy;

        bool m_enableautomon;
        bool m_enablednd;
        bool m_enablevoicemail;
        bool m_enablexfer;
        bool m_incallfilter;
        bool m_callrecord;
};

namespace phone {

/*! \brief Find a phone for a given phone identity
 * \param identity The phone's protocol/id
 * \return the PhoneInfo or NULL */
const PhoneInfo * findByIdentity(const QString & identity);

} // namespace phone

#endif

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

#ifndef __PHONEINFO_H__
#define __PHONEINFO_H__

#include "baselib_export.h"
#include <QString>
#include <QStringList>
#include <QVariant>
#include <QMap>

/*! \brief Store Phone information
 */
class BASELIB_EXPORT PhoneInfo
{
public:
    //! constructor
    PhoneInfo(const QString & astid,
              const QMap<QString, QVariant> & prop);
    //! destructor
    ~PhoneInfo();
    //! update attribute members
    void update(const QMap<QString, QVariant> & prop);
    //! context this phone belongs to
    const QString & context() const { return m_context; };
    //! access to the status values of this phone
    const QString hintstatus(const QString & key) const {
        return m_hintstatus.contains(key) ? m_hintstatus.value(key) : QString("");
    };
    //! asterisk id
    const QString & astid() const { return m_astid; };
    //! phone number
    const QString & number() const { return m_number; };
    //! phone technology (sip, iax, etc...)
    const QString & tech() const { return m_tech; };
    //! phone id
    const QString & phoneid() const { return m_phoneid; };
    //! current communications of this phone
    const QMap<QString, QVariant> & comms() const { return m_comms; };
private:
    QString m_astid;
    QString m_tech;
    QString m_context;
    QString m_phoneid;
    QString m_number;
    bool m_initialized;
    bool m_enable_hint;
    QMap<QString, QString> m_hintstatus;
    QMap<QString, QVariant> m_comms;
};

#endif

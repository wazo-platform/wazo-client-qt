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

#ifndef __QUEUEINFO_H__
#define __QUEUEINFO_H__

#include "baselib_export.h"
#include <QString>
#include <QVariant>
#include <QMap>
#include "xinfo.h"

/*! \brief Store Queue information
 */
class BASELIB_EXPORT QueueInfo : public XInfo
{
    public:
        QueueInfo(const QString &, const QString &);  //! constructor
        bool updateConfig(const QVariantMap &);  //! update config members
        bool updateStatus(const QVariantMap &);  //! update status members
        const QString & context() const { return m_context; };  //! context this queue belongs to
        const QString & queueNumber() const { return m_number; };  //! queue number
        const QString & queueName() const  { return m_name; };  //! queue name

        const QVariantMap & properties() const { return m_properties; };  //! queue properties
        const QStringList & xincalls() const { return m_xincalls; };  //! incoming channel calls

        const QStringList & xagentids() const { return m_xagentids; };  //! agent members of this queue
        const QStringList & agentmembers() const { return m_agentmembers; };  //! agent members of this queue
        const QStringList & xphoneids() const { return m_xphoneids; };  //! phone members of this queue
        const QStringList & phonemembers() const { return m_phonemembers; };  //! phone members of this queue
        const QStringList & xtrunkids() const { return m_xtrunkids; };  //! trunk members of this queue
        const QStringList & trunkmembers() const { return m_trunkmembers; };  //! trunk members of this queue
        QString reference(const QString &, const QString &) const;

    private:
        QString m_context;
        QString m_name;
        QString m_number;

        QStringList m_xagentids;
        QStringList m_agentmembers;
        QStringList m_xphoneids;
        QStringList m_phonemembers;
        QStringList m_xtrunkids;
        QStringList m_trunkmembers;

        QStringList m_xincalls;
        QVariantMap m_properties;
};

#endif

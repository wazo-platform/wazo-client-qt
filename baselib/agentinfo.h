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

#ifndef __AGENTINFO_H__
#define __AGENTINFO_H__

#include "baselib_export.h"
#include <QString>
#include <QVariant>
#include <QMap>

/*! \brief Store Agent information
 */
class BASELIB_EXPORT AgentInfo
{
    public:
        AgentInfo(const QString &, const QString &, const QMap<QString, QVariant> &); //! constructor

        bool update(const QMap<QString, QVariant> &);  //! update attribute members
        bool updateQueue(const QMap<QString, QVariant> &);
        const QString &astid() const;  //! asterisk this agent belongs to
        const QString &id() const;  //! reference id of this agent on the server
        const QString &context() const;  //! context this agent belongs to
        const QString hintstatus(const QString &key) const {
            return m_hintstatus.contains(key) ? m_hintstatus.value(key) : QString("");
        };  //! access to the status values of this agent
        const QString &agentNumber() const;  //! agent number
        const QString &fullname() const;  //! agent fullname
        //! agent properties
        const QVariantMap& properties() const;
        //! current communications of this agent
        const QMap<QString, QVariant>& comms() const { return m_comms; };

    private:
        QString m_astid;
        QString m_id;
        QString m_context;
        QString m_agentname;
        QString m_agentnumber;
        QString m_fullname;
        QMap<QString, QString> m_hintstatus;
        QMap<QString, QVariant> m_comms;
        QVariantMap m_properties;
};

#endif

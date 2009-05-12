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

#ifndef __QUEUEINFO_H__
#define __QUEUEINFO_H__

#include <QString>
#include <QVariant>
#include <QMap>

/*! \brief Store Queue information
 */
class QueueInfo
{
 public:
    //! constructor
    QueueInfo(const QString & astid,
              const QMap<QString, QVariant> & prop);
    //! destructor
    ~QueueInfo();
    //! update attribute members
    bool update(const QMap<QString, QVariant> & prop);
    //! update attribute members
    bool updateAgent(const QMap<QString, QVariant> & prop);
    //! asterisk this queue belongs to
    const QString & astid() const;
    //! context this queue belongs to
    const QString & context() const;
    //! access to the status values of this queue
    const QString hintstatus(const QString & key) const {
        return m_hintstatus.contains(key) ? m_hintstatus.value(key) : QString("");
    };
    //! queue number
    const QString & number() const { return m_number; };
    //! queuename
    const QString & queuename() const;
    //! queue properties
    const QVariantMap & properties() const;
    //! current communications of this queue
    const QMap<QString, QVariant> & comms() const { return m_comms; };
 private:
    QString m_astid;
    QString m_context;
    QString m_queuename;
    QString m_number;
    QMap<QString, QString> m_hintstatus;
    QMap<QString, QVariant> m_comms;
    QVariantMap m_properties;
};

#endif

/* XiVO Client
 * Copyright (C) 2007-2013, Avencall
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

#ifndef __XINFO_H__
#define __XINFO_H__

#include "baselib_export.h"
#include <QString>
#include <QStringList>
#include <QVariant>
#include <QVariantMap>

class BASELIB_EXPORT XInfo
{
    public:
        XInfo(const QString &, const QString &);  //!< constructor
        virtual ~XInfo() {};
        bool setIfChangeString(const QVariantMap &, const char * const, QString * const);
        bool setIfChangeBool(const QVariantMap &, const char * const, bool * const);
        bool setIfChangeInt(const QVariantMap &, const char * const, int * const);
        bool setIfChangeDouble(const QVariantMap &, const char * const, double * const);

        //! IPBX this object belongs to
        const QString & ipbxid() const { return m_ipbxid; };
        //! reference id of this object on the server
        const QString & id() const { return m_id; };
        //! reference xid of this object
        const QString & xid() const { return m_xid; };

        //! update config members
        virtual bool updateConfig(const QVariantMap &) { return false; };
        //! update status members
        virtual bool updateStatus(const QVariantMap &) { return false; };
    protected:
        QString m_ipbxid;
        QString m_id;
        QString m_xid;
};


template <class T>
XInfo * newXInfo(const QString & ipbxid,
                 const QString & id)
{
        return new T(ipbxid, id);
}

typedef XInfo* (*newXInfoProto)(const QString &, const QString &);

#endif

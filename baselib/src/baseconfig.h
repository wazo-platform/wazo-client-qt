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

#ifndef __BASECONFIG_H__
#define __BASECONFIG_H__

#include <baselib_export.h>
#include <QObject>
#include <QVariantMap>
#include <QStringList>

/*! \brief Stores the config of BaseEngine
 * It associates keys to values.
 * These values can be read-write or read-only (frozen).
 * To exchange informations with this class, you can use operator[] or toQVariantMap() and merge().
 */
class BASELIB_EXPORT BaseConfig: public QObject
{
    Q_OBJECT
    
    public:
        
        BaseConfig();
        const QVariant operator[](const QString &) const;
        QVariant & operator[](const QString &);
        QVariantMap getSubSet (const QString &);
        bool isFrozen(const QString &);
        QVariantMap toQVariantMap();
        void merge (QVariantMap extern_qvm);
        
    private:
    
        void setFreezeMode (bool);
        
        QVariantMap m_qvm;
        QStringList m_frozen;
        QVariant m_blackhole;
        bool m_freezemode;
    
    friend class BaseEngine;
};

#endif /* __BASECONFIG_H__ */

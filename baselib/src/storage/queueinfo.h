/*
 * Copyright 2007-2017 The Wazo Authors  (see the AUTHORS file)
 *
 * This program is free software: you can redistribute it and/or modify
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
 * This programm is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
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
        QueueInfo(const QString &, const QString &);
        bool updateConfig(const QVariantMap &);
        bool updateStatus(const QVariantMap &);
        const QString & context() const;
        const QString & queueNumber() const;
        const QString & queueName() const;
        const QString & queueDisplayName() const;

    private:
        QString m_context;
        QString m_name;
        QString m_displayname;
        QString m_number;
};

#endif

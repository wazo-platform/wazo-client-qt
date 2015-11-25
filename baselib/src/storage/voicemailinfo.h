/* XiVO Client
 * Copyright (C) 2007-2015 Avencall
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

#ifndef __VOICEMAILINFO_H__
#define __VOICEMAILINFO_H__

#include "baselib_export.h"
#include <QString>
#include <QStringList>
#include <QVariant>
#include <QVariantMap>
#include "xinfo.h"

class BASELIB_EXPORT VoiceMailInfo : public XInfo
{
    public:
        VoiceMailInfo(const QString &, const QString &);  //! constructor
        bool updateConfig(const QVariantMap &);  //! update config members
        bool updateStatus(const QVariantMap &);  //! update status members

        const QString & context() const { return m_context; };
        const QString & mailbox() const { return m_mailbox; };
        const QString & fullname() const { return m_fullname; };

        bool waiting() const { return m_waiting; };
        int oldMessages() const { return m_old; };
        int newMessages() const { return m_new; };

    private:
        QString m_context;
        QString m_mailbox;
        QString m_fullname;

        bool m_waiting;
        int m_old;
        int m_new;
};

#endif

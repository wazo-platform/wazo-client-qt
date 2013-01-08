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

#ifndef __QUEUE_MEMBER_INFO_H__
#define __QUEUE_MEMBER_INFO_H__

#include "xinfo.h"
#include <baselib_export.h>
#include <QString>

class BASELIB_EXPORT QueueMemberInfo : public XInfo
{
    public:
        QueueMemberInfo(const QString &, const QString &); //! constructor
        bool updateConfig(const QVariantMap &);  //! update config members
        bool updateStatus(const QVariantMap &);  //! update status members

        const QString & status() const { return m_status; };
        const QString & paused() const { return m_paused; };
        const QString & membership() const { return m_membership; };
        const QString & callstaken() const { return m_callstaken; };
        const QString & penalty() const { return m_penalty; };
        const QString & queueName() const { return m_queue_name; };
        const QString & interface() const { return m_interface; };
        QString agent_or_phone() const;
        QString agentNumber() const;
        QString lastcall() const { return m_lastcall; };
        bool is_agent() const;
        bool isCallingOrBusy() const;
    private:
        QString m_status;
        QString m_paused;
        QString m_membership;
        QString m_callstaken;
        QString m_penalty;
        QString m_queue_name;
        QString m_interface;
        QString m_lastcall;
};

#endif

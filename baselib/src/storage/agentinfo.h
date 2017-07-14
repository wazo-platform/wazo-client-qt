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

#ifndef __AGENTINFO_H__
#define __AGENTINFO_H__

#include "baselib_export.h"
#include <QString>
#include <QVariant>
#include <QMap>
#include "xinfo.h"

enum AgentPauseStatus {
    UNPAUSED,
    PAUSED,
    PARTIALLY_PAUSED
};

class BASELIB_EXPORT AgentInfo : public XInfo
{
    public:
        enum AgentAvailability {
            LOGGED_OUT,
            AVAILABLE,
            UNAVAILABLE,
            ON_CALL_NONACD_INCOMING_INTERNAL,
            ON_CALL_NONACD_INCOMING_EXTERNAL,
            ON_CALL_NONACD_OUTGOING_INTERNAL,
            ON_CALL_NONACD_OUTGOING_EXTERNAL
        };

        AgentInfo(const QString &, const QString &);
        bool updateConfig(const QVariantMap &);
        bool updateStatus(const QVariantMap &);

        const QString & context() const;
        const QString & agentNumber() const;
        const QString & fullname() const;
        const QString & firstname() const;
        const QString & lastname() const;

        bool logged() const;
        enum AgentAvailability availability() const;
        QString availabilitySince() const;
        const QVariantMap & properties() const;

        const QStringList & queue_ids() const;

        int joinedQueueCount() const;
        int pausedQueueCount() const;

        bool paused() const;
        enum AgentPauseStatus pausedStatus() const;
        QStringList pausedQueueNames() const;
        QStringList joinedQueueNames() const;
    private:
        QString m_context;
        QString m_agentnumber;
        QString m_firstname;
        QString m_lastname;

        QString m_fullname;

        QString m_availability;
        double m_availability_since;
        QVariantMap m_properties;

        QStringList m_queue_ids;
};

#endif

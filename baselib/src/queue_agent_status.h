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

#ifndef __QUEUE_AGENT_STATUS_H__
#define __QUEUE_AGENT_STATUS_H__

#include "baselib_export.h"
#include <QColor>
#include <QString>

/*! \brief Store Queue Membership information
 */
class BASELIB_EXPORT QueueAgentStatus: public QObject
{
    Q_OBJECT

    public:
        QueueAgentStatus();

        bool update(const QString &, const QString &, const QString &);  //! update attribute members
        const QString & ipbxid() const;  //! IPBX this queue/agent belongs to
        const QString & id() const;  //! reference id of this queue/agent on the server
        const QString & context() const;  //! context this queue/agent belongs to
        const QString & queueName() const;  //! queuename

        const QColor &display_status_color() const;
        const QString &display_status_queue() const;
        const QString &display_status_logged() const;
        const QString &display_status_membership() const;
        const QString &display_status_paused() const;
        const QColor &display_status_paused_color() const;
        const QString &display_action_join() const;
        const QString &display_action_pause() const;

    private:
        QString m_ipbxid;
        QString m_id;
        QString m_context;
        QString m_queuename;

        QString m_display_status_queue;
        QString m_display_status_logged;
        QString m_display_status_membership;
        QString m_display_status_paused;
        QString m_display_action_join;
        QString m_display_action_pause;

        QColor m_display_status_color;
        QColor m_display_status_paused_color;
        int m_display_status_darkfactor;
};

#endif // __QUEUE_AGENT_STATUS_H__

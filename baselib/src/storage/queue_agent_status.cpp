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

#include <QDebug>
#include "queue_agent_status.h"

const QColor Orange = QColor(255, 128, 0);
QString QueueAgentStatus::logged_out_status = "101";

QueueAgentStatus::QueueAgentStatus()
    : m_display_status_darkfactor(0)
{
}

bool QueueAgentStatus::update(const QString & dynstatus,
                              const QString & sstatus,
                              const QString & pstatus)
{
    m_status = sstatus;
    if(dynstatus == "") {
        m_display_status_membership = "";
        m_display_status_darkfactor = 100;
        m_display_action_join = ":/images/button_ok.png";
    } else if (dynstatus == "dynamic") {
        m_display_status_membership = tr("Dynamic membership");
        m_display_status_darkfactor = 100;
        m_display_action_join = ":/images/cancel.png";
    } else if ((dynstatus == "static") || (dynstatus == "realtime")) {
        m_display_status_membership = tr("Static/RT membership");
        m_display_status_darkfactor = 150;
        m_display_action_join = ":/images/cancel.png";
    } else {
        m_display_status_membership = QString("unknown membership : %1").arg(dynstatus);
        m_display_status_darkfactor = 300;
        m_display_action_join = "";
    }

    QColor basecolor;
    if (sstatus == "") {
        basecolor = Qt::gray;
        m_display_status_queue = tr("Agent not in Queue");
        m_display_status_logged = "";
    } else if (sstatus == "1") {
        basecolor = Qt::green;
        m_display_status_queue = tr("Agent in Queue");
        m_display_status_logged = tr("Logged");
    } else if (sstatus == "2" || sstatus == "3" || sstatus == "6" || sstatus == "7") {
        basecolor = Qt::yellow;
        m_display_status_queue = tr("Agent Called or Busy");
        m_display_status_logged = tr("Logged");
    } else if (sstatus == "4" || sstatus == "5") {
        basecolor = Qt::red;
        m_display_status_queue = tr("Agent in Queue but Invalid");
        m_display_status_logged = "";
    } else if (sstatus == this->logged_out_status) {
        basecolor = Qt::cyan;
        m_display_status_queue = tr("Agent in Queue");
        m_display_status_logged = tr("Unlogged");
    } else {
        basecolor = Qt::black;
        m_display_status_queue = QString("unknown-%1").arg(sstatus);
        m_display_status_logged = "";
    }

    if(pstatus == "0") {
        m_display_status_paused = tr("Unpaused");
        m_display_status_paused_color = Qt::green;
        m_display_action_pause = ":/images/cancel.png";
    } else if(pstatus == "1") {
        m_display_status_paused = tr("Paused");
        m_display_status_paused_color = Orange;
        m_display_action_pause = ":/images/button_ok.png";
    } else if(pstatus == "") {
        m_display_status_paused = tr("Not relevant");
        m_display_status_paused_color = Qt::gray;
        m_display_action_pause = "";
    } else {
        m_display_status_paused = QString("unknown-%1").arg(pstatus);
        m_display_status_paused_color = Qt::black;
        m_display_action_pause = "";
    }

    m_display_status_color = basecolor.darker(m_display_status_darkfactor);
    return true;
}

const QColor & QueueAgentStatus::display_status_color() const
{
    return m_display_status_color;
}

const QColor & QueueAgentStatus::display_status_paused_color() const
{
    return m_display_status_paused_color;
}

const QString & QueueAgentStatus::display_status_queue() const
{
    return m_display_status_queue;
}

const QString & QueueAgentStatus::display_status_logged() const
{
    return m_display_status_logged;
}

const QString & QueueAgentStatus::display_status_membership() const
{
    return m_display_status_membership;
}

const QString & QueueAgentStatus::display_status_paused() const
{
    return m_display_status_paused;
}

const QString & QueueAgentStatus::display_action_join() const
{
    return m_display_action_join;
}

const QString & QueueAgentStatus::display_action_pause() const
{
    return m_display_action_pause;
}

const QString & QueueAgentStatus::ipbxid() const
{
    return m_ipbxid;
}

const QString & QueueAgentStatus::id() const
{
    return m_id;
}

const QString & QueueAgentStatus::context() const
{
    return m_context;
}

const QString & QueueAgentStatus::queueName() const
{
    return m_queuename;
}

bool QueueAgentStatus::is_logged() const
{
    return m_status != this->logged_out_status;
}

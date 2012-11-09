/* XiVO Client
 * Copyright (C) 2007-2011, Avencall
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

#ifdef FUNCTESTS

#include "configwidget.h"

#include "xlets/queue_members/queue_members.h"
#include "xlets/queue_members/queue_members_view.h"
#include "xlets/queue_members/queue_members_sort_filter_proxy_model.h"
#include "xlets/queue_members/queue_members_model.h"

#include "unordered_string_list.h"
#include "remote_control.h"

QString convert_id_to_xid(QString element_id)
{
    return QString("xivo/%1").arg(element_id);
}

void RemoteControl::when_i_enable_the_hide_unlogged_agents_option()
{
    this->i_go_to_the_xivo_client_configuration();

    m_exec_obj.win->m_configwindow->m_hide_unlogged_agents->setChecked(true);

    this->i_close_the_xivo_client_configuration();
}

void RemoteControl::when_i_disable_the_hide_unlogged_agents_option()
{
    this->i_go_to_the_xivo_client_configuration();

    m_exec_obj.win->m_configwindow->m_hide_unlogged_agents->setChecked(false);

    this->i_close_the_xivo_client_configuration();
}

void RemoteControl::then_the_queue_members_xlet_is_empty()
{
    this->queue_members_xlet_is_empty();
}

void RemoteControl::then_the_queue_members_xlet_for_queue_1_is_empty_(const QVariantList & args)
{
    QString queue_id = args[0].toString();
    QString queue_xid = convert_id_to_xid(queue_id);
    emit select_queue(queue_xid);
    this->queue_members_xlet_is_empty();
}

void RemoteControl::queue_members_xlet_is_empty()
{
    XletQueueMembers *xlet = static_cast<XletQueueMembers *>(m_exec_obj.win->m_xletlist.value("queuemembers"));
    this->assert(xlet != NULL, "Queue members xlet is null");

    QueueMembersView *view = xlet->m_view;
    this->assert(view != NULL, "The view is null");

    QAbstractItemModel * model = view->model();
    this->assert(model != NULL, "The model is null");
    this->assert(model->rowCount() == 0, QString("The view is not empty, it has %1 rows")
                 .arg(model->rowCount()));
}

void RemoteControl::then_the_queue_members_xlet_for_queue_1_displays_agents(const QVariantList & args)
{
    QString queue_id = args[0].toString();
    QString queue_xid = convert_id_to_xid(queue_id);
    QVariantList agents = args[1].toList();

    emit select_queue(queue_xid);

    QStringList expected_agent_numbers = this->_extract_agent_numbers(agents);
    QStringList displayed_agent_numbers = this->_get_agent_numbers_from_queue_members();

    this->assert(equalsUnordered(displayed_agent_numbers, expected_agent_numbers),
                 "Queue members displayed wrong elements");
}

QStringList RemoteControl::_extract_agent_numbers(const QVariantList & agents)
{
    QStringList ret;

    foreach(QVariant agent, agents) {
        QVariantMap agent_map = agent.toMap();
        QString agent_number = agent_map.value("number").toString();
        ret << agent_number;
    }

    return ret;
}

QStringList RemoteControl::_get_agent_numbers_from_queue_members()
{
    XletQueueMembers *xlet = static_cast<XletQueueMembers *>(m_exec_obj.win->m_xletlist.value("queuemembers"));
    this->assert(xlet != NULL, "Queue members xlet is null");

    QueueMembersView *view = xlet->m_view;
    this->assert(view != NULL, "The view is null");

    QAbstractItemModel * model = view->model();
    this->assert(model != NULL, "The model is null");

    QStringList agent_numbers;

    int row_count = model->rowCount();
    for(int row_index = 0; row_index < row_count; row_index ++) {
        QString agent_number = model->index(row_index, QueueMembersModel::NUMBER).data().toString();
        agent_numbers << agent_number;
    }

    return agent_numbers;
}

#endif

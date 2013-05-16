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

#ifdef FUNCTESTS

#include <config_widget/config_widget.h>

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

void RemoteControl::set_queue_for_queue_members(const QVariantList & args)
{
    QString queue_id = args[0].toString();
    QString queue_xid = convert_id_to_xid(queue_id);

    connect(this,
            SIGNAL(select_queue(const QString &)),
            b_engine,
            SIGNAL(changeWatchedQueueSignal(const QString &)));

    emit select_queue(queue_xid);
}

QVariantMap RemoteControl::get_queue_members_infos()
{
    XletQueueMembers *xlet = static_cast<XletQueueMembers *>(this->m_xlet_dispatcher->m_xletlist.value("queuemembers"));
    QueueMembersView *view = xlet->m_view;
    QAbstractItemModel * model = view->model();

    QVariantMap return_value;

    return_value["xlet"] = xlet != NULL;
    return_value["view"] = view != NULL;
    return_value["model"] = model != NULL;
    return_value["row_count"] = model->rowCount();

    QStringList agent_numbers;

    int row_count = model->rowCount();
    for(int row_index = 0; row_index < row_count; row_index ++) {
        QString agent_number = model->index(row_index, QueueMembersModel::NUMBER).data().toString();
        agent_numbers << agent_number;
    }
    return_value["members"] = agent_numbers;

    return return_value;
}

#endif

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

#include "xlets/agents/agents.h"
#include "xletlib/agents_model.h"
#include "remote_control.h"

QVariantMap RemoteControl::get_agent_list_infos()
{
    QVariantMap args;
    XletAgents* agents_xlet = static_cast<XletAgents*>(this->m_xlet_container->m_xletlist.value("agents"));
    QAbstractItemModel* agents_model = agents_xlet->findChild<QAbstractItemModel*>("agents_model");

    args["agents_xlet"] = agents_xlet != NULL;
    args["agents_model"] = agents_model != NULL;

    QVariantList content;
    QMap<QString, int> header_data;
    header_data["firstname"] = AgentsModel::FIRSTNAME;
    header_data["lastname"] = AgentsModel::LASTNAME;
    header_data["number"] = AgentsModel::NUMBER;
    header_data["availability"] = AgentsModel::AVAILABILITY;
    header_data["status_label"] = AgentsModel::STATUS_LABEL;
    header_data["status_since"] = AgentsModel::STATUS_SINCE;
    header_data["logged_status"] = AgentsModel::LOGGED_STATUS;
    header_data["joined_queues"] = AgentsModel::JOINED_QUEUES;
    header_data["joined_queue_list"] = AgentsModel::JOINED_QUEUE_LIST;
    header_data["paused_status"] = AgentsModel::PAUSED_STATUS;
    header_data["paused_queues"] = AgentsModel::PAUSED_QUEUES;

    int nb_rows = agents_model->rowCount(QModelIndex());
    for (int row = 0; row < nb_rows; row++) {
        QVariantMap row_data;
        QMap<QString, int>::const_iterator column_map;
        for(column_map = header_data.begin(); column_map != header_data.end(); column_map++) {
            row_data[column_map.key()] = getValueInModel(agents_model, row, column_map.value());
        }
        content.append(row_data);
    }
    args["content"] = content;

    return args;
}

#endif

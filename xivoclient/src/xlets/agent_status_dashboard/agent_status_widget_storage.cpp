/* Copyright (C) 2007-2014 Avencall
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

#include <QDebug>
#include <xletlib/agents_model.h>

#include "agent_status_widget.h"
#include "agent_status_widget_builder.h"
#include "agent_status_widget_storage.h"

AgentStatusWidgetStorage::AgentStatusWidgetStorage(AgentStatusWidgetBuilder * builder)
    : m_builder(builder)
{
}

AgentStatusWidgetStorage::~AgentStatusWidgetStorage()
{
    foreach (AgentStatusWidget *widget, this->m_widgets.values()) {
        delete widget;
    }
}

AgentStatusWidget * AgentStatusWidgetStorage::getWidget(const QModelIndex & index)
{
    const QAbstractItemModel * model = index.model();
    QModelIndex agent_id_index = model->index(index.row(), AgentsModel::ID);
    QString agent_id = model->data(agent_id_index).toString();
    AgentStatusWidget * return_value = NULL;
    if (this->m_widgets.contains(agent_id)) {
        return_value = this->m_widgets.value(agent_id);
    } else if (m_builder == NULL) {
        qDebug() << Q_FUNC_INFO << "Builder is NULL";
    } else {
        AgentStatusWidget * new_widget = this->m_builder->build();
        this->m_widgets.insert(agent_id, new_widget);
        return_value = new_widget;
    }
    return return_value;
}

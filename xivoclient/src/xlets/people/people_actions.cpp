/* Copyright (C) 2015 Avencall
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

#include <baseengine.h>
#include <message_factory.h>

#include "people_actions.h"

PeopleActions::PeopleActions(QList< QPair<QString, ColumnType> > fields, const PeopleEntry &entry, int current_column)
    : m_entry(entry),
      m_call_action(NULL),
      m_call_mobile_action(NULL)
{
    this->m_mobile_column = this->m_number_column = -1;
    for (int i = 0; i < fields.size(); ++i) {
        const QPair<QString, enum ColumnType> &field = fields[i];
        switch(field.second) {
            case MOBILE: {
                this->m_mobile_column = i;
                break;
            }
            default: {
                break;
            }
        }
    }

    if (fields[current_column].second == NUMBER) {
        m_number_column = current_column;
        m_call_action = new QAction(tr("Call"), this);
        connect(m_call_action, SIGNAL(triggered()),
                this, SLOT(call()));
    }

    if (this->m_mobile_column != -1) {
        QString mobile_label = fields.at(this->m_mobile_column).first;
        QVariant mobile_number = this->m_entry.data(this->m_mobile_column);
        if (! mobile_number.isNull()) {
            QString action_name = tr("%1 - %2").arg(mobile_label).arg(mobile_number.toString());
            m_call_mobile_action = new QAction(action_name, this);
            connect(m_call_mobile_action, SIGNAL(triggered()),
                    this, SLOT(callMobile()));
        }
    }
}

QAction *PeopleActions::callAction()
{
    return this->m_call_action;
}

void PeopleActions::call()
{
    QString extension = this->m_entry.data(this->m_number_column).toString();
    b_engine->sendJsonCommand(MessageFactory::dial(extension));
}

QAction *PeopleActions::callMobileAction()
{
    return this->m_call_mobile_action;
}

void PeopleActions::callMobile()
{
    QString extension = this->m_entry.data(this->m_mobile_column).toString();
    b_engine->sendJsonCommand(MessageFactory::dial(extension));
}

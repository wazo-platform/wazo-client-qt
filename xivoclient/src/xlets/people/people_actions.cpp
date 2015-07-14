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
#include "people_enum.h"

PeopleActions::PeopleActions(const QList<QVariant> &action_items)
    : m_call_action(NULL),
      m_call_mobile_action(NULL)
{
    for (int i = 0; i < action_items.size(); i++)
    {
        QVariantMap item = action_items[i].toMap();
        QString label = item["label"].toString();
        QString value = item["value"].toString();
        PeopleAction action = (PeopleAction)item["action"].toInt();

        if (value.isEmpty()) {
            continue;
        }

        if (action == CALL) {
            m_number_extension = value;
            m_call_action = new QAction(tr("Call"), this);
            connect(m_call_action, SIGNAL(triggered()),
                    this, SLOT(call()));

        } else if (action == MOBILECALL) {
            m_mobile_extension = value;
            QString action_name = tr("%1 - %2").arg(label).arg(value);
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
    b_engine->sendJsonCommand(MessageFactory::dial(m_number_extension));
}

QAction *PeopleActions::callMobileAction()
{
    return this->m_call_mobile_action;
}

void PeopleActions::callMobile()
{
    b_engine->sendJsonCommand(MessageFactory::dial(m_mobile_extension));
}

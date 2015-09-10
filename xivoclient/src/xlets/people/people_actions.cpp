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
    : m_call_action(NULL)
{
    for (int i = 0; i < action_items.size(); i++)
    {
        QVariantMap item = action_items[i].toMap();
        QString title = item["label"].toString();
        QString value = item["value"].toString();
        PeopleAction action = (PeopleAction)item["action"].toInt();

        if (value.isEmpty()) {
            continue;
        }

        switch(action) {
        case CALL:
            m_call_action = newCallAction(tr("Call"), value);
            break;
        case CALLABLECALL:
            const QString &label = QString("%1 - %2").arg(title).arg(value);
            QAction *click = newCallAction(label, value);
            m_call_callable_actions.append(click);
            break;
        }
    }
}

QAction *PeopleActions::newCallAction(const QString &label, const QString &number)
{
        QAction *click = new QAction(label, this);
        click->setData(number);
        connect(click, SIGNAL(triggered()), this, SLOT(call()));
        return click;
}

void PeopleActions::call()
{
    const QString &number = static_cast<QAction*>(sender())->data().toString();
    b_engine->sendJsonCommand(MessageFactory::dial(number));
}

QAction *PeopleActions::getCallAction() const
{
    return this->m_call_action;
}

const QList<QAction *> &PeopleActions::getCallCallableActions() const
{
    return this->m_call_callable_actions;
}

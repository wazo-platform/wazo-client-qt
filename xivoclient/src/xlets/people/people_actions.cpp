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

#include <QDesktopServices>
#include <QUrl>
#include <QDebug>
#include <baseengine.h>
#include <message_factory.h>

#include "xletlib/chitchat.h"

#include "people_actions.h"
#include "people_enum.h"

PeopleActions::PeopleActions()
    : QObject(0)
{}

PeopleActions::PeopleActions(const QList<QVariant> &action_items)
    : QObject(0)
{
    for (int i = 0; i < action_items.size(); i++)
    {
        QVariantMap item = action_items[i].toMap();
        QString title = item["label"].toString();
        QString value = item["value"].toString();
        PeopleAction action = static_cast<PeopleAction>(item["action"].toInt());

        switch(action) {
            case CALL:
            {
                m_call_destination = value;
                break;
            }
            case CALLABLE_CALL:
            {
                if (value.isEmpty()) {
                    continue;
                }
                const QString &label = QString("%1 - %2").arg(title).arg(value);
                m_call_callable_destinations.append(QStringPair(label, value));
                break;
            case CHAT:
            {
                m_chat_action = newChatAction(item["value"].toList());
                break;
            }
            case BLIND_TRANSFER:
            {
                if (value.isEmpty()) {
                    continue;
                }
                const QString &label = QString("%1 - %2").arg(title).arg(value);
                m_blind_transfer_destinations.append(QStringPair(label, value));
                break;
            }
            case ATTENDED_TRANSFER:
            {
                if (value.isEmpty()) {
                    continue;
                }
                const QString &label = QString("%1 - %2").arg(title).arg(value);
                m_attended_transfer_destinations.append(QStringPair(label, value));
                break;
            }
            case MAILTO:
            {
                const QString &label = QString("%1 - %2").arg(title).arg(value);
                m_mailto_destinations.append(QStringPair(label, value));
                break;
            }
        }
    }
}

PeopleActions::PeopleActions(const PeopleActions &other)
    : QObject(0),
      m_call_callable_destinations(other.m_call_callable_destinations),
      m_blind_transfer_destinations(other.m_blind_transfer_destinations),
      m_attended_transfer_destinations(other.m_attended_transfer_destinations),
      m_mailto_destinations(other.m_mailto_destinations),
      m_call_destination(other.m_call_destination),
      m_chat_destination(other.m_chat_destination)
{}

void PeopleActions::attendedTransfer()
{
    const QString &number = static_cast<QAction*>(sender())->data().toString();
    b_engine->sendJsonCommand(MessageFactory::attendedTransfer(number));
}

void PeopleActions::blindTransfer()
{
    const QString &number = static_cast<QAction*>(sender())->data().toString();
    b_engine->sendJsonCommand(MessageFactory::directTransfer(number));
}

void PeopleActions::call()
{
    const QString &number = static_cast<QAction*>(sender())->data().toString();
    b_engine->sendJsonCommand(MessageFactory::dial(number));
}

void PeopleActions::mailto()
{
    const QString &email = static_cast<QAction*>(sender())->data().toString();
    QDesktopServices::openUrl(QUrl(QString("mailto:%1").arg(email)));
}

void PeopleActions::chat()
{
    const QVariantList &params = static_cast<QAction*>(sender())->data().toList();
    const QString &name = params[0].toString();
    const QString &xivo_uuid = params[1].toString();
    int user_id = params[2].toInt();

    ChitChatWindow::chitchat_instance->writeMessageTo(name, xivo_uuid, user_id);
}

QAction *PeopleActions::newCallAction(QWidget *parent) const
{
    if (m_call_destination.isEmpty()) {
        return NULL;
    }

    QAction *action = new QAction(tr("Call"), parent);
    action->setData(m_call_destination);
    connect(action, SIGNAL(triggered()), this, SLOT(call()));
    return action;
}

QAction *PeopleActions::newChatAction(QWidget *parent) const
{
    if (m_chat_destination.isEmpty()) {
        return NULL;
    }

    QAction *action = new QAction(tr("Send a message"), parent);
    action->setData(m_chat_destination);
    connect(action, SIGNAL(triggered()), this, SLOT(chat()));
    return action;
}

QList<QAction *> PeopleActions::newAttendedTransferActions(QWidget *parent) const
{
    QList<QAction*> actions;

    foreach (const QStringPair &label_number, m_attended_transfer_destinations) {
        QAction *action = new QAction(label_number.first, parent);
        action->setData(label_number.second);
        connect(action, SIGNAL(triggered()), this, SLOT(attendedTransfer()));
        actions.append(action);
    }

    return actions;
}

QList<QAction *> PeopleActions::newBlindTransferActions(QWidget *parent) const
{
    QList<QAction*> actions;

    foreach (const QStringPair &label_number, m_blind_transfer_destinations) {
        QAction *action = new QAction(label_number.first, parent);
        action->setData(label_number.second);
        connect(action, SIGNAL(triggered()), this, SLOT(blindTransfer()));
        actions.append(action);
    }

    return actions;
}

QList<QAction *> PeopleActions::newCallCallableActions(QWidget *parent) const
{
    QList<QAction*> actions;

    foreach (const QStringPair &label_number, m_call_callable_destinations) {
        QAction *action = new QAction(label_number.first, parent);
        action->setData(label_number.second);
        connect(action, SIGNAL(triggered()), this, SLOT(call()));
        actions.append(action);
    }

    return actions;
}

const QList<QAction *> PeopleActions::getMailtoActions(QWidget *parent) const
{
    QList<QAction *> actions;

    foreach (const QStringPair &label_email, m_mailto_destinations) {
        QAction *action = new QAction(label_email.first, parent);
        action->setData(label_email.second);
        connect(action, SIGNAL(triggered()), this, SLOT(mailto()));
    }

    return actions;
}

bool PeopleActions::hasCallCallables() const
{
    return m_call_callable_destinations.isEmpty() == false;
};

bool PeopleActions::hasChat() const
{
    return m_chat_destination.isEmpty() == false;
}

bool PeopleActions::hasTransfers() const
{
    return m_blind_transfer_destinations.isEmpty() == false
        || m_attended_transfer_destinations.isEmpty() == false;
}

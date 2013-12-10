/* XiVO Client
 * Copyright (C) 2012-2014 Avencall
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

#include <QDebug>
#include <QKeyEvent>
#include <QShortcut>

#include <baseengine.h>
#include <dao/queuedao.h>
#include <id_converter.h>
#include <message_factory.h>

#include <storage/queueinfo.h>
#include <storage/phoneinfo.h>

#include <xletlib/queue_entries/queue_entries_model.h>
#include <xletlib/queue_entries/queue_entries_sort_filter_proxy_model.h>
#include <xletlib/queue_entries/queue_entries_view.h>

#include "switchboard.h"
#include "ui_current_call.h"
#include "current_call.h"

Switchboard::Switchboard(QWidget *parent)
    : XLet(parent),
      m_current_call(new CurrentCall(this)),
      m_incoming_call_model(new QueueEntriesModel(this)),
      m_incoming_call_proxy_model(new QueueEntriesSortFilterProxyModel(this)),
      m_waiting_call_model(new QueueEntriesModel(this)),
      m_waiting_call_proxy_model(new QueueEntriesSortFilterProxyModel(this)),
      m_switchboard_user(b_engine->getXivoClientUser())
{
    this->setTitle(tr("Switchboard"));

    this->m_incoming_call_proxy_model->setSourceModel(this->m_incoming_call_model);
    this->m_waiting_call_proxy_model->setSourceModel(this->m_waiting_call_model);

    this->setupUi();

    this->registerListener("current_calls");

    QShortcut * waiting_calls_focus_shortcut = new QShortcut(QKeySequence(Qt::Key_F9), this);
    waiting_calls_focus_shortcut->setContext(Qt::ApplicationShortcut);
    connect(waiting_calls_focus_shortcut, SIGNAL(activated()),
            this, SLOT(focusOnWaitingCalls()));

    connect(b_engine, SIGNAL(queueEntryUpdate(const QString &, const QVariantList &)),
            this, SLOT(updateIncomingHeader(const QString &, const QVariantList &)));
    connect(b_engine, SIGNAL(queueEntryUpdate(const QString &, const QVariantList &)),
            this, SLOT(updateWaitingHeader(const QString &, const QVariantList &)));
    connect(b_engine, SIGNAL(queueEntryUpdate(const QString &, const QVariantList &)),
            this, SLOT(queueEntryUpdate(const QString &, const QVariantList &)));

    connect(b_engine, SIGNAL(initialized()),
            this, SLOT(postInitializationSetup()));

    connect(ui.incomingCallsView, SIGNAL(clicked(const QModelIndex &)),
            this, SLOT(incomingCallClicked(const QModelIndex &)));
    connect(ui.incomingCallsView, SIGNAL(activated(const QModelIndex &)),
            this, SLOT(incomingCallClicked(const QModelIndex &)));

    connect(ui.waitingCallsView, SIGNAL(clicked(const QModelIndex &)),
           this, SLOT(waitingCallClicked(const QModelIndex &)));
    connect(ui.waitingCallsView, SIGNAL(activated(const QModelIndex &)),
            this, SLOT(waitingCallClicked(const QModelIndex &)));

    this->setFocus();
}

Switchboard::~Switchboard()
{
}

void Switchboard::parseCommand(const QVariantMap &message)
{
    QString message_class = message["class"].toString();
    if (message_class == "current_calls") {
        this->parseCurrentCalls(message);
    }
}

void Switchboard::parseCurrentCalls(const QVariantMap &message)
{
    const QVariantList &calls = message["current_calls"].toList();
    bool has_incoming_calls = this->hasIncomingCalls();
    this->m_current_call->updateCurrentCall(calls, has_incoming_calls);
}

void Switchboard::incomingCallsUpdated(const QModelIndex &, const QModelIndex &)
{
    this->focusOnIncomingCalls();
}

void Switchboard::setupUi()
{
    ui.setupUi(this);

    this->m_current_call->setParentWidget(ui.current_call_widget);

    ui.incomingCallsView->setModel(this->m_incoming_call_proxy_model);
    ui.incomingCallsView->hideColumn(QueueEntriesModel::ID);
    ui.incomingCallsView->hideColumn(QueueEntriesModel::POSITION);
    ui.incomingCallsView->hideColumn(QueueEntriesModel::UNIQUE_ID);
    ui.incomingCallsView->setSortingEnabled(false);

    ui.waitingCallsView->setModel(this->m_waiting_call_proxy_model);
    ui.waitingCallsView->hideColumn(QueueEntriesModel::ID);
    ui.waitingCallsView->hideColumn(QueueEntriesModel::POSITION);
    ui.waitingCallsView->hideColumn(QueueEntriesModel::UNIQUE_ID);
    ui.waitingCallsView->setSortingEnabled(false);
}

void Switchboard::postInitializationSetup()
{
    this->subscribeCurrentCalls();
    this->connectPhoneStatus();
    this->watch_switchboard_queue();
}

void Switchboard::connectPhoneStatus() const
{
    connect(b_engine, SIGNAL(updatePhoneStatus(const QString &)),
            this, SLOT(updatePhoneStatus(const QString &)));
}

void Switchboard::subscribeCurrentCalls() const
{
    b_engine->sendJsonCommand(MessageFactory::subscribeCurrentCalls());
}

void Switchboard::updatePhoneStatus(const QString &phone_id)
{
    if (! isSwitchboardPhone(phone_id)) {
        return;
    }

    const PhoneInfo *phone = b_engine->phone(phone_id);
    if (phone && phone->hintstatus() == PhoneHint::available) {
        this->setFocus();
    }
}

bool Switchboard::isSwitchboardPhone(const QString &phone_id)
{
    if (! m_switchboard_user) {
        m_switchboard_user = b_engine->getXivoClientUser();
    }
    return m_switchboard_user && m_switchboard_user->hasPhoneId(phone_id);
}

void Switchboard::queueEntryUpdate(const QString &queue_id,
                                   const QVariantList &)
{
    if (this->isSwitchboardQueue(queue_id) == false) {
        return;
    }
    this->focusOnIncomingCalls();
}

void Switchboard::incomingCallClicked(const QModelIndex &index)
{
    int clicked_row = index.row();

    if (clicked_row == 0) {
        this->answerIncomingCall();
    }
}

void Switchboard::waitingCallClicked(const QModelIndex &index)
{
    QModelIndex id_index = index.child(index.row(), QueueEntriesModel::UNIQUE_ID);
    const QString &call_unique_id = m_waiting_call_proxy_model->data(id_index, Qt::DisplayRole).toString();

    this->retrieveCallOnHold(call_unique_id);
}

void Switchboard::handleEnterKeys()
{
    if (this->ui.incomingCallsView->hasFocus()) {
        return;
    } else if (this->ui.waitingCallsView->hasFocus()) {
        return;
    } else {
        this->answerIncomingCall();
    }
}

void Switchboard::keyPressEvent(QKeyEvent *event)
{
    if (! event) {
        return;
    }

    switch (event->key()) {
    case Qt::Key_Enter:
    case Qt::Key_Return:
        this->handleEnterKeys();
        break;
    default:
        break;
    }
}

void Switchboard::focusOnIncomingCalls()
{
    this->ui.incomingCallsView->setFocus();
    this->m_current_call->noticeIncoming(this->hasIncomingCalls());
}

bool Switchboard::hasIncomingCalls() {
    return this->ui.incomingCallsView->model()->rowCount() > 0;
}

void Switchboard::focusOnWaitingCalls()
{
    this->ui.waitingCallsView->setFocus();
}

void Switchboard::watch_switchboard_queue()
{
    const QString &switchboard_queue_name = b_engine->getConfig("switchboard_queue_name").toString();
    const QString &switchboard_hold_queue_name = b_engine->getConfig("switchboard_hold_queue_name").toString();
    this->m_incoming_call_model->changeWatchedQueue(QueueDAO::findQueueIdByName(switchboard_queue_name));
    this->m_waiting_call_model->changeWatchedQueue(QueueDAO::findQueueIdByName(switchboard_hold_queue_name));
}

void Switchboard::answerIncomingCall() const
{
    b_engine->sendJsonCommand(MessageFactory::answer());
}

void Switchboard::retrieveCallOnHold(const QString & call_unique_id) const
{
    b_engine->sendJsonCommand(MessageFactory::unholdSwitchboard(call_unique_id));
}

void Switchboard::updateIncomingHeader(const QString & queue_id, const QVariantList & entries)
{
    if (this->isSwitchboardQueue(queue_id) == false) {
        return;
    }

    QString header_text = QString(tr("%n call(s)", "", entries.size()));
    this->ui.incomingCallCountLabel->setText(header_text);
}

void Switchboard::updateWaitingHeader(const QString & queue_id, const QVariantList & entries)
{
    if (this->isSwitchboardHoldQueue(queue_id) == false) {
        return;
    }

    QString header_text = QString(tr("%n call(s)", "", entries.size()));
    this->ui.waitingCallCountLabel->setText(header_text);
}

bool Switchboard::isSwitchboardQueue(const QString &queue_id) const
{
    const QueueInfo *queue = b_engine->queue(IdConverter::idToXId(queue_id));
    const QString &switchboard_queue_name = b_engine->getConfig("switchboard_queue_name").toString();
    return queue && queue->queueName() == switchboard_queue_name;
}

bool Switchboard::isSwitchboardHoldQueue(const QString &queue_id) const
{
    const QueueInfo *queue = b_engine->queue(IdConverter::idToXId(queue_id));
    const QString &switchboard_hold_queue_name = b_engine->getConfig("switchboard_hold_queue_name").toString();
    return queue && queue->queueName() == switchboard_hold_queue_name;
}

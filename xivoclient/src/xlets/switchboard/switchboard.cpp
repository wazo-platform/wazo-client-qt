/* XiVO Client
 * Copyright (C) 2012, Avencall
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

#include <baseengine.h>
#include <dao/queuedao.h>
#include <id_converter.h>
#include <message_factory.h>

#include <queueinfo.h>
#include <phoneinfo.h>

#include <queue_entries/queue_entries_model.h>
#include <queue_entries/queue_entries_sort_filter_proxy_model.h>
#include <queue_entries/queue_entries_view.h>

#include "switchboard.h"
#include "ui_current_call.h"
#include "current_call.h"

#include <QKeyEvent>

QString Switchboard::switchboard_queue_name = "__switchboard";

Switchboard::Switchboard(QWidget *parent)
    : XLet(parent),
      m_current_call(new CurrentCall()),
      m_switchboard_user(b_engine->getXivoClientUser())
{
    this->setTitle(tr("Switchboard"));
    this->m_model = new QueueEntriesModel(this);
    this->m_proxy_model = new QueueEntriesSortFilterProxyModel(this);
    this->m_proxy_model->setSourceModel(this->m_model);

    this->setupUi();

    connect(b_engine, SIGNAL(queueEntryUpdate(const QString &, const QVariantList &)),
            this, SLOT(updateHeader(const QString &, const QVariantList &)));
    connect(b_engine, SIGNAL(queueEntryUpdate(const QString &, const QVariantList &)),
            this, SLOT(queueEntryUpdate(const QString &, const QVariantList &)));
    connect(b_engine, SIGNAL(initialized()),
            this, SLOT(postInitializationSetup()));
    connect(ui.incomingCallsView, SIGNAL(clicked(const QModelIndex &)),
            this, SLOT(clicked(const QModelIndex &)));

    this->setFocus();
}

Switchboard::~Switchboard()
{
}

void Switchboard::setupUi()
{
    ui.setupUi(this);

    this->m_current_call->setParentWidget(ui.current_call_widget);

    ui.incomingCallsView->setModel(this->m_proxy_model);
    ui.incomingCallsView->hideColumn(QueueEntriesModel::ID);
    ui.incomingCallsView->hideColumn(QueueEntriesModel::POSITION);
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
    if (phone && phone->hintstatus() == "0") {
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
    this->setFocus();
}

void Switchboard::clicked(const QModelIndex &index)
{
    int clicked_row = index.row();

    if (clicked_row == 0) {
        this->on_answerButton_clicked();
    }
}

void Switchboard::keyPressEvent(QKeyEvent *event)
{
    int key_code = event->key();

    if (key_code == Qt::Key_Enter || key_code == Qt::Key_Return) {
        this->on_answerButton_clicked();
    }
}

void Switchboard::watch_switchboard_queue()
{
    const QString &queue_id = QueueDAO::findQueueIdByName(this->switchboard_queue_name);
    this->m_model->changeWatchedQueue(queue_id);
}

void Switchboard::on_answerButton_clicked() const
{
    b_engine->sendJsonCommand(MessageFactory::answer());
}

void Switchboard::updateHeader(const QString & queue_id, const QVariantList & entries)
{
    if (this->isSwitchboardQueue(queue_id) == false) {
        return;
    }

    QString header_text = QString(tr("%1 call(s)"))
        .arg(entries.size());
    this->ui.incomingCallCountLabel->setText(header_text);
}

bool Switchboard::isSwitchboardQueue(const QString &queue_id) const
{
    const QueueInfo *queue = b_engine->queue(IdConverter::idToXId(queue_id));
    return queue && queue->queueName() == this->switchboard_queue_name;
}

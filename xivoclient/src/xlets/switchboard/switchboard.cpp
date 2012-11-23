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

#include <queueinfo.h>

#include <queue_entries/queue_entries_model.h>
#include <queue_entries/queue_entries_sort_filter_proxy_model.h>
#include <queue_entries/queue_entries_view.h>

#include "switchboard.h"

Switchboard::Switchboard(QWidget *parent)
    : XLet(parent)
{
    ui.setupUi(this);

    connect(b_engine, SIGNAL(queueEntryUpdate(const QString &, const QVariantList &)),
            this, SLOT(updateHeader(const QString &, const QVariantList &)));

    this->m_model = new QueueEntriesModel(this);
    this->m_proxy_model = new QueueEntriesSortFilterProxyModel(this);
    this->m_proxy_model->setSourceModel(this->m_model);
    ui.incomingCallsView->setModel(this->m_proxy_model);

    connect(b_engine, SIGNAL(initialized()),
            this, SLOT(watch_switchboard_queue()));
}

Switchboard::~Switchboard()
{
}

void Switchboard::watch_switchboard_queue()
{
    const QString &queue_id = QueueDAO::findQueueIdByName("__switchboard");
    this->m_model->changeWatchedQueue(queue_id);
}

void Switchboard::on_answerButton_clicked() const
{
    QVariantMap pickupCommand;
    pickupCommand["class"] = "answer";

    b_engine->sendJsonCommand(pickupCommand);

}

void Switchboard::updateHeader(const QString & queue_id, const QVariantList & entries)
{
    QString queue_xid = IdConverter::idToXId(queue_id);

    const QueueInfo * queue = b_engine->queue(queue_xid);
    if (queue == NULL) {
        return;
    }
    if (queue->queueName() != "__switchboard") {
        return;
    }

    QString header_text = QString(tr("%1 call(s)"))
        .arg(entries.size());
    this->ui.incomingCallCountLabel->setText(header_text);
}

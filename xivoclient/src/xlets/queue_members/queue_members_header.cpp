/* XiVO Client
 * Copyright (C) 2007-2012, Avencall
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

#include <QVBoxLayout>
#include <QLabel>

#include <baseengine.h>

#include "queue_members_header.h"

QueueMembersHeader::QueueMembersHeader(QWidget * parent)
    : QWidget(parent), m_queue_id(""), m_header_label(NULL)
{
    m_header_label = new QLabel(this);
    m_header_label->setAlignment(Qt::AlignHCenter);

    QVBoxLayout * layout = new QVBoxLayout(this);
    layout->addWidget(this->m_header_label);

    this->updateContent();

    connect(b_engine, SIGNAL(changeWatchedQueueSignal(const QString &)),
            this, SLOT(changeWatchedQueue(const QString &)));
}

void QueueMembersHeader::changeWatchedQueue(const QString & queue_id)
{
    this->m_queue_id = queue_id;
    this->updateContent();
}

void QueueMembersHeader::updateContent()
{
    const QueueInfo * queue = b_engine->queue(m_queue_id);
    if (queue == NULL) {
        qDebug() << Q_FUNC_INFO << "unknown queue" << m_queue_id;
        this->clearContent();
        return;
    }

    QString header_text = buildHeaderText(queue);

    this->m_header_label->setText(header_text);
}

QString QueueMembersHeader::buildHeaderText(const QueueInfo * queue) const
{
    QString queue_name = queue->queueDisplayName();
    QString queue_number = queue->queueNumber();
    QString queue_context = queue->context();
    int agent_count = queue->agentmembers().size();
    int phone_count = queue->phonemembers().size();

    QString agents_text = tr("%n agent(s)", "", agent_count);
    QString phones_text = tr("%n phone(s)", "", phone_count);
    QString header_text = tr("<b>%1</b> (%2@%3) : %4 and %5")
        .arg(queue_name)
        .arg(queue_number)
        .arg(queue_context)
        .arg(agents_text)
        .arg(phones_text);

    return header_text;
}

void QueueMembersHeader::clearContent()
{
    this->m_header_label->setText(tr("No selected queue"));
}

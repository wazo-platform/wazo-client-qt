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

#include <QTimerEvent>
#include <QVBoxLayout>

#include <queueinfo.h>
#include <baseengine.h>
#include <id_converter.h>

#include "queue_entries.h"
#include <queue_entries/queue_entries_model.h>
#include <queue_entries/queue_entries_sort_filter_proxy_model.h>
#include <queue_entries/queue_entries_view.h>

Q_EXPORT_PLUGIN2(queueentriesplugin, QueueEntriesPlugin);

XLet* QueueEntriesPlugin::newXLetInstance(QWidget *parent)
{
    b_engine->registerTranslation(":/obj/queue_entries_%1");
    return new QueueEntries(parent);
}

QueueEntries::QueueEntries(QWidget *parent)
    : XLet(parent)
{
    setTitle(tr("Calls of a Queue"));
    connect(b_engine, SIGNAL(changeWatchedQueueSignal(const QString &)),
            this, SLOT(changeWatchedQueue(const QString &)));
    connect(b_engine, SIGNAL(queueEntryUpdate(const QString &, const QVariantList &)),
            this, SLOT(updateHeader(const QString &, const QVariantList &)));

    this->m_model = new QueueEntriesModel(this);
    this->m_proxy_model = new QueueEntriesSortFilterProxyModel(this);
    this->m_proxy_model->setSourceModel(this->m_model);
    this->m_view = new QueueEntriesView(this);
    this->m_view->setModel(this->m_proxy_model);
    this->m_view->hideColumn(QueueEntriesModel::ID);
    this->m_view->hideColumn(QueueEntriesModel::UNIQUE_ID);

    connect(b_engine, SIGNAL(changeWatchedQueueSignal(const QString &)),
            this->m_model, SLOT(changeWatchedQueue(const QString &)));

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(&m_queue_description);
    layout->addWidget(this->m_view);
}

void QueueEntries::changeWatchedQueue(const QString & queue_id)
{
    const QueueInfo * queue = b_engine->queue(queue_id);
    if (queue == NULL) {
        return;
    }

    this->m_monitored_queue_id = queue->xid();
    this->updateHeaderLabel(queue->queueDisplayName(),
                            queue->queueNumber(),
                            0);
}

void QueueEntries::updateHeader(const QString & queue_id, const QVariantList & entries)
{
    QString queue_xid = IdConverter::idToXId(queue_id);
    if (queue_xid != this->m_monitored_queue_id) {
        return;
    }

    const QueueInfo * queue = b_engine->queue(queue_xid);
    if (queue == NULL) {
        return;
    }

    this->updateHeaderLabel(queue->queueDisplayName(),
                            queue->queueNumber(),
                            entries.size());
}

void QueueEntries::updateHeaderLabel(const QString &display_name,
                                     const QString &number,
                                     int call_count)
{
    QString header_text = QString(tr("<b>%1</b> (%2): %3 call(s)"))
        .arg(display_name)
        .arg(number)
        .arg(call_count);
    this->m_queue_description.setText(header_text);
}

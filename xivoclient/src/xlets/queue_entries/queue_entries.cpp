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
#include <QGridLayout>
#include <QLabel>
#include <QString>

#include <queueinfo.h>
#include <baseengine.h>

#include "queue_entries.h"

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
    m_gridlayout = new QGridLayout(this);

    m_queuedescription = new QLabel("", this);
    m_gridlayout->setColumnStretch(5, 1);
    m_gridlayout->setRowStretch(100, 1);
    m_gridlayout->addWidget(m_queuedescription, 0, 0);
    startTimer(1000);

    connect(b_engine, SIGNAL(updateQueueConfig(const QString &)),
            this, SLOT(updateQueueConfig(const QString &)));
    connect(b_engine, SIGNAL(changeWatchedQueueSignal(const QString &)),
            this, SLOT(monitorThisQueue(const QString &)));
    connect(b_engine, SIGNAL(changeWatchedQueueSignal(const QString &)),
            this, SLOT(subscribeQueueEntry(const QString &)));
    connect(b_engine, SIGNAL(queueEntryUpdate(const QString &, const QVariantList &)),
            this, SLOT(queueEntryUpdate(const QString &, const QVariantList &)));
}

void QueueEntries::updateQueueConfig(const QString & xqueueid)
{
    if (xqueueid == m_monitored_queueid)
        updatePanel();
}

void QueueEntries::monitorThisQueue(const QString & queueid)
{
    if(m_monitored_queueid != queueid && b_engine->hasQueue(queueid)) {
        m_monitored_queueid = queueid;
        updatePanel();
        clearPanel();
        m_queue_entries.clear();
    }
}

void QueueEntries::subscribeQueueEntry(const QString &queue_xid)
{
    if (const QueueInfo *queue = b_engine->queue(queue_xid)) {
        QVariantMap subscribe_command;
        subscribe_command["class"] = "subscribe";
        subscribe_command["message"] = "queueentryupdate";
        subscribe_command["queueid"] = queue->id();

        b_engine->sendJsonCommand(subscribe_command);
    }
}

void QueueEntries::clearPanel()
{
    foreach(int position, m_entrypos.keys()) {
        m_gridlayout->removeWidget(m_entrypos[position]);
        delete m_entrypos[position];
    }
    m_entrypos.clear();
}

void QueueEntries::updatePanel()
{
    if (const QueueInfo *queue = b_engine->queue(m_monitored_queueid)) {
        int count = m_queue_entries.size();
        updateDescription(queue, count);
        clearPanel();
        showEntries();
    }
}

void QueueEntries::showEntries()
{
    foreach(const QVariant &v_entry, m_queue_entries) {
        const QVariantMap &entry = v_entry.toMap();
        QString time_stamp = b_engine->timeElapsed(entry["join_time"].toDouble());
        int position = entry["position"].toInt();
        QString name = entry["name"].toString();
        QString number = entry["number"].toString();
        QString text = QString("%0: %1 <%2> %3").arg(position).arg(name).arg(number).arg(time_stamp);
        QLabel *label = new QLabel(text, this);
        m_entrypos[position] = label;
        m_gridlayout->addWidget(m_entrypos[position], position, 0, Qt::AlignLeft);
    }
}

void QueueEntries::updateDescription(const QueueInfo *queue,
                                              int count)
{
    m_queuedescription->setText(tr("<b>%1</b> (%2) on <b>%3</b> (%4) (%5 call(s))")
                                .arg(queue->queueDisplayName())
                                .arg(queue->queueNumber())
                                .arg(queue->ipbxid())
                                .arg(queue->context())
                                .arg(count));
}

void QueueEntries::queueEntryUpdate(const QString &queue_id,
                                             const QVariantList &entry_list)
{
    if (const QueueInfo *queue = b_engine->queue(m_monitored_queueid)) {
        if (queue->id() == queue_id) {
            m_queue_entries = entry_list;
            updatePanel();
        }
    }
}

void QueueEntries::timerEvent(QTimerEvent *)
{
    clearPanel();
    showEntries();
}

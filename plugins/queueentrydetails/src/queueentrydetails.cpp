/* XiVO Client
 * Copyright (C) 2007-2011, Proformatique
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

/* $Revision$
 * $Date$
 */

#include "queueentrydetails.h"

Q_EXPORT_PLUGIN2(xletqueueentrydetailsplugin, XLetQueueEntryDetailsPlugin);

XLet* XLetQueueEntryDetailsPlugin::newXLetInstance(QWidget *parent)
{
    b_engine->registerTranslation(":/queueentrydetails_%1");
    return new XLetQueueEntryDetails(parent);
}

XLetQueueEntryDetails::XLetQueueEntryDetails(QWidget *parent)
    : XLet(parent)
{
    setTitle(tr("Calls of a Queue"));
    m_gridlayout = new QGridLayout(this);

    m_queuedescription = new QLabel("", this);
    m_gridlayout->setColumnStretch(5, 1);
    m_gridlayout->setRowStretch(100, 1);
    m_gridlayout->addWidget(m_queuedescription, 0, 0);
    startTimer(1000);
    // connect signal slots to engine
    connect(b_engine, SIGNAL(updateAgentConfig(const QString &)),
            this, SLOT(updateAgentConfig(const QString &)));
    connect(b_engine, SIGNAL(updateAgentStatus(const QString &)),
            this, SLOT(updateAgentStatus(const QString &)));
    connect(b_engine, SIGNAL(updateQueueConfig(const QString &)),
            this, SLOT(updateQueueConfig(const QString &)));
    connect(b_engine, SIGNAL(updateQueueStatus(const QString &)),
            this, SLOT(updateQueueStatus(const QString &)));

    connect(b_engine, SIGNAL(changeWatchedQueueSignal(const QString &)),
            this, SLOT(monitorThisQueue(const QString &)));
}

void XLetQueueEntryDetails::updateAgentConfig(const QString & xagentid)
{
    qDebug() << Q_FUNC_INFO << xagentid;
}

void XLetQueueEntryDetails::updateAgentStatus(const QString & xagentid)
{
    qDebug() << Q_FUNC_INFO << xagentid;
}

void XLetQueueEntryDetails::updateQueueConfig(const QString & xqueueid)
{
    if (xqueueid == m_monitored_queueid)
        updatePanel();
}

void XLetQueueEntryDetails::updateQueueStatus(const QString & xqueueid)
{
    qDebug() << Q_FUNC_INFO << xqueueid << m_monitored_queueid;
}

void XLetQueueEntryDetails::monitorThisQueue(const QString & queueid)
{
    // qDebug() << Q_FUNC_INFO << queueid;
    if(b_engine->hasQueue(queueid)) {
        m_monitored_queueid = queueid;
        updatePanel();
    }
}

void XLetQueueEntryDetails::clearPanel()
{
    foreach(QString q, m_entrypos.keys())
        delete m_entrypos[q];
    foreach(QString q, m_entrytime.keys())
        delete m_entrytime[q];

    m_entrypos.clear();
    m_entrytime.clear();
}

/*! \brief update entries
 */
void XLetQueueEntryDetails::updatePanel()
{
    const QueueInfo * qinfo = b_engine->queue(m_monitored_queueid);
    QVariantMap properties = qinfo->properties();
    QVariantMap channels = properties["channels"].toMap();

    m_queuedescription->setText(tr("<b>%1</b> (%2) on <b>%3</b> (%4) (%5 call(s))")
                                .arg(qinfo->queueName())
                                .arg(qinfo->queueNumber())
                                .arg(qinfo->ipbxid())
                                .arg(qinfo->context())
                                .arg(channels.count())
                                );

    // queue legends
    clearPanel();

    foreach(QString channel, channels.keys()) {
        m_entrypos[channel] = new QLabel(this);
        updateEntryChannel(channel);
    }
}

void XLetQueueEntryDetails::updateEntryChannel(const QString & channel)
{
    const QueueInfo * qinfo = b_engine->queue(m_monitored_queueid);
    QVariantMap properties = qinfo->properties();
    QVariantMap channels = properties["channels"].toMap();

    if(m_entrypos.contains(channel)) {
        QVariantMap entryinfos = channels[channel].toMap();
        int time_spent = b_engine->timeServer() - entryinfos["entrytime"].toDouble(); // from the server point of view
        QDateTime inittime = b_engine->timeClient().addSecs(- time_spent); // from the client point of view
        int nsec = inittime.secsTo(QDateTime::currentDateTime());
        int position = entryinfos["position"].toInt();

        m_entrypos[channel]->setText(QString("%1 : %2 %3 : %4 sec")
                                     .arg(position)
                                     .arg(entryinfos["calleridname"].toString())
                                     .arg(entryinfos["calleridnum"].toString())
                                     .arg(nsec));
        m_gridlayout->addWidget( m_entrypos[channel], position, 0, Qt::AlignLeft );
    }
}

void XLetQueueEntryDetails::timerEvent(QTimerEvent *)
{
    // qDebug() << Q_FUNC_INFO;
    foreach(QString channel, m_entrypos.keys())
        updateEntryChannel(channel);
}

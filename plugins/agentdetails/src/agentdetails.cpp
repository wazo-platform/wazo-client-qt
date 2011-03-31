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

#include "agentdetails.h"

Q_EXPORT_PLUGIN2(xletagentdetailsplugin, XLetAgentDetailsPlugin);

XLet* XLetAgentDetailsPlugin::newXLetInstance(QWidget *parent)
{
    b_engine->registerTranslation(":/agentdetails_%1");
    return new XletAgentDetails(parent);
}

XletAgentDetails::XletAgentDetails(QWidget *parent)
    : XLet(parent)
{
    setTitle(tr("Agent Details"));
    m_linenum = 0;
    m_gridlayout = new QGridLayout(this);

    m_agentstatus = new QLabel(this);
    m_agentlegend_qname = new QLabel(tr("Queues"), this);
    m_agentlegend_joined = new QLabel(tr("Joined"), this);
    m_agentlegend_paused = new QLabel(tr("Paused"), this);
    m_agentlegend_njoined = new QLabel("0", this);
    m_agentlegend_npaused = new QLabel("0", this);

    m_actionlegends["record"] = new QLabel(tr("Record"), this);
    m_actionlegends["agentlogin"] = new QLabel(tr("Login"), this);

    foreach (QString function, m_actionlegends.keys())
        m_action[function] = new QPushButton(this);
    m_action["record"]->setIconSize(QSize(10, 10));
    m_action["record"]->setIcon(QIcon(":/images/player_stop.png"));
    m_action["agentlogin"]->setIconSize(QSize(10, 10));
    m_action["agentlogin"]->setIcon(QIcon(":/images/button_ok.png"));

    m_gridlayout->setRowStretch( 100, 1 );
    m_gridlayout->addWidget(m_agentstatus, m_linenum, 0, 1, 9);
    m_linenum ++;

    int colnum = 0;
    foreach (QString function, m_actionlegends.keys()) {
        m_gridlayout->addWidget(m_actionlegends[function], m_linenum, 2 + 3 * colnum, 1, 1, Qt::AlignCenter);
        m_gridlayout->addWidget(m_action[function], m_linenum, 3 + 3 * colnum, 1, 2, Qt::AlignCenter);
        colnum ++;
    }
    m_gridlayout->setColumnStretch( 8, 1 );
    m_linenum ++;

    m_gridlayout->addWidget(m_agentlegend_qname, m_linenum, 0, Qt::AlignLeft);
    m_gridlayout->addWidget(m_agentlegend_joined, m_linenum, 2, 1, 3, Qt::AlignCenter);
    m_gridlayout->addWidget(m_agentlegend_paused, m_linenum, 5, 1, 3, Qt::AlignCenter);
    m_linenum ++;
    m_gridlayout->addWidget(m_agentlegend_njoined, m_linenum, 2, 1, 3, Qt::AlignCenter);
    m_gridlayout->addWidget(m_agentlegend_npaused, m_linenum, 5, 1, 3, Qt::AlignCenter);
    m_linenum ++;

    m_gridlayout->setVerticalSpacing(0);
    m_agentlegend_qname->hide();
    m_agentlegend_joined->hide();
    m_agentlegend_paused->hide();
    m_agentlegend_njoined->hide();
    m_agentlegend_npaused->hide();

    foreach (QString function, m_actionlegends.keys()) {
        m_actionlegends[function]->hide();
        m_action[function]->hide();
        m_action[function]->setProperty("function", function);
        connect( m_action[function], SIGNAL(clicked()),
                 this, SLOT(actionClicked()));
    }
    setGuiOptions(b_engine->getGuiOptions("merged_gui"));

    // connect signal/slots with engine
    connect(b_engine, SIGNAL(newAgentList(const QStringList &)),
            this, SLOT(newAgentList(const QStringList &)));
    connect(b_engine, SIGNAL(updateAgentConfig(const QString &)),
            this, SLOT(updateAgentConfig(const QString &)));
    connect(b_engine, SIGNAL(updateAgentStatus(const QString &)),
            this, SLOT(updateAgentStatus(const QString &)));
    connect(b_engine, SIGNAL(updateQueueConfig(const QString &)),
            this, SLOT(updateQueueConfig(const QString &)));
    connect(b_engine, SIGNAL(updateQueueStatus(const QString &)),
            this, SLOT(updateQueueStatus(const QString &)));

    connect(b_engine, SIGNAL(changeWatchedAgentSignal(const QString &)),
            this, SLOT(monitorThisAgent(const QString &)));

    connect(b_engine, SIGNAL(serverFileList(const QStringList &)),
            this, SLOT(serverFileList(const QStringList &)));
    connect(b_engine, SIGNAL(fileReceived()),
            this, SLOT(saveToFile()));
    connect(b_engine, SIGNAL(statusRecord(const QString &, const QString &, const QString &)),
            this, SLOT(statusRecord(const QString &, const QString &, const QString &)));
    connect(b_engine, SIGNAL(statusListen(const QString &, const QString &, const QString &)),
            this, SLOT(statusListen(const QString &, const QString &, const QString &)));
}

/*! \brief set options
 */
void XletAgentDetails::setGuiOptions(const QVariantMap &optionsMap)
{
    m_optionsMap = optionsMap;
}


void XletAgentDetails::updateAgentConfig(const QString & xagentid)
{
    if (xagentid == m_monitored_agentid)
        updatePanel();
}

void XletAgentDetails::updateAgentStatus(const QString & xagentid)
{
    qDebug() << Q_FUNC_INFO << xagentid;
}

void XletAgentDetails::updateQueueConfig(const QString & xqueueid)
{
    if (m_queue_labels.contains(xqueueid))
        setQueueProps(xqueueid);
}

void XletAgentDetails::updateQueueStatus(const QString & xqueueid)
{
    qDebug() << Q_FUNC_INFO << xqueueid << m_monitored_agentid;
}

void XletAgentDetails::monitorThisAgent(const QString & agentid)
{
    if (b_engine->hasAgent(agentid)) {
        m_monitored_agentid = agentid;
        clearPanel();
        updatePanel();
    }
}

void XletAgentDetails::clearPanel()
{
    // qDebug() << Q_FUNC_INFO;
    foreach (QString q, m_queue_labels.keys())
        delete m_queue_labels[q];
    foreach (QString q, m_queue_more.keys())
        delete m_queue_more[q];
    foreach (QString q, m_queue_join_status.keys())
        delete m_queue_join_status[q];
    foreach (QString q, m_queue_join_action.keys())
        delete m_queue_join_action[q];
    foreach (QString q, m_queue_pause_status.keys())
        delete m_queue_pause_status[q];
    foreach (QString q, m_queue_pause_action.keys())
        delete m_queue_pause_action[q];

    m_queue_labels.clear();
    m_queue_more.clear();
    m_queue_join_status.clear();
    m_queue_join_action.clear();
    m_queue_pause_status.clear();
    m_queue_pause_action.clear();
}

void XletAgentDetails::updatePanel()
{
    const AgentInfo * ainfo = b_engine->agent(m_monitored_agentid);
    if (ainfo == NULL)
        return;

    QStringList agent_descriptions;
    agent_descriptions << QString("<b>%1</b> (%2)").arg(ainfo->fullname()).arg(ainfo->agentNumber());
    if (! m_optionsMap.value("xlet.agentdetails.hideastid").toBool())
        agent_descriptions << tr("on <b>%1</b>").arg(ainfo->ipbxid());
    if (! m_optionsMap.value("xlet.agentdetails.hidecontext").toBool())
        agent_descriptions << QString("(%1)").arg(ainfo->context());
    QString lstatus = ainfo->status();
    QString phonenum = ainfo->phonenumber();

    QVariantMap properties = ainfo->properties();
    QVariant agentstats = properties["agentstats"];

    if (lstatus == "AGENT_LOGGEDOFF") {
        agent_descriptions << tr("logged off <b>%1</b>").arg(phonenum);
        m_action["agentlogin"]->setProperty("function", "agentlogin");
        m_action["agentlogin"]->setIcon(QIcon(":/images/button_ok.png"));
        m_actionlegends["agentlogin"]->setText(tr("Login"));
    } else if (lstatus == "AGENT_IDLE") {
        agent_descriptions << tr("logged on phone number <b>%1</b>").arg(phonenum);
        m_action["agentlogin"]->setProperty("function", "agentlogout");
        m_action["agentlogin"]->setIcon(QIcon(":/images/cancel.png"));
        m_actionlegends["agentlogin"]->setText(tr("Logout"));
    } else if (lstatus == "AGENT_ONCALL") {
        // QString talkingto = agentstats.toMap().value("talkingto").toString();
        // agent_status = tr("logged (busy with %1) on phone number <b>%2</b>").arg(talkingto).arg(phonenum);
        agent_descriptions << tr("logged on phone number <b>%1</b>").arg(phonenum);
        m_action["agentlogin"]->setProperty("function", "agentlogout");
        m_action["agentlogin"]->setIcon(QIcon(":/images/cancel.png"));
        m_actionlegends["agentlogin"]->setText(tr("Logout"));
    } else
        qDebug() << Q_FUNC_INFO << "unknown status" << m_monitored_agentid << lstatus;

    m_agentstatus->setText(agent_descriptions.join(" "));

    m_agentlegend_qname->show();
    m_agentlegend_joined->show();
    m_agentlegend_paused->show();
    m_agentlegend_njoined->show();
    m_agentlegend_npaused->show();
    foreach (QString function, m_actionlegends.keys()) {
        m_actionlegends[function]->show();
        m_action[function]->show();
    }

    QStringList xqueueids;
    m_agentlegend_njoined->setText(agentstats.toMap().value("Xivo-NQJoined").toString());
    m_agentlegend_npaused->setText(agentstats.toMap().value("Xivo-NQPaused").toString());

    QHashIterator<QString, XInfo *> iter = QHashIterator<QString, XInfo *>(b_engine->iterover("queues"));
    while (iter.hasNext()) {
        iter.next();
        QString xqueueid = iter.key();
        QueueInfo * qinfo = (QueueInfo *) iter.value();
        // newQueue(qinfo->ipbxid(), qinfo->queueName(), qinfo->properties());
        xqueueids << xqueueid;
        bool isnewqueue = false;
        if (! m_queue_labels.contains(xqueueid))
            isnewqueue = true;

        if (isnewqueue) {
            m_queue_labels[xqueueid] = new QLabel(this);
            m_queue_more[xqueueid] = new QPushButton(this);
            m_queue_join_status[xqueueid] = new QLabel(this);
            m_queue_join_action[xqueueid] = new QPushButton(this);
            m_queue_pause_status[xqueueid] = new QLabel(this);
            m_queue_pause_action[xqueueid] = new QPushButton(this);
            m_queue_join_status[xqueueid]->setProperty("Status", "undefined");
            m_queue_pause_status[xqueueid]->setProperty("Paused", "undefined");

            m_queue_join_status[xqueueid]->hide();
            m_queue_join_action[xqueueid]->hide();
            m_queue_pause_status[xqueueid]->hide();
            m_queue_pause_action[xqueueid]->hide();
        }

        setQueueLookProps(xqueueid);
        setQueueProps(xqueueid);
        if (qinfo->ipbxid() == ainfo->ipbxid()) {
            QString refmember = qinfo->reference("agents", ainfo->xid());
            setQueueAgentProps(xqueueid, refmember);
        }

        if (isnewqueue)
            setQueueAgentSignals(xqueueid);
    }

    xqueueids.sort();
    int i = 0;
    foreach (QString xqueueid, xqueueids) {
        fillQueue(i, xqueueid);
        i ++;
    }
}

void XletAgentDetails::setQueueLookProps(const QString & xqueueid)
{
    m_queue_more[xqueueid]->setIconSize(QSize(10, 10));
    m_queue_join_action[xqueueid]->setIconSize(QSize(8, 8));
    m_queue_pause_action[xqueueid]->setIconSize(QSize(8, 8));
    m_queue_more[xqueueid]->setIcon(QIcon(":/images/add.png"));
}

void XletAgentDetails::setQueueProps(const QString & xqueueid)
{
    const QueueInfo * qinfo = b_engine->queue(xqueueid);
    if (qinfo == NULL)
        return;
    bool showNumber = b_engine->getGuiOptions("client_gui").value("queue_displaynu").toBool();
    if (showNumber)
        m_queue_labels[xqueueid]->setText(QString("%1 (%2)")
                                          .arg(qinfo->queueName())
                                          .arg(qinfo->queueNumber()));
    else
        m_queue_labels[xqueueid]->setText(qinfo->queueName());
    QStringList tooltips;
    if (! m_optionsMap.value("xlet.agentdetails.hideastid").toBool())
        tooltips << tr("Server: %1").arg(qinfo->ipbxid());
    if (! m_optionsMap.value("xlet.agentdetails.hidecontext").toBool())
        tooltips << tr("Context: %1").arg(qinfo->context());
    m_queue_labels[xqueueid]->setToolTip(tooltips.join("\n"));
}

void XletAgentDetails::setQueueAgentSignals(const QString & xqueueid)
{
    m_queue_more[xqueueid]->setProperty("queueid", xqueueid);
    m_queue_more[xqueueid]->setProperty("action", "changequeue");

    connect( m_queue_more[xqueueid], SIGNAL(clicked()),
             this, SLOT(queueClicked()));
    if (! m_optionsMap.value("xlet.agentdetails.noqueueaction").toBool()) {
        connect( m_queue_join_action[xqueueid], SIGNAL(clicked()),
                 this, SLOT(queueClicked()));
        connect( m_queue_pause_action[xqueueid], SIGNAL(clicked()),
                 this, SLOT(queueClicked()));
    }
}

void XletAgentDetails::setQueueAgentProps(const QString & xqueueid, const QString & xqueuemember)
{
    const QueueMemberInfo * qmi = b_engine->queuemembers().value(xqueuemember);
    if ((qmi == NULL) && (! xqueuemember.isEmpty()))
        return;
    m_queue_join_action[xqueueid]->setProperty("queueid", xqueueid);
    m_queue_join_action[xqueueid]->setProperty("action", "leavejoin");
    m_queue_pause_action[xqueueid]->setProperty("queueid", xqueueid);
    m_queue_pause_action[xqueueid]->setProperty("action", "pause");
    m_queue_join_status[xqueueid]->show();
    m_queue_pause_status[xqueueid]->show();

    QString oldstatus = m_queue_join_status[xqueueid]->property("Status").toString();
    QString oldpaused = m_queue_pause_status[xqueueid]->property("Paused").toString();

    QString status = "";
    QString paused = "";
    QString membership = "";
    QString callstaken = "";
    QString penalty = "";
    int lastcall = 0;

    if (qmi != NULL) {
        status = qmi->status();
        paused = qmi->paused();
        membership = qmi->membership();
        callstaken = qmi->callstaken();
        penalty = qmi->penalty();
        lastcall = qmi->lastcall();
    }

    QueueAgentStatus * qas = new QueueAgentStatus();
    qas->update(membership, status, paused);

    QString joinicon = qas->display_action_join();
    if (joinicon.isEmpty()) {
        m_queue_join_action[xqueueid]->hide();
    } else {
        m_queue_join_action[xqueueid]->setIcon(QIcon(joinicon));
        m_queue_join_action[xqueueid]->show();
    }

    if (status != oldstatus) {
        QPixmap square(12, 12);
        square.fill(qas->display_status_color());
        m_queue_join_status[xqueueid]->setPixmap(square);
        m_queue_join_status[xqueueid]->setToolTip(QString("%1\n%2\n%3")
                                                 .arg(qas->display_status_queue())
                                                 .arg(qas->display_status_logged())
                                                 .arg(qas->display_status_membership()));
        m_queue_join_status[xqueueid]->setProperty("Status", status);
    }

    if (paused != oldpaused) {
        QString pauseicon = qas->display_action_pause();
        if (pauseicon.isEmpty()) {
            m_queue_pause_action[xqueueid]->hide();
        } else {
            m_queue_pause_action[xqueueid]->setIcon(QIcon(pauseicon));
            m_queue_pause_action[xqueueid]->show();
        }
        m_queue_pause_status[xqueueid]->setToolTip(qas->display_status_paused());
        QPixmap square(12, 12);
        square.fill(qas->display_status_paused_color());
        m_queue_pause_status[xqueueid]->setPixmap(square);
        m_queue_pause_status[xqueueid]->setProperty("Paused", paused);
    }

    delete qas;
}

void XletAgentDetails::fillQueue(int ii, const QString & xqueueid)
{
    m_gridlayout->addWidget( m_queue_labels[xqueueid], ii + m_linenum, 0, Qt::AlignLeft );
    m_gridlayout->addWidget( m_queue_more[xqueueid], ii + m_linenum, 1, Qt::AlignCenter );
    m_gridlayout->addWidget( m_queue_join_status[xqueueid], ii + m_linenum, 2, Qt::AlignCenter );
    m_gridlayout->addWidget( m_queue_join_action[xqueueid], ii + m_linenum, 3, Qt::AlignCenter );
    m_gridlayout->addWidget( m_queue_pause_status[xqueueid], ii + m_linenum, 5, Qt::AlignCenter );
    m_gridlayout->addWidget( m_queue_pause_action[xqueueid], ii + m_linenum, 6, Qt::AlignCenter );
}

/*! \brief execute action on queue
 *
 * supports actions "changequeue", "leavejoin", "pause"
 */
void XletAgentDetails::queueClicked()
{
    // qDebug() << Q_FUNC_INFO << sender()->property("queueid");
    QString queueid = sender()->property("queueid").toString();
    QString action  = sender()->property("action").toString();

    QString astid = b_engine->queue(queueid)->ipbxid();
    QString qid = b_engine->queue(queueid)->id();
    QString queuename = b_engine->queue(queueid)->queueName();
    QVariant mstatus = b_engine->agent(m_monitored_agentid)->properties().value("queues_by_agent").toMap().value(qid);
    QString smstatus = mstatus.toMap().value("Status").toString();
    QString pmstatus = mstatus.toMap().value("Paused").toString();

    QVariantMap ipbxcommand;
    ipbxcommand["agentids"] = m_monitored_agentid;
    ipbxcommand["queueids"] = queueid;

    if (action == "changequeue")
        b_engine->changeWatchedQueue(queueid);
    else if (action == "leavejoin") {
        if ((smstatus == "1") || (smstatus == "3") || (smstatus == "4") || (smstatus == "5")) {
            ipbxcommand["command"] = "agentleavequeue";
        } else if (smstatus == "") {
            ipbxcommand["command"] = "agentjoinqueue";
        } else
            qDebug() << Q_FUNC_INFO << queuename << m_monitored_agentid << smstatus << pmstatus;
        // join the queue in the previously recorded paused status (to manage on the server side)
    } else if (action == "pause") {
        if (pmstatus == "0") {
            ipbxcommand["command"] = "agentpausequeue";
        } else if (pmstatus == "1") {
            ipbxcommand["command"] = "agentunpausequeue";
        } else
            qDebug() << Q_FUNC_INFO << queuename << m_monitored_agentid << smstatus << pmstatus;
    } else
        qDebug() << Q_FUNC_INFO << "unknown action" << action;

    emit ipbxCommand(ipbxcommand);
}

/*! \brief left click actions (record, stoprecord, login, logout) */
void XletAgentDetails::actionClicked()
{
    // qDebug() << Q_FUNC_INFO << sender()->property("function").toString() << m_monitored_agentid;
    QString function = sender()->property("function").toString();
    QVariantMap ipbxcommand;
    if (function == "record") {
        ipbxcommand["command"] = "record";
        ipbxcommand["target"] = m_monitored_agentid;
        emit ipbxCommand(ipbxcommand);
    } else if (function == "stoprecord") {
        ipbxcommand["command"] = "stoprecord";
        ipbxcommand["target"] = m_monitored_agentid;
        emit ipbxCommand(ipbxcommand);
    } else if (function == "agentlogin") {
        ipbxcommand["command"] = "agentlogin";
        ipbxcommand["agentids"] = m_monitored_agentid;
        emit ipbxCommand(ipbxcommand);
    } else if (function == "agentlogout") {
        ipbxcommand["command"] = "agentlogout";
        ipbxcommand["agentids"] = m_monitored_agentid;
        emit ipbxCommand(ipbxcommand);
    }
}

/*! \brief triggerred on right click */
void XletAgentDetails::contextMenuEvent(QContextMenuEvent *event)
{
    // qDebug() << Q_FUNC_INFO << event;
    m_eventpoint = event->globalPos();
    QVariantMap ipbxcommand;
    ipbxcommand["command"] = "getfilelist";
    ipbxcommand["agentid"] = m_monitored_agentid;
    emit ipbxCommand(ipbxcommand);
}

/*! \brief display file list */
void XletAgentDetails::serverFileList(const QStringList &qsl)
{
    // qDebug() << Q_FUNC_INFO << qsl;
    QMenu contextMenu(this);
    QStringList qsl_sorted = qsl;
    qsl_sorted.sort();
    if (qsl.size() > 0) {
        QAction *actiontitle = new QAction(tr("Record files for this agent"), this);
        contextMenu.addAction(actiontitle);
        contextMenu.addSeparator();
        foreach (QString filename, qsl_sorted) {
            QAction *action = new QAction(filename, this);
            action->setProperty("filename", filename);
            connect( action, SIGNAL(triggered()),
                     this, SLOT(getFile()) );
            contextMenu.addAction(action);
            delete action;
        }
        delete actiontitle;
    } else {
        QAction *action = new QAction(tr("No Record for this agent"), this);
        contextMenu.addAction(action);
        delete action;
    }
    contextMenu.exec( m_eventpoint );
}

/*! \brief update Record/Stop Record buttons
 */
void XletAgentDetails::statusRecord(const QString &astid, const QString &agentid, const QString &status)
{
    // qDebug() << Q_FUNC_INFO << agentnum << m_monitored_agentid << status;
    QString gagentid = QString("%1/%2").arg(astid).arg(agentid);
    if (gagentid == m_monitored_agentid) {
        if (status == "started") {
            m_action["record"]->setProperty("function", "stoprecord");
            m_action["record"]->setStyleSheet("QPushButton {background: #fbb638}");
        } else if (status == "stopped") {
            m_action["record"]->setProperty("function", "record");
            m_action["record"]->setStyleSheet("");
        }
    }
}

/*! \brief update Listen/Stop Listen buttons
 */
void XletAgentDetails::statusListen(const QString &astid, const QString &agentid, const QString &status)
{
    qDebug() << Q_FUNC_INFO << astid << agentid << status;
}

/*! \brief retrieve a sound file
 */
void XletAgentDetails::getFile()
{
    // qDebug() << Q_FUNC_INFO;
    QString filename = sender()->property("filename").toString();
    QVariantMap ipbxcommand;
    ipbxcommand["command"] = "getfile";
    ipbxcommand["agentid"] = m_monitored_agentid;
    ipbxcommand["filename"] = filename;
    emit ipbxCommand(ipbxcommand);
}

/*! \brief to save sound files
 *
 * open a QFileDialog and emit setFileName()
 */
void XletAgentDetails::saveToFile()
{
    // qDebug() << Q_FUNC_INFO;
    QString selectedFilter;
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("Save Sound File"),
                                                    "",
                                                    tr("All Files (*)"),
                                                    &selectedFilter);
    if (!fileName.isEmpty())
        b_engine->saveToFile(fileName);
}

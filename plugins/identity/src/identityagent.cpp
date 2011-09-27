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

#include "baseengine.h"
#include "identityagent.h"

const QString icon_color_red = "xivo-red";
const QString icon_color_black = "xivo-black";
const QString icon_color_green = "xivo-green";

const QColor Orange = QColor(255, 128, 0);

IdentityAgent::IdentityAgent(QWidget *parent)
    : QFrame(parent),
      m_allow_logagent(false),
      m_allow_pauseagent(false)
{
    m_layout = new QGridLayout(this);

    m_icon = new QLabel(this);
    m_icon->setPixmap(QPixmap(":/in/applixware.png"));
    m_icon->setContentsMargins(0, 0, 5, 0);
    m_text = new QLabel(this);
    m_status = new QLabel(this);
    m_statustxt = new QLabel(this);
    m_pause = new QLabel(this);
    m_pausetxt = new QLabel(this);

    m_layout->addWidget(m_icon, 0, 0, 3, 1, Qt::AlignHCenter | Qt::AlignTop);
    m_layout->addWidget(m_text, 0, 1, 1, 2);
    m_layout->addWidget(m_status, 1, 1);
    m_layout->addWidget(m_statustxt, 1, 2);
    m_layout->addWidget(m_pause, 2, 1);
    m_layout->addWidget(m_pausetxt, 2, 2);

    setStyleSheet("QFrame {background: white;}");
    // setFrameStyle(QFrame::Panel | QFrame::Raised);
    // setLineWidth(3);
    // m_layout->setSpacing(0);
    // m_layout->setMargin(0);
}

void IdentityAgent::setAgentId(const QString & xagentid)
{
    m_xagentid = xagentid;
}

void IdentityAgent::updateAgentConfig(const QString & xagentid)
{
    if(m_xagentid != xagentid)
        return;
    const AgentInfo * agentinfo = b_engine->agent(m_xagentid);
    if (agentinfo == NULL)
        return;
    QString agentnumber = agentinfo->agentNumber();
    m_text->setText(QString("Agent %1").arg(agentnumber));
}

void IdentityAgent::updateAgentStatus(const QString & xagentid)
{
    if(m_xagentid != xagentid)
        return;
    const AgentInfo * agentinfo = b_engine->agent(m_xagentid);
    if (agentinfo == NULL)
        return;

    setSystrayIcon(icon_color_black);
    setPausedColors(7, 3);
    setStatusColors();

    const QString & agstatus = agentinfo->status();
    if(agstatus != m_agstatus) {
        m_agstatus = agstatus;
        if(agstatus == "AGENT_LOGGEDOFF") {
            setSystrayIcon(icon_color_black);
        } else if(agstatus == "AGENT_IDLE") {
            setSystrayIcon(icon_color_green);
        } else if(agstatus == "AGENT_ONCALL") {
            setSystrayIcon(icon_color_green);
        } else {
            qDebug() << Q_FUNC_INFO << "unknown status" << agstatus;
        }
        setStatusColors();
    }

    const QStringList joined = agentinfo->xqueueids();
    int unpaused = 0;

    foreach (const QString & qxid, joined) {
        if (const QueueInfo * q = b_engine->queue(qxid)) {
            QString qmemberxid = QString("%0/qa:%1-%2")
                    .arg(agentinfo->ipbxid())
                    .arg(q->id())
                    .arg(agentinfo->id());
            if (b_engine->queuemembers().contains(qmemberxid)) {
                const QueueMemberInfo * qmi = b_engine->queuemembers()[qmemberxid];
                if (qmi->paused() == "0") {
                    ++unpaused;
                }
            }
        }
    }

    int njoined = joined.size();
    setPausedColors(njoined, njoined - unpaused);
}

void IdentityAgent::setStatusColors()
{
    QPixmap square(10, 10);
    if (const AgentInfo * agentinfo = b_engine->agent(m_xagentid)) {
        const QString phonenumber = b_engine->getConfig("agentphonenumber").toString();
        if(agentinfo->status() == "AGENT_IDLE") {
            square.fill("#00ff00");
            m_statustxt->setText(tr("Connected to %1").arg(phonenumber));
        } else {
            square.fill("#ff0000");
            m_statustxt->setText(tr("Disconnected from %1").arg(phonenumber));
        }
        m_status->setPixmap(square);
    }
}

void IdentityAgent::setPausedColors(int nj, int np)
{
    QPixmap square(10, 10);
    if(nj > 0) {
        if(np == nj) {
            setSystrayIcon(icon_color_red);
            square.fill("#ff0000");
            m_pause->setToolTip(tr("Paused"));
            m_pausetxt->setText(tr("Paused"));
        } else if(np == 0) {
            const AgentInfo * agentinfo = b_engine->agent(m_xagentid);
            square.fill("#00ff00");
            m_pause->setToolTip(tr("Unpaused"));
            m_pausetxt->setText(tr("Unpaused"));
            if(agentinfo->status() == "AGENT_IDLE")
                setSystrayIcon(icon_color_green);
            else
                setSystrayIcon(icon_color_black);
        } else {
            square.fill(Orange);
            m_pause->setToolTip(tr("Partially paused"));
            m_pausetxt->setText(tr("Partially paused"));
        }
    } else {
        square.fill(Qt::gray);
        m_pause->setToolTip(tr("Not relevant"));
        m_pausetxt->setText(tr("Not relevant"));
    }
    m_pause->setPixmap(square);
}

void IdentityAgent::contextMenuEvent(QContextMenuEvent * event)
{
    QMenu contextMenu(this);

    if (const AgentInfo * agentinfo = b_engine->agent(m_xagentid)) {
        bool loggedin = agentinfo->status() == "AGENT_IDLE";
        bool paused = agentinfo->paused();

        if(m_allow_logagent) {
            QAction * logAction = new QAction(this);
            if(loggedin) {
                logAction->setText(tr("Logout"));
                connect(logAction, SIGNAL(triggered()),
                        this, SLOT(logout()) );
            } else {
                logAction->setText(tr("Login"));
                connect(logAction, SIGNAL(triggered()),
                        this, SLOT(login()) );
            }
            contextMenu.addAction(logAction);
        }

        if(m_allow_pauseagent) {
            if (paused) {
                QAction * unpauseAction = new QAction(tr("Unpause"), this);
                connect(unpauseAction, SIGNAL(triggered()),
                        this, SLOT(unpause()) );
                contextMenu.addAction(unpauseAction);
            } else {
                QAction * pauseAction = new QAction(tr("Pause"), this);
                connect(pauseAction, SIGNAL(triggered()),
                        this, SLOT(pause()) );
                contextMenu.addAction(pauseAction);
            }
        }

        if(m_allow_logagent || m_allow_pauseagent)
            contextMenu.exec(event->globalPos());
    }
}

void IdentityAgent::login()
{
    QVariantMap ipbxcommand;
    ipbxcommand["command"] = "agentlogin";
    ipbxcommand["agentphonenumber"] = b_engine->getConfig("agentphonenumber");
    b_engine->ipbxCommand(ipbxcommand);
}

void IdentityAgent::logout()
{
    QVariantMap ipbxcommand;
    ipbxcommand["command"] = "agentlogout";
    b_engine->ipbxCommand(ipbxcommand);
}

void IdentityAgent::pause()
{
    QVariantMap ipbxcommand;
    QString ipbxid = b_engine->getXivoClientUser()->ipbxid();
    ipbxcommand["command"] = "queuepause";
    ipbxcommand["member"] = QString("agent:%1/me").arg(ipbxid);
    ipbxcommand["queue"] = QString("queue:%1/all").arg(ipbxid);
    b_engine->ipbxCommand(ipbxcommand);
}

void IdentityAgent::unpause()
{
    QVariantMap ipbxcommand;
    QString ipbxid = b_engine->getXivoClientUser()->ipbxid();
    ipbxcommand["command"] = "queueunpause";
    ipbxcommand["member"] = QString("agent:%1/me").arg(ipbxid);
    ipbxcommand["queue"] = QString("queue:%1/all").arg(ipbxid);
    b_engine->ipbxCommand(ipbxcommand);
}

void IdentityAgent::setAllowedActions(bool allow_logagent, bool allow_pauseagent)
{
    m_allow_logagent = allow_logagent;
    m_allow_pauseagent = allow_pauseagent;
}

/* XiVO Client
 * Copyright (C) 2007-2010, Proformatique
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
    connect(this, SIGNAL(ipbxCommand(const QVariantMap &)),
            b_engine, SLOT(ipbxCommand(const QVariantMap &)));
}

void IdentityAgent::setText(const QString & text)
{
    m_text->setText(text);
}

void IdentityAgent::updateStatus(const QVariantMap & properties)
{
    QVariantMap agqjoined = properties["queues_by_agent"].toMap();
    QVariantMap agentstats = properties["agentstats"].toMap();
    QString agstatus = agentstats["status"].toString();
    QString phonenum = agentstats["agent_phone_number"].toString();

    if(agstatus != m_agstatus) {
        m_agstatus = agstatus;
        if(agstatus == "AGENT_LOGGEDOFF") {
            setSystrayIcon(icon_color_black);
            m_statustxt->setProperty("connected", false);
            setStatusColors(phonenum);
        } else if(agstatus == "AGENT_IDLE") {
            setSystrayIcon(icon_color_green);
            m_statustxt->setProperty("connected", true);
            setStatusColors(phonenum);
        } else if(agstatus == "AGENT_ONCALL") {
            setSystrayIcon(icon_color_green);
            m_statustxt->setProperty("connected", true);
            setStatusColors(phonenum);
        } else
            qDebug() << Q_FUNC_INFO << "unknown status" << agstatus;
    }

    QStringList joined_queues;
    QStringList unpaused_queues;
    foreach (QString qname, agqjoined.keys()) {
        QVariantMap qv = agqjoined[qname].toMap();
        if(qv.contains("Status")) {
            QString pstatus = qv["Paused"].toString();
            //QString sstatus = qv["Status"].toString();
            joined_queues << qname;
            if(pstatus == "0")
                unpaused_queues << qname;
        }
    }

    int njoined = joined_queues.size();
    int nunpaused = unpaused_queues.size();
    setPausedColors(njoined, njoined - nunpaused);
}

void IdentityAgent::setStatusColors(const QString & phonenum)
{
    QPixmap square(10, 10);
    if(m_statustxt->property("connected").toBool()) {
        square.fill("#00ff00");
        m_statustxt->setText(tr("Connected on %1").arg(phonenum));
    } else {
        square.fill("#ff0000");
        m_statustxt->setText(tr("Disconnected from %1").arg(phonenum));
    }
    m_status->setPixmap(square);
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
            bool loggedin = m_statustxt->property("connected").toBool();
            square.fill("#00ff00");
            m_pause->setToolTip(tr("Unpaused"));
            m_pausetxt->setText(tr("Unpaused"));
            if(loggedin)
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
    // qDebug() << Q_FUNC_INFO;
    QMenu contextMenu(this);
    
    if(m_allow_logagent) {
        QAction * logAction = new QAction(this);
        bool connected = m_statustxt->property("connected").toBool();
        if(connected) {
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
        QAction * pauseAction = new QAction(tr("Pause"), this);
        connect(pauseAction, SIGNAL(triggered()),
                this, SLOT(pause()) );
        contextMenu.addAction(pauseAction);
        
        QAction * unpauseAction = new QAction(tr("Unpause"), this);
        connect(unpauseAction, SIGNAL(triggered()),
                this, SLOT(unpause()) );
        contextMenu.addAction(unpauseAction);
    }
    
    if(m_allow_logagent || m_allow_pauseagent)
        contextMenu.exec(event->globalPos());
}

void IdentityAgent::login()
{
    QVariantMap ipbxcommand;
    ipbxcommand["command"] = "agentlogin";
    ipbxcommand["agentids"] = "agent:special:me";
    emit ipbxCommand(ipbxcommand);
}

void IdentityAgent::logout()
{
    QVariantMap ipbxcommand;
    ipbxcommand["command"] = "agentlogout";
    ipbxcommand["agentids"] = "agent:special:me";
    emit ipbxCommand(ipbxcommand);
}

void IdentityAgent::pause()
{
    QVariantMap ipbxcommand;
    QString astid = b_engine->getXivoClientUser()->astid();
    ipbxcommand["command"] = "agentpausequeue";
    ipbxcommand["agentids"] = "agent:special:me";
    ipbxcommand["queueids"] = QString("queue:%1/special:all").arg(astid);
    emit ipbxCommand(ipbxcommand);
}

void IdentityAgent::unpause()
{
    QVariantMap ipbxcommand;
    QString astid = b_engine->getXivoClientUser()->astid();
    ipbxcommand["command"] = "agentunpausequeue";
    ipbxcommand["agentids"] = "agent:special:me";
    ipbxcommand["queueids"] = QString("queue:%1/special:all").arg(astid);
    emit ipbxCommand(ipbxcommand);
}

void IdentityAgent::setAllowedActions(bool allow_logagent, bool allow_pauseagent)
{
    // qDebug() << Q_FUNC_INFO << allow_logagent << allow_pauseagent;
    m_allow_logagent = allow_logagent;
    m_allow_pauseagent = allow_pauseagent;
}

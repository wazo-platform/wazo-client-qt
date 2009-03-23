/* XIVO CTI clients
 * Copyright (C) 2007-2009  Proformatique
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License version 2 for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Linking the Licensed Program statically or dynamically with other
 * modules is making a combined work based on the Licensed Program. Thus,
 * the terms and conditions of the GNU General Public License version 2
 * cover the whole combination.
 *
 * In addition, as a special exception, the copyright holders of the
 * Licensed Program give you permission to combine the Licensed Program
 * with free software programs or libraries that are released under the
 * GNU Library General Public License version 2.0 or GNU Lesser General
 * Public License version 2.1 or any later version of the GNU Lesser
 * General Public License, and with code included in the standard release
 * of OpenSSL under a version of the OpenSSL license (with original SSLeay
 * license) which is identical to the one that was published in year 2003,
 * or modified versions of such code, with unchanged license. You may copy
 * and distribute such a system following the terms of the GNU GPL
 * version 2 for the Licensed Program and the licenses of the other code
 * concerned, provided that you include the source code of that other code
 * when and as the GNU GPL version 2 requires distribution of source code.
 */

/* $Revision$
 * $Date$
 */

#include <QAction>
#include <QComboBox>
#include <QDebug>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QMouseEvent>
#include <QSizePolicy>
#include <QProgressBar>
#include <QPushButton>
#include <QRegExp>
#include <QScrollArea>

#include "baseengine.h"
#include "extendedlabel.h"
#include "identitydisplay.h"
#include "userinfo.h"
#include "agentinfo.h"
#include "queueinfo.h"

QString icon_color_red = "xivo-red";
QString icon_color_black = "xivo-black";
QString icon_color_green = "xivo-green";

/*! \brief Constructor
 */
IdentityDisplay::IdentityDisplay(BaseEngine * engine,
                                 const QVariant & options,
                                 QWidget * parent)
    : QWidget(parent), m_engine(engine),
      m_ui(NULL)
{
    m_gui_buttonsize = 16;
    
    QGridLayout * glayout = new QGridLayout(this);
    // glayout->setMargin(0);
    m_user = new QLabel(this);
    m_user->setObjectName("fullname");
    //m_user->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    
    m_phonenum = new QLabel();
    m_presencevalue = new QComboBox();
    m_presencevalue->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    m_presencevalue->setProperty("function", "presence");
    
    m_voicemail_old = new QLabel();
    m_voicemail_new = new QLabel();
    m_voicemail_name = new QLabel();
    
    m_agent = new QLabel();
    m_agentstatus = new QLabel();
    m_agentpause = new QLabel();
    m_agentpausetxt = new QLabel();
    
    connect(m_presencevalue, SIGNAL(currentIndexChanged(const QString &)),
            this, SLOT(idxChanged(const QString &)));
    
    int bigiconsize = 50;
    m_icon_user = new ExtendedLabel();
    m_icon_agent = new ExtendedLabel();
    m_icon_voicemail = new ExtendedLabel();
    m_icon_voicemail->hide();
    
    m_icon_user->setPixmap(QPixmap(":/images/personal.png").scaled(QSize(bigiconsize, bigiconsize)));
    m_icon_agent->setPixmap(QPixmap(":/images/applixware.png").scaled(QSize(bigiconsize, bigiconsize)));
    m_icon_voicemail->setPixmap(QPixmap(":/images/kthememgr.png").scaled(QSize(bigiconsize, bigiconsize)));
    m_icon_user->setProperty("iconname", "user");
    m_icon_agent->setProperty("iconname", "agent");
    m_icon_voicemail->setProperty("iconname", "voicemail");
    
    connect( m_icon_user, SIGNAL(context_menu(QContextMenuEvent *)),
             this, SLOT(contextMenuEvent(QContextMenuEvent *)) );
    connect( m_icon_agent, SIGNAL(context_menu(QContextMenuEvent *)),
             this, SLOT(contextMenuEvent(QContextMenuEvent *)) );
    connect( m_icon_voicemail, SIGNAL(context_menu(QContextMenuEvent *)),
             this, SLOT(contextMenuEvent(QContextMenuEvent *)) );
    
    glayout->setSpacing(2);
    // glayout->setMargin(0);
    
    Qt::Alignment iconAlign = Qt::AlignHCenter | Qt::AlignTop; // Qt::AlignVCenter
    glayout->addWidget( m_icon_user, 0, 1, 3, 1, iconAlign );
    glayout->addWidget( m_icon_agent, 0, 4, 3, 1, iconAlign );
    glayout->addWidget( m_icon_voicemail, 0, 8, 3, 1, iconAlign );
    
    int idline = 0;
    Qt::Alignment textAlign = Qt::AlignLeft | Qt::AlignTop; // Qt::AlignVCenter
    glayout->addWidget( m_user, idline, 2, textAlign );
    glayout->addWidget( m_agent, idline, 5, 1, 2, textAlign );
    glayout->addWidget( m_voicemail_name, idline, 9, textAlign );
    idline ++;
    glayout->addWidget( m_phonenum, idline, 2, textAlign );
    glayout->addWidget( m_agentstatus, idline, 5, 1, 2, textAlign );
    glayout->addWidget( m_voicemail_old, idline, 9, textAlign );
    idline ++;
    glayout->addWidget( m_presencevalue, idline, 2, textAlign );
    glayout->addWidget( m_agentpause, idline, 5, textAlign );
    glayout->addWidget( m_agentpausetxt, idline, 6, textAlign );
    glayout->addWidget( m_voicemail_new, idline, 9, textAlign );
    
    glayout->setColumnStretch( 10, 1 );
    
    // although it might be convenient in some cases (prevent some expansions),
    // in the basic xivoclient/grid case, it fills too much room without no resizing available
    //glayout->setRowStretch( idline, 1 );
    
    hideAgentProps();
    
    setGuiOptions(options);
    //         glayout->setColumnStretch( 0, 1 );
}

void IdentityDisplay::setGuiOptions(const QVariant & options)
{
    // qDebug() << "IdentityDisplay::setGuiOptions()" << options;
    if(options.toMap().contains("fontname") && options.toMap().contains("fontsize"))
        m_gui_font = QFont(options.toMap()["fontname"].toString(),
                           options.toMap()["fontsize"].toInt());
    if(options.toMap().contains("iconsize"))
        m_gui_buttonsize = options.toMap()["iconsize"].toInt();
    
    m_loginkind = options.toMap()["loginkind"].toUInt();
    m_functions = options.toMap()["functions"].toStringList();
    
    m_allow_logagent = options.toMap()["logagent"].toBool();
    m_allow_pauseagent = options.toMap()["pauseagent"].toBool();
    
    setFont(m_gui_font);
}

void IdentityDisplay::contextMenuEvent(QContextMenuEvent * event)
{
    if(sender() != NULL) {
        QString iconname = sender()->property("iconname").toString();
        // QMenu contextMenu(this);
        if(iconname == "agent") {
            QMenu contextMenu(this);
                        
            if(m_allow_logagent) {
                QAction * logAction = new QAction(this);
                bool connected = m_agentstatus->property("connected").toBool();
                if(connected)
                    logAction->setText(tr("Logout"));
                else
                    logAction->setText(tr("Login"));
                logAction->setProperty("iconname", iconname);
                logAction->setProperty("kind", "log");
                connect(logAction, SIGNAL(triggered()),
                        this, SLOT(contextMenuAction()) );
                contextMenu.addAction(logAction);
            }
            
            if(m_allow_pauseagent) {
                QAction * pauseAction = new QAction(tr("Pause"), this);
                pauseAction->setProperty("iconname", iconname);
                pauseAction->setProperty("kind", "pause");
                connect(pauseAction, SIGNAL(triggered()),
                        this, SLOT(contextMenuAction()) );
                contextMenu.addAction(pauseAction);
                
                QAction * unpauseAction = new QAction(tr("Unpause"), this);
                unpauseAction->setProperty("iconname", iconname);
                unpauseAction->setProperty("kind", "unpause");
                connect(unpauseAction, SIGNAL(triggered()),
                        this, SLOT(contextMenuAction()) );
                contextMenu.addAction(unpauseAction);
            }
            
            if(m_allow_logagent || m_allow_pauseagent)
                contextMenu.exec(event->globalPos());
            //                 } else if(iconname == "user") {
            //                         QAction * xAction = new QAction(tr("Change State"), this);
            //                         connect(xAction, SIGNAL(triggered()),
            //                                 this, SLOT(contextMenuAction()) );
            //                         contextMenu.addAction(xAction);
            //                 } else if(iconname == "voicemail") {
            //                         QAction * xAction = new QAction(tr("Listen Message"), this);
            //                         connect(xAction, SIGNAL(triggered()),
            //                                 this, SLOT(contextMenuAction()) );
            //                         contextMenu.addAction(xAction);
        }
        // contextMenu.exec(event->globalPos());
    }
}

void IdentityDisplay::contextMenuAction()
{
    QString iconname = sender()->property("iconname").toString();
    if(iconname == "agent") {
        QString kind = sender()->property("kind").toString();
        if(kind == "log")
            doAgentLogActions();
        else if(kind == "pause")
            doAgentPauseActions();
        else if(kind == "unpause")
            doAgentUnPauseActions();
    }
}

void IdentityDisplay::updatePresence(const QVariant & presence)
{
    // qDebug() << "IdentityDisplay::updatePresence()" << presence;
    m_presencevalue->hide();
    if(! m_functions.contains("presence"))
        return;
        
    disconnect(m_presencevalue, SIGNAL(currentIndexChanged(const QString &)),
               this, SLOT(idxChanged(const QString &)));
    QVariantMap presencemap = presence.toMap();
    if(presencemap.contains("names")) {
        foreach (QString avstate, presencemap["names"].toMap().keys()) {
            QString name = presencemap["names"].toMap()[avstate].toMap()["longname"].toString();
            if(m_presencevalue->findText(name) == -1) {
                m_presencevalue->addItem(name);
                m_presence_names[avstate] = name;
            }
        }
    }
    if(presencemap.contains("allowed")) {
        QMapIterator<QString, QVariant> capapres(presencemap["allowed"].toMap());
        while (capapres.hasNext()) {
            capapres.next();
            QString avstate = capapres.key();
            bool allow = capapres.value().toBool();
            if(m_presence_names.contains(avstate)) {
                QString name = m_presence_names[avstate];
                int idx = m_presencevalue->findText(name);
                if(idx != -1) {
                    if(! allow)
                        m_presencevalue->removeItem(idx);
                }
            }
        }
    }
    if(presencemap.contains("state")) {
        QString avstate = presencemap["state"].toMap()["stateid"].toString();
        if(m_presence_names.contains(avstate)) {
            QString name = m_presence_names[avstate];
            int idx = m_presencevalue->findText(name);
            m_presencevalue->setCurrentIndex(idx);
        }
    }
    connect(m_presencevalue, SIGNAL(currentIndexChanged(const QString &)),
            this, SLOT(idxChanged(const QString &)));
    m_presencevalue->show();
}


void IdentityDisplay::setUserInfo(const UserInfo * ui)
{
    // qDebug() << "IdentityDisplay::setUserInfo()";
    m_ui = ui;

    m_user->setText(m_ui->fullname());
    m_phonenum->setText(m_ui->phonenumber());
    QStringList vm = m_ui->mwi();
    if(vm.size() > 2) {
        m_icon_voicemail->show();
        m_voicemail_old->setText(tr("%1 old").arg(vm[1]));
        m_voicemail_new->setText(tr("%1 new").arg(vm[2]));
        m_voicemail_name->setText(tr("VoiceMailBox %1").arg(m_ui->voicemailnumber()));
    }
    // m_voicemail->hide();
    // changes the "watched agent" only if no one else has done it before
    changeWatchedAgent(m_ui->astid() + " " + m_ui->agentnumber(), false);
}

void IdentityDisplay::newAgentList()
{
    if (m_loginkind == 0)
        return;
    if (m_ui == NULL)
        return;
    // qDebug() << "IdentityDisplay::newAgentList()";
    QHashIterator<QString, AgentInfo *> iter = QHashIterator<QString, AgentInfo *>(m_engine->agents());
    while( iter.hasNext() )
        {
            iter.next();
            AgentInfo * ainfo = iter.value();
            QString agentid = iter.key();
            if((m_ui->astid() == ainfo->astid()) && (m_ui->agentnumber() == ainfo->agentnumber())) {
                m_agent->setText(QString("Agent %1").arg(ainfo->agentnumber()));
                showAgentProps();
                updateAgentStatus(ainfo->properties());
            }
        }
}

void IdentityDisplay::newQueueList()
{
    if (m_loginkind == 0)
        return;
    if (m_ui == NULL)
        return;
    // qDebug() << "IdentityDisplay::newQueueList()";
}

void IdentityDisplay::updateAgentStatus(const QVariantMap & properties)
{
    QVariantMap agqjoined = properties["queues_by_agent"].toMap();
    QString agstatus = properties["agentstats"].toMap()["status"].toString();
    QString phonenum = properties["agentstats"].toMap()["agent_phone_number"].toString();
    
    if(agstatus == "AGENT_LOGGEDOFF") {
        setSystrayIcon(icon_color_black);
        m_agentstatus->setProperty("connected", false);
        m_agentstatus->setText(tr("Disconnected from %1").arg(phonenum));
    } else if((agstatus == "AGENT_IDLE") || (agstatus == "AGENT_ONCALL")) {
        setSystrayIcon(icon_color_green);
        m_agentstatus->setProperty("connected", true);
        m_agentstatus->setText(tr("Connected on %1").arg(phonenum));
    } else
        qDebug() << "IdentityDisplay::setAgentList() unknown status" << agstatus;
    
    QStringList joined_queues;
    QStringList unpaused_queues;
    foreach (QString qname, agqjoined.keys()) {
        QVariant qv = agqjoined[qname];
        if(qv.toMap().contains("Status")) {
            QString pstatus = qv.toMap()["Paused"].toString();
            QString sstatus = qv.toMap()["Status"].toString();
            joined_queues << qname;
            if(pstatus == "0")
                unpaused_queues << qname;
        }
    }
    
    int njoined = joined_queues.size();
    int nunpaused = unpaused_queues.size();
    setStatusColors(njoined, njoined - nunpaused);
}

void IdentityDisplay::showAgentProps()
{
    m_agent->show();
    m_agentstatus->show();
    m_agentpause->show();
    m_agentpausetxt->show();
    m_icon_agent->show();
}

void IdentityDisplay::hideAgentProps()
{
    m_agent->hide();
    m_agentstatus->hide();
    m_agentpause->hide();
    m_agentpausetxt->hide();
    m_icon_agent->hide();
}

void IdentityDisplay::setStatusColors(int nj, int np)
{
    QPixmap * p_square = new QPixmap(16, 16);
    if(nj > 0) {
        if(np == nj) {
            setSystrayIcon(icon_color_red);
            p_square->fill("#ff0000");
            m_agentpause->setToolTip(tr("Paused"));
            m_agentpausetxt->setText(tr("Paused"));
        } else {
            bool loggedin = m_agentstatus->property("connected").toBool();
            p_square->fill("#00ff00");
            m_agentpause->setToolTip(tr("Unpaused"));
            m_agentpausetxt->setText(tr("Unpaused"));
            if(loggedin)
                setSystrayIcon(icon_color_green);
            else
                setSystrayIcon(icon_color_black);
        }
    } else {
        p_square->fill("#ff0000");
        m_agentpause->setToolTip(tr("Paused"));
        m_agentpausetxt->setText(tr("Paused"));
    }
    m_agentpause->setPixmap(* p_square);
}

void IdentityDisplay::doAgentLogActions()
{
    bool connected = m_agentstatus->property("connected").toBool();
    if(connected)
        agentAction("logout");
    else
        agentAction("login");
}

void IdentityDisplay::doAgentPauseActions()
{
    // bool paused = m_agentpause->property("paused").toBool();
    agentAction("pause_all");
}

void IdentityDisplay::doAgentUnPauseActions()
{
    // bool paused = m_agentpause->property("paused").toBool();
    agentAction("unpause_all");
}

void IdentityDisplay::idxChanged(const QString & newidx)
{
    QString function = sender()->property("function").toString();
    // qDebug() << "IdentityDisplay::idxChanged" << newidx << sender() << function;
    if(function == "presence") {
        foreach (QString avstate, m_presence_names.keys())
            if(m_presence_names[avstate] == newidx)
                setAvailState(avstate, false);
    }
}

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

#include <QDebug>
#include <QFrame>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QVariant>

#include "baseengine.h"
#include "agentspanel.h"
#include "agentinfo.h"
#include "phoneinfo.h"
#include "userinfo.h"

/*! \brief Constructor
 */
AgentsPanel::AgentsPanel(BaseEngine * engine,
                         const QVariant & optionmap,
                         QWidget * parent)
    : QWidget(parent), m_engine(engine)
{
    m_gui_buttonsize = 10;
    
    m_gridlayout = new QGridLayout(this);
    m_title1 = new QLabel(tr("Agent"), this);
    m_title2 = new QLabel(tr("Record"), this);
    m_title3 = new QLabel(tr("Listen"), this);
    m_title4 = new QLabel(tr("On Line"), this);
    m_title5 = new QLabel(tr("Presence"), this);
    m_title_logged = new QLabel(tr("Logged"), this);
    m_title_paused = new QLabel(tr("Paused"), this);
    m_title_njoined = new QLabel(tr("Joined\nqueues"), this);
    m_title_npaused = new QLabel(tr("Paused\nqueues"), this);
    
    m_gridlayout->addWidget(m_title1, 0, 0, 1, 2, Qt::AlignLeft );
    m_gridlayout->addWidget(m_title2, 0, 2, 1, 1, Qt::AlignCenter );
    m_gridlayout->addWidget(m_title3, 0, 3, 1, 1, Qt::AlignCenter );
    m_gridlayout->addWidget(m_title4, 0, 4, 1, 1, Qt::AlignCenter );
    m_gridlayout->addWidget(m_title5, 0, 6, 1, 1, Qt::AlignCenter );
    m_gridlayout->addWidget(m_title_logged, 0, 8, 1, 2, Qt::AlignCenter );
    m_gridlayout->addWidget(m_title_njoined, 0, 10, 1, 1, Qt::AlignCenter );
    m_gridlayout->addWidget(m_title_paused, 0, 12, 1, 2, Qt::AlignCenter );
    m_gridlayout->addWidget(m_title_npaused, 0, 14, 1, 1, Qt::AlignCenter );
    m_gridlayout->setColumnStretch( 15, 1 );
    m_gridlayout->setRowStretch( 100, 1 );
    m_gridlayout->setVerticalSpacing(0);
    
    setGuiOptions(optionmap);
}

/*! \brief Destructor */
AgentsPanel::~AgentsPanel()
{
    // qDebug() << "AgentsPanel::~AgentsPanel()";
}

/*! \brief set font
 */
void AgentsPanel::setGuiOptions(const QVariant & optionmap)
{
    if(optionmap.toMap().contains("fontname") && optionmap.toMap().contains("fontsize"))
        m_gui_font = QFont(optionmap.toMap()["fontname"].toString(),
                           optionmap.toMap()["fontsize"].toInt());
    if(optionmap.toMap().contains("iconsize"))
        m_gui_buttonsize = optionmap.toMap()["iconsize"].toInt();
    
    // setFont(m_gui_font);
    m_title1->setFont(m_gui_font);
    m_title2->setFont(m_gui_font);
    m_title3->setFont(m_gui_font);
    m_title4->setFont(m_gui_font);
    m_title5->setFont(m_gui_font);
    m_title_logged->setFont(m_gui_font);
    m_title_paused->setFont(m_gui_font);
    m_title_njoined->setFont(m_gui_font);
    m_title_npaused->setFont(m_gui_font);
}

/*! \brief set agent presence status
 */
void AgentsPanel::updateAgentPresence(const QString & astid, const QString & agent_number, const QVariant & presencestatus)
{
    // qDebug() << "AgentsPanel::updateAgentPresence()" << astid << agent_number << presencestatus;
    QString agentid = QString("agent:%1/%2").arg(astid).arg(agent_number);
    if(m_engine->agents().contains(agentid))
        if(m_agent_presence.contains(agentid)) {
            QPixmap * p_square = new QPixmap(m_gui_buttonsize, m_gui_buttonsize);
            p_square->fill(QColor(presencestatus.toMap()["color"].toString()));
            m_agent_presence[agentid]->setPixmap(QPixmap(* p_square));
            m_agent_presence[agentid]->setToolTip(presencestatus.toMap()["longname"].toString());
        }
}

/*! \brief 
 */
void AgentsPanel::newQueueList()
{
    // qDebug() << "AgentsPanel::newQueueList()";
}

/*! \brief 
 */
void AgentsPanel::newAgentList()
{
    // qDebug() << "AgentsPanel::newAgentList()";
    QHashIterator<QString, AgentInfo *> iter = QHashIterator<QString, AgentInfo *>(m_engine->agents());
    while( iter.hasNext() ) {
        iter.next();
        AgentInfo * ainfo = iter.value();
        QString agentid = iter.key();
        
        // QString agentcontext = properties.toMap()["context"].toString();
        // if(m_ui->contexts().contains(agentcontext))
        bool newagentid = false;
        
        if(! m_agent_labels.contains(agentid)) {
            newAgentLine(agentid);
            newagentid = true;
        }
        updateAgentLineAdmin(agentid, ainfo->fullname(), ainfo->agentnumber());
        updateAgentLineEvent(agentid);
        if(newagentid)
            displayLine(agentid, m_agent_labels.size());
        updateAgentStatus(agentid, ainfo->properties());
    }
}

void AgentsPanel::newAgentLine(const QString & agentid)
{
    m_agent_labels[agentid] = new QLabel(this);
    m_agent_more[agentid] = new QPushButton(this);
    connect( m_agent_more[agentid], SIGNAL(clicked()),
             this, SLOT(agentClicked()));
    m_agent_record[agentid] = new QPushButton(this);
    connect( m_agent_record[agentid], SIGNAL(clicked()),
             this, SLOT(agentClicked()));
    m_agent_listen[agentid] = new QPushButton(this);
    connect( m_agent_listen[agentid], SIGNAL(clicked()),
             this, SLOT(agentClicked()));
    m_agent_logged_action[agentid] = new QPushButton(this);
    connect( m_agent_logged_action[agentid], SIGNAL(clicked()),
             this, SLOT(agentClicked()));
    m_agent_paused_action[agentid] = new QPushButton(this);
    connect( m_agent_paused_action[agentid], SIGNAL(clicked()),
             this, SLOT(agentClicked()));
    
    m_agent_busy[agentid] = new QLabel(this);
    m_agent_presence[agentid] = new QLabel(this);
    m_agent_logged_status[agentid] = new QLabel(this);
    m_agent_joined_number[agentid] = new QLabel(this);
    // m_agent_joined_status[agentid] = new QLabel(this);
    m_agent_paused_number[agentid] = new QLabel(this);
    m_agent_paused_status[agentid] = new QLabel(this);
}

// update according to admin-defined parameters
void AgentsPanel::updateAgentLineAdmin(const QString & agentid, const QString & agfullname, const QString & agentnumber)
{
    m_agent_labels[agentid]->setText(QString("%1 (%2)").arg(agfullname).arg(agentnumber));
    m_agent_more[agentid]->setProperty("agentid", agentid);
    m_agent_more[agentid]->setProperty("action", "changeagent");
    m_agent_record[agentid]->setProperty("agentid", agentid);
    m_agent_listen[agentid]->setProperty("agentid", agentid);
    m_agent_busy[agentid]->setProperty("agentid", agentid);
    m_agent_logged_action[agentid]->setProperty("agentid", agentid);
    m_agent_paused_action[agentid]->setProperty("agentid", agentid);
}

// update according to misc parameters
void AgentsPanel::updateAgentLineEvent(const QString & agentid)
{
    m_agent_record[agentid]->setProperty("action", "record");
    m_agent_listen[agentid]->setProperty("action", "listen");
    m_agent_logged_action[agentid]->setProperty("action", "loginoff");
    m_agent_paused_action[agentid]->setProperty("action", "pause");
}

void AgentsPanel::displayLine(const QString & agentid, int linenum)
{
    QFrame * qvline1 = new QFrame(this);
    qvline1->setFrameShape(QFrame::VLine);
    qvline1->setLineWidth(1);
    QFrame * qvline2 = new QFrame(this);
    qvline2->setFrameShape(QFrame::VLine);
    qvline2->setLineWidth(1);
    QFrame * qvline3 = new QFrame(this);
    qvline3->setFrameShape(QFrame::VLine);
    qvline3->setLineWidth(1);
    
    QPixmap * p_square = new QPixmap(m_gui_buttonsize, m_gui_buttonsize);
    p_square->fill(Qt::gray);
    m_agent_presence[agentid]->setPixmap(QPixmap(* p_square));
    
    m_agent_more[agentid]->setIconSize(QSize(m_gui_buttonsize, m_gui_buttonsize));
    m_agent_more[agentid]->setIcon(QIcon(":/images/add.png"));
    m_agent_record[agentid]->setIconSize(QSize(m_gui_buttonsize, m_gui_buttonsize));
    m_agent_record[agentid]->setIcon(QIcon(":/images/player_stop.png"));
    m_agent_listen[agentid]->setIconSize(QSize(m_gui_buttonsize, m_gui_buttonsize));
    m_agent_listen[agentid]->setIcon(QIcon(":/images/player_play.png"));
    m_agent_logged_action[agentid]->setIconSize(QSize(m_gui_buttonsize, m_gui_buttonsize));
    m_agent_paused_action[agentid]->setIconSize(QSize(m_gui_buttonsize, m_gui_buttonsize));
    
    int colnum = 0;
    m_gridlayout->addWidget( m_agent_labels[agentid], linenum, colnum++, Qt::AlignLeft );
    m_gridlayout->addWidget( m_agent_more[agentid], linenum, colnum++, Qt::AlignCenter );
    m_gridlayout->addWidget( m_agent_record[agentid], linenum, colnum++, Qt::AlignCenter );
    m_gridlayout->addWidget( m_agent_listen[agentid], linenum, colnum++, Qt::AlignCenter );
    m_gridlayout->addWidget( m_agent_busy[agentid], linenum, colnum++, Qt::AlignCenter );
    m_gridlayout->addWidget( qvline1, linenum, colnum++, Qt::AlignHCenter );
    m_gridlayout->addWidget( m_agent_presence[agentid], linenum, colnum++, Qt::AlignCenter );
    m_gridlayout->addWidget( qvline2, linenum, colnum++, Qt::AlignHCenter );
    m_gridlayout->addWidget( m_agent_logged_status[agentid], linenum, colnum++, Qt::AlignCenter );
    m_gridlayout->addWidget( m_agent_logged_action[agentid], linenum, colnum++, Qt::AlignCenter );
    m_gridlayout->addWidget( m_agent_joined_number[agentid], linenum, colnum++, Qt::AlignRight );
    m_gridlayout->addWidget( qvline3, linenum, colnum++, Qt::AlignHCenter );
    m_gridlayout->addWidget( m_agent_paused_status[agentid], linenum, colnum++, Qt::AlignCenter );
    m_gridlayout->addWidget( m_agent_paused_action[agentid], linenum, colnum++, Qt::AlignCenter );
    m_gridlayout->addWidget( m_agent_paused_number[agentid], linenum, colnum++, Qt::AlignRight );
}

void AgentsPanel::updateAgentStatus(const QString & agentid, const QVariantMap & properties)
{
    QVariantMap agqjoined = properties["queues_by_agent"].toMap();
    QVariantMap aggjoined = properties["groups_by_agent"].toMap();
    QString agstatus = properties["agentstats"].toMap()["status"].toString();
    QString phonenum = properties["agentstats"].toMap()["agent_phone_number"].toString();
    
    QVariantMap slink = properties["agentstats"].toMap()["Xivo-Status-Link"].toMap();
    bool link = false;
    if(! slink.isEmpty()) {
        QString linkmode = slink["linkmode"].toString();
        if((linkmode == "phonelink") || (linkmode == "agentlink"))
            link = true;
        else
            shouldNotOccur("AgentsPanel::updateAgentStatus",
                           QString("agentid %1 linkmode %2").arg(agentid).arg(linkmode));
    }
    
    AgentInfo * ainfo = m_engine->agents()[agentid];
    QPixmap * p_square = new QPixmap(m_gui_buttonsize, m_gui_buttonsize);
    QStringList ttips;
    if(link) {
        p_square->fill(Qt::green);
        QHashIterator<QString, PhoneInfo *> iter = QHashIterator<QString, PhoneInfo *>(m_engine->phones());
        while( iter.hasNext() )
            {
                iter.next();
                if((iter.value()->number() == phonenum) && (iter.value()->astid() == ainfo->astid())) {
                    foreach(QString uniqueid, iter.value()->comms().keys()) {
                        QVariantMap commval = iter.value()->comms()[uniqueid].toMap();
                        ttips << tr("online with %1 (%2)").arg(commval["calleridname"].toString()).arg(commval["calleridnum"].toString());
                    }
                }
            }
    } else
        p_square->fill(Qt::gray);
    m_agent_busy[agentid]->setPixmap(QPixmap(* p_square));
    m_agent_busy[agentid]->setToolTip(ttips.join("\n"));
    
    QString tooltip;
    if(agstatus == "AGENT_IDLE") {
        p_square->fill(Qt::green);
        m_agent_logged_action[agentid]->setIcon(QIcon(":/images/cancel.png"));
        tooltip = tr("Agent logged");
    } else if(agstatus == "AGENT_ONCALL") {
        p_square->fill(Qt::darkGreen);
        m_agent_logged_action[agentid]->setIcon(QIcon(":/images/cancel.png"));
        tooltip = tr("Agent busy");
    } else if(agstatus == "AGENT_LOGGEDOFF") {
        p_square->fill(Qt::red);
        m_agent_logged_action[agentid]->setIcon(QIcon(":/images/button_ok.png"));
        tooltip = tr("Agent NOT logged");
    } else {
        p_square->fill(Qt::gray);
        m_agent_logged_action[agentid]->setIcon(QIcon(QPixmap(* p_square)));
        tooltip = tr("Unknown %1").arg(agstatus);
        shouldNotOccur("AgentsPanel::updateAgentStatus",
                       QString("agentid %1 agstatus %2").arg(agentid).arg(agstatus));
    }
    m_agent_logged_status[agentid]->setPixmap(QPixmap(* p_square));
    m_agent_logged_status[agentid]->setToolTip(tooltip);
    
    QStringList joined_queues;
    QStringList paused_queues;
    foreach (QString qname, agqjoined.keys()) {
        QVariant qv = agqjoined[qname];
        if(qv.toMap().contains("Status")) {
            QString pstatus = qv.toMap()["Paused"].toString();
            QString sstatus = qv.toMap()["Status"].toString();
            if((sstatus == "1") || (sstatus == "3") || (sstatus == "4") || (sstatus == "5"))
                joined_queues << qname;
            if(pstatus == "1")
                paused_queues << qname;
        }
    }
    
    int njoined = joined_queues.size();
    int npaused = paused_queues.size();
    QString sep = ", ";
    
    m_agent_joined_number[agentid]->setText(QString::number(njoined));
    m_agent_paused_number[agentid]->setText(QString::number(npaused));
    
    if(njoined)
        m_agent_joined_number[agentid]->setToolTip(tr("Joined queues : %1").arg(joined_queues.join(sep)));
    else
        m_agent_joined_number[agentid]->setToolTip("");
    
    if(njoined == 0) {
        m_agent_paused_status[agentid]->hide();
        m_agent_paused_action[agentid]->hide();
        m_agent_paused_number[agentid]->hide();
    } else {
        QString tooltip = tr("Paused queues : %1").arg(paused_queues.join(sep));
        if(npaused == 0) {
            m_agent_paused_status[agentid]->setText(tr("No"));
            m_agent_paused_action[agentid]->setIcon(QIcon(":/images/cancel.png"));
            m_agent_paused_action[agentid]->setProperty("action", "pause");
            m_agent_paused_status[agentid]->setToolTip("");
            m_agent_paused_number[agentid]->setToolTip("");
        } else if(npaused == njoined) {
            m_agent_paused_status[agentid]->setText(tr("Yes"));
            m_agent_paused_action[agentid]->setIcon(QIcon(":/images/button_ok.png"));
            m_agent_paused_action[agentid]->setProperty("action", "unpause");
            m_agent_paused_status[agentid]->setToolTip(tooltip);
            m_agent_paused_number[agentid]->setToolTip(tooltip);
        } else {
            m_agent_paused_status[agentid]->setText(tr("Partial"));
            m_agent_paused_status[agentid]->setToolTip(tooltip);
            m_agent_paused_number[agentid]->setToolTip(tooltip);
        }
        m_agent_paused_status[agentid]->show();
        m_agent_paused_action[agentid]->show();
        m_agent_paused_number[agentid]->show();
    }
}

/*! \brief process actions
 */
void AgentsPanel::agentClicked()
{
    QString agentid = sender()->property("agentid").toString();
    QString action  = sender()->property("action").toString();
    
    if(! m_engine->agents().keys().contains(agentid))
        return;
    
    AgentInfo * ainfo = m_engine->agents()[agentid];
    QString astid = ainfo->astid();
    QString agentnumber = ainfo->agentnumber();
    
    if(action == "changeagent")
        emit changeWatchedAgent(agentid, true);
    
    else if(action == "loginoff") {
        QString status = ainfo->properties()["agentstats"].toMap()["status"].toString();
        if((status == "AGENT_IDLE") || (status == "AGENT_ONCALL"))
            emit agentAction(QString("logout %1 %2").arg(astid).arg(agentnumber));
        else if(status == "AGENT_LOGGEDOFF")
            emit agentAction(QString("login %1 %2").arg(astid).arg(agentnumber));
        else
            shouldNotOccur("AgentsPanel::agentClicked",
                           QString("agentid %1 action %2 status %3").arg(agentid).arg(action).arg(status));
    } else if(action == "unpause")
        emit agentAction(QString("unpause_all %1 %2").arg(astid).arg(agentnumber));
    else if(action == "pause")
        emit agentAction(QString("pause_all %1 %2").arg(astid).arg(agentnumber));
    else if(action == "listen")
        emit agentAction(QString("listen %1 %2").arg(astid).arg(agentnumber));
    else if(action == "stoplisten")
        emit agentAction(QString("stoplisten %1 %2").arg(astid).arg(agentnumber));
    else if(action == "record")
        emit agentAction(QString("record %1 %2").arg(astid).arg(agentnumber));
    else if(action == "stoprecord")
        emit agentAction(QString("stoprecord %1 %2").arg(astid).arg(agentnumber));
}

/*! \brief update Record/Stop Record buttons
 */
void AgentsPanel::statusRecord(const QString & astid, const QString & agentid, const QString & status)
{
    // qDebug() << "AgentsPanel::statusRecord()" << astid << agentid << status;
    QString gagentid = QString("agent:%1/%2").arg(astid).arg(agentid);
    if(! m_agent_record.contains(gagentid))
        return;
    
    if(status == "started") {
        m_agent_record[gagentid]->setProperty("action", "stoprecord");
        m_agent_record[gagentid]->setStyleSheet("QPushButton {background: #fbb638}");
    } else if(status == "stopped") {
        m_agent_record[gagentid]->setProperty("action", "record");
        m_agent_record[gagentid]->setStyleSheet("");
    }
}

/*! \brief update Listen/Stop Listen buttons
 */
void AgentsPanel::statusListen(const QString & astid, const QString & agentid, const QString & status)
{
    // qDebug() << "AgentsPanel::statusListen()" << astid << agentid << status;
    QString gagentid = QString("agent:%1/%2").arg(astid).arg(agentid);
    if(! m_agent_listen.contains(gagentid))
        return;
    
    if(status == "started") {
        m_agent_listen[gagentid]->setProperty("action", "stoplisten");
        m_agent_listen[gagentid]->setStyleSheet("QPushButton {background: #fbb638}");
    } else if(status == "stopped") {
        m_agent_listen[gagentid]->setProperty("action", "listen");
        m_agent_listen[gagentid]->setStyleSheet("");
    }
}

/* XIVO CTI clients
 * Copyright (C) 2007, 2008  Proformatique
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

/* $Revision: 4701 $
 * $Date: 2008-11-17 12:39:36 +0100 (lun, 17 nov 2008) $
 */

#include <QDebug>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QScrollArea>
#include <QVariant>
#include <QVBoxLayout>

#include "agentspanel_next.h"
#include "userinfo.h"

QStringList glist = (QStringList() << "gr1" << "gr2" << "gr3" << "gr4" << "gr5" << "gr6");

/*! \brief Constructor
 */
AgentsPanelNext::AgentsPanelNext(const QVariant & optionmap,
                                 QWidget * parent)
        : QWidget(parent)
{
        m_gui_buttonsize = 10;
        
        m_hlayout = new QHBoxLayout(this);
        m_hlayout->setSpacing(1);
        foreach(QString gn, glist) {
                m_vlayout[gn] = new QVBoxLayout();
                m_hlayout->addLayout(m_vlayout[gn]);
                m_title[gn] = new QLabel("MARCHE " + gn);
                m_title[gn]->setMargin(10);
                m_title[gn]->setStyleSheet("QLabel {background: yellow};");
                m_titleedit[gn] = new QPushButton();
                m_vlayout[gn]->addWidget(m_title[gn], 0, Qt::AlignCenter);
                m_vlayout[gn]->addWidget(m_titleedit[gn], 0, Qt::AlignCenter);
                m_vlayout[gn]->insertStretch(100, 1);
                m_vlayout[gn]->setSpacing(1);
        }
        m_hlayout->insertStretch(100, 1);
        m_vlayout["gr1"]->setProperty("queues", (QStringList() << "martinique" << "guadeloupe" << "metropole"));
        m_vlayout["gr2"]->setProperty("queues", (QStringList() << "martinique"));
        m_vlayout["gr3"]->setProperty("queues", (QStringList() << "guadeloupe"));
        m_vlayout["gr4"]->setProperty("queues", (QStringList() << "metropole"));
        m_vlayout["gr5"]->setProperty("queues", (QStringList() << "commercial"));
        m_vlayout["gr6"]->setProperty("queues", (QStringList() << "tous"));
        // m_gridlayout->setVerticalSpacing(0);
        
        setGuiOptions(optionmap);
}

AgentsPanelNext::~AgentsPanelNext()
{
        // qDebug() << "AgentsPanelNext::~AgentsPanelNext()";
}

void AgentsPanelNext::setGuiOptions(const QVariant & optionmap)
{
        if(optionmap.toMap().contains("fontname") && optionmap.toMap().contains("fontsize"))
                m_gui_font = QFont(optionmap.toMap()["fontname"].toString(),
                                   optionmap.toMap()["fontsize"].toInt());
        if(optionmap.toMap().contains("iconsize"))
                m_gui_buttonsize = optionmap.toMap()["iconsize"].toInt();
        
        // setFont(m_gui_font);
        foreach(QString gn, glist)
                m_title[gn]->setFont(m_gui_font);
}

void AgentsPanelNext::setUserInfo(const UserInfo * ui)
{
        m_userinfo = ui;
}

void AgentsPanelNext::setAgentProps(const QString & idx)
{
        QString astid = m_agent_labels[idx]->property("astid").toString();
        QString agentid = m_agent_labels[idx]->property("agentid").toString();
        QString groupid = m_agent_labels[idx]->property("groupid").toString();
        QString idxa = astid + "-" + agentid;
        m_vlayout[groupid]->insertWidget( 2, m_agent_labels[idx] );
        
        QVariant properties = m_agent_props[idxa].toMap()["properties"];
        QString firstname = m_agent_props[idxa].toMap()["firstname"].toString();
        QString lastname = m_agent_props[idxa].toMap()["lastname"].toString();
        
        QString agstatus = properties.toMap()["status"].toString();
        QString phonenum = properties.toMap()["phonenum"].toString();
        bool link = properties.toMap()["link"].toBool();
        
        m_agent_labels[idx]->setText(firstname + " " + lastname + " " + agentid);
        
        QString colorqss;
        if(agstatus == "AGENT_IDLE")
                colorqss = "grey";
        else if(agstatus == "AGENT_LOGGEDOFF")
                colorqss = "red";
        else if(agstatus == "AGENT_ONCALL")
                colorqss = "green";
        else
                colorqss = "blue";
        if(link)
                colorqss = "green";
        QString qss = "QPushButton {border: 5px solid " + colorqss + "; border-radius: 0px; background: " + colorqss + "};";
        m_agent_labels[idx]->setStyleSheet(qss);
}

void AgentsPanelNext::updateAgentPresence(const QString &, const QVariant &)
{
        // qDebug() << "AgentsPanelNext::updateAgentPresence" << agentname << presencestatus;
        
        // QColor color = QColor(presencestatus.toMap()["color"].toString());
}

void AgentsPanelNext::updatePeerAgent(const QString &,
                                      const QString & what,
                                      const QVariant & params)
{
        if(what != "agentstatus")
                return;
        // qDebug() << "AgentsPanelNext::updatePeerAgent()" << what << params;
        QString action = params.toMap()["action"].toString();
        QString astid = params.toMap()["astid"].toString();
        QString agentnum = params.toMap()["agent_channel"].toString().mid(6);
        QString qname = params.toMap()["queuename"].toString();
        QString jstatus = params.toMap()["joinedstatus"].toString();
        QString pstatus = params.toMap()["pausedstatus"].toString();
        
        if(action == "queuememberstatus") {
                if(m_agent_labels.contains(agentnum)) {
                        if(jstatus == "1") {
                                // m_square->fill(Qt::green);
                        } else if(jstatus == "5") {
                                // m_square->fill(Qt::red);
                        } else if(jstatus == "3") {
                                qDebug() << "AgentsPanelNext::updatePeerAgent()" << what << jstatus;
                        } else {
                                qDebug() << "AgentsPanelNext::updatePeerAgent()" << what << jstatus;
                        }
                }
        } else if(action == "joinqueue") {
                foreach(QString groupid, glist) {
                        QStringList lqueues = m_vlayout[groupid]->property("queues").toStringList();
                        if(lqueues.contains(qname)) {
                                QString idx = astid + "-" + agentnum + "-" + groupid;
                                if(! m_agent_labels.contains(idx)) {
                                        m_agent_labels[idx] = new QPushButton();
                                        m_agent_labels[idx]->setProperty("astid", astid);
                                        m_agent_labels[idx]->setProperty("agentid", agentnum);
                                        m_agent_labels[idx]->setProperty("groupid", groupid);
                                        connect( m_agent_labels[idx], SIGNAL(clicked()),
                                                 this, SLOT(agentClicked()) );
                                } else {
                                        qDebug() << "AgentsPanelNext::updatePeerAgent() joinqueue : already" << idx;
                                }
                        }
                }
        } else if(action == "leavequeue") {
                foreach(QString groupid, glist) {
                        QStringList lqueues = m_vlayout[groupid]->property("queues").toStringList();
                        if(lqueues.contains(qname)) {
                                QString idx = astid + "-" + agentnum + "-" + groupid;
                                if(m_agent_labels.contains(idx)) {
                                        delete m_agent_labels[idx];
                                        m_agent_labels.remove(idx);
                                } else {
                                        qDebug() << "AgentsPanelNext::updatePeerAgent() leavequeue : no" << idx;
                                }
                        }
                }
        } else if(action == "unpaused") {
                foreach(QString groupid, glist) {
                        QStringList lqueues = m_vlayout[groupid]->property("queues").toStringList();
                        if(lqueues.contains(qname)) {
                                QString idx = astid + "-" + agentnum + "-" + groupid;
                                if(m_agent_labels.contains(idx))
                                        qDebug() << action << idx;
                        }
                }
        } else if(action == "paused") {
                foreach(QString groupid, glist) {
                        QStringList lqueues = m_vlayout[groupid]->property("queues").toStringList();
                        if(lqueues.contains(qname)) {
                                QString idx = astid + "-" + agentnum + "-" + groupid;
                                if(m_agent_labels.contains(idx))
                                        qDebug() << action << idx;
                        }
                }
        } else if(action == "agentlogin") {
                QString idxa = astid + "-" + agentnum;
                if(m_agent_props.contains(idxa)) {
                        QVariantMap proptemp = m_agent_props[idxa].toMap();
                        QVariantMap properties = proptemp["properties"].toMap();
                        properties["status"] = "AGENT_IDLE";
                        proptemp["properties"] = properties;
                        m_agent_props[idxa] = proptemp;
                }
        } else if(action == "agentlogout") {
                QString idxa = astid + "-" + agentnum;
                if(m_agent_props.contains(idxa)) {
                        QVariantMap proptemp = m_agent_props[idxa].toMap();
                        QVariantMap properties = proptemp["properties"].toMap();
                        properties["status"] = "AGENT_LOGGEDOFF";
                        proptemp["properties"] = properties;
                        m_agent_props[idxa] = proptemp;
                }
        } else if((action == "agentlink") || (action == "phonelink")) {
                QString idxa = astid + "-" + agentnum;
                if(m_agent_props.contains(idxa)) {
                        QVariantMap proptemp = m_agent_props[idxa].toMap();
                        QVariantMap properties = proptemp["properties"].toMap();
                        properties["link"] = true;
                        proptemp["properties"] = properties;
                        m_agent_props[idxa] = proptemp;
                }
        } else if((action == "agentunlink") || (action == "phoneunlink")) {
                QString idxa = astid + "-" + agentnum;
                if(m_agent_props.contains(idxa)) {
                        QVariantMap proptemp = m_agent_props[idxa].toMap();
                        QVariantMap properties = proptemp["properties"].toMap();
                        properties["link"] = false;
                        proptemp["properties"] = properties;
                        m_agent_props[idxa] = proptemp;
                }
        }
        
        foreach (QString idx, m_agent_labels.keys())
                setAgentProps(idx);
}

void AgentsPanelNext::agentClicked()
{
        QPushButton * qpb = qobject_cast<QPushButton *>(sender());
        qDebug() << "AgentsPanelNext::agentClicked()" << qpb->property("agentid") << qpb->pos();
        QMessageBox * msgbox = new QMessageBox();
        QString text = sender()->property("astid").toString();
        msgbox->setWindowTitle("Agent " + sender()->property("agentid").toString());
        msgbox->setIcon(QMessageBox::Information);
        msgbox->setText(text);
        msgbox->setGeometry(100, 100, 0, 0);
        msgbox->show();
}

void AgentsPanelNext::setAgentList(const QVariant & alist)
{
        // qDebug() << "AgentsPanelNext::setAgentList()" << alist;
        QVariantMap alistmap = alist.toMap();
        QString astid = alistmap["astid"].toString();
        QStringList agentids = alistmap["newlist"].toMap().keys();
        
        // sets the labels
        foreach (QString agentid, agentids) {
                QVariantMap agqjoined = alistmap["newlist"].toMap()[agentid].toMap()["queues"].toMap();
                
                QString idxa = astid + "-" + agentid;
                if(! m_agent_props.contains(idxa)) {
                        QVariantMap p;
                        p["astid"] = astid;
                        p["agentid"] = agentid;
                        p["properties"] = alistmap["newlist"].toMap()[agentid].toMap()["properties"];
                        p["firstname"] = alistmap["newlist"].toMap()[agentid].toMap()["firstname"];
                        p["lastname"] = alistmap["newlist"].toMap()[agentid].toMap()["lastname"];
                        m_agent_props[idxa] = p;
                } else {
                        qDebug() << "already there" << idxa;
                }
                
                foreach (QString qname, agqjoined.keys()) {
                        QString sstatus = agqjoined[qname].toMap()["Status"].toString();
                        if((sstatus == "1") || (sstatus == "5"))
                                foreach(QString groupid, glist) {
                                        QStringList lqueues = m_vlayout[groupid]->property("queues").toStringList();
                                        if(lqueues.contains(qname)) {
                                                QString idx = astid + "-" + agentid + "-" + groupid;
                                                if(! m_agent_labels.contains(idx)) {
                                                        m_agent_labels[idx] = new QPushButton();
                                                        m_agent_labels[idx]->setProperty("astid", astid);
                                                        m_agent_labels[idx]->setProperty("agentid", agentid);
                                                        m_agent_labels[idx]->setProperty("groupid", groupid);
                                                        connect( m_agent_labels[idx], SIGNAL(clicked()),
                                                                 this, SLOT(agentClicked()) );
                                                }
                                        }
                                }
                }
        }
        
        qDebug() << "AgentsPanelNext::setAgentList()" << m_agent_labels.keys();
        
        foreach (QString idx, m_agent_labels.keys())
                setAgentProps(idx);
        
        foreach (QString agentid, agentids) {
                QVariant properties = alistmap["newlist"].toMap()[agentid].toMap()["properties"];
                QVariantMap agqjoined = alistmap["newlist"].toMap()[agentid].toMap()["queues"].toMap();
                QString agstatus = properties.toMap()["status"].toString();
                QString agfullname = properties.toMap()["name"].toString();
                QString phonenum = properties.toMap()["phonenum"].toString();
                
                if(! m_agent_labels.contains(agentid)) {
                        foreach (QString qname, agqjoined.keys()) {
                                // qDebug() << astid << agentid << qname;
                                QVariant qv = agqjoined[qname];
                                QString pstatus = qv.toMap()["Paused"].toString();
                                QString sstatus = qv.toMap()["Status"].toString();
                                // m_agent_joined_list[agentid] << qname;
                                // if(pstatus == "0")
                                // m_agent_paused_list[agentid] << qname;
                        }
                }
        }
}

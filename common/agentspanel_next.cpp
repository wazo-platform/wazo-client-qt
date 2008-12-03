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

/* $Revision$
 * $Date$
 */

#include <QContextMenuEvent>
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

/*! \brief Constructor
 */
AgentsPanelNext::AgentsPanelNext(const QVariant & optionmap,
                                 QWidget * parent)
        : QWidget(parent)
{
        m_groups["gr1"] = "MARCHE A";
        m_groups["gr2"] = "MARCHE C";
        m_groups["gr3"] = "MARCHE F";
        m_groups["gr4"] = "MARCHE G";
        m_groups["gr5"] = "MARCHE J";
        m_groups["gr6"] = "MARCHE L";
        
        m_hlayout = new QHBoxLayout(this);
        m_hlayout->setSpacing(1);
        foreach(QString groupid, m_groups.keys()) {
                m_vlayout[groupid] = new QVBoxLayout();
                m_hlayout->addLayout(m_vlayout[groupid]);
                m_title[groupid] = new QLabel(m_groups[groupid]);
                m_title[groupid]->setMargin(10);
                m_title[groupid]->setStyleSheet("QLabel {background: #ffff80};");
                m_titleedit[groupid] = new QPushButton();
                m_vlayout[groupid]->addWidget(m_title[groupid], 0, Qt::AlignCenter);
                m_vlayout[groupid]->addWidget(m_titleedit[groupid], 0, Qt::AlignCenter);
                m_vlayout[groupid]->insertStretch(100, 1);
                m_vlayout[groupid]->setSpacing(1);
        }
        m_hlayout->insertStretch(100, 1);
        m_vlayout["gr1"]->setProperty("queues", (QStringList() << "technique" << "eclair" << "normal"));
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
        
        // setFont(m_gui_font);
        foreach(QString groupid, m_groups.keys())
                m_title[groupid]->setFont(m_gui_font);
}

void AgentsPanelNext::contextMenuEvent(QContextMenuEvent * event)
{
        qDebug() << "AgentsPanelNext::contextMenuEvent" << event << event->pos() << event->reason();
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
        QVariant properties = m_agent_props[idxa].toMap()["properties"];
        QVariant queues = m_agent_props[idxa].toMap()["queues"];
        QString firstname = m_agent_props[idxa].toMap()["firstname"].toString();
        QString lastname = m_agent_props[idxa].toMap()["lastname"].toString();
        
        // foreach(QString qname, queues.toMap().keys())
        // qDebug() << idx << qname << queues.toMap()[qname].toMap()["Status"].toString() << queues.toMap()[qname].toMap()["Paused"].toString();
        m_vlayout[groupid]->insertWidget( 2, m_agent_labels[idx] );
        
        QString agstatus = properties.toMap()["status"].toString();
        QString phonenum = properties.toMap()["phonenum"].toString();
        bool link = properties.toMap()["link"].toBool();
        
        QString disptext = firstname + " " + lastname + " " + agentid;
        int nsec = 356;
        int dmin = nsec / 60;
        int dsec = nsec % 60;
        QString displayedtime;
        if(dmin > 0)
                displayedtime = tr("%1 min %2 sec").arg(QString::number(dmin), QString::number(dsec));
        else
                displayedtime = tr("%1 sec").arg(QString::number(dsec));
        
        QString colorqss;
        if(agstatus == "AGENT_IDLE")
                colorqss = "grey";
        else if(agstatus == "AGENT_LOGGEDOFF")
                colorqss = "#ff8080";
        else if(agstatus == "AGENT_ONCALL") {
                colorqss = "green";
                disptext += " I";
                disptext += " " + displayedtime;
        } else
                colorqss = "blue";
        if(link) {
                colorqss = "#80ff80";
                disptext += " S";
                disptext += " " + displayedtime;
        }
        
        QString qss = "QPushButton {border: 5px solid " + colorqss + "; border-radius: 0px; background: " + colorqss + "};";
        m_agent_labels[idx]->setStyleSheet(qss);
        m_agent_labels[idx]->setText(disptext);
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
        qDebug() << "AgentsPanelNext" << action << agentnum << qname << jstatus << pstatus;
        
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
                foreach(QString groupid, m_groups.keys()) {
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
                foreach(QString groupid, m_groups.keys()) {
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
                QString idxa = astid + "-" + agentnum;
                if(m_agent_props.contains(idxa)) {
                        QVariantMap proptemp = m_agent_props[idxa].toMap();
                        QVariantMap pqueues = proptemp["queues"].toMap();
                        QVariantMap qprops = pqueues[qname].toMap();
                        qprops["Paused"] = "0";
                        pqueues[qname] = qprops;
                        proptemp["queues"] = pqueues;
                        m_agent_props[idxa] = proptemp;
                }
        } else if(action == "paused") {
                QString idxa = astid + "-" + agentnum;
                if(m_agent_props.contains(idxa)) {
                        QVariantMap proptemp = m_agent_props[idxa].toMap();
                        QVariantMap pqueues = proptemp["queues"].toMap();
                        QVariantMap qprops = pqueues[qname].toMap();
                        qprops["Paused"] = "1";
                        pqueues[qname] = qprops;
                        proptemp["queues"] = pqueues;
                        m_agent_props[idxa] = proptemp;
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
        
        // qDebug() << "AgentsPanelNext::updatePeerAgent()" << m_agent_labels.keys();
        foreach (QString idx, m_agent_labels.keys())
                setAgentProps(idx);
}

void AgentsPanelNext::agentClicked()
{
        // qDebug() << "AgentsPanelNext::agentClicked()";
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
                        p["queues"] = alistmap["newlist"].toMap()[agentid].toMap()["queues"];
                        p["firstname"] = alistmap["newlist"].toMap()[agentid].toMap()["firstname"];
                        p["lastname"] = alistmap["newlist"].toMap()[agentid].toMap()["lastname"];
                        m_agent_props[idxa] = p;
                } else {
                        qDebug() << "already there" << idxa;
                }
                
                foreach (QString qname, agqjoined.keys()) {
                        QString sstatus = agqjoined[qname].toMap()["Status"].toString();
                        if((sstatus == "1") || (sstatus == "5"))
                                foreach(QString groupid, m_groups.keys()) {
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
        
        // qDebug() << "AgentsPanelNext::setAgentList()" << m_agent_labels.keys();
        foreach (QString idx, m_agent_labels.keys())
                setAgentProps(idx);
}

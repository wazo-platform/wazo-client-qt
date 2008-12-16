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

#include <QAction>
#include <QComboBox>
#include <QContextMenuEvent>
#include <QDateTime>
#include <QDebug>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QTextEdit>
#include <QMenu>
#include <QMessageBox>
#include <QPushButton>
#include <QScrollArea>
#include <QVariant>
#include <QVBoxLayout>

#include "agentspanel_next.h"
#include "extendedlabel.h"
#include "userinfo.h"

#define NCOLS 3

/*! \brief Constructor
 */
AgentsPanelNext::AgentsPanelNext(const QVariant & optionmap,
                                 QWidget * parent)
        : QWidget(parent)
{
        m_glayout = new QGridLayout(this);
        m_glayout->setSpacing(1);
        
        // m_gridlayout->setVerticalSpacing(0);
        
        setGuiOptions(optionmap);
        startTimer(1000);
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
        foreach (QString groupid, m_title.keys())
                m_title[groupid]->setFont(m_gui_font);
}

void AgentsPanelNext::contextMenuEvent(QContextMenuEvent * event)
{
        QMenu contextMenu(this);
        
        QAction * newGroupAction = new QAction(tr("New Group"), this);
        newGroupAction->setProperty("where", event->globalPos());
        connect(newGroupAction, SIGNAL(triggered()),
                this, SLOT(newGroup()) );
        
        if(sender() == NULL) {
                contextMenu.addAction(newGroupAction);
        } else {
                // group or agent label
                if(sender()->property("agentid").isNull()) {
                        // group label
                        ExtendedLabel * el = qobject_cast<ExtendedLabel *>(sender());
                        QStringList thisqueuelist = el->property("queues").toStringList();
                        QString thisgroupid = el->property("groupid").toString();
                        
                        QAction * renameAction = new QAction(tr("Rename this Group"), this);
                        
                        contextMenu.addAction(renameAction);
                        renameAction->setProperty("groupid", thisgroupid);
                        renameAction->setProperty("where", event->globalPos());
                        connect(renameAction, SIGNAL(triggered()),
                                this, SLOT(renameQueueGroup()) );
                        
                        QAction * removeAction = new QAction(tr("Remove this Group"), this);
                        contextMenu.addAction(removeAction);
                        removeAction->setProperty("groupid", thisgroupid);
                        connect(removeAction, SIGNAL(triggered()),
                                this, SLOT(removeQueueGroup()) );
                        
                        contextMenu.addAction(newGroupAction);
                        contextMenu.addSeparator();
                        
                        QMenu * menu_remove = contextMenu.addMenu(tr("Remove a Queue"));
                        foreach (QString qname, el->property("queues").toStringList()) {
                                QAction * qremove = new QAction(qname, this);
                                qremove->setProperty("groupid", thisgroupid);
                                qremove->setProperty("queuename", qname);
                                menu_remove->addAction(qremove);
                                connect(qremove, SIGNAL(triggered()),
                                        this, SLOT(removeQueueFromGroup()) );
                        }
                        
                        QMenu * menu_add = contextMenu.addMenu(tr("Add a Queue"));
                        foreach (QString qname, m_queuelist)
                                if(! thisqueuelist.contains(qname)) {
                                        QAction * qadd = new QAction(qname, this);
                                        qadd->setProperty("groupid", thisgroupid);
                                        qadd->setProperty("queuename", qname);
                                        menu_add->addAction(qadd);
                                        connect(qadd, SIGNAL(triggered()),
                                                this, SLOT(addQueueToGroup()) );
                                }
                } else
                        return;
        }
        
        contextMenu.exec(event->globalPos());
}

void AgentsPanelNext::newGroup()
{
        QPoint where = sender()->property("where").toPoint();
        QGridLayout * gl = new QGridLayout();
        QDialog * dialog = new QDialog();
        dialog->setWindowTitle(tr("New Group"));
        dialog->setLayout(gl);
        QLabel * q1 = new QLabel(tr("Name"));
        QTextEdit * q2 = new QTextEdit();
        QPushButton * q3 = new QPushButton(tr("OK"));
        QPushButton * q4 = new QPushButton(tr("Cancel"));
        gl->addWidget(q1, 0, 0);
        gl->addWidget(q2, 0, 1);
        gl->addWidget(q3, 1, 0);
        gl->addWidget(q4, 1, 1);
        q2->setFixedSize(QSize(200, 60));
        connect( q3, SIGNAL(clicked()),
                 dialog, SLOT(close()) );
        connect( q4, SIGNAL(clicked()),
                 dialog, SLOT(close()) );
        dialog->move(where);
        dialog->exec();
        if(! q2->toPlainText().trimmed().isEmpty()) {
                QString groupid = QString::number(QDateTime::currentDateTime().toTime_t());
                m_title[groupid] = new ExtendedLabel();
                m_title[groupid]->setText(q2->toPlainText().trimmed());
                m_title[groupid]->setProperty("queues", (QStringList()));
                m_title[groupid]->setProperty("groupid", groupid);
                m_title[groupid]->setAlignment(Qt::AlignCenter);
                connect( m_title[groupid], SIGNAL(context_menu(QContextMenuEvent *)),
                         this, SLOT(contextMenuEvent(QContextMenuEvent *)) );
                m_title[groupid]->setStyleSheet("QLabel {background: #ffff80};");
                refreshContents();
                refreshDisplay();
        }
}

void AgentsPanelNext::mouseReleasedEvent(QMouseEvent * event)
{
        qDebug() << "AgentsPanelNext::mouseReleasedEvent()" << event << event->pos();
}

void AgentsPanelNext::setUserInfo(const UserInfo * ui)
{
        m_userinfo = ui;
}

void AgentsPanelNext::loadGroups()
{
        loadQueueGroups();
}

void AgentsPanelNext::setGroups(const QVariant & groups)
{
        // qDebug() << "AgentsPanelNext::setGroups()" << groups;
        foreach (QString groupid, groups.toMap().keys()) {
                m_title[groupid] = new ExtendedLabel(groups.toMap()[groupid].toMap()["label"].toString());
                m_title[groupid]->setProperty("groupid", groupid);
                m_title[groupid]->setProperty("queues", groups.toMap()[groupid].toMap()["queues"].toStringList());
                m_title[groupid]->setToolTip(groups.toMap()[groupid].toMap()["queues"].toStringList().join(", "));
                m_title[groupid]->setAlignment(Qt::AlignCenter);
                connect( m_title[groupid], SIGNAL(context_menu(QContextMenuEvent *)),
                         this, SLOT(contextMenuEvent(QContextMenuEvent *)) );
                int ix = m_title.keys().indexOf(groupid);
                m_title[groupid]->setStyleSheet("QLabel {background: #ffff80};");
                m_glayout->addWidget(m_title[groupid], 0, ix * NCOLS, 1, NCOLS, Qt::AlignCenter);
        }
        refreshContents();
        refreshDisplay();
}

void AgentsPanelNext::saveGroups()
{
        QVariantMap save;
        foreach (QString groupid, m_title.keys()) {
                QVariantMap tmp;
                tmp["queues"] = m_title[groupid]->property("queues");
                tmp["groupid"] = m_title[groupid]->property("groupid");
                tmp["label"] = m_title[groupid]->text();
                save[groupid] = tmp;
        }
        saveQueueGroups(save);
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
        
        m_agent_labels[idx]->setProperty("sorter", firstname + lastname);
        
        // foreach (QString qname, queues.toMap().keys())
        // qDebug() << idx << qname << queues.toMap()[qname].toMap()["Status"].toString() << queues.toMap()[qname].toMap()["Paused"].toString();
        
        QString agstatus = properties.toMap()["status"].toString();
        QString phonenum = properties.toMap()["phonenum"].toString();
        QString link = properties.toMap()["link"].toString();
        
        // qDebug() << astid << agentid << link << properties.toMap();
        
        QString disptext = firstname + " " + lastname + " " + agentid;
        
        QString colorqss;
        if(agstatus == "AGENT_IDLE")
                colorqss = "grey";
        else if(agstatus == "AGENT_LOGGEDOFF")
                colorqss = "#ff8080";
        else if(agstatus == "AGENT_ONCALL") {
                colorqss = "green";
                disptext += " I";
        } else
                colorqss = "blue";
        
        if(link == "agentlink") {
                colorqss = "#80ff80";
                disptext += " E";
        }
        if(link == "phonelink") {
                colorqss = "#80ff80";
                bool isdid = properties.toMap()["did"].toBool();
                bool isoutcall = properties.toMap()["outcall"].toBool();
                if(isdid)
                        disptext += " E";
                else if(isoutcall)
                        disptext += " S";
                else
                        disptext += " I";
        }
        
        QDateTime inittime;
        if(properties.toMap().contains("inittime"))
                inittime = properties.toMap()["inittime"].toDateTime();
        
        QVariantMap qvm = queues.toMap();
        foreach (QString qname_group, m_title[groupid]->property("queues").toStringList()) {
                if (qvm.contains(qname_group)) {
                        QString pstatus = qvm[qname_group].toMap()["Paused"].toString();
                        // qDebug() << idx << idxa << qname_group
                        // << qvm[qname_group].toMap()["Status"].toString()
                        // << qvm[qname_group].toMap()["Paused"].toString()
                        // << qvm[qname_group].toMap()["PausedTime"].toString();
                        if(pstatus == "1") {
                                inittime = qvm[qname_group].toMap()["PausedTime"].toDateTime();
                                if(inittime.isNull())
                                        inittime = QDateTime::currentDateTime();
                        }
                }
        }
        
        if(inittime.isValid()) {
                int nsec = inittime.secsTo(QDateTime::currentDateTime());
                int dmin = nsec / 60;
                int dsec = nsec % 60;
                QString displayedtime;
                if(dmin > 0)
                        displayedtime = tr("%1 min %2 sec").arg(QString::number(dmin), QString::number(dsec));
                else
                        displayedtime = tr("%1 sec").arg(QString::number(dsec));
                disptext += " " + displayedtime;
        }
        
        QString qss = "QLabel {border: 5px solid " + colorqss + "; border-radius: 0px; background: " + colorqss + "};";
        m_agent_labels[idx]->setStyleSheet(qss);
        m_agent_labels[idx]->setText(disptext);
}

void AgentsPanelNext::updateAgentPresence(const QString &, const QVariant &)
{
        // qDebug() << "AgentsPanelNext::updateAgentPresence()" << agentname << presencestatus;
        
        // QColor color = QColor(presencestatus.toMap()["color"].toString());
}

void AgentsPanelNext::updatePeerAgent(const QString &,
                                      const QString & what,
                                      const QVariant & params)
{
        if(what != "agentstatus")
                return;
        // qDebug() << "AgentsPanelNext::updatePeerAgent()" << params;
        QString action = params.toMap()["action"].toString();
        QString astid = params.toMap()["astid"].toString();
        QString agentnum = params.toMap()["agent_channel"].toString().mid(6);
        QString qname = params.toMap()["queuename"].toString();
        QString jstatus = params.toMap()["joinedstatus"].toString();
        QString pstatus = params.toMap()["pausedstatus"].toString();
        // qDebug() << "AgentsPanelNext::updatePeerAgent()" << action << agentnum << qname << jstatus << pstatus;
        
        if(action == "queuememberstatus") {
                if(m_agent_labels.contains(agentnum)) {
                        if(jstatus == "1") {
                                // queue member has logged in  / is available
                        } else if(jstatus == "5") {
                                // queue member has logged off
                        } else if(jstatus == "3") {
                                // queue member is called
                        } else {
                                qDebug() << "AgentsPanelNext::updatePeerAgent()" << what << jstatus;
                        }
                }
        } else if(action == "joinqueue") {
                QString idxa = astid + "-" + agentnum;
                if(m_agent_props.contains(idxa)) {
                        QVariantMap proptemp = m_agent_props[idxa].toMap();
                        QVariantMap pqueues = proptemp["queues"].toMap();
                        if(pqueues[qname].toMap().isEmpty()) {
                                QVariantMap qprops;
                                qprops["Paused"] = pstatus;
                                qprops["Status"] = "1";
                                pqueues[qname] = qprops;
                                proptemp["queues"] = pqueues;
                                m_agent_props[idxa] = proptemp;
                                refreshContents();
                        } else
                                qDebug() << action << idxa << "not empty" << qname;
                } else {
                        qDebug() << "AgentsPanelNext::updatePeerAgent() warning : undefined" << idxa << action;
                }
        } else if(action == "leavequeue") {
                QString idxa = astid + "-" + agentnum;
                if(m_agent_props.contains(idxa)) {
                        QVariantMap proptemp = m_agent_props[idxa].toMap();
                        QVariantMap pqueues = proptemp["queues"].toMap();
                        if(pqueues[qname].toMap().isEmpty())
                                qDebug() << action << idxa << "already empty" << qname;
                        else {
                                QVariantMap qprops;
                                pqueues[qname] = qprops;
                                proptemp["queues"] = pqueues;
                                m_agent_props[idxa] = proptemp;
                                refreshContents();
                        }
                } else {
                        qDebug() << "AgentsPanelNext::updatePeerAgent() warning : undefined" << idxa << action;
                }
        } else if(action == "unpaused") {
                QString idxa = astid + "-" + agentnum;
                if(m_agent_props.contains(idxa)) {
                        QVariantMap proptemp = m_agent_props[idxa].toMap();
                        QVariantMap properties = proptemp["properties"].toMap();
                        QVariantMap pqueues = proptemp["queues"].toMap();
                        QVariantMap qprops = pqueues[qname].toMap();
                        qprops["Paused"] = "0";
                        qprops.remove("PausedTime");
                        pqueues[qname] = qprops;
                        proptemp["properties"] = properties;
                        proptemp["queues"] = pqueues;
                        m_agent_props[idxa] = proptemp;
                }
        } else if(action == "paused") {
                QString idxa = astid + "-" + agentnum;
                if(m_agent_props.contains(idxa)) {
                        QVariantMap proptemp = m_agent_props[idxa].toMap();
                        QVariantMap properties = proptemp["properties"].toMap();
                        QVariantMap pqueues = proptemp["queues"].toMap();
                        QVariantMap qprops = pqueues[qname].toMap();
                        qprops["Paused"] = "1";
                        qprops["PausedTime"] = QDateTime::currentDateTime();
                        pqueues[qname] = qprops;
                        proptemp["properties"] = properties;
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
                        properties["link"] = action;
                        properties["inittime"] = QDateTime::currentDateTime();
                        properties["dir"] = params.toMap()["dir"].toString();
                        properties["did"] = params.toMap()["did"].toBool();
                        properties["outcall"] = params.toMap()["outcall"].toBool();
                        proptemp["properties"] = properties;
                        m_agent_props[idxa] = proptemp;
                }
        } else if((action == "agentunlink") || (action == "phoneunlink")) {
                QString idxa = astid + "-" + agentnum;
                if(m_agent_props.contains(idxa)) {
                        QVariantMap proptemp = m_agent_props[idxa].toMap();
                        QVariantMap properties = proptemp["properties"].toMap();
                        properties["link"] = action;
                        properties.remove("inittime");
                        proptemp["properties"] = properties;
                        m_agent_props[idxa] = proptemp;
                }
        }
        
        // refreshContents();
        refreshDisplay();
}

void AgentsPanelNext::renameQueueGroup()
{
        QString groupid = sender()->property("groupid").toString();
        QPoint where = sender()->property("where").toPoint();
        QGridLayout * gl = new QGridLayout();
        QDialog * dialog = new QDialog();
        dialog->setWindowTitle(tr("Rename a Group"));
        dialog->setLayout(gl);
        QLabel * q1 = new QLabel(tr("New Name"));
        QTextEdit * q2 = new QTextEdit();
        q2->setPlainText(m_title[groupid]->text());
        QPushButton * q3 = new QPushButton(tr("OK"));
        QPushButton * q4 = new QPushButton(tr("Cancel"));
        gl->addWidget(q1, 0, 0);
        gl->addWidget(q2, 0, 1);
        gl->addWidget(q3, 1, 0);
        gl->addWidget(q4, 1, 1);
        q2->setFixedSize(QSize(200, 60));
        connect( q3, SIGNAL(clicked()),
                 dialog, SLOT(close()) );
        connect( q4, SIGNAL(clicked()),
                 dialog, SLOT(close()) );
        dialog->move(where);
        dialog->exec();
        if(! q2->toPlainText().trimmed().isEmpty())
                m_title[groupid]->setText(q2->toPlainText().trimmed());
        saveGroups();
}

void AgentsPanelNext::removeQueueGroup()
{
        QString groupid = sender()->property("groupid").toString();
        if(m_title.contains(groupid)) {
                m_title[groupid]->deleteLater();
                m_title.remove(groupid);
                
                // trick in order to hide an extra item (Qt Bug ?)
                QLayoutItem * layoutitem = m_glayout->itemAtPosition(0, m_title.keys().size() * NCOLS);
                if(layoutitem) {
                        layoutitem->widget()->hide();
                        m_glayout->removeWidget(layoutitem->widget());
                }
                
                refreshContents();
                refreshDisplay();
        }
}

void AgentsPanelNext::removeQueueFromGroup()
{
        QString groupid = sender()->property("groupid").toString();
        QString queuename = sender()->property("queuename").toString();
        // qDebug() << "AgentsPanelNext::removeQueueFromGroup()" << groupid << queuename;
        QStringList qlist = m_title[groupid]->property("queues").toStringList();
        if(qlist.contains(queuename)) {
                qlist.removeAll(queuename);
                m_title[groupid]->setProperty("queues", qlist);
                m_title[groupid]->setToolTip(qlist.join(", "));
                refreshContents();
                refreshDisplay();
        }
}

void AgentsPanelNext::addQueueToGroup()
{
        QString groupid = sender()->property("groupid").toString();
        QString queuename = sender()->property("queuename").toString();
        // qDebug() << "AgentsPanelNext::addQueueToGroup()" << groupid << queuename;
        QStringList qlist = m_title[groupid]->property("queues").toStringList();
        if(! qlist.contains(queuename)) {
                qlist.append(queuename);
                m_title[groupid]->setProperty("queues", qlist);
                m_title[groupid]->setToolTip(qlist.join(", "));
                refreshContents();
                refreshDisplay();
        }
}

void AgentsPanelNext::agentClicked(QMouseEvent * event)
{
        if(event->button() == Qt::LeftButton) {
                QString astid = sender()->property("astid").toString();
                QString agentid = sender()->property("agentid").toString();
                QString groupid = sender()->property("groupid").toString();
                QPoint where = event->globalPos();
                QString idxa = astid + "-" + agentid;
                
                QGridLayout * gl = new QGridLayout();
                QDialog * dialog = new QDialog(this);
                dialog->setWindowTitle(tr("Agent %1 on %2").arg(agentid, astid));
                dialog->setLayout(gl);
                
                QLabel * q_name = new QLabel(m_agent_props[idxa].toMap()["firstname"].toString());
                QLabel * q_agentid = new QLabel(agentid);
                QString n = QString::number(0);
                QString m = QString::number(0);
                QLabel * q_received = new QLabel(tr("%1 calls received since connection").arg(n));
                QLabel * q_lost = new QLabel(tr("%1 calls lost since connection").arg(m));
                
                QPushButton * q_cancelpause = new QPushButton(tr("Cancel Pause"));
                q_cancelpause->setProperty("action", "cancelpause");
                q_cancelpause->setProperty("astid", astid);
                q_cancelpause->setProperty("agentid", agentid);
                q_cancelpause->setProperty("groupid", groupid);
                connect( q_cancelpause, SIGNAL(clicked()),
                         this, SLOT(actionclicked()) );
                
                QPushButton * q_logout = new QPushButton(tr("Logout"));
                q_logout->setProperty("action", "logout");
                q_logout->setProperty("astid", astid);
                q_logout->setProperty("agentid", agentid);
                connect( q_logout, SIGNAL(clicked()),
                         this, SLOT(actionclicked()) );
                
                QPushButton * q_transfer = new QPushButton(tr("Transfer"));
                q_transfer->setProperty("action", "transfer");
                q_transfer->setProperty("astid", astid);
                q_transfer->setProperty("agentid", agentid);
                connect( q_transfer, SIGNAL(clicked()),
                         this, SLOT(actionclicked()) );
                
                QLabel * q_labelqueues = new QLabel(tr("Available Queues"));
                m_queue_chose = new QComboBox();
                m_queue_chose->setSizeAdjustPolicy(QComboBox::AdjustToContents);
                foreach (QString queuename, m_queuelist)
                        m_queue_chose->addItem(queuename);
                
                QPushButton * q_close = new QPushButton(tr("Close"));
                QFrame * q_hline1 = new QFrame(this);
                QFrame * q_hline2 = new QFrame(this);
                q_hline1->setFrameShape(QFrame::HLine);
                q_hline2->setFrameShape(QFrame::HLine);
                
                int iy = 0;
                gl->addWidget(q_name, iy, 0, Qt::AlignCenter);
                gl->addWidget(q_agentid, iy, 1, Qt::AlignCenter);
                iy ++;
                gl->addWidget(q_received, iy, 0, 1, 2, Qt::AlignCenter);
                iy ++;
                gl->addWidget(q_lost, iy, 0, 1, 2, Qt::AlignCenter);
                iy ++;
                gl->addWidget(q_cancelpause, iy, 0, Qt::AlignCenter);
                gl->addWidget(q_logout, iy, 1, Qt::AlignCenter);
                iy ++;
                gl->addWidget(q_hline1, iy, 0, 1, 2);
                iy ++;
                gl->addWidget(q_labelqueues, iy, 1, Qt::AlignCenter);
                iy ++;
                gl->addWidget(q_transfer, iy, 0, Qt::AlignCenter);
                gl->addWidget(m_queue_chose, iy, 1, Qt::AlignCenter);
                iy ++;
                gl->addWidget(q_hline2, iy, 0, 1, 2);
                iy ++;
                gl->addWidget(q_close, iy, 1, Qt::AlignCenter);
                iy ++;
                connect( q_close, SIGNAL(clicked()),
                         dialog, SLOT(close()) );
                dialog->move(where);
                dialog->exec();
        }
}

void AgentsPanelNext::actionclicked()
{
        QString action = sender()->property("action").toString();
        QString astid = sender()->property("astid").toString();
        QString agentid = sender()->property("agentid").toString();
        QString groupid = sender()->property("groupid").toString();
        if(action == "transfer")
                agentAction("transfer " + astid + " " + agentid + " " + m_queue_chose->currentText());
        else if(action == "cancelpause")
                foreach (QString qname, m_title[groupid]->property("queues").toStringList())
                        agentAction("unpause " + qname + " " + astid + " " + agentid);
        else if(action == "logout")
                agentAction("logout " + astid + " " + agentid);
}

void AgentsPanelNext::refreshContents()
{
        foreach (QString idx, m_agent_labels.keys()) {
                delete m_agent_labels[idx];
                m_agent_labels.remove(idx);
        }
        
        foreach (QString idxa, m_agent_props.keys()) {
                QString astid = m_agent_props[idxa].toMap()["astid"].toString();
                QString agentid = m_agent_props[idxa].toMap()["agentid"].toString();
                QVariantMap agqjoined = m_agent_props[idxa].toMap()["queues"].toMap();
                
                foreach (QString qname, agqjoined.keys()) {
                        if(! agqjoined[qname].toMap().isEmpty()) {
                                QString sstatus = agqjoined[qname].toMap()["Status"].toString();
                                if((sstatus == "1") || (sstatus == "5")) {
                                        foreach (QString groupid, m_title.keys()) {
                                                QStringList lqueues = m_title[groupid]->property("queues").toStringList();
                                                if(lqueues.contains(qname)) {
                                                        QString idx = astid + "-" + agentid + "-" + groupid;
                                                        if(! m_agent_labels.contains(idx)) {
                                                                m_agent_labels[idx] = new ExtendedLabel();
                                                                m_agent_labels[idx]->setProperty("astid", astid);
                                                                m_agent_labels[idx]->setProperty("agentid", agentid);
                                                                m_agent_labels[idx]->setProperty("groupid", groupid);
                                                                connect( m_agent_labels[idx], SIGNAL(mouse_release(QMouseEvent *)),
                                                                         this, SLOT(agentClicked(QMouseEvent *)) );
                                                                connect( m_agent_labels[idx], SIGNAL(context_menu(QContextMenuEvent *)),
                                                                         this, SLOT(contextMenuEvent(QContextMenuEvent *)) );
                                                        }
                                                }
                                        }
                                } else if (sstatus == "3") {
                                        qDebug() << "AgentsPanelNext::refreshContents()" << idxa << sstatus;
                                } else {
                                        qDebug() << "AgentsPanelNext::refreshContents()" << idxa << sstatus;
                                }
                        }
                }
        }
        saveGroups();
}

void AgentsPanelNext::refreshDisplay()
{
        int nmax = 1;
        QHash<QString, QMap<QString, QString> > columns_sorter;
        // qDebug() << "AgentsPanelNext::refreshDisplay()";
        
        foreach (QString groupid, m_title.keys()) {
                QMap<QString, QString> map;
                columns_sorter[groupid] = map;
        }
        foreach (QString idx, m_agent_labels.keys()) {
                setAgentProps(idx);
                QString groupid = m_agent_labels[idx]->property("groupid").toString();
                QString sorter = m_agent_labels[idx]->property("sorter").toString();
                columns_sorter[groupid].insertMulti(sorter, idx);
        }
        foreach (QString groupid, m_title.keys()) {
                int iy = 1;
                int ix = m_title.keys().indexOf(groupid);
                m_glayout->setColumnStretch(ix * NCOLS, 0);
                m_glayout->addWidget(m_title[groupid], 0, ix * NCOLS, 1, NCOLS);
                QMap<QString, QString> lst = columns_sorter[groupid];
                foreach (QString srt, lst.uniqueKeys()) {
                        foreach (QString value, lst.values(srt)) {
                                m_glayout->setRowStretch(iy, 0);
                                m_glayout->addWidget(m_agent_labels[value], iy++, ix * NCOLS);
                        }
                }
                if(iy > nmax)
                        nmax = iy;
        }
        m_glayout->setRowStretch(nmax, 1);
        m_glayout->setColumnStretch(m_title.size() * NCOLS, 1);
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
                        qDebug() << "idxa already there" << idxa;
                }
        }
        loadGroups();
}

void AgentsPanelNext::setQueueList(bool, const QVariant & qlist)
{
        // qDebug() << "QueuesPanel::setQueueList()" << qlist;
        QVariantMap qlistmap = qlist.toMap();
        QString astid = qlistmap["astid"].toString();
        QStringList queues = qlistmap["queuestats"].toMap().keys();
        foreach (QString queuename, queues)
                if(! m_queuelist.contains(queuename))
                        m_queuelist.append(queuename);
}

void AgentsPanelNext::timerEvent(QTimerEvent *)
{
        // qDebug() << "AgentsPanelNext::timerEvent()";
        foreach (QString idx, m_agent_labels.keys())
                setAgentProps(idx);
}

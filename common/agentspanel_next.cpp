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

/*! \brief Constructor
 */
AgentsPanelNext::AgentsPanelNext(const QVariant & optionmap,
                                 QWidget * parent)
        : QWidget(parent)
{
        // m_title["gr1"] = new ExtendedLabel("MARCHE\nA\nB");
        // m_title["gr1"]->setProperty("queues", (QStringList() << "technique" << "eclair" << "normal"));
        
        m_glayout = new QGridLayout(this);
        m_glayout->setSpacing(1);
        
        foreach (QString groupid, m_title.keys()) {
                m_title[groupid]->setProperty("groupid", groupid);
                m_title[groupid]->setAlignment(Qt::AlignCenter);
                connect( m_title[groupid], SIGNAL(mouse_release(QMouseEvent *)),
                         this, SLOT(titleClicked(QMouseEvent *)) );
                int index = m_title.keys().indexOf(groupid);
                m_title[groupid]->setStyleSheet("QLabel {background: #ffff80};");
                m_glayout->addWidget(m_title[groupid], 0, index, Qt::AlignCenter);
        }
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
        foreach (QString groupid, m_title.keys())
                m_title[groupid]->setFont(m_gui_font);
}

void AgentsPanelNext::contextMenuEvent(QContextMenuEvent * event)
{
        // qDebug() << "AgentsPanelNext::contextMenuEvent()" << event << event->pos() << event->reason();
        QMenu contextMenu(this);
        
        QAction * loadAction = new QAction(tr("Load Settings"), this);
        contextMenu.addAction(loadAction);
        connect(loadAction, SIGNAL(triggered()),
                this, SLOT(loadGroups()) );
        
        QAction * newGroupAction = new QAction(tr("New Group"), this);
        contextMenu.addAction(newGroupAction);
        newGroupAction->setProperty("where", event->globalPos());
        connect(newGroupAction, SIGNAL(triggered()),
                this, SLOT(newGroup()) );
        
        contextMenu.exec(event->globalPos());
}

void AgentsPanelNext::newGroup()
{
        QPoint where = sender()->property("where").toPoint();
        QGridLayout * gl = new QGridLayout();
        QDialog * dialog = new QDialog();
        dialog->setWindowTitle("New Group");
        dialog->setLayout(gl);
        QLabel * q1 = new QLabel("Name");
        QTextEdit * q2 = new QTextEdit();
        QPushButton * q3 = new QPushButton("OK");
        QPushButton * q4 = new QPushButton("Cancel");
        gl->addWidget(q1, 0, 0);
        gl->addWidget(q2, 0, 1);
        gl->addWidget(q3, 1, 0);
        gl->addWidget(q4, 1, 1);
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
                connect( m_title[groupid], SIGNAL(mouse_release(QMouseEvent *)),
                         this, SLOT(titleClicked(QMouseEvent *)) );
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
                m_title[groupid]->setAlignment(Qt::AlignCenter);
                connect( m_title[groupid], SIGNAL(mouse_release(QMouseEvent *)),
                         this, SLOT(titleClicked(QMouseEvent *)) );
                int index = m_title.keys().indexOf(groupid);
                m_title[groupid]->setStyleSheet("QLabel {background: #ffff80};");
                m_glayout->addWidget(m_title[groupid], 0, index, Qt::AlignCenter);
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
        // qDebug() << "AgentsPanelNext::updateAgentPresence()" << agentname << presencestatus;
        
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
                foreach (QString groupid, m_title.keys()) {
                        QStringList lqueues = m_title[groupid]->property("queues").toStringList();
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
                foreach (QString groupid, m_title.keys()) {
                        QStringList lqueues = m_title[groupid]->property("queues").toStringList();
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
        
        refreshDisplay();
}

void AgentsPanelNext::titleClicked(QMouseEvent * event)
{
        ExtendedLabel * el = qobject_cast<ExtendedLabel *>(sender());
        QStringList thisqueuelist = el->property("queues").toStringList();
        QString thisgroupid = el->property("groupid").toString();
        // qDebug() << "AgentsPanelNext::titleClicked()" << thisgroupid << thisqueuelist;
        
        if(event->button() == Qt::LeftButton) {
                QMenu contextMenu(this);
                
                QAction * renameAction = new QAction(tr("Rename Label"), this);
                contextMenu.addAction(renameAction);
                renameAction->setProperty("groupid", thisgroupid);
                renameAction->setProperty("where", event->globalPos());
                connect(renameAction, SIGNAL(triggered()),
                        this, SLOT(renameQueueGroup()) );
                
                QAction * removeAction = new QAction(tr("Remove Group"), this);
                contextMenu.addAction(removeAction);
                removeAction->setProperty("groupid", thisgroupid);
                connect(removeAction, SIGNAL(triggered()),
                        this, SLOT(removeQueueGroup()) );
                
                QMenu * menu_remove = contextMenu.addMenu(tr("Remove Queue"));
                foreach (QString qname, el->property("queues").toStringList()) {
                        QAction * qremove = new QAction(qname, this);
                        qremove->setProperty("groupid", thisgroupid);
                        qremove->setProperty("queuename", qname);
                        menu_remove->addAction(qremove);
                        connect(qremove, SIGNAL(triggered()),
                                this, SLOT(removeQueueFromGroup()) );
                }
                
                QMenu * menu_add = contextMenu.addMenu(tr("Add Queue"));
                foreach (QString qname, m_queuelist)
                        if(! thisqueuelist.contains(qname)) {
                                QAction * qadd = new QAction(qname, this);
                                qadd->setProperty("groupid", thisgroupid);
                                qadd->setProperty("queuename", qname);
                                menu_add->addAction(qadd);
                                connect(qadd, SIGNAL(triggered()),
                                        this, SLOT(addQueueToGroup()) );
                        }
                
                contextMenu.exec(event->globalPos());
        }
}

void AgentsPanelNext::renameQueueGroup()
{
        QString groupid = sender()->property("groupid").toString();
        QPoint where = sender()->property("where").toPoint();
        QGridLayout * gl = new QGridLayout();
        QDialog * dialog = new QDialog();
        dialog->setWindowTitle("Rename Group");
        dialog->setLayout(gl);
        QLabel * q1 = new QLabel("New Name");
        QTextEdit * q2 = new QTextEdit();
        q2->setPlainText(m_title[groupid]->text());
        QPushButton * q3 = new QPushButton("OK");
        QPushButton * q4 = new QPushButton("Cancel");
        gl->addWidget(q1, 0, 0);
        gl->addWidget(q2, 0, 1);
        gl->addWidget(q3, 1, 0);
        gl->addWidget(q4, 1, 1);
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
                refreshContents();
                refreshDisplay();
        }
}

void AgentsPanelNext::agentClicked()
{
        QPushButton * qpb = qobject_cast<QPushButton *>(sender());
        QString astid = sender()->property("astid").toString();
        QString agentid = sender()->property("agentid").toString();
        QPoint where = qpb->pos(); // XXX
        QString idxa = astid + "-" + agentid;
        
        QGridLayout * gl = new QGridLayout();
        QDialog * dialog = new QDialog(this);
        dialog->setWindowTitle("Agent " + agentid + " on " + astid);
        dialog->setLayout(gl);
        
        QLabel * q_name = new QLabel(m_agent_props[idxa].toMap()["firstname"].toString());
        QLabel * q_agentid = new QLabel(agentid);
        QLabel * q_received = new QLabel("n calls received since connection");
        QLabel * q_lost = new QLabel("m calls lost since connection");
        QPushButton * q_cancel = new QPushButton("Cancel");
        QPushButton * q_logout = new QPushButton("Logout");
        QPushButton * q_transfer = new QPushButton("Transfer");
        QComboBox * q_queues = new QComboBox();
        QPushButton * q_close = new QPushButton("Close");
        
        int iy = 0;
        gl->addWidget(q_name, iy, 0, Qt::AlignCenter);
        gl->addWidget(q_agentid, iy, 1, Qt::AlignCenter);
        iy ++;
        gl->addWidget(q_received, iy, 0, 1, 2, Qt::AlignCenter);
        iy ++;
        gl->addWidget(q_lost, iy, 0, 1, 2, Qt::AlignCenter);
        iy ++;
        gl->addWidget(q_cancel, iy, 0, Qt::AlignCenter);
        gl->addWidget(q_logout, iy, 1, Qt::AlignCenter);
        iy ++;
        gl->addWidget(q_transfer, iy, 0, Qt::AlignCenter);
        gl->addWidget(q_queues, iy, 1, Qt::AlignCenter);
        iy ++;
        gl->addWidget(q_close, iy, 1, Qt::AlignCenter);
        iy ++;
        connect( q_close, SIGNAL(clicked()),
                 dialog, SLOT(close()) );
        dialog->move(where);
        dialog->exec();
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
                        QString sstatus = agqjoined[qname].toMap()["Status"].toString();
                        if((sstatus == "1") || (sstatus == "5"))
                                foreach (QString groupid, m_title.keys()) {
                                        QStringList lqueues = m_title[groupid]->property("queues").toStringList();
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
        saveGroups();
}

void AgentsPanelNext::refreshDisplay()
{
        int nmax = 1;
        QHash<QString, QMap<QString, QString> > columns_sorter;
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
                m_glayout->setColumnStretch(ix, 0);
                m_glayout->addWidget(m_title[groupid], 0, ix);
                QMap<QString, QString> lst = columns_sorter[groupid];
                foreach (QString srt, lst.uniqueKeys())
                        foreach (QString value, lst.values(srt))
                        m_glayout->addWidget(m_agent_labels[value], iy++, ix);
                if(iy > nmax)
                        nmax = iy;
        }
        m_glayout->setRowStretch(nmax, 1);
        m_glayout->setColumnStretch(m_title.size(), 1);
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
                                foreach (QString groupid, m_title.keys()) {
                                        QStringList lqueues = m_title[groupid]->property("queues").toStringList();
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
        
        refreshDisplay();
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

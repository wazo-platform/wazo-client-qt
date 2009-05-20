/* XiVO Client
 * Copyright (C) 2007-2009, Proformatique
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
#include <QVariant>
#include <QVBoxLayout>

#include "baseengine.h"
#include "agentspanel_next.h"
#include "extendedlabel.h"
#include "agentinfo.h"
#include "queueinfo.h"
#include "userinfo.h"

#define NCOLS 3

/*! \brief Constructor
 */
AgentsPanelNext::AgentsPanelNext(BaseEngine * engine,
                                 QWidget * parent)
    : QWidget(parent), m_engine(engine)
{
    m_glayout = new QGridLayout(this);
    m_glayout->setSpacing(1);
    
    // m_gridlayout->setVerticalSpacing(0);
    m_blinktime = 300;
    
    setGuiOptions(m_engine->getGuiOptions("server_gui"));
    startTimer(1000);
}

AgentsPanelNext::~AgentsPanelNext()
{
    // qDebug() << "AgentsPanelNext::~AgentsPanelNext()";
}

void AgentsPanelNext::setGuiOptions(const QVariant & optionmap)
{
    if(optionmap.toMap().contains("blinktime"))
        m_blinktime = optionmap.toMap()["blinktime"].toInt();
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
    
    QAction * p_newGroupAction = new QAction(tr("New Group"), &contextMenu);
    p_newGroupAction->setProperty("where", event->globalPos());
    connect(p_newGroupAction, SIGNAL(triggered()),
            this, SLOT(newGroup()) );
    
    if(sender() == NULL) {
        contextMenu.addAction(p_newGroupAction);
    } else {
        // group or agent label
        if(sender()->property("agentid").isNull()) {
            // group label
            ExtendedLabel * el = qobject_cast<ExtendedLabel *>(sender());
            QStringList thisqueuelist = el->property("queues").toStringList();
            QString thisgroupid = el->property("groupid").toString();
            
            QAction * p_renameAction = new QAction(tr("Rename this Group"), &contextMenu);
            
            contextMenu.addAction(p_renameAction);
            p_renameAction->setProperty("groupid", thisgroupid);
            p_renameAction->setProperty("where", event->globalPos());
            connect(p_renameAction, SIGNAL(triggered()),
                    this, SLOT(renameQueueGroup()) );
            
            QAction * p_removeAction = new QAction(tr("Remove this Group"), &contextMenu);
            contextMenu.addAction(p_removeAction);
            p_removeAction->setProperty("groupid", thisgroupid);
            connect(p_removeAction, SIGNAL(triggered()),
                    this, SLOT(removeQueueGroup()) );
            
            contextMenu.addAction(p_newGroupAction);
            contextMenu.addSeparator();
            
            if(thisqueuelist.size() > 0) {
                QMenu * menu_remove = contextMenu.addMenu(tr("Remove a Queue"));
                foreach (QString qname, thisqueuelist) {
                    QAction * p_qremove = new QAction(qname, this);
                    p_qremove->setProperty("groupid", thisgroupid);
                    p_qremove->setProperty("queuename", qname);
                    menu_remove->addAction(p_qremove);
                    connect(p_qremove, SIGNAL(triggered()),
                            this, SLOT(removeQueueFromGroup()) );
                }
                
                menu_remove->addSeparator();
                
                QAction * p_removeAllQueuesAction = new QAction(tr("Remove them all"), &contextMenu);
                menu_remove->addAction(p_removeAllQueuesAction);
                p_removeAllQueuesAction->setProperty("groupid", thisgroupid);
                connect(p_removeAllQueuesAction, SIGNAL(triggered()),
                        this, SLOT(removeQueuesFromGroup()) );
            }
            
            QStringList queuestoadd;
            foreach (QString qname, m_queuelist)
                if(! thisqueuelist.contains(qname))
                    queuestoadd << qname;
            if(queuestoadd.size() > 0) {
                QMenu * menu_add = contextMenu.addMenu(tr("Add a Queue"));
                foreach (QString qname, queuestoadd) {
                    QAction * p_qadd = new QAction(qname, this);
                    p_qadd->setProperty("groupid", thisgroupid);
                    p_qadd->setProperty("queuename", qname);
                    menu_add->addAction(p_qadd);
                    connect(p_qadd, SIGNAL(triggered()),
                            this, SLOT(addQueueToGroup()) );
                }
                
                menu_add->addSeparator();
                
                QAction * p_addAllQueuesAction = new QAction(tr("Add them all"), this);
                menu_add->addAction(p_addAllQueuesAction);
                p_addAllQueuesAction->setProperty("groupid", thisgroupid);
                connect(p_addAllQueuesAction, SIGNAL(triggered()),
                        this, SLOT(addQueuesToGroup()) );
            }
        } else
            return;
    }
    
    // QAction * chosen =
    contextMenu.exec(event->globalPos());
}

void AgentsPanelNext::newGroup()
{
    QPoint where = sender()->property("where").toPoint();
    QDialog * dialog = new QDialog(this);
    QGridLayout * gl = new QGridLayout(dialog);
    dialog->setWindowTitle(tr("New Group"));
    dialog->setLayout(gl);
    QLabel * q1 = new QLabel(tr("Name"), dialog);
    QTextEdit * q2 = new QTextEdit(dialog);
    QPushButton * q3 = new QPushButton(tr("OK"), dialog);
    QPushButton * q4 = new QPushButton(tr("Cancel"), dialog);
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
        if(! m_title.contains(groupid))
            m_title[groupid] = new ExtendedLabel(this);
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

void AgentsPanelNext::setQueueGroups(const QVariant & groups)
{
    // qDebug() << "AgentsPanelNext::setQueueGroups()" << groups;
    foreach (QString groupid, groups.toMap().keys()) {
        if(! m_title.contains(groupid)) {
            m_title[groupid] = new ExtendedLabel(groups.toMap()[groupid].toMap()["label"].toString(), this);
            m_title[groupid]->setProperty("groupid", groupid);
            m_title[groupid]->setAlignment(Qt::AlignCenter);
            connect( m_title[groupid], SIGNAL(context_menu(QContextMenuEvent *)),
                     this, SLOT(contextMenuEvent(QContextMenuEvent *)) );
        }
        m_title[groupid]->setProperty("queues", groups.toMap()[groupid].toMap()["queues"].toStringList());
        m_title[groupid]->setToolTip(groups.toMap()[groupid].toMap()["queues"].toStringList().join(", "));
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
    emit saveQueueGroups(save);
}

void AgentsPanelNext::setQueueOrder(const QVariant & queueorder)
{
    // qDebug() << "AgentsPanelNext::setQueueOrder()" << queueorder;
    m_queueorder = queueorder;
}

void AgentsPanelNext::setAgentProps(const QString & idx)
{
    QString agentid = m_agent_labels[idx]->property("agentid").toString();
    QString groupid = m_agent_labels[idx]->property("groupid").toString();
    AgentInfo * ainfo = m_engine->agents()[agentid];
    
    QVariantMap properties = ainfo->properties();
    QString agstatus = properties["agentstats"].toMap()["status"].toString();
    QString phonenum = properties["agentstats"].toMap()["agent_phone_number"].toString();
    QVariant queues = properties["queues_by_agent"];
    m_agent_labels[idx]->setProperty("sorter", ainfo->fullname());
    
    double statetime = properties["agentstats"].toMap()["Xivo-Agent-StateTime"].toDouble();
    QVariantMap slink = properties["agentstats"].toMap()["Xivo-Agent-Status-Link"].toMap();
    bool link = false;
    bool isdid = false;
    bool isoutcall = false;
    QString queuename;
    if(! slink.isEmpty()) {
        QString linkmode = slink["linkmode"].toString();
        if((linkmode == "phonelink") || (linkmode == "agentlink")) {
            link = true;
            isdid = slink["did"].toBool();
            isoutcall = slink["outcall"].toBool();
            queuename = slink["linkqueue"].toString();
        } else
            shouldNotOccur("AgentsPanelNext::setAgentProps",
                           QString("slink: agentid %1 linkmode %2").arg(agentid).arg(linkmode));
    }
    
    QString calldirection;
    QStringList groupqueues = m_title[groupid]->property("queues").toStringList();
    
    bool doshowtime = false;
    double dsec = 0;
    
    QString colorqss;
    if(agstatus == "AGENT_IDLE") {
        colorqss = "grey";
    } else if(agstatus == "AGENT_ONCALL") {
        colorqss = "grey";
        // define it the same way as AGENT_IDLE, since it can not been really trusted now
        // colorqss = "green";
        // calldirection = "I";
    } else if(agstatus == "AGENT_LOGGEDOFF") {
        shouldNotOccur("AgentsPanelNext::setAgentProps",
                       QString("agentid %1 agstatus %2").arg(agentid).arg(agstatus));
        colorqss = "red";
    } else {
        shouldNotOccur("AgentsPanelNext::setAgentProps",
                       QString("agentid %1 agstatus %2").arg(agentid).arg(agstatus));
        colorqss = "black";
    }
    
    if(link) {
        colorqss = "#80ff80";
        if(isdid && (! queuename.isEmpty())) {
            if(groupqueues.contains(queuename)) {
                doshowtime = true;
                calldirection = "E";
            }
        } else if(isoutcall) {
            doshowtime = true;
            calldirection = "S";
        } else {
            doshowtime = true;
            calldirection = "I";
        }
        
        if(doshowtime) {
            QDateTime now = QDateTime::currentDateTime();
            int d1 = m_engine->timeClient().secsTo(now);
            double d2 = m_engine->timeServer() - statetime;
            dsec = d1 + d2;
            // if(dsec) dsec = 0;
        }
    }
    
    QVariantMap qvm = queues.toMap();
    if(calldirection.isEmpty()) foreach (QString qname_group, groupqueues) {
        if (qvm.contains(qname_group)) {
            QString pstatus = qvm[qname_group].toMap()["Paused"].toString();
            // qDebug() << idx << idxa << qname_group
            // << qvm[qname_group].toMap()["Status"].toString()
            // << qvm[qname_group].toMap()["Paused"].toString()
            // << qvm[qname_group].toMap()["PausedTime"].toString();
            if(pstatus == "1") {
                colorqss = "#ff8080";
                
                QDateTime now = QDateTime::currentDateTime();
                int d1 = m_engine->timeClient().secsTo(now);
                double d2 = m_engine->timeServer() - qvm[qname_group].toMap()["Xivo-QueueMember-StateTime"].toDouble();
                doshowtime = true;
                dsec = d1 + d2;
                
                // rounding quite often leads to a "-1" value
                // in order not to hurt sensitivities, set it to zero
                // if(dsec) dsec = 0;
            }
        }
    }
    
    QString displayedtime;
    if(doshowtime) {
        int nsec = int(dsec + 0.5);
        int dhr  = nsec / 3600;
        int dmin = (nsec - dhr * 3600) / 60;
        int dsec = nsec % 60;
        if((nsec > m_blinktime) && (nsec % 2) && (colorqss == "#ff8080"))
            colorqss = "#ffb0b0";
        if(dhr > 0)
            displayedtime = tr("%1 hr %2 min %3 sec").arg(dhr).arg(dmin).arg(dsec);
        else if(dmin > 0)
            displayedtime = tr("%1 min %2 sec").arg(dmin).arg(dsec);
        else
            displayedtime = tr("%1 sec").arg(dsec);
    }
    
    QString disptext = QString("%1 %2 %3 %4").arg(ainfo->fullname()).arg(ainfo->agentnumber()).arg(calldirection).arg(displayedtime);
    
    m_agent_labels[idx]->setStyleSheet(QString("QLabel {border: 5px solid %1; border-radius: 0px; background: %1};").arg(colorqss));
    m_agent_labels[idx]->setText(disptext);
}

void AgentsPanelNext::renameQueueGroup()
{
    QString groupid = sender()->property("groupid").toString();
    QPoint where = sender()->property("where").toPoint();
    QGridLayout * gl = new QGridLayout();
    QDialog * dialog = new QDialog(this);
    dialog->setWindowTitle(tr("Rename a Group"));
    dialog->setLayout(gl);
    QLabel * q1 = new QLabel(tr("New Name"), this);
    QTextEdit * q2 = new QTextEdit(this);
    q2->setPlainText(m_title[groupid]->text());
    QPushButton * q3 = new QPushButton(tr("OK"), this);
    QPushButton * q4 = new QPushButton(tr("Cancel"), this);
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

void AgentsPanelNext::removeQueuesFromGroup()
{
    QString groupid = sender()->property("groupid").toString();
    m_title[groupid]->setProperty("queues", QStringList());
    m_title[groupid]->setToolTip("");
    refreshContents();
    refreshDisplay();
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

void AgentsPanelNext::addQueuesToGroup()
{
    QString groupid = sender()->property("groupid").toString();
    m_title[groupid]->setProperty("queues", m_queuelist);
    m_title[groupid]->setToolTip(m_queuelist.join(", "));
    refreshContents();
    refreshDisplay();
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
        QString agentid = sender()->property("agentid").toString();
        QString groupid = sender()->property("groupid").toString();
        AgentInfo * ainfo = m_engine->agents()[agentid];
        QPoint where = event->globalPos();
        
        QString astid = ainfo->astid();
        QString agentnumber = ainfo->agentnumber();
        changeWatchedAgent(QString("%1 %2").arg(astid).arg(agentnumber), true);
        
        QGridLayout * gl = new QGridLayout();
        QDialog * dialog = new QDialog(this);
        dialog->setWindowTitle(tr("Agent %1 on %2").arg(agentnumber, astid));
        dialog->setLayout(gl);
        
        QLabel * q_name = new QLabel(ainfo->fullname(), this);
        QLabel * q_agentid = new QLabel(agentnumber, this);
        
        QString nreceived = ainfo->properties()["agentstats"].toMap()["Xivo-ReceivedCalls"].toString();
        QString nlost = ainfo->properties()["agentstats"].toMap()["Xivo-LostCalls"].toString();
        QLabel * q_received = new QLabel(tr("%1 calls received since connection").arg(nreceived), this);
        QLabel * q_lost = new QLabel(tr("%1 calls lost since connection").arg(nlost), this);
        
        QPushButton * q_pause;
        bool isinpause = false;
        foreach (QString queueid, m_engine->queues().keys()) {
            QueueInfo * qinfo = m_engine->queues()[queueid];
            QString queuename = qinfo->queuename();
            if(m_title[groupid]->property("queues").toStringList().contains(queuename)) {
                QVariantMap qvm = ainfo->properties()["queues_by_agent"].toMap()[queuename].toMap();
                if(! qvm.isEmpty())
                    if(qvm["Paused"].toString() == "1")
                        isinpause = true;
            }
        }
        if(isinpause) {
            q_pause = new QPushButton(tr("Cancel Pause"), this);
            q_pause->setProperty("action", "unpause");
        } else {
            q_pause = new QPushButton(tr("Pause"), this);
            q_pause->setProperty("action", "pause");
        }
        
        q_pause->setProperty("agentid", agentid);
        q_pause->setProperty("groupid", groupid);
        connect( q_pause, SIGNAL(clicked()),
                 this, SLOT(actionclicked()) );
        connect( q_pause, SIGNAL(clicked()),
                 dialog, SLOT(close()) );
        
        QPushButton * q_logout = new QPushButton(tr("Logout"), this);
        q_logout->setProperty("action", "logout");
        q_logout->setProperty("agentid", agentid);
        connect( q_logout, SIGNAL(clicked()),
                 this, SLOT(actionclicked()) );
        connect( q_logout, SIGNAL(clicked()),
                 dialog, SLOT(close()) );
        
        QPushButton * q_transfer = new QPushButton(tr("Transfer"), this);
        q_transfer->setProperty("action", "transfer");
        q_transfer->setProperty("agentid", agentid);
        connect( q_transfer, SIGNAL(clicked()),
                 this, SLOT(actionclicked()) );
        
        QLabel * q_labelqueues = new QLabel(tr("Available Queues"), this);
        m_queue_chose = new QComboBox(this);
        m_queue_chose->setSizeAdjustPolicy(QComboBox::AdjustToContents);
        foreach(QString qname, m_queueorder.toStringList()) {
            if(m_queuelist.contains(qname))
                m_queue_chose->addItem(qname);
        }
        
        QPushButton * q_close = new QPushButton(tr("Close"), this);
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
        gl->addWidget(q_pause, iy, 0, Qt::AlignCenter);
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
    QString agentid = sender()->property("agentid").toString();
    QString groupid = sender()->property("groupid").toString();
    
    if(! m_engine->agents().keys().contains(agentid))
        return;
    AgentInfo * ainfo = m_engine->agents()[agentid];
    QString astid = ainfo->astid();
    QString agentnumber = ainfo->agentnumber();
    
    if(action == "transfer")
        agentAction(QString("transfer %1 %2 %3").arg(astid).arg(agentnumber).arg(m_queue_chose->currentText()));
    else if(action == "unpause") {
        // foreach (QString qname, m_title[groupid]->property("queues").toStringList())
        // agentAction(QString("unpause %1 %2").arg(astid).arg(agentnumber));
        agentAction(QString("unpause_all %1 %2").arg(astid).arg(agentnumber));
    }
    else if(action == "pause") {
        // foreach (QString qname, m_title[groupid]->property("queues").toStringList())
        // agentAction(QString("pause %1 %2").arg(astid).arg(agentnumber));
        agentAction(QString("pause_all %1 %2").arg(astid).arg(agentnumber));
    }
    else if(action == "logout")
        agentAction(QString("logout %1 %2").arg(astid).arg(agentnumber));
}

void AgentsPanelNext::refreshContents()
{
    // qDebug() << "AgentsPanelNext::refreshContents()";
    foreach (QString idx, m_agent_labels.keys()) {
        delete m_agent_labels[idx];
        m_agent_labels.remove(idx);
    }
    
    QHashIterator<QString, AgentInfo *> iter = QHashIterator<QString, AgentInfo *>(m_engine->agents());
    while( iter.hasNext() ) {
        iter.next();
        QString agentid = iter.key();
        AgentInfo * ainfo = iter.value();
        
        QString agentnumber = ainfo->agentnumber();
        QString agstatus = ainfo->properties()["agentstats"].toMap()["status"].toString();
        QVariantMap agqjoined = ainfo->properties()["queues_by_agent"].toMap();
        
        if(agstatus != "AGENT_LOGGEDOFF") foreach (QString qname, agqjoined.keys()) {
            if(! agqjoined[qname].toMap().isEmpty()) {
                QString sstatus = agqjoined[qname].toMap()["Status"].toString();
                // qDebug() << "AgentsPanelNext::refreshContents()" << qname << idxa << sstatus;
                if((sstatus == "1") || (sstatus == "3") || (sstatus == "4") || (sstatus == "5")) {
                    foreach (QString groupid, m_title.keys()) {
                        QStringList lqueues = m_title[groupid]->property("queues").toStringList();
                        if(lqueues.contains(qname)) {
                            QString idx = QString("%1-%2").arg(agentid).arg(groupid);
                            if(! m_agent_labels.contains(idx)) {
                                m_agent_labels[idx] = new ExtendedLabel(this);
                                m_agent_labels[idx]->setProperty("agentid", agentid);
                                m_agent_labels[idx]->setProperty("groupid", groupid);
                                connect( m_agent_labels[idx], SIGNAL(mouse_release(QMouseEvent *)),
                                         this, SLOT(agentClicked(QMouseEvent *)) );
                                connect( m_agent_labels[idx], SIGNAL(context_menu(QContextMenuEvent *)),
                                         this, SLOT(contextMenuEvent(QContextMenuEvent *)) );
                            }
                        }
                    }
                }
                // "3", "" (agent not in queue)
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

void AgentsPanelNext::newAgentList(const QStringList &)
{
    //qDebug() << "AgentsPanelNext::newAgentList" << list;
#if 0
    QHashIterator<QString, AgentInfo *> iter = QHashIterator<QString, AgentInfo *>(m_engine->agents());
    while( iter.hasNext() ) {
        iter.next();
        AgentInfo * ainfo = iter.value();
        QString agentid = iter.key();
        
        QString astid = ainfo->astid();
        QString agentnumber = ainfo->agentnumber();
        QString idxa = QString("%1-%2").arg(astid).arg(agentnumber);
    }
#endif
    emit loadQueueGroups();
}

void AgentsPanelNext::newQueueList(const QStringList &)
{
    //qDebug() << "AgentsPanelNext::newQueueList" << list;
    QHashIterator<QString, QueueInfo *> iter = QHashIterator<QString, QueueInfo *>(m_engine->queues());
    while( iter.hasNext() ) {
        iter.next();
        QString queueid = iter.key();
        // if(list.contains(queueid)) {
        QueueInfo * qinfo = iter.value();
        newQueue(qinfo->astid(), qinfo->queuename(), qinfo->properties());
        // }
    }
}

void AgentsPanelNext::newQueue(const QString & /*astid*/, const QString & queuename, const QVariant & queueprops)
{
    QString queuecontext = queueprops.toMap()["context"].toString();
    // qDebug() << "AgentsPanelNext::newQueue()" << astid << queuename << queuecontext;
    UserInfo * userinfo = m_engine->getXivoClientUser();
    
    if(userinfo == NULL)
        return;
    
    if((! m_queuelist.contains(queuename)) && (userinfo->context() == queuecontext))
        m_queuelist.append(queuename);
}

void AgentsPanelNext::timerEvent(QTimerEvent *)
{
    // qDebug() << "AgentsPanelNext::timerEvent()";
    foreach (QString idx, m_agent_labels.keys())
        setAgentProps(idx);
}

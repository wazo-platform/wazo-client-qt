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

#include <QCheckBox>
#include <QDebug>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QProgressBar>
#include <QScrollArea>
#include <QVariant>

#include "queuespanel.h"
#include "userinfo.h"

const QString commonqss = "QProgressBar {border: 2px solid black;border-radius: 3px;text-align: center;width: 100px; height: 15px}";

/*! \brief Constructor
 */
QueuesPanel::QueuesPanel(const QVariant & options,
                         QWidget * parent)
        : QWidget(parent)
{
        // qDebug() << "QueuesPanel::QueuesPanel()" << options;
        m_gui_buttonsize = 10;
        m_gui_showqueuenames = true;
        
        bool is_supervisor = false;
        if(options.toMap().contains("supervisor"))
                is_supervisor = options.toMap()["supervisor"].toBool();
        QStringList xletlist;
        foreach(QString xletdesc, options.toMap()["xlets"].toStringList())
                xletlist.append(xletdesc.split("-")[0]);
        m_gui_showmore = xletlist.contains("queuedetails") || xletlist.contains("queueentrydetails");
        
	m_gridlayout = new QGridLayout(this);
        m_statlegends["Completed"] = tr("Completed");
        m_statlegends["Abandoned"] = tr("Abandoned");
        m_statlegends["Holdtime"] = tr("Holdtime (s)");
        m_statlegends["ServicelevelPerf"] = tr("ServicelevelPerf");
        m_statlegends["ServiceLevel"] = tr("ServiceLevel");
        m_statlegends["Max"] = tr("Max");
        m_statlegends["Weight"] = tr("Weight");
        m_statlegends["Xivo-Conn"] = tr("Connected");
        m_statlegends["Xivo-Avail"] = tr("Available");
        m_statlegends["Xivo-Join"] = tr("Joined");
        m_statlegends["Xivo-Link"] = tr("Linked");
        m_statlegends["Xivo-Lost"] = tr("Lost");
        m_statlegends["Xivo-Rate"] = tr("Pickup\nrate (%)");
        m_statlegends["Xivo-Chat"] = tr("Conversation");
        //         m_statitems = (QStringList()
        //                        << "Completed" << "Abandoned"
        //                        << "Holdtime" //  << "ServicelevelPerf"
        //                        << "Xivo-Join" << "Xivo-Link"
        //                        << "ServiceLevel" << "Max" << "Weight");
        if(is_supervisor)
                m_statitems = (QStringList()
                               << "Xivo-Conn" << "Xivo-Avail"
                               << "Xivo-Rate"
                               << "Xivo-Join" << "Xivo-Link" << "Xivo-Lost" << "Xivo-Chat"
                               << "Holdtime");
        else
                m_statitems = (QStringList() << "Xivo-Conn" << "Xivo-Avail" << "Xivo-Rate");
        m_maxbusy = 0;
        
        m_qtitle = new QLabel(tr("Queues"), this);
        m_busytitle = new QLabel(tr("Busy"), this);
        
        foreach (QString statitem, m_statitems)
                m_title_infos[statitem] = new QLabel(m_statlegends[statitem], this);
        
        int colnum = 1;
        m_gridlayout->addWidget( m_qtitle, 1, colnum++, Qt::AlignLeft );
        colnum++;
        colnum++;
        
        m_gridlayout->addWidget( m_busytitle, 0, colnum++, Qt::AlignCenter );
        foreach (QString statitem, m_statitems)
                m_gridlayout->addWidget( m_title_infos[statitem],
                                         0,
                                         m_statitems.indexOf(statitem) + colnum,
                                         Qt::AlignCenter );
//  	m_gridlayout->setColumnStretch( 0, 1 );
  	m_gridlayout->setColumnStretch( 100, 1 );
 	m_gridlayout->setRowStretch( 100, 1 );
        m_gridlayout->setVerticalSpacing(0);
        
        setGuiOptions(options);
}

QueuesPanel::~QueuesPanel()
{
        // qDebug() << "QueuesPanel::~QueuesPanel()";
}

void QueuesPanel::setGuiOptions(const QVariant & options)
{
        m_options = options;
        
        if(m_options.toMap().contains("fontname") && m_options.toMap().contains("fontsize"))
                m_gui_font = QFont(m_options.toMap()["fontname"].toString(),
                                   m_options.toMap()["fontsize"].toInt());
        if(m_options.toMap().contains("iconsize"))
                m_gui_buttonsize = m_options.toMap()["iconsize"].toInt();
        if(m_options.toMap().contains("queues-showqueuenames"))
                m_gui_showqueuenames = m_options.toMap()["queues-showqueuenames"].toBool();
        
        m_busytitle->setFont(m_gui_font);
        m_qtitle->setFont(m_gui_font);
        foreach (QString statitem, m_statitems)
                m_title_infos[statitem]->setFont(m_gui_font);
        
        if(m_gui_showqueuenames)
                m_qtitle->show();
        else
                m_qtitle->hide();
}

void QueuesPanel::setUserInfo(const UserInfo * ui)
{
        m_userinfo = ui;
}

void QueuesPanel::updateCounter(const QVariant & counters)
{
        QVariantMap countersmap = counters.toMap();
        int ntot = countersmap["connected"].toInt();
        foreach (QString queuename, m_queueinfos.keys()) {
                int navail = 0;
                if(countersmap["byqueue"].toMap().contains(queuename))
                        navail = countersmap["byqueue"].toMap()[queuename].toInt();
                if(m_queueinfos[queuename].contains("Xivo-Conn"))
                        m_queueinfos[queuename]["Xivo-Conn"]->setText(QString::number(ntot));
                if(m_queueinfos[queuename].contains("Xivo-Avail"))
                        m_queueinfos[queuename]["Xivo-Avail"]->setText(QString::number(navail));
        }
}

void QueuesPanel::updatePeerAgent(int,
                                  const QString &,
                                  const QString &,
                                  const QVariant &)
{
}

void QueuesPanel::removeQueues(const QString &, const QStringList & queues)
{
        // qDebug() << "QueuesPanel::removeQueues()" << astid << queues;
        foreach (QString queuename, queues) {
                if(m_queuelabels.contains(queuename)) {
                        m_gridlayout->removeWidget( m_queuelabels[queuename] );
                        m_gridlayout->removeWidget( m_queuemore[queuename] );
                        m_gridlayout->removeWidget( m_queuebusies[queuename] );
                        delete m_queuelabels[queuename];
                        delete m_queuemore[queuename];
                        delete m_queuebusies[queuename];
                        m_queuelabels.remove(queuename);
                        m_queuemore.remove(queuename);
                        m_queuebusies.remove(queuename);
                        foreach (QString statitem, m_statitems) {
                                m_gridlayout->removeWidget( m_queueinfos[queuename][statitem] );
                                delete m_queueinfos[queuename][statitem];
                                m_queueinfos[queuename].remove(statitem);
                        }
                        m_queueinfos.remove(queuename);
                }
        }
}

void QueuesPanel::addQueue(const QString & astid, const QString & queuename, const QString & queuecontext, bool isvirtual)
{
        if(m_userinfo == NULL)
                return;
        if(m_userinfo->astid() != astid)
                return;
        if(! m_userinfo->contexts().contains(queuecontext))
                return;
        
        m_queuelabels[queuename] = new QLabel(queuename, this);
        m_queuelabels[queuename]->setFont(m_gui_font);
        m_queuelabels[queuename]->setProperty("virtual", isvirtual);
        m_queuemore[queuename] = new QPushButton(this);
        m_queuemore[queuename]->setProperty("astid", astid);
        m_queuemore[queuename]->setProperty("queueid", queuename);
        m_queuemore[queuename]->setProperty("queuecontext", queuecontext);
        m_queuemore[queuename]->setProperty("function", "more");
        m_queuemore[queuename]->setIconSize(QSize(m_gui_buttonsize, m_gui_buttonsize));
        m_queuemore[queuename]->setIcon(QIcon(":/images/add.png"));
        
        if(m_gui_showqueuenames) {
                m_queuelabels[queuename]->show();
                if(m_gui_showmore)
                        m_queuemore[queuename]->show();
                else
                        m_queuemore[queuename]->hide();
        } else {
                m_queuelabels[queuename]->hide();
                m_queuemore[queuename]->hide();
        }
        if(! isvirtual) {
                connect( m_queuemore[queuename], SIGNAL(clicked()),
                         this, SLOT(queueClicked()));
        }
        m_queuebusies[queuename] = new QProgressBar(this);
        m_queuebusies[queuename]->setFont(m_gui_font);
        m_queuebusies[queuename]->setProperty("queueid", queuename);
        m_queuebusies[queuename]->setStyleSheet(commonqss + "QProgressBar::chunk {background-color: #ffffff;}");
        m_queuebusies[queuename]->setFormat("%v");
        foreach (QString statitem, m_statitems) {
                m_queueinfos[queuename][statitem] = new QLabel();
                m_queueinfos[queuename][statitem]->setFont(m_gui_font);
        }
        
        m_queuemove[queuename] = new QPushButton(this);
        m_queuemove[queuename]->setProperty("astid", astid);
        m_queuemove[queuename]->setProperty("queueid", queuename);
        m_queuemove[queuename]->setProperty("function", "display_up");
        m_queuemove[queuename]->setProperty("position", m_queuelabels.size() - 1);
        m_queuemove[queuename]->setIconSize(QSize(m_gui_buttonsize, m_gui_buttonsize));
        m_queuemove[queuename]->setIcon(QIcon(":/images/red_up.png"));
        connect( m_queuemove[queuename], SIGNAL(clicked()),
                 this, SLOT(queueClicked()));
        m_queue_lines << queuename;
}

void QueuesPanel::affWidgets(bool isvirtual)
{
        int delta = 1;
        if(isvirtual)
                delta = 50;
        foreach(QString queuename, m_queuelabels.keys()) {
                int colnum = 1;
                int linenum = m_queuemove[queuename]->property("position").toInt() + 1;
                m_gridlayout->addWidget( m_queuelabels[queuename], delta + linenum, colnum++, Qt::AlignLeft );
                m_gridlayout->addWidget( m_queuemore[queuename], delta + linenum, colnum++, Qt::AlignCenter );
                m_gridlayout->addWidget( m_queuemove[queuename], delta + linenum, colnum++, Qt::AlignCenter );
                m_gridlayout->addWidget( m_queuebusies[queuename], delta + linenum, colnum++, Qt::AlignCenter );
                foreach (QString statitem, m_statitems)
                        m_gridlayout->addWidget( m_queueinfos[queuename][statitem],
                                                 delta + linenum,
                                                 m_statitems.indexOf(statitem) + colnum,
                                                 Qt::AlignCenter );
        }
}

void QueuesPanel::setQueueList(const QVariant & qlist)
{
        // qDebug() << "QueuesPanel::setQueueList()" << qlist;
        QVariantMap qlistmap = qlist.toMap();
        QString astid = qlistmap["astid"].toString();
        QVariantMap queuestats = qlistmap["queuestats"].toMap();
        foreach (QString queuename, queuestats.keys()) {
                QVariantMap queuestatcontents = queuestats[queuename].toMap();
                QHash <QString, QString> infos;
                infos["Calls"] = "0";
                foreach (QString statname, queuestatcontents.keys()) {
                        infos[statname] = queuestatcontents[statname].toString();
                }
                if((! m_queuelabels.contains(queuename)) && (qlistmap["queueprops"].toMap().keys().contains(queuename))) {
                        QString queuecontext = qlistmap["queueprops"].toMap()[queuename].toString();
                        addQueue(astid, queuename, queuecontext, false);
                }
                if(m_queuebusies.contains(queuename)) {
                        m_queuebusies[queuename]->setProperty("value", infos["Calls"]);
                        foreach (QString statitem, m_statitems)
                                if(infos.contains(statitem))
                                        m_queueinfos[queuename][statitem]->setText(infos[statitem]);
                }
        }
        
        affWidgets(false);
        loadQueueOrder();
        update();
}

void QueuesPanel::update()
{
        m_maxbusy = 0;
        foreach (QProgressBar * qpb, m_queuebusies) {
                quint32 val = qpb->property("value").toUInt();
                if(val > m_maxbusy)
                        m_maxbusy = val;
        }
        // qDebug() << "QueuesPanel::update() maxbusy =" << m_maxbusy;
        
        quint32 greenlevel = m_options.toMap()["queuelevels"].toMap()["green"].toUInt();
        quint32 orangelevel = m_options.toMap()["queuelevels"].toMap()["orange"].toUInt();
        foreach (QProgressBar * qpb, m_queuebusies) {
                QString qname = qpb->property("queueid").toString();
                quint32 val = qpb->property("value").toUInt();
                qpb->setRange(0, m_maxbusy + 1);
                // qpb->setValue(0); // trick in order to refresh
                qpb->setValue(val);
                
                if(val <= greenlevel)
                        qpb->setStyleSheet(commonqss + "QProgressBar::chunk {background-color: #00ff00;}");
                else if(val <= orangelevel)
                        qpb->setStyleSheet(commonqss + "QProgressBar::chunk {background-color: #f38402;}");
                else
                        qpb->setStyleSheet(commonqss + "QProgressBar::chunk {background-color: #ff0000;}");
        }
}

void QueuesPanel::setQueueOrder(const QVariant & queueorder)
{
        QStringList qlist;
        foreach (QString qname, queueorder.toStringList())
                if (m_queue_lines.contains(qname) && (! qlist.contains(qname)))
                        qlist << qname;
        foreach (QString qname, m_queue_lines)
                if (! qlist.contains(qname))
                        qlist << qname;
        m_queue_lines = qlist;
        int num = 0;
        foreach (QString qname, m_queue_lines)
                m_queuemove[qname]->setProperty("position", num ++);
        affWidgets(false);
}

void QueuesPanel::queueClicked()
{
        // qDebug() << "QueuesPanel::queueClicked()" << sender()->property("queueid");
        QString function = sender()->property("function").toString();
        QString astid = sender()->property("astid").toString();
        QString queueid = sender()->property("queueid").toString();
        if(function == "more")
                changeWatchedQueue(astid + " " + queueid);
        else if(function == "display_up") {
                int nold = m_queuemove[queueid]->property("position").toInt();
                if(nold > 0) {
                        int nnew = nold - 1;
                        m_queuemove[m_queue_lines[nold]]->setProperty("position", nnew);
                        m_queuemove[m_queue_lines[nnew]]->setProperty("position", nold);
                        m_queue_lines[nold] = m_queue_lines[nnew];
                        m_queue_lines[nnew] = queueid;
                        affWidgets(false);
                        saveQueueOrder(QVariant(m_queue_lines));
                }
        }
}

void QueuesPanel::setQueueStatus(const QVariant & newstatuses)
{
        // qDebug() << "QueuesPanel::setQueueStatus()" << newstatuses;
        QString astid = newstatuses.toMap()["astid"].toString();
        QString queuename = newstatuses.toMap()["queuename"].toString();
        QString busyness = newstatuses.toMap()["count"].toString();
        if(m_queuebusies.contains(queuename)) {
                m_queuebusies[queuename]->setProperty("value", busyness);
                update();
        }
}

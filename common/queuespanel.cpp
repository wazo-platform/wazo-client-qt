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

#include <QCheckBox>
#include <QDebug>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QProgressBar>
#include <QScrollArea>
#include <QVariant>

#include "baseengine.h"
#include "queuespanel.h"
#include "userinfo.h"
#include "queueinfo.h"

const QString commonqss = "QProgressBar {border: 2px solid black;border-radius: 3px;text-align: center;width: 100px; height: 15px}";

/*! \brief Constructor
 */
QueuesPanel::QueuesPanel(BaseEngine * engine,
                         const QVariant & options,
                         QWidget * parent)
    : QWidget(parent), m_engine(engine)
{
    // qDebug() << "QueuesPanel::QueuesPanel()" << options;
    m_gui_buttonsize = 10;
    m_gui_showqueuenames = true;
    
    QString statscols = "Xivo-Conn,Xivo-Avail,Xivo-Rate,Xivo-Join,Xivo-Link,Xivo-Lost,Xivo-Chat,Holdtime";
    // statscols = "Xivo-Conn,Xivo-Avail,Xivo-Rate";
    if(options.toMap().contains("queues-statscolumns"))
        statscols = options.toMap()["queues-statscolumns"].toString();
    bool shortlegends = false;
    if(options.toMap().contains("queues-shortlegends"))
        shortlegends = options.toMap()["queues-shortlegends"].toBool();
    
    QStringList xletlist;
    foreach(QString xletdesc, options.toMap()["xlets"].toStringList())
        xletlist.append(xletdesc.split("-")[0]);
    m_gui_showmore = xletlist.contains("queuedetails") || xletlist.contains("queueentrydetails");
    
    m_gridlayout = new QGridLayout(this);
    
    m_statlegends_short["Completed"] = tr("Cmptd");
    m_statlegends_short["Abandoned"] = tr("Abdnd");
    m_statlegends_short["Holdtime"] = tr("HT\n(s)");
    m_statlegends_short["ServicelevelPerf"] = tr("SLPerf\n(%)");
    m_statlegends_short["ServiceLevel"] = tr("SL");
    m_statlegends_short["Max"] = tr("Mx");
    m_statlegends_short["Weight"] = tr("Wght");
    m_statlegends_short["Xivo-Conn"] = tr("Conn.");
    m_statlegends_short["Xivo-Avail"] = tr("Avail.");
    m_statlegends_short["Xivo-Join"] = tr("Jnd");
    m_statlegends_short["Xivo-Link"] = tr("Lnkd");
    m_statlegends_short["Xivo-Lost"] = tr("Lst");
    m_statlegends_short["Xivo-Rate"] = tr("PR\n(%)");
    m_statlegends_short["Xivo-Chat"] = tr("Conv.\n(s)");
    
    m_statlegends_long["Completed"] = tr("Completed");
    m_statlegends_long["Abandoned"] = tr("Abandoned");
    m_statlegends_long["Holdtime"] = tr("Holdtime(s)");
    m_statlegends_long["ServicelevelPerf"] = tr("ServicelevelPerf(%)");
    m_statlegends_long["ServiceLevel"] = tr("ServiceLevel");
    m_statlegends_long["Max"] = tr("Max");
    m_statlegends_long["Weight"] = tr("Weight");
    m_statlegends_long["Xivo-Conn"] = tr("Connected");
    m_statlegends_long["Xivo-Avail"] = tr("Available");
    m_statlegends_long["Xivo-Join"] = tr("Joined");
    m_statlegends_long["Xivo-Link"] = tr("Linked");
    m_statlegends_long["Xivo-Lost"] = tr("Lost");
    m_statlegends_long["Xivo-Rate"] = tr("Pickup rate(%)");
    m_statlegends_long["Xivo-Chat"] = tr("Conversation(s)");
    
    foreach(QString statcol, statscols.split(","))
        if(m_statlegends_long.contains(statcol))
            m_statitems << statcol;
    m_maxbusy = 0;
    
    m_qtitle = new QLabel(tr("Queues"), this);
    m_busytitle = new QLabel(tr("Busy"), this);
    
    foreach (QString statitem, m_statitems) {
        if(shortlegends) {
            m_title_infos[statitem] = new QLabel(m_statlegends_short[statitem], this);
            m_title_infos[statitem]->setToolTip(m_statlegends_long[statitem]);
        } else
            m_title_infos[statitem] = new QLabel(m_statlegends_long[statitem], this);
    }
    
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
    //          m_gridlayout->setColumnStretch( 0, 1 );
    m_gridlayout->setColumnStretch( 100, 1 );
    m_gridlayout->setRowStretch( 100, 1 );
    m_gridlayout->setVerticalSpacing(0);
    
    setGuiOptions(options);
}

/*! \brief Destructor
 */
QueuesPanel::~QueuesPanel()
{
    // qDebug() << "QueuesPanel::~QueuesPanel()";
}

/*! \brief set user interface options
 *
 * set m_gui_font, m_gui_buttonsize, m_gui_showqueuenames
 * set fonts, etc...
 */
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

/*! \brief update counters
 *
 * update Xivo-Conn value
 */
void QueuesPanel::updateCounter(const QVariant & counters)
{
    QVariantMap countersmap = counters.toMap();
    int ntot = countersmap["connected"].toInt();
    foreach (QString queuename, m_queueinfos.keys()) {
        if(m_queueinfos[queuename].contains("Xivo-Conn"))
            m_queueinfos[queuename]["Xivo-Conn"]->setText(QString::number(ntot));
    }
}

/*! \brief remove the queues
 */
void QueuesPanel::removeQueues(const QString &, const QStringList & queues)
{
    // qDebug() << "QueuesPanel::removeQueues()" << astid << queues;
    foreach (QString queuename, queues) {
        if(m_queuelabels.contains(queuename)) {
            m_gridlayout->removeWidget( m_queuelabels[queuename] );
            m_gridlayout->removeWidget( m_queuemore[queuename] );
            m_gridlayout->removeWidget( m_queuebusies[queuename] );
            // TODO : used ->deleteLater() ?
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

/*! \brief Add a new queue
 */
void QueuesPanel::addQueue(const QString & astid, const QString & queuename, const QString & queuecontext)
{
    // qDebug() << "QueuesPanel::addQueue()" << astid << queuename << queuecontext;
    // UserInfo * userinfo = m_engine->getXivoClientUser();
    // if(userinfo == NULL) return;
    
    m_queuelabels[queuename] = new QLabel(queuename, this);
    m_queuelabels[queuename]->setFont(m_gui_font);
    m_queuelabels[queuename]->setToolTip(tr("Server: %1\nContext: %2").arg(astid).arg(queuecontext));
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
    connect( m_queuemore[queuename], SIGNAL(clicked()),
             this, SLOT(queueClicked()));
    m_queuebusies[queuename] = new QProgressBar(this);
    m_queuebusies[queuename]->setFont(m_gui_font);
    m_queuebusies[queuename]->setProperty("queueid", queuename);
    m_queuebusies[queuename]->setProperty("astid", astid);
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

/*! \brief add all necessaries widgets
 *
 * iterate through m_queuelabels and add all related
 * widgets to the layout.
 */
void QueuesPanel::affWidgets()
{
    //qDebug() << "QueuesPanel::affWidgets()";
    int delta = 1;
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

/*! \brief update display once the queues have been received
 */
void QueuesPanel::newQueueList(const QStringList & qsl)
{
    bool addedNewQueue = false;
    // qDebug() << "QueuesPanel::newQueueList()" << qsl;
    QHashIterator<QString, QueueInfo *> iter = QHashIterator<QString, QueueInfo *>(m_engine->queues());
    while( iter.hasNext() )
    {
        iter.next();
        if(qsl.contains(iter.key()))
        {
            QueueInfo * qinfo = iter.value();
            if(updateQueue(qinfo->astid(), qinfo->queuename(), qinfo->properties()))
                addedNewQueue = true;
        }
    }
    if(addedNewQueue)
    {
        //affWidgets();
        loadQueueOrder();
    }
    update(qsl);
}

/*! \brief update display once the agents have been received
 */
void QueuesPanel::newAgentList(const QStringList &)
{
    // qDebug() << "QueuesPanel::newAgentList()";
}

/*! \brief update list of queues
 *
 * update m_queueinfos
 */
bool QueuesPanel::updateQueue(const QString & astid, const QString & queuename, const QVariant & queueprops)
{
    bool newQueue = false;
    QVariantMap queuestatcontents = queueprops.toMap()["queuestats"].toMap();
    QString queuecontext = queueprops.toMap()["context"].toString();
    // qDebug() << "QueuesPanel::newQueue()" << astid << queuename << queuecontext;
    
    QHash <QString, QString> infos;
    infos["Calls"] = "0";
    foreach (QString statname, queuestatcontents.keys())
        infos[statname] = queuestatcontents[statname].toString();
    if(! m_queuelabels.contains(queuename)) {
        addQueue(astid, queuename, queuecontext);
        newQueue = true;
    }
    if(m_queuebusies.contains(queuename)) {
        m_queuebusies[queuename]->setProperty("value", infos["Calls"]);
        foreach (QString statitem, m_statitems)
            if(infos.contains(statitem))
                m_queueinfos[queuename][statitem]->setText(infos[statitem]);
    }
    
    QVariantMap queueagents = queueprops.toMap()["agents_in_queue"].toMap();
    QStringList queueagents_list;
    int navail = 0;
    foreach(QString agentname, queueagents.keys()) {
        QVariantMap qaprops = queueagents[agentname].toMap();
        if((qaprops["Status"].toString() == "1") && (qaprops["Paused"].toString() == "0")) {
            navail ++;
            queueagents_list << agentname;
        }
    }
    if(m_queueinfos[queuename].contains("Xivo-Avail")) {
        m_queueinfos[queuename]["Xivo-Avail"]->setText(QString::number(navail));
        if(navail)
            m_queueinfos[queuename]["Xivo-Avail"]->setToolTip(tr("Available agents : %1").arg(queueagents_list.join(", ")));
        else
            m_queueinfos[queuename]["Xivo-Avail"]->setToolTip("");
    }
    return newQueue;
}

/*! \brief update display of busy levels bars
 *
 * update m_maxbusy and color of QProgressBar 
 */
void QueuesPanel::update(const QStringList & list)
{
    // qDebug() << "QueuesPanel::update()";
    quint32 maxbusy = 0;
    bool maxbusychanged;
    foreach (QProgressBar * qpb, m_queuebusies) {
        quint32 val = qpb->property("value").toUInt();
        if(val > maxbusy)
            maxbusy = val;
    }
    if( (maxbusychanged = (maxbusy != m_maxbusy)) )
        m_maxbusy = maxbusy;
    
    quint32 greenlevel = m_options.toMap()["queuelevels"].toMap()["green"].toUInt();
    quint32 orangelevel = m_options.toMap()["queuelevels"].toMap()["orange"].toUInt();
    QHashIterator<QString, QProgressBar *> it(m_queuebusies);
    while(it.hasNext()) {
        it.next();
        QProgressBar * qpb = it.value();
        QString astid = qpb->property("astid").toString();
        QString queueid = QString("queue:%1/%2").arg(astid).arg(it.key());
        if(maxbusychanged || list.contains(queueid)) {
            quint32 val = qpb->property("value").toUInt();
            //qDebug() << "QueuesPanel::update()" << queueid;
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
}

/*! \brief set queue order
 *
 * Set new order un m_queue_lines and then call affWidget()
 * to update display.
 */
void QueuesPanel::setQueueOrder(const QVariant & queueorder)
{
    // qDebug() << "QueuesPanel::setQueueOrder()" << queueorder;
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
    saveQueueOrder(QVariant(m_queue_lines));
    affWidgets();
}

/*! \brief triggered when a queue is clicked
 *
 * Two possible actions : "more" or "display_up"
 */
void QueuesPanel::queueClicked()
{
    // qDebug() << "QueuesPanel::queueClicked()" << sender()->property("queueid");
    QString function = sender()->property("function").toString();
    QString astid = sender()->property("astid").toString();
    QString queueid = sender()->property("queueid").toString();
    if(function == "more")
        changeWatchedQueue("queue:" + astid + "/" + queueid);
    else if(function == "display_up") {
        int nold = m_queuemove[queueid]->property("position").toInt();
        if(nold > 0) {
            int nnew = nold - 1;
            m_queuemove[m_queue_lines[nold]]->setProperty("position", nnew);
            m_queuemove[m_queue_lines[nnew]]->setProperty("position", nold);
            m_queue_lines[nold] = m_queue_lines[nnew];
            m_queue_lines[nnew] = queueid;
            affWidgets();
            saveQueueOrder(QVariant(m_queue_lines));
        }
    }
}


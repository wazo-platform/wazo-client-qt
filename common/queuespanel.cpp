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

#include <QCheckBox>
#include <QDebug>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QProgressBar>
#include <QScrollArea>
#include <QVariant>
#include <QTimer>

#include "baseengine.h"
#include "queuespanel.h"
#include "userinfo.h"
#include "queueinfo.h"

const QString commonqss = "QProgressBar {border: 2px solid black;border-radius: 3px;text-align: center;width: 100px; height: 15px}";

/*! \brief Constructor
 */
QueuesPanel::QueuesPanel(BaseEngine * engine,
                         QWidget * parent)
    : XLet(engine, parent), m_show_display_queue_toggle(0)
{
    // qDebug() << "QueuesPanel::QueuesPanel()" << options;
    setTitle( tr("Queues' List") );
    m_gui_buttonsize = 10;
    m_gui_showqueuenames = true;
    
    // statscols = "Xivo-Conn,Xivo-Avail,Xivo-Rate";
    QVariantMap optionsMap = m_engine->getGuiOptions("merged_gui");
    QString statscols = optionsMap.value("queues-statscolumns",
                                         "Xivo-Conn,Xivo-Avail,Xivo-Rate,Xivo-Join,Xivo-Link,Xivo-Lost,Xivo-Chat,Holdtime").toString();
    bool shortlegends = optionsMap.value("queues-shortlegends",
                                         false).toBool();
    QStringList xletlist;
    foreach(QString xletdesc, m_engine->getCapaXlets())
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
    m_longestwaittitle = new QLabel(tr("Longest Wait"), this);
    m_displaytitle = new QLabel(tr("Hide queue"), this);
    m_displaytitle->hide();
    

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateLongestWaitWidgets()));
    timer->start(1000);

    
    foreach (QString statitem, m_statitems) {
        if(shortlegends) {
            m_title_infos[statitem] = new QLabel(m_statlegends_short[statitem], this);
            m_title_infos[statitem]->setToolTip(m_statlegends_long[statitem]);
        } else
            m_title_infos[statitem] = new QLabel(m_statlegends_long[statitem], this);
    }
    
    int colnum = 1;
    m_gridlayout->addWidget(m_displaytitle, 0, colnum++, Qt::AlignLeft);
    m_gridlayout->addWidget(m_qtitle, 0, colnum++, Qt::AlignLeft);
    colnum++;
    colnum++;
    
    m_gridlayout->addWidget(m_busytitle, 0, colnum++, Qt::AlignCenter);
    m_gridlayout->addWidget(m_longestwaittitle, 0, colnum++, Qt::AlignCenter);
    
    foreach (QString statitem, m_statitems)
        m_gridlayout->addWidget(m_title_infos[statitem],
                                0,
                                m_statitems.indexOf(statitem) + colnum,
                                Qt::AlignCenter );
    //          m_gridlayout->setColumnStretch( 0, 1 );
    m_gridlayout->setColumnStretch(100, 1);
    m_gridlayout->setRowStretch(100, 1);
    m_gridlayout->setVerticalSpacing(0);
    
    setGuiOptions(optionsMap);
    // connect signal/slots to engine
    connect(m_engine, SIGNAL(newAgentList(const QStringList &)),
            this, SLOT(newAgentList(const QStringList &)));
    connect(m_engine, SIGNAL(newQueueList(const QStringList &)),
            this, SLOT(newQueueList(const QStringList &)));
    
    connect(m_engine, SIGNAL(updateCounter(const QVariant &)),
            this, SLOT(updateCounter(const QVariant &)));
    connect(m_engine, SIGNAL(removeQueues(const QString &, const QStringList &)),
            this, SLOT(removeQueues(const QString &, const QStringList &)));
    connect(this, SIGNAL(changeWatchedQueue(const QString &)),
            m_engine, SLOT(changeWatchedQueueSlot(const QString &)));
    connect(this, SIGNAL(saveQueueOrder(const QVariant &)),
            m_engine, SLOT(saveQueueOrder(const QVariant &)));
    connect(this, SIGNAL(loadQueueOrder()),
            m_engine, SLOT(loadQueueOrder()));
    connect(m_engine, SIGNAL(setQueueOrder(const QVariant &)),
            this, SLOT(setQueueOrder(const QVariant &)));
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
void QueuesPanel::setGuiOptions(const QVariantMap & optionsMap)
{
    m_optionsMap = optionsMap;
    
    if(m_optionsMap.contains("fontname") && m_optionsMap.contains("fontsize"))
        m_gui_font = QFont(m_optionsMap["fontname"].toString(),
                           m_optionsMap["fontsize"].toInt());
    if(m_optionsMap.contains("iconsize"))
        m_gui_buttonsize = m_optionsMap["iconsize"].toInt();
    if(m_optionsMap.contains("queues-showqueuenames"))
        m_gui_showqueuenames = m_optionsMap["queues-showqueuenames"].toBool();
    
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
    foreach (QString queueid, m_queueinfos.keys()) {
        if(m_queueinfos[queueid].contains("Xivo-Conn"))
            m_queueinfos[queueid]["Xivo-Conn"]->setText(QString::number(ntot));
    }
}

/*! \brief remove the queues
 */
void QueuesPanel::removeQueues(const QString & astid, const QStringList & queues)
{
    // qDebug() << "QueuesPanel::removeQueues()" << astid << queues;
    foreach (QString queuename, queues) {
        QString queueid = QString("queue:%1/%2").arg(astid).arg(queuename);
        if(m_queuelabels.contains(queueid)) {
            m_gridlayout->removeWidget(m_queuedisplay[queueid]);
            m_gridlayout->removeWidget(m_queuelabels[queueid]);
            m_gridlayout->removeWidget(m_queuemore[queueid]);
            m_gridlayout->removeWidget(m_queuebusies[queueid]);
            m_gridlayout->removeWidget(m_queuelongestwait[queueid]);
            // TODO : used ->deleteLater() ?
            delete m_queuedisplay[queueid];
            delete m_queuelabels[queueid];
            delete m_queuemore[queueid];
            delete m_queuebusies[queueid];
            delete m_queuelongestwait[queueid];

            m_queuedisplay.remove(queueid);
            m_queuelabels.remove(queueid);
            m_queuemore.remove(queueid);
            m_queuebusies.remove(queueid);
            m_queuelongestwait.remove(queueid);
            foreach (QString statitem, m_statitems) {
                m_gridlayout->removeWidget( m_queueinfos[queueid][statitem]);
                delete m_queueinfos[queueid][statitem];
                m_queueinfos[queueid].remove(statitem);
            }
            m_queueinfos.remove(queueid);
        }
    }
}

/*! \brief Add a new queue
 */
void QueuesPanel::addQueue(const QString & astid, const QString & queueid, const QString & queuename, const QString & queuecontext)
{
    // qDebug() << "QueuesPanel::addQueue()" << astid << queuename << queuecontext;
    // UserInfo * userinfo = m_engine->getXivoClientUser();
    // if(userinfo == NULL) return;

    m_queuedisplay[queueid] = new QCheckBox(this);
    m_queuedisplay[queueid]->setProperty("queueid", queueid);
    m_queuedisplay[queueid]->setProperty("queuecontext", queuecontext);
    m_queuedisplay[queueid]->hide();
    m_queuedisplay[queueid]->setProperty("queuecontext", queuecontext);

    QVariantMap v = m_engine->getGuiOptions("client_gui");
    int hideQueue = v[queueid].toBool();
    m_queuedisplay[queueid]->setChecked(hideQueue);

    
    m_queuelabels[queueid] = new QLabel(queuename, this);
    m_queuelabels[queueid]->setFont(m_gui_font);
    m_queuelabels[queueid]->setToolTip(tr("Server: %1\nContext: %2").arg(astid).arg(queuecontext));

    m_queuemore[queueid] = new QPushButton(this);
    m_queuemore[queueid]->setProperty("queueid", queueid);
    m_queuemore[queueid]->setProperty("queuecontext", queuecontext);
    m_queuemore[queueid]->setProperty("function", "more");
    m_queuemore[queueid]->setIconSize(QSize(m_gui_buttonsize, m_gui_buttonsize));
    m_queuemore[queueid]->setIcon(QIcon(":/images/add.png"));
    
    if(m_gui_showqueuenames) {
        m_queuelabels[queueid]->show();
        if(m_gui_showmore)
            m_queuemore[queueid]->show();
        else
            m_queuemore[queueid]->hide();
    } else {
        m_queuelabels[queueid]->hide();
        m_queuemore[queueid]->hide();
    }
    connect( m_queuemore[queueid], SIGNAL(clicked()),
             this, SLOT(queueClicked()));
    m_queuebusies[queueid] = new QProgressBar(this);
    m_queuebusies[queueid]->setFont(m_gui_font);
    m_queuebusies[queueid]->setProperty("queueid", queueid);
    m_queuebusies[queueid]->setStyleSheet(commonqss + "QProgressBar::chunk {background-color: #ffffff;}");
    m_queuebusies[queueid]->setFormat("%v");


    m_queuelongestwait[queueid] = new QLabel(this);
    m_queuelongestwait[queueid]->setText(">)-|-/('>");
    m_queuelongestwait[queueid]->setStyleSheet("border-radius: 3px;background-color:red;border: 2px solid black;");
    m_queuelongestwait[queueid]->setAlignment(Qt::AlignCenter);
    m_queuelongestwait[queueid]->setFixedSize(100, 19);


    foreach (QString statitem, m_statitems) {
        m_queueinfos[queueid][statitem] = new QLabel();
        m_queueinfos[queueid][statitem]->setFont(m_gui_font);
    }
    
    m_queuemove[queueid] = new QPushButton(this);
    m_queuemove[queueid]->setProperty("queueid", queueid);
    m_queuemove[queueid]->setProperty("function", "display_up");
    m_queuemove[queueid]->setProperty("position", m_queuelabels.size() - 1);
    m_queuemove[queueid]->setIconSize(QSize(m_gui_buttonsize, m_gui_buttonsize));
    m_queuemove[queueid]->setIcon(QIcon(":/images/red_up.png"));
    connect( m_queuemove[queueid], SIGNAL(clicked()),
             this, SLOT(queueClicked()));
    m_queue_lines << queueid;
    if (hideQueue) {
        m_queuelabels[queueid]->hide();
        m_queuemore[queueid]->hide();
        m_queuelongestwait[queueid]->hide();
        m_queuemove[queueid]->hide();
        m_queuebusies[queueid]->hide();
        foreach (QString statitem, m_statitems)
            m_queueinfos[queueid][statitem]->hide();
    }
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
    foreach(QString queueid, m_queuelabels.keys()) {
        int colnum = 1;
        int linenum = m_queuemove[queueid]->property("position").toInt() + 1;
        m_gridlayout->addWidget(m_queuedisplay[queueid], delta + linenum, colnum++, Qt::AlignCenter);
        m_gridlayout->addWidget(m_queuelabels[queueid], delta + linenum, colnum++, Qt::AlignLeft);
        m_gridlayout->addWidget(m_queuemore[queueid], delta + linenum, colnum++, Qt::AlignCenter);
        m_gridlayout->addWidget(m_queuemove[queueid], delta + linenum, colnum++, Qt::AlignCenter);
        m_gridlayout->addWidget(m_queuebusies[queueid], delta + linenum, colnum++, Qt::AlignCenter);
        m_gridlayout->addWidget(m_queuelongestwait[queueid], delta + linenum, colnum++, Qt::AlignCenter);
        foreach (QString statitem, m_statitems)
            m_gridlayout->addWidget(m_queueinfos[queueid][statitem],
                                    delta + linenum,
                                    m_statitems.indexOf(statitem) + colnum,
                                    Qt::AlignCenter);
    }
}

/*! \brief update display once the queues have been received
 */
void QueuesPanel::newQueueList(const QStringList & qsl)
{
    bool addedNewQueue = false;
    // qDebug() << "QueuesPanel::newQueueList()" << qsl;
    QHashIterator<QString, QueueInfo *> iter = QHashIterator<QString, QueueInfo *>(m_engine->queues());
    while (iter.hasNext()) {
        iter.next();
        if(qsl.contains(iter.key())) {
            QueueInfo * qinfo = iter.value();
            if(updateQueue(qinfo->astid(), iter.key(), qinfo->queuename(), qinfo->properties()))
                addedNewQueue = true;
        }
    }
    if(addedNewQueue) {
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
bool QueuesPanel::updateQueue(const QString & astid, const QString & queueid,
                              const QString & queuename, const QVariant & queueprops)
{
    bool newQueue = false;
    QVariantMap queuestatcontents = queueprops.toMap()["queuestats"].toMap();
    QString queuecontext = queueprops.toMap()["context"].toString();
    // qDebug() << "QueuesPanel::newQueue()" << astid << queuename << queuecontext;
    
    QHash <QString, QString> infos;
    infos["Calls"] = "0";
    foreach (QString statname, queuestatcontents.keys())
        infos[statname] = queuestatcontents[statname].toString();
    if(! m_queuelabels.contains(queueid)) {
        addQueue(astid, queueid, queuename, queuecontext);
        newQueue = true;
    }
    if(m_queuebusies.contains(queueid)) {
        m_queuebusies[queueid]->setProperty("value", infos["Calls"]);
        foreach (QString statitem, m_statitems)
            if(infos.contains(statitem))
                m_queueinfos[queueid][statitem]->setText(infos[statitem]);
    }

    if (infos["Calls"].toInt() == 0 ) {
        if(m_queuelongestwait.contains(queueid)) {
            m_queuelongestwait[queueid]->setProperty("running_time", 0);
        m_queuelongestwait[queueid]->setProperty("time", 0);
        }
    } else if(m_queuelongestwait.contains(queueid)) {
        QueueInfo * qinfo = m_engine->queues()[QString("queue:%1/%2").arg(astid).arg(queuename)];
        QVariantMap properties = qinfo->properties();
        QVariantMap channel_list = properties["channels"].toMap();

        uint oldest = 0;
        int first_item = 1;
        uint current_entrytime;

        foreach (QString channel_name, channel_list.keys()) {
          current_entrytime = channel_list[channel_name].toMap()["entrytime"].toUInt();
          if (first_item) {
            oldest = current_entrytime;
            first_item = 0;
          }
          oldest = (oldest < current_entrytime) ? oldest : current_entrytime ;
        }

        uint oldest_waiting_time = (oldest == 0 ) ? oldest : (m_engine->timeServer() - oldest);

        m_queuelongestwait[queueid]->setProperty("time", oldest_waiting_time);
        m_queuelongestwait[queueid]->setProperty("running_time", !first_item);

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

    if(m_queueinfos[queueid].contains("Xivo-Avail")) {
        m_queueinfos[queueid]["Xivo-Avail"]->setText(QString::number(navail));
        if(navail)
            m_queueinfos[queueid]["Xivo-Avail"]->setToolTip(tr("Available agents : %1").arg(queueagents_list.join(", ")));
        else
            m_queueinfos[queueid]["Xivo-Avail"]->setToolTip("");
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
    
    quint32 greenlevel = m_optionsMap.value("queuelevels").toMap().value("green").toUInt();
    quint32 orangelevel = m_optionsMap.value("queuelevels").toMap().value("orange").toUInt();
    QHashIterator<QString, QProgressBar *> it(m_queuebusies);
    while(it.hasNext()) {
        it.next();
        QProgressBar * qpb = it.value();
        QString queueid = it.key();
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
    QString queueid = sender()->property("queueid").toString();
    if(function == "more")
        changeWatchedQueue(queueid);
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


/*! \brief updateLongestWaitWidgets
 *
 *  update the longest waiting time label for each queues
 */
void QueuesPanel::updateLongestWaitWidgets()
{
    uint greenlevel = m_optionsMap["queuelevels_wait"].toMap()["green"].toUInt() - 1;
    uint orangelevel = m_optionsMap["queuelevels_wait"].toMap()["orange"].toUInt() - 1;

    // if we don't want this widget displayed
    int display_column = m_optionsMap["queue_longestwait"].toBool();

    if (!display_column) { 
        if (m_longestwaittitle->isVisible()) {
            QHashIterator<QString, QLabel *> i(m_queuelongestwait);

            while (i.hasNext()) {
                i.next();
                QLabel *longestwait = i.value();
                longestwait->hide();
            }

            m_longestwaittitle->hide();
        }
        return ;
    } else {
        if (!m_longestwaittitle->isVisible()) {
            QHashIterator<QString, QLabel *> i(m_queuelongestwait);

            while (i.hasNext()) {
                i.next();
                QLabel *longestwait = i.value();
                longestwait->show();
            }

            m_longestwaittitle->show();
        }
    }

    QHashIterator<QString, QLabel *> i(m_queuelongestwait);

    while (i.hasNext()) {
        i.next();
        QLabel *longestwait = i.value();
        uint new_time = longestwait->property("time").toUInt();

        if(longestwait->property("running_time").toInt()) {
            new_time += 1;
            longestwait->setProperty("time", new_time);
        }

        int sec =   ( new_time % 60);
        int min =   ( new_time - sec ) / 60 % 60;
        int hou = ( ( new_time - sec - min * 60 ) / 60 ) / 60;

        QString time_label;
        
        if (hou) {
            time_label += QString("%0:%1:%2").arg(hou,2).arg(min,2,10,QChar('0')).arg(sec,2,10,QChar('0'));
        } else {
            time_label += QString("%0 Min ").arg(min,2);

            if(longestwait->property("running_time").toInt())
                time_label += QString("%0 Sec ").arg(sec,2,10,QChar('0'));
            else
                time_label += QString("%0 Sec ").arg(sec,2);
        }

        if(new_time == 0)
            longestwait->setStyleSheet("border-radius: 3px;background-color: #fff;border: 2px solid black;background-color: #ffffff;");
        else if(new_time <= greenlevel)
            longestwait->setStyleSheet("border-radius: 3px;background-color: #fff;border: 2px solid black;background-color: #00ff00;");
        else if(new_time <= orangelevel)
            longestwait->setStyleSheet("border-radius: 3px;background-color: #fff;border: 2px solid black;background-color: #f38402;");
        else
            longestwait->setStyleSheet("border-radius: 3px;background-color: #fff;border: 2px solid black;background-color: #ff0000;");

        longestwait->setText(time_label);
    }
}



/*! \brief mousePressEvent
 *
 *  if it is a right click, allow the user to hide / show queues
 */
void QueuesPanel::mousePressEvent(QMouseEvent *ev)
{
    int display_longuestwait = m_optionsMap["queue_longestwait"].toBool();
    QVariantMap v = m_engine->getGuiOptions("client_gui");

    if ((!m_show_display_queue_toggle)&&(ev->button()&Qt::RightButton)) {
        QHashIterator<QString, QCheckBox *> i(m_queuedisplay);

        m_displaytitle->show();

        while (i.hasNext()) {
            i.next();
            QCheckBox *queuedisplay = i.value();
            QString queueid = queuedisplay->property("queueid").toString();
            
            m_queuedisplay[queueid]->show();
            m_queuelabels[queueid]->show();
            m_queuemore[queueid]->show();
            m_queuemove[queueid]->show();
            m_queuebusies[queueid]->show();
            if (display_longuestwait)
                m_queuelongestwait[queueid]->show();
            foreach (QString statitem, m_statitems)
                m_queueinfos[queueid][statitem]->show();
        }

        m_show_display_queue_toggle = !m_show_display_queue_toggle;

    } else {
        QHashIterator<QString, QCheckBox *> i(m_queuedisplay);

        m_displaytitle->hide();


        while (i.hasNext()) {
            i.next();
            QCheckBox *queuedisplay = i.value();
            QString queueid = queuedisplay->property("queueid").toString();
            
            m_queuedisplay[queueid]->hide();
            v[queueid] = m_queuedisplay[queueid]->isChecked();
            if (m_queuedisplay[queueid]->isChecked()) {
                m_queuelabels[queueid]->hide();
                m_queuemore[queueid]->hide();
                m_queuelongestwait[queueid]->hide();
                m_queuemove[queueid]->hide();
                m_queuebusies[queueid]->hide();
                foreach (QString statitem, m_statitems)
                    m_queueinfos[queueid][statitem]->hide();
            }
        }

        m_show_display_queue_toggle = !m_show_display_queue_toggle;

        m_engine->setGuiOption("client_gui", v);
        m_engine->saveSettings();
    }
}

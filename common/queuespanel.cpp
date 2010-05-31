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


const static QString commonqss = "QProgressBar {border: 2px solid black;border-radius: 3px;text-align: center;width: 100px; height: 15px; margin-right:5px;}";

/*! \brief Constructor
 */
QueuesPanel::QueuesPanel(BaseEngine * engine,
                         QWidget * parent)
    : XLet(engine, parent), m_show_display_queue_toggle(0), m_configureWindow(NULL)
{
    // qDebug() << "QueuesPanel::QueuesPanel()" << options;
    setTitle(tr("Queues' List"));
    m_gui_buttonsize = 10;
    m_gui_showqueuenames = true;
    
    // statscols = "Xivo-Conn, Xivo-Avail, Xivo-Rate";
    QVariantMap optionsMap = m_engine->getGuiOptions("merged_gui");
    QString statscols = optionsMap.value("queues-statscolumns",
                                         "Holdtime,Xivo-Conn,Xivo-Avail,Xivo-Talking,"
                                         "Xivo-Join,Xivo-Link,Xivo-Lost,Xivo-Holdtime-max,"
                                         "Xivo-Rate,Xivo-QoS").toString();
    
    QStringList xletlist;
    foreach (QString xletdesc, m_engine->getCapaXlets())
        xletlist.append(xletdesc.split("-")[0]);
    m_gui_showmore = xletlist.contains("queuedetails") || xletlist.contains("queueentrydetails");
    
    m_gridlayout = new QGridLayout(this);
    
    not_realtime_stat << "Xivo-Holdtime-max" << "Xivo-Holdtime-avg" << "Xivo-QoS" << "Xivo-Join" << "Xivo-Lost" << "Xivo-TalkingTime" << "Xivo-Rate" << "Xivo-Link";
    
    // m_statlegends_short[] = ;
    
    // Asterisk-given indicators, kept for the record, just in case
    m_statlegends_long["Completed"] = tr("Completed");
    m_statlegends_long["Abandoned"] = tr("Abandoned");
    m_statlegends_long["ServicelevelPerf"] = tr("ServicelevelPerf(%)");
    m_statlegends_long["ServiceLevel"] = tr("ServiceLevel");
    m_statlegends_long["Max"] = tr("Max");
    m_statlegends_long["Weight"] = tr("Weight");
    m_statlegends_tooltip["Completed"] = tr("Completed");
    m_statlegends_tooltip["Abandoned"] = tr("Abandoned");
    m_statlegends_tooltip["ServicelevelPerf"] = tr("ServicelevelPerf(%)");
    m_statlegends_tooltip["ServiceLevel"] = tr("ServiceLevel");
    m_statlegends_tooltip["Max"] = tr("Max");
    m_statlegends_tooltip["Weight"] = tr("Weight");
    
    m_statlegends_long["Holdtime"] = tr("Estimated\nHoldtime");
    m_statlegends_long["Xivo-Holdtime-avg"] = tr("Average\nHoldtime");
    m_statlegends_long["Xivo-Holdtime-max"] = tr("Max\nHoldtime");
    m_statlegends_long["Xivo-Conn"] = tr("Connected");
    m_statlegends_long["Xivo-Avail"] = tr("Available");
    m_statlegends_long["Xivo-Join"] = tr("Joined");
    m_statlegends_long["Xivo-Link"] = tr("Linked");
    m_statlegends_long["Xivo-Lost"] = tr("Lost");
    m_statlegends_long["Xivo-Rate"] = tr("Efficiency\n(%)");
    m_statlegends_long["Xivo-TalkingTime"] = tr("Conversation\nTime");
    m_statlegends_long["Xivo-Talking"] = tr("Currently\nTalking");
    m_statlegends_long["Xivo-Qos"] = tr("Quality of Service (%)");
    
    m_statlegends_tooltip["Holdtime"] = tr("The average waiting time before getting an agent, calculated by asterisk");
    m_statlegends_tooltip["Xivo-Holdtime-avg"] = tr("The average waiting time before getting an agent");
    m_statlegends_tooltip["Xivo-Holdtime-max"] = tr("The maximum waiting time before getting an agent");
    m_statlegends_tooltip["Xivo-Conn"] = tr("Number of agents in this queue");
    m_statlegends_tooltip["Xivo-Avail"] = tr("Number of available agents in this queue");
    m_statlegends_tooltip["Xivo-Join"] = tr("Number of calls this queue has received");
    m_statlegends_tooltip["Xivo-Link"] = tr("Number of calls that were answered on this queue");
    m_statlegends_tooltip["Xivo-Lost"] = tr("Number of calls where the caller has left "
                                            "before getting an answer from an agent");
    m_statlegends_tooltip["Xivo-Rate"] = tr("Ratio (Linked) / (Joined) (%)");
    m_statlegends_tooltip["Xivo-TalkingTime"] = tr("The average length of a conversation");
    m_statlegends_tooltip["Xivo-Talking"] = tr("Number of agents in this queue, currently talking");
    m_statlegends_tooltip["Xivo-Qos"] = tr("Ratio (Number of calls answered in less than X sec) / "
                                      "(Number of calls answered) (%)");
    
    foreach (QString statcol, statscols.split(","))
        if (m_statlegends_long.contains(statcol))
            m_statitems << statcol;
    m_maxbusy = 0;
    
    m_qtitle = new QLabel(tr("Queues"), this);
    m_qtitle->setMinimumWidth(100);
    m_qtitle->setAlignment(Qt::AlignLeft);
    
    m_busytitle = new QLabel(tr("Busy"), this);
    m_busytitle->setAlignment(Qt::AlignCenter);
    m_longestwaittitle = new QLabel(tr("Longest Wait"), this);
    m_longestwaittitle->setAlignment(Qt::AlignCenter);
    //m_displaytitle = new QLabel(tr("Hide"), this);
    //m_displaytitle->setStyleSheet("QLabel { padding-right:10px }");
    //m_displaytitle->setAlignment(Qt::AlignLeft);
    //m_displaytitle->hide();
    
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateLongestWaitWidgets()));
    connect(timer, SIGNAL(timeout()), this, SLOT(updateQueueStats()));
    timer->start(1000);
    
    int colnum = 0, i = 0;
    int padding_width = 5;
    QString stattitle_qss = QString("QLabel { background-color: darkgrey;"
                                    "padding-right:%1px;"
                                    "padding-left:%2px;"
                                    "margin-bottom:2px; }").arg(padding_width).arg(padding_width);
    
    foreach (QString statitem, m_statitems) {
        m_title_infos[statitem] = new QLabel(m_statlegends_long[statitem], this);
        m_title_infos[statitem]->setAlignment(Qt::AlignCenter);
        m_title_infos[statitem]->setStyleSheet(stattitle_qss);
        m_title_infos[statitem]->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        m_title_infos[statitem]->setToolTip(m_statlegends_tooltip[statitem]);
        
        if ((not_realtime_stat.contains(statitem)) && (colnum==0))
            colnum = i;
        i++;
    }
    
    if ((colnum != i) && (colnum)) {
        QLabel *stat_title;
        
        stat_title = new QLabel(tr("Live state"), this);
        stat_title->setAlignment(Qt::AlignCenter);
        stat_title->setStyleSheet("QLabel { font-weight:bold;background-color:#444;color:white;padding:2px}");
        m_gridlayout->addWidget(stat_title, 0, 3, 1, colnum+4);

        stat_title = new QLabel(tr("Stats on slice"), this);
        stat_title->setAlignment(Qt::AlignCenter);
        stat_title->setStyleSheet("QLabel { font-weight:bold;background-color:#333;color:white;margin-left:1px;padding:2px}");
        m_gridlayout->addWidget(stat_title, 0, 5+colnum+2, 1, i-colnum);
    }
    
    colnum = 0;
    m_gridlayout->addWidget(m_qtitle, 1, colnum++);

    colnum += 2; // the + - widget
    
    m_gridlayout->addWidget(m_busytitle, 1, colnum++, Qt::AlignCenter);
    m_gridlayout->addWidget(m_longestwaittitle, 1, colnum++, Qt::AlignCenter);
    
    foreach (QString statitem, m_statitems)
        m_gridlayout->addWidget(m_title_infos[statitem], 1, colnum++);

    m_gridlayout->setColumnStretch(100, 1);
    m_gridlayout->setRowStretch(100, 1);
    m_gridlayout->setSpacing(0);
    
    setGuiOptions(optionsMap);

    connect(m_engine, SIGNAL(newQueueList(const QStringList &)),
            this, SLOT(newQueueList(const QStringList &)));
    
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
    connect(m_engine, SIGNAL(settingChanged(const QVariantMap &)),
            this, SLOT(setGuiOptions(const QVariantMap &)));

    m_engine->registerClassEvent("queuestats", QueuesPanel::eatQueuesStats_t, this);
}

void __format_duration(QString *field, int duration)
{
    int sec =   ( duration % 60 );
    int min =   ( duration - sec ) / 60 % 60;
    int hou = ( ( duration - sec - min * 60 ) / 60 ) / 60;

    if (hou)
        *field = QString(QString("%0:%1:%2").arg(hou, 2)
                                 .arg(min, 2, 10, QChar('0'))
                                 .arg(sec, 2, 10, QChar('0')));
    else
        *field = QString(QString("%0:%1").arg(min, 2, 10, QChar('0'))
                                 .arg(sec, 2, 10, QChar('0')));
}


void QueuesPanel::eatQueuesStats(const QVariantMap &p)
{
    QStringList duration_stats;
    duration_stats << "Xivo-TalkingTime" << "Xivo-Holdtime-max" << "Xivo-Holdtime-avg";
    foreach (QString queueid, p["stats"].toMap().keys()) {
        QVariantMap qvm = p["stats"].toMap()[queueid].toMap();
        foreach (QString stats, qvm.keys()) {
            QString field;
            if (duration_stats.contains(stats)) {
                if (qvm[stats].toString() != "na") {
                    int sec_total = qRound(qvm[stats].toDouble());
                    __format_duration(&field, sec_total);
                } else {
                    field = qvm[stats].toString();
                }
            } else {
                field = qvm[stats].toString();
            }
            if (m_queueinfos[m_queueid_map[queueid]].contains(stats))
                m_queueinfos[m_queueid_map[queueid]][stats]->setText(field);
        }
    }
}

/*! \brief set user interface options
 *
 * set m_gui_font, m_gui_buttonsize, m_gui_showqueuenames
 * set fonts, etc...
 */
void QueuesPanel::setGuiOptions(const QVariantMap & optionsMap)
{
    qDebug() << "plop";
    m_optionsMap = optionsMap;
    
    if (m_optionsMap.contains("fontname") && m_optionsMap.contains("fontsize")) {
        m_gui_font = QFont(m_optionsMap["fontname"].toString(),
                           m_optionsMap["fontsize"].toInt());
    }
    if (m_optionsMap.contains("iconsize")) {
        m_gui_buttonsize = m_optionsMap["iconsize"].toInt();
    }
    if (m_optionsMap.contains("queues-showqueuenames")) {
        m_gui_showqueuenames = m_optionsMap["queues-showqueuenames"].toBool();
    }
    
    m_busytitle->setFont(m_gui_font);
    m_qtitle->setFont(m_gui_font);
    foreach (QString statitem, m_statitems) {
        m_title_infos[statitem]->setFont(m_gui_font);
    }
    
    if (m_gui_showqueuenames) {
        m_qtitle->show();
    } else {
        m_qtitle->hide();
    }
}

QWidget* QueuesPanel::buildConfigureQueueList(QWidget *parent)
{
    QWidget *root = new QWidget(parent);
    QGridLayout *layout = new QGridLayout(root);
    root->setLayout(layout);


    layout->addWidget(new QLabel(tr("Display Queue"), root), 0, 0, Qt::AlignLeft);
    layout->addWidget(new QLabel(tr("Qos - X (s)"), root),   0, 1, Qt::AlignLeft);
    layout->addWidget(new QLabel(tr("Window (s)"), root),    0, 2, Qt::AlignLeft);

    QCheckBox *displayQueue;
    QSpinBox *spinBox;
    int row;
    int column;

    row = 1;

    QHashIterator<QString, QueueInfo *> i = QHashIterator<QString, QueueInfo *>(b_engine->queues());
    while (i.hasNext()) {
        column = 0;
        i.next();
        QueueInfo *qinfo = i.value();
        QString queueid = QString("queue:%1/%2").arg(qinfo->astid()).arg(qinfo->id());

        displayQueue = new QCheckBox(qinfo->queueName(), root);
        layout->addWidget(displayQueue, row, column++);

        spinBox = new QSpinBox();
        spinBox->setAlignment(Qt::AlignCenter);
        spinBox->setMaximum(240);
        spinBox->setProperty("queueid", queueid);
        spinBox->setProperty("param", "xqos");
        layout->addWidget(spinBox, row, column++);
        connect(spinBox, SIGNAL(valueChanged(int)), this, SLOT(changeQueueStatParam(int)));

        spinBox = new QSpinBox(root);
        spinBox->setAlignment(Qt::AlignCenter);
        spinBox->setMaximum(3600*24);
        spinBox->setProperty("queueid", queueid);
        spinBox->setProperty("param", "window");
        layout->addWidget(spinBox, row, column++);
        connect(spinBox, SIGNAL(valueChanged(int)), this, SLOT(changeQueueStatParam(int)));
    }

    return root;
}

void QueuesPanel::openConfigureWindow()
{
    if (m_configureWindow) {
        m_configureWindow->show();
        return ;
    }

    m_configureWindow = new QWidget(NULL);
    QVBoxLayout *layout = new QVBoxLayout(m_configureWindow);
    m_configureWindow->setLayout(layout);
    m_configureWindow->setWindowTitle(tr("Queues configuration"));
    QLabel *label = new QLabel(tr("Configure queues settings:"), m_configureWindow);
    layout->addWidget(label);
    layout->addWidget(buildConfigureQueueList(this));

    m_configureWindow->show();
}

void QueuesPanel::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu *menu = new QMenu(this);
    QAction *configure = new QAction(tr("Configure"), menu);
    
    menu->addAction(configure);
    if (menu->exec(event->globalPos()) == configure)
        openConfigureWindow();
}


/*! \brief remove the queues
 */
void QueuesPanel::removeQueues(const QString & astid, const QStringList & queues)
{
    // qDebug() << "QueuesPanel::removeQueues()" << astid << queues;
    foreach (QString queuename, queues) {
        QString queueid = QString("queue:%1/%2").arg(astid).arg(queuename);
        if (m_queuelabels.contains(queueid)) {
            m_gridlayout->removeWidget(m_queuelabels[queueid]);
            m_gridlayout->removeWidget(m_queuemore[queueid]);
            m_gridlayout->removeWidget(m_queuebusies[queueid]);
            m_gridlayout->removeWidget(m_queuelongestwait[queueid]);
            // TODO : used ->deleteLater() ?
            delete m_queuelabels[queueid];
            delete m_queuemore[queueid];
            delete m_queuebusies[queueid];
            delete m_queuelongestwait[queueid];

            m_queuelabels.remove(queueid);
            m_queuemore.remove(queueid);
            m_queuebusies.remove(queueid);
            m_queuelongestwait.remove(queueid);
            foreach (QString statitem, m_statitems) {
                m_gridlayout->removeWidget(m_queueinfos[queueid][statitem]);
                delete m_queueinfos[queueid][statitem];
                m_queueinfos[queueid].remove(statitem);
            }
            m_queueinfos.remove(queueid);
        }
    }
}

/*! \brief Add a new queue
 */
void QueuesPanel::addQueue(const QString & astid, const QString & queueid, const QString & queuename, const QString & queuecontext, const QString & number)
{
    QVariantMap v = m_engine->getGuiOptions("client_gui");
    int hideQueue = v[queueid].toBool();
    int displayQueueNum = m_optionsMap["queue_displaynu"].toBool();
    if (displayQueueNum) {
        m_queuelabels[queueid] = new QLabel(queuename + " (" + number + ") ", this);
    } else {
        m_queuelabels[queueid] = new QLabel(queuename, this);
    }

    m_queuelabels[queueid]->setFont(m_gui_font);
    m_queuelabels[queueid]->setProperty("queueid", queueid);
    m_queuelabels[queueid]->setToolTip(tr("Server: %1\nContext: %2").arg(astid).arg(queuecontext));

    m_queuemore[queueid] = new QPushButton(this);
    m_queuemore[queueid]->setProperty("queueid", queueid);
    m_queuemore[queueid]->setProperty("queuecontext", queuecontext);
    m_queuemore[queueid]->setProperty("function", "more");
    m_queuemore[queueid]->setIconSize(QSize(m_gui_buttonsize, m_gui_buttonsize));
    m_queuemore[queueid]->setIcon(QIcon(":/images/add.png"));
    m_queuemore[queueid]->setStyleSheet("QPushButton { height:20px;width:20px;margin-left:20px;margin-right:5px; }");
    
    if (m_gui_showqueuenames) {
        m_queuelabels[queueid]->show();
        if (m_gui_showmore) {
            m_queuemore[queueid]->show();
        } else {
            m_queuemore[queueid]->hide();
        }
    } else {
        m_queuelabels[queueid]->hide();
        m_queuemore[queueid]->hide();
    }
    connect(m_queuemore[queueid], SIGNAL(clicked()),
            this, SLOT(queueClicked()));
    m_queuebusies[queueid] = new QProgressBar(this);
    m_queuebusies[queueid]->setFont(m_gui_font);
    m_queuebusies[queueid]->setProperty("queueid", queueid);
    m_queuebusies[queueid]->setStyleSheet(commonqss + "QProgressBar::chunk { background-color: #ffffff; }");
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
    m_queuemove[queueid]->setStyleSheet("QPushButton { height:20px;width:20px;margin-right:10px; }");
    connect(m_queuemove[queueid], SIGNAL(clicked()),
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
    foreach (QString queueid, m_queuelabels.keys()) {
        int colnum = 0;
        int linenum = m_queuemove[queueid]->property("position").toInt() + 1;
        m_gridlayout->addWidget(m_queuelabels[queueid], delta + linenum, colnum++, Qt::AlignLeft);
        m_gridlayout->addWidget(m_queuemore[queueid], delta + linenum, colnum++, Qt::AlignCenter);
        m_gridlayout->addWidget(m_queuemove[queueid], delta + linenum, colnum++, Qt::AlignCenter);
        m_gridlayout->addWidget(m_queuebusies[queueid], delta + linenum, colnum++, Qt::AlignCenter);
        m_gridlayout->addWidget(m_queuelongestwait[queueid], delta + linenum, colnum++, Qt::AlignCenter);
        foreach (QString statitem, m_statitems)
            m_gridlayout->addWidget(m_queueinfos[queueid][statitem],
                                    delta + linenum,
                                    colnum++,
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
        if (qsl.contains(iter.key())) {
            QueueInfo * qinfo = iter.value();
            if (updateQueue(qinfo->astid(), iter.key(), qinfo->queuename(), qinfo->number(), qinfo->properties())) {
                addedNewQueue = true;
            }
        }
    }
    if (addedNewQueue) {
        loadQueueOrder();
    }
    update(qsl);
}

/*! \brief update list of queues
 *
 * update m_queueinfos
 */
bool QueuesPanel::updateQueue(const QString & astid, const QString & queueid,
                              const QString & queuename, const QString & number, const QVariant & queueprops)
{
    bool newQueue = false;
    QVariantMap queuestatcontents = queueprops.toMap()["queuestats"].toMap();
    QString queuecontext = queueprops.toMap()["context"].toString();
    // qDebug() << "QueuesPanel::newQueue()" << astid << queuename << queuecontext;
    
    
    QHash <QString, QString> infos;
    infos["Calls"] = "0";
    foreach (QString statname, queuestatcontents.keys())
        infos[statname] = queuestatcontents[statname].toString();
    if (! m_queuelabels.contains(queueid)) {
        addQueue(astid, queueid, queuename, queuecontext, number);
        newQueue = true;
    }
    if (m_queuebusies.contains(queueid)) {
        m_queuebusies[queueid]->setProperty("value", infos["Calls"]);
        foreach (QString statitem, m_statitems)
            if (infos.contains(statitem) && (!not_realtime_stat.contains(statitem))) {
                QString text;
                if (statitem == "Holdtime") {
                    if (infos[statitem]=="0") {
                        text = "na";
                    } else {
                        int sec_total = infos[statitem].toInt();
                        __format_duration(&text, sec_total);
                    }
                } else {
                    text = infos[statitem];
                }

                m_queueinfos[queueid][statitem]->setText(text);
            }
    }

    if (infos["Calls"].toInt() == 0) {
        if (m_queuelongestwait.contains(queueid)) {
            m_queuelongestwait[queueid]->setProperty("running_time", 0);
            m_queuelongestwait[queueid]->setProperty("time", 0);
        }
    } else if (m_queuelongestwait.contains(queueid)) {
        QueueInfo * qinfo = m_engine->queues()[queueid];
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
    foreach (QString agentname, queueagents.keys()) {
        QVariantMap qaprops = queueagents[agentname].toMap();
        if ((qaprops["Status"].toString() == "1") && (qaprops["Paused"].toString() == "0")) {
            navail ++;
            queueagents_list << agentname;
        }
    }

    if (m_queueinfos[queueid].contains("Xivo-Avail")) {
        m_queueinfos[queueid]["Xivo-Avail"]->setText(QString::number(navail));
        if (navail) {
            m_queueinfos[queueid]["Xivo-Avail"]->setToolTip(tr("Available agents : %1").arg(queueagents_list.join(", ")));
        } else {
            m_queueinfos[queueid]["Xivo-Avail"]->setToolTip("");
        }
    }

    navail = 0;
    foreach (QString agentname, queueagents.keys()) {
        QVariantMap qaprops = queueagents[agentname].toMap();
        if ((qaprops["Status"].toString() == "3") && (qaprops["Paused"].toString() == "0")) {
            navail ++;
        }
    }

    if (m_queueinfos[queueid].contains("Xivo-Talking"))
        m_queueinfos[queueid]["Xivo-Talking"]->setText(QString::number(navail));

    navail = 0;
    foreach (QString agentname, queueagents.keys()) {
        QVariantMap qaprops = queueagents[agentname].toMap();
        if ((qaprops["Status"].toString() == "3") || (qaprops["Status"].toString() == "1")) {
            navail ++;
        }
    }

    if (m_queueinfos[queueid].contains("Xivo-Conn"))
        m_queueinfos[queueid]["Xivo-Conn"]->setText(QString::number(navail));

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
        if (val > maxbusy) {
            maxbusy = val;
        }
    }
    if ((maxbusychanged = (maxbusy != m_maxbusy))) {
        m_maxbusy = maxbusy;
    }
    
    quint32 greenlevel = m_optionsMap.value("queuelevels").toMap().value("green").toUInt();
    quint32 orangelevel = m_optionsMap.value("queuelevels").toMap().value("orange").toUInt();
    QHashIterator<QString, QProgressBar *> it(m_queuebusies);
    while (it.hasNext()) {
        it.next();
        QProgressBar * qpb = it.value();
        QString queueid = it.key();
        if (maxbusychanged || list.contains(queueid)) {
            quint32 val = qpb->property("value").toUInt();
            //qDebug() << "QueuesPanel::update()" << queueid;
            qpb->setRange(0, m_maxbusy + 1);
            // qpb->setValue(0); // trick in order to refresh
            qpb->setValue(val);
            
            if (val <= greenlevel) {
                qpb->setStyleSheet(commonqss + "QProgressBar::chunk {background-color: #00ff00;}");
            } else if (val <= orangelevel) {
                qpb->setStyleSheet(commonqss + "QProgressBar::chunk {background-color: #f38402;}");
            } else {
                qpb->setStyleSheet(commonqss + "QProgressBar::chunk {background-color: #ff0000;}");
            }
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
    foreach (QString qname, queueorder.toStringList()) {
        if (m_queue_lines.contains(qname) && (! qlist.contains(qname))) {
            qlist << qname;
        }
    }
    foreach (QString qname, m_queue_lines) {
        if (! qlist.contains(qname)) {
            qlist << qname;
        }
    }
    m_queue_lines = qlist;
    int num = 0;
    foreach (QString qname, m_queue_lines) {
        m_queuemove[qname]->setProperty("position", num++);
    }
    saveQueueOrder(QVariant(m_queue_lines));
    affWidgets();
}

void QueuesPanel::changeQueueStatParam(int v)
{
    QString queueid = sender()->property("queueid").toString();
    QString param = sender()->property("param").toString();

    QVariantMap statConfig = b_engine->getGuiOptions("client_gui");
    QVariantMap qcfg = statConfig.value("queuespanel").toMap();
    qcfg[param + queueid] = v;
    statConfig["queuespanel"] = qcfg;

    b_engine->setGuiOption("client_gui", statConfig);
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
    if (function == "more") {
        changeWatchedQueue(queueid);
    } else if (function == "display_up") {
        int nold = m_queuemove[queueid]->property("position").toInt();
        if (nold > 0) {
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

        if (longestwait->property("running_time").toInt()) {
            new_time += 1;
            longestwait->setProperty("time", new_time);
        }


        QString time_label;
        __format_duration(&time_label, new_time);

        if (new_time == 0) {
            longestwait->setStyleSheet("border-radius: 3px;background-color: #fff;border: 2px solid black;background-color: #ffffff;width:105px;margin-right:5px;");
        } else if (new_time <= greenlevel) {
            longestwait->setStyleSheet("border-radius: 3px;background-color: #fff;border: 2px solid black;background-color: #00ff00;width:105px;margin-right:5px;");
        } else if (new_time <= orangelevel) {
            longestwait->setStyleSheet("border-radius: 3px;background-color: #fff;border: 2px solid black;background-color: #f38402;width:105px;margin-right:5px;");
        } else {
            longestwait->setStyleSheet("border-radius: 3px;background-color: #fff;border: 2px solid black;background-color: #ff0000;width:105px;margin-right:5px;");
        }

        longestwait->setText(time_label);
    }
}

/*! \brief updateQueueStats
 * Request queue stats periodicly
 */
void QueuesPanel::updateQueueStats()
{
    QHashIterator<QString, QLabel *> i(m_queuelabels);
    QVariantMap _for;
    
    QVariantMap statConfig = b_engine->getGuiOptions("client_gui").value("queuespanel").toMap();

    while (i.hasNext()) {
        i.next();
        QLabel *queuelabel = i.value();
        QString queueid = queuelabel->property("queueid").toString();
        
        QVariantMap _param;
        _param["window"] = statConfig.value("window" + queueid, 3600).toString();
        _param["xqos"] = statConfig.value("xqos" + queueid, 60).toString();
        qDebug() << queueid << _param << statConfig ;

        QString serverQueueId = QString(queueid).replace(QRegExp("queue:[^/]*/"), "");
        m_queueid_map[serverQueueId] = queueid;
        _for[serverQueueId] = _param;
    }
    QVariantMap command;
    command["class"] = "getqueuesstats";
    command["on"] = _for;

    m_engine->sendJsonCommand(command);
}


/*! \brief mousePressEvent
 *
 *  if it is a right click, allow the user to hide / show queues
 */
void QueuesPanel::mousePressEvent(QMouseEvent *ev)
{
    //int display_longuestwait = m_optionsMap["queue_longestwait"].toBool();
    //QVariantMap v = m_engine->getGuiOptions("client_gui");

    //if ((!m_show_display_queue_toggle) && (ev->button() & Qt::RightButton)) {
    //    QHashIterator<QString, QCheckBox *> i(m_queuedisplay);

    //    m_displaytitle->show();
    //    m_stats_xqostitle->show();
    //    m_stats_windowtitle->show();

    //    while (i.hasNext()) {
    //        i.next();
    //        QCheckBox *queuedisplay = i.value();
    //        QString queueid = queuedisplay->property("queueid").toString();
    //        
    //        m_queuedisplay[queueid]->show();
    //        m_queuelabels[queueid]->show();
    //        m_queuemore[queueid]->show();
    //        m_queuemove[queueid]->show();
    //        m_queuexqos[queueid]->show();
    //        if (display_longuestwait) {
    //            m_queuelongestwait[queueid]->show();
    //        }
    //        foreach (QString statitem, m_statitems) {
    //            m_queueinfos[queueid][statitem]->show();
    //        }
    //    }

    //    m_show_display_queue_toggle = !m_show_display_queue_toggle;

    //} else {
    //    QHashIterator<QString, QCheckBox *> i(m_queuedisplay);

    //    m_displaytitle->hide();
    //    m_stats_xqostitle->hide();
    //    m_stats_windowtitle->hide();


    //    while (i.hasNext()) {
    //        i.next();
    //        QCheckBox *queuedisplay = i.value();
    //        QString queueid = queuedisplay->property("queueid").toString();
    //        
    //        m_queuedisplay[queueid]->hide();
    //        //v[queueid] = m_queuedisplay[queueid]->isChecked();
    //        //v[queueid + "xqos"] = m_queuexqos[queueid]->value();
    //        //v[queueid + "window"] = m_queuewindow[queueid]->value();
    //        //m_queuewindow[queueid]->hide();
    //        //m_queuexqos[queueid]->hide();
    //        if (m_queuedisplay[queueid]->isChecked()) {
    //            m_queuelabels[queueid]->hide();
    //            m_queuemore[queueid]->hide();
    //            m_queuelongestwait[queueid]->hide();
    //            m_queuemove[queueid]->hide();
    //            m_queuebusies[queueid]->hide();
    //            foreach (QString statitem, m_statitems)
    //                m_queueinfos[queueid][statitem]->hide();
    //        }
    //    }

    //    m_show_display_queue_toggle = !m_show_display_queue_toggle;

    //    m_engine->setGuiOption("client_gui", v);
    //    m_engine->saveSettings();
    //}
}

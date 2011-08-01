/* XiVO Client
 * Copyright (C) 2007-2011, Proformatique
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


#include "queues.h"

Q_EXPORT_PLUGIN2(xletqueuesplugin, XLetQueuesPlugin);

XLet* XLetQueuesPlugin::newXLetInstance(QWidget *parent)
{
    b_engine->registerTranslation(":/queues_%1");
    return new XletQueues(parent);
}


QList<int> QueueRow::m_colWidth = QList<int>();
static QStringList statItems;  //!< list of stats items which are reported for each queue
static QStringList statsOfDurationType;

static QStringList statsToRequest;

const static QString commonqss = "QProgressBar { "
                                    "border: 2px solid black;"
                                    "border-radius: 3px;"
                                    "text-align: center;"
                                    "margin-left: 5px;"
                                 "}";

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

void XletQueues::settingChanged(const QVariantMap &)
{
    m_showNumber = b_engine->getGuiOptions("client_gui").value("queue_displaynu").toBool();

    QHashIterator<QString, QueueRow *> i(m_queueList);

    QVariantMap statConfig = b_engine->getGuiOptions("client_gui").value("queuespanel").toMap();

    while (i.hasNext()) {
        i.next();
        i.value()->updateName();
        QString queueid = i.value()->property("id").toString();

        if (statConfig.value("visible" + queueid, true).toBool()) {
            i.value()->show();
        } else {
            i.value()->hide();
        }
    }
}

void XletQueues::loadQueueOrder()
{
    QStringList order = b_engine->getGuiOptions("client_gui")
                            .value("queuespanel").toMap()
                            .value("queue_order").toStringList();
    setQueueOrder(order);
}

void XletQueues::saveQueueOrder(const QStringList &queueOrder)
{
    QVariantMap clientGui = b_engine->getGuiOptions("client_gui");
    QVariantMap queuesPanelConfig = clientGui.value("queuespanel").toMap();

    queuesPanelConfig["queue_order"] = queueOrder;
    clientGui["queuespanel"] = queuesPanelConfig;

    b_engine->setGuiOption("client_gui", clientGui);
}


XletQueues::XletQueues(QWidget *parent)
    : XLet(parent),
      m_configureWindow(NULL)
{
    setTitle(tr("Queues' List"));

    QStringList xletlist;
    foreach (QString xletdesc, b_engine->getCapaXlets())
        xletlist.append(xletdesc.split("-")[0]);
    m_showMore = xletlist.contains("queuedetails") || xletlist.contains("queueentrydetails");
    m_showNumber = b_engine->getGuiOptions("client_gui").value("queue_displaynu").toBool();
    uint nsecs = 30;
    if (b_engine->getGuiOptions("server_gui").contains("xlet.queues.statsfetchperiod"))
        nsecs = b_engine->getGuiOptions("server_gui").value("xlet.queues.statsfetchperiod").toInt();

    QVBoxLayout *xletLayout = new QVBoxLayout(this);
    xletLayout->setSpacing(0);

    QWidget *ListWidget = new QWidget(this);
    m_layout = new QVBoxLayout(ListWidget);
    m_layout->setSpacing(0);
    m_titleRow = QueueRow::makeTitleRow(this);
    m_layout->addWidget(m_titleRow);

    xletLayout->addWidget(ListWidget);
    xletLayout->insertStretch(-1, 1);

    setLayout(xletLayout);

    QTimer * timer_display = new QTimer(this);
    QTimer * timer_request = new QTimer(this);
    connect(timer_display, SIGNAL(timeout()), this, SLOT(updateLongestWaitWidgets()));
    connect(timer_request, SIGNAL(timeout()), this, SLOT(askForQueueStats()));
    timer_display->start(1000);
    timer_request->start(nsecs * 1000);

    connect(b_engine, SIGNAL(newQueueList(const QStringList &)),
            this, SLOT(newQueueList(const QStringList &)));
    connect(b_engine, SIGNAL(removeQueues(const QString &, const QStringList &)),
            this, SLOT(removeQueues(const QString &, const QStringList &)));
    connect(b_engine, SIGNAL(settingChanged(const QVariantMap &)),
            this, SLOT(settingChanged(const QVariantMap &)));
    connect(this, SIGNAL(changeWatchedQueue(const QString &)),
            b_engine, SLOT(changeWatchedQueueSlot(const QString &)));

    b_engine->registerClassEvent("queuestats", XletQueues::eatQueuesStats_t, this);
    updateLongestWaitWidgets();
    QTimer::singleShot(0, this, SLOT(display()));
}

void XletQueues::display()
{
    show();
    QueueRow::getLayoutColumnsWidth(static_cast<QGridLayout*>(m_titleRow->layout()));
}

void XletQueues::eatQueuesStats(const QVariantMap &p)
{
    foreach (QString queueid, p.value("stats").toMap().keys()) {
        QVariantMap qvm = p.value("stats").toMap().value(queueid).toMap();
        foreach (QString stats, qvm.keys()) {
            QString field;
            if (statsOfDurationType.contains(stats)) {
                if (qvm.value(stats).toString() != "na") {
                    int sec_total = qRound(qvm.value(stats).toDouble());
                    __format_duration(&field, sec_total);
                } else {
                    field = qvm.value(stats).toString();
                }
            } else {
                field = qvm.value(stats).toString();
            }
            m_queueList[queueid]->updateSliceStat(stats, field);
        }
    }
}

void XletQueues::openConfigureWindow()
{
    if (m_configureWindow) {
        m_configureWindow->show();
        return ;
    }

    m_configureWindow = new XletQueuesConfigure(this);
}

void XletQueues::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu *menu = new QMenu(this);
    QAction *configure = new QAction(tr("Configure"), menu);

    menu->addAction(configure);
    if (menu->exec(event->globalPos()) == configure)
        openConfigureWindow();
}

void XletQueues::removeQueues(const QString &, const QStringList &queues)
{
    foreach (QString queueId, queues) {
        if (m_queueList.contains(queueId)) {
            QueueRow *to_remove = m_queueList[queueId];
            m_queueList.remove(queueId);
            delete to_remove;
        }
    }
}

/*! \brief update display once the queues have been received
 */
void XletQueues::newQueueList(const QStringList &qsl)
{
    // qDebug() << "newQueueList" << qsl;
    QHashIterator<QString, QueueInfo *> iter = \
        QHashIterator<QString, QueueInfo *>(b_engine->queues());

    while (iter.hasNext()) {
        iter.next();
        if (qsl.contains(iter.key())) {
            QueueInfo * qinfo = iter.value();
            QString queueId = qinfo->id();

            // qDebug() << Q_FUNC_INFO << this->parentWidget() << m_queueList.size() << queueId << qinfo->queueName();
            if (! m_queueList.contains(queueId)) {
                m_queueList[queueId] = new QueueRow(qinfo, this);
                m_layout->addWidget(m_queueList[queueId]);
                updateLongestWaitWidgets();
            } else {
                m_queueList.value(queueId)->updateRow();
            }
        }
    }
    loadQueueOrder();
}

/*! \brief set queue order
 *
 * to update display.
 */
void XletQueues::setQueueOrder(const QStringList & queueOrder)
{
    QueueRow *rowAtPos;
    QueueRow *rowAtWrongPos;
    int rowMovedIndex;
    int index = 1;
    foreach(QString queue, queueOrder) {
        QLayoutItem * qli = m_layout->itemAt(index);
        if(qli != NULL) {
            rowAtPos = qobject_cast<QueueRow *>(qli->widget());
            if (rowAtPos != NULL) {
                if (rowAtPos->property("id").toString() != queue ) {
                    QHashIterator<QString, QueueRow *> i(m_queueList);
                    rowAtWrongPos = NULL;
                    while (i.hasNext()) {
                        i.next();
                        if (i.value()->property("id").toString() == queue) {
                            rowAtWrongPos = i.value();
                            break;
                        }
                    }
                    if (rowAtWrongPos != NULL) {
                        rowMovedIndex = m_layout->indexOf(rowAtWrongPos);
                        m_layout->removeWidget(rowAtPos);
                        m_layout->removeWidget(rowAtWrongPos);

                        m_layout->insertWidget(rowMovedIndex, rowAtPos);
                        m_layout->insertWidget(index, rowAtWrongPos);
                    }
                }
                index++;
            }
        } else {
            qDebug() << "WARNING qli is NULL for queue" << queue << "and index" << index;
        }
    }
}

/*! \brief triggered when a queue is clicked
 *
 * Two possible actions : "more" or "display_up"
 */
void XletQueues::queueClicked()
{
    QueueRow *row = qobject_cast<QueueRow *>(qobject_cast<QPushButton *>(sender())->parentWidget());
    QueueRow *prevRow;
    QString function = sender()->property("function").toString();
    QString queueid = sender()->property("queueid").toString();

    if (function == "more") {
        emit changeWatchedQueue(queueid);
    } else if (function == "display_up") {
        int index = m_layout->indexOf(row);
        if (index > 1) {
            prevRow = qobject_cast<QueueRow *>(m_layout->itemAt(index-1)->widget());
            m_layout->removeWidget(prevRow);
            m_layout->removeWidget(row);
            m_layout->insertWidget(index - 1, row);
            m_layout->insertWidget(index, prevRow);
        }
    }

    QStringList queueOrder;
    int nbItem = m_layout->count();

    int i;
    for(i=1;i<nbItem;i++) {
        row = qobject_cast<QueueRow *>(m_layout->itemAt(i)->widget());
        queueOrder.append(row->property("id").toString());
    }

    saveQueueOrder(queueOrder);
}


/*! \brief updateLongestWaitWidgets
 *
 *  update the longest waiting time label for each queues
 */
void XletQueues::updateLongestWaitWidgets()
{
    QVariantMap optionMap = b_engine->getGuiOptions("client_gui");
    uint greenlevel = optionMap.value("queuelevels_wait").toMap().value("green").toUInt() - 1;
    uint orangelevel = optionMap.value("queuelevels_wait").toMap().value("orange").toUInt() - 1;

    // if we don't want this widget displayed
    int display_column = optionMap.value("queue_longestwait").toBool();

    QGridLayout *titleLayout = static_cast<QGridLayout*>(m_layout->itemAt(0)->widget()->layout());
    QWidget *longestWaitTitle = titleLayout->itemAtPosition(1, 4)->widget();

    if (!display_column) {
        longestWaitTitle->hide();
        titleLayout->setColumnMinimumWidth(4, 0);
    } else {
        titleLayout->setColumnMinimumWidth(4, 100);
        longestWaitTitle->show();
    }

    QHashIterator<QString, QueueRow *> i(m_queueList);

    while (i.hasNext()) {
        i.next();
        i.value()->updateLongestWaitWidget(display_column, greenlevel, orangelevel);
    }
}

/*! \brief askForQueueStats
 * Request queue stats periodicly
 */
void XletQueues::askForQueueStats()
{
    QHashIterator<QString, QueueRow *> i(m_queueList);
    QVariantMap _for;

    QVariantMap statConfig = b_engine->getGuiOptions("client_gui").value("queuespanel").toMap();

    while (i.hasNext()) {
        i.next();
        QueueRow *row = i.value();
        QString queueid = row->property("id").toString();

        QVariantMap _param;
        _param["window"] = statConfig.value("window" + queueid, 3600).toString();
        _param["xqos"] = statConfig.value("xqos" + queueid, 60).toString();

        _for[queueid] = _param;
    }
    QVariantMap command;
    command["class"] = "getqueuesstats";
    command["on"] = _for;

    b_engine->sendJsonCommand(command);
}




XletQueuesConfigure::XletQueuesConfigure(XletQueues *)
    : QWidget(NULL)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    setLayout(layout);
    setWindowTitle(tr("Queues configuration"));

    QLabel *desc = new QLabel(tr("Choose which queue should be displayed, and the\n"
                                 "queues parameters for the Stats on slice:"), this);

    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setFrameStyle(QFrame::NoFrame);
    scrollArea->setWidget(buildConfigureQueueList(scrollArea));

    layout->addWidget(desc);
    layout->addWidget(scrollArea);

    QPushButton *leave = new QPushButton(tr("&Close"), this);
    connect(leave, SIGNAL(clicked()), this, SLOT(close()));
    layout->addWidget(leave);
    show();
    setMaximumSize(QSize(width(), 600));
}

QWidget* XletQueuesConfigure::buildConfigureQueueList(QWidget *parent)
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
    QVariantMap statConfig = b_engine->getGuiOptions("client_gui")
                                .value("queuespanel").toMap();
    QString queueid;

    row = 1;

    QHashIterator<QString, QueueInfo *> i = \
        QHashIterator<QString, QueueInfo *>(b_engine->queues());

    while (i.hasNext()) {
        column = 0;
        i.next();
        QueueInfo *qinfo = i.value();
        queueid = qinfo->id();

        displayQueue = new QCheckBox(qinfo->queueName(), root);
        displayQueue->setProperty("queueid", queueid);
        displayQueue->setProperty("param", "visible");
        displayQueue->setChecked(statConfig.value("visible" + queueid, true).toBool());
        layout->addWidget(displayQueue, row, column++);
        connect(displayQueue, SIGNAL(stateChanged(int)),
                this, SLOT(changeQueueStatParam(int)));

        spinBox = new QSpinBox(root);
        spinBox->setAlignment(Qt::AlignCenter);
        spinBox->setMaximum(240);
        spinBox->setProperty("queueid", queueid);
        spinBox->setProperty("param", "xqos");
        spinBox->setValue(statConfig.value("xqos" + queueid, 60).toInt());
        layout->addWidget(spinBox, row, column++);
        connect(spinBox, SIGNAL(valueChanged(int)),
                this, SLOT(changeQueueStatParam(int)));

        spinBox = new QSpinBox(root);
        spinBox->setAlignment(Qt::AlignCenter);
        spinBox->setMaximum(3600*24);
        spinBox->setProperty("queueid", queueid);
        spinBox->setProperty("param", "window");
        spinBox->setValue(statConfig.value("window" + queueid, 3600).toInt());
        layout->addWidget(spinBox, row, column++);
        connect(spinBox, SIGNAL(valueChanged(int)),
                this, SLOT(changeQueueStatParam(int)));

        row++;
    }

    return root;
}

void XletQueuesConfigure::changeQueueStatParam(int v)
{
    QString queueid = sender()->property("queueid").toString();
    QString param = sender()->property("param").toString();

    QVariantMap statConfig = b_engine->getGuiOptions("client_gui");
    QVariantMap qcfg = statConfig.value("queuespanel").toMap();
    qcfg[param + queueid] = v;
    statConfig["queuespanel"] = qcfg;

    b_engine->setGuiOption("client_gui", statConfig);
}

void XletQueuesConfigure::closeEvent(QCloseEvent *)
{
    hide();
}




QueueRow::QueueRow(const QueueInfo *qInfo, XletQueues *parent)
    : QWidget(parent), qinfo(qInfo), xlet(parent)
{
    setProperty("id", qinfo->id());
    m_layout = new QGridLayout(this);
    m_layout->setSpacing(0);
    QString queueId = qinfo->id();

    int visible = b_engine->getGuiOptions("client_gui")
                  .value("queuespanel").toMap()
                  .value("visible"+queueId, true).toBool();

    if (!visible)
        hide();

    int col = 0;
    m_name = new QLabel(this);
    m_layout->addWidget(m_name, 0, col++);

    m_more = new QPushButton(this);
    m_more->setProperty("queueid", QString("queue:%1/%2")
                                    .arg(qinfo->astid())
                                    .arg(qinfo->id()));
    m_more->setProperty("function", "more");
    m_more->setIcon(QIcon(":/images/add.png"));
    m_more->setFixedSize(20, 20);
    m_more->setFlat(true);
    m_layout->addWidget(m_more, 0, col++);
    connect(m_more, SIGNAL(clicked()), xlet, SLOT(queueClicked()));

    if (!xlet->showMoreQueueDetailButton()) {
        m_more->hide();
    }

    m_move = new QPushButton(this);
    m_move->setProperty("queueid", queueId);
    m_move->setProperty("function", "display_up");
    m_move->setProperty("position", 0);
    m_move->setIcon(QIcon(":/images/red_up.png"));
    m_move->setFixedSize(20, 20);
    m_move->setFlat(true);
    m_layout->addWidget(m_move, 0, col++);
    connect(m_move, SIGNAL(clicked()), xlet, SLOT(queueClicked()));


    m_busy = new QProgressBar(this);
    m_busy->setProperty("queueid", queueId);
    m_busy->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    m_busy->setStyleSheet(commonqss + "QProgressBar::chunk { background-color: #fff; }");
    m_busy->setFormat("%v");
    m_busy->setFixedHeight(20);
    m_layout->addWidget(m_busy, 0, col++);

    m_longestWait = new QLabel(this);
    m_longestWait->setAlignment(Qt::AlignCenter);
    m_longestWait->setStyleSheet("margin-left: 5px;"
                                 "border-radius: 3px;"
                                 "background-color:red;"
                                 "border: 2px solid black;");
    m_longestWait->setText(">)-|-/('>");
    m_longestWait->setFixedHeight(20);
    m_layout->addWidget(m_longestWait, 0, col++);

    foreach (QString stat, statItems) {
        m_infoList[stat] = new QLabel(tr("na"), this);
        m_infoList[stat]->setAlignment(Qt::AlignCenter);
        m_layout->addWidget(m_infoList[stat], 0, col++, 1, 1, Qt::AlignCenter);
    }

    setLayoutColumnWidth(m_layout, statItems.count());

    QSpacerItem *spacer = new QSpacerItem(1, 1);
    m_layout->addItem(spacer, 1, col, 1,-1);
    m_layout->setColumnStretch(col, 1);
    m_layout->setMargin(0);

    // setMaximumHeight(30);
    updateRow();
}

void QueueRow::setLayoutColumnWidth(QGridLayout *layout, int nbStat)
{
    layout->setColumnMinimumWidth(0, 150); // queue name
    layout->setColumnMinimumWidth(1, 25);  // queue more
    layout->setColumnMinimumWidth(2, 25);  // queue move
    layout->setColumnMinimumWidth(3, 100); // queue busy
    layout->setColumnMinimumWidth(4, 100); // queue longest waiting time

    int i;
    for (i = 0 ; i < nbStat ; i++) {
        if (m_colWidth[i] != -1) {
            layout->setColumnMinimumWidth(i+5, m_colWidth[i]);
        } else {
            layout->setColumnMinimumWidth(i+5, 55);
        }
    }
}

void QueueRow::getLayoutColumnsWidth(QGridLayout *layout)
{
    int i;
    for(i=0;i<m_colWidth.size();i++) {
        m_colWidth[i] = layout->itemAtPosition(1, i+5)->widget()->width();
    }
    // warning : it seems that m_colWidth is set only here,
    // so that if the xlet is hidden these values are wrong
    // - 3 when in a tab
    // - 100 when starting in systray
    // qDebug() << Q_FUNC_INFO << m_colWidth;
}

void QueueRow::updateSliceStat(const QString &stat, const QString &value)
{
    if (m_infoList.contains(stat)) {
        m_infoList[stat]->setText(value);
    } else {
        qDebug() << "shouldnt happen queue.." << stat << value;
    }
}

uint QueueRow::m_maxbusy = 0;

void QueueRow::updateBusyWidget()
{
    QVariantMap optionMap = b_engine->getGuiOptions("client_gui");
    uint greenlevel = optionMap.value("queuelevels").toMap().value("green").toUInt() - 1;
    uint orangelevel = optionMap.value("queuelevels").toMap().value("orange").toUInt() - 1;
    uint val = m_busy->property("value").toUInt();

    if (m_maxbusy < val) {
        m_maxbusy = val;
    }

    m_busy->setRange(0, m_maxbusy + 1);
    m_busy->setValue(val);

    if (val <= greenlevel) {
        m_busy->setStyleSheet(commonqss + "QProgressBar::chunk {background-color: #0f0;}");
    } else if (val <= orangelevel) {
        m_busy->setStyleSheet(commonqss + "QProgressBar::chunk {background-color: #f38402;}");
    } else {
        m_busy->setStyleSheet(commonqss + "QProgressBar::chunk {background-color: #f00;}");
    }
}

void QueueRow::updateLongestWaitWidget(int display, uint greenlevel, uint orangelevel)
{
    if (display) {
        m_layout->setColumnMinimumWidth(4, 100); // queue longest waiting time
        m_longestWait->show();
    } else {
        m_layout->setColumnMinimumWidth(4, 0);
        m_longestWait->hide();
    }

    uint new_time = m_longestWait->property("time").toUInt();

    if (m_longestWait->property("running_time").toInt()) {
        new_time += 1;
        m_longestWait->setProperty("time", new_time);
    }

    QString time_label;
    __format_duration(&time_label, new_time);
    QString base_css = "margin-left:5px;border-radius: 3px;border: 2px solid black;";

    if (new_time == 0) {
        m_longestWait->setStyleSheet(base_css + "background-color: #fff;");
    } else if (new_time <= greenlevel) {
        m_longestWait->setStyleSheet(base_css + "background-color: #0f0;");
    } else if (new_time <= orangelevel) {
        m_longestWait->setStyleSheet(base_css + "background-color: #f38402;");
    } else {
        m_longestWait->setStyleSheet(base_css + "background-color: #f00;");
    }

    m_longestWait->setText(time_label);

    if (!display) {
        m_longestWait->hide();
    }
}

void QueueRow::updateRow()
{
    QVariantMap queueStats = qinfo->properties().value("queuestats").toMap();
    QString queueName = qinfo->queueName();

    QHash <QString, QString> infos;
    infos["Calls"] = "0";

    foreach (QString stat, queueStats.keys())
        infos[stat] = queueStats[stat].toString();

    m_busy->setProperty("value", infos["Calls"]);

    foreach (QString stat, statItems) {
        if (infos.contains(stat) && (!statsToRequest.contains(stat))) {
            QString text;
            if (stat == "Holdtime") {
                if (infos[stat] == "0") {
                    text = tr("na");
                } else {
                    int sec_total = infos[stat].toInt();
                    __format_duration(&text, sec_total);
                }
            } else {
                text = infos[stat];
            }

            if (m_infoList.contains(stat)) {
                m_infoList[stat]->setText(infos[stat]);
            } else {
                qDebug() << "ERR received stats for: " << stat;
            }
        }
    }

    /* stat cols who aren't made by server */
    QVariantMap queueagents = qinfo->properties().value("agents_in_queue").toMap();
    QStringList queueagents_list;
    int nagents;

    QRegExp agentfilter = QRegExp("Agent/[0-9]*");

    // number of Available agents
    nagents = 0;
    queueagents_list.clear();
    foreach (QString queuemember, queueagents.keys()) {
        if (agentfilter.exactMatch(queuemember)) {
            QVariantMap qaprops = queueagents.value(queuemember).toMap();
            if ((qaprops.value("Status").toString() == "1") &&
                (qaprops.value("Paused").toString() == "0")) {
                nagents++;
                queueagents_list << queuemember.mid(6);
            }
        }
    }
    if (m_infoList.contains("Xivo-Avail")) {
        m_infoList["Xivo-Avail"]->setText(QString::number(nagents));
        QString todisp;
        if (nagents)
            todisp = tr("Available agents : %1").arg(queueagents_list.join(", "));
        m_infoList["Xivo-Avail"]->setToolTip(todisp);
    }


    // number of Connected agents
    nagents = 0;
    queueagents_list.clear();
    foreach (QString queuemember, queueagents.keys()) {
        if (agentfilter.exactMatch(queuemember)) {
            QVariantMap qaprops = queueagents.value(queuemember).toMap();
            if ((qaprops.value("Status").toString() == "3") ||
                (qaprops.value("Status").toString() == "1")) {
                nagents++;
                queueagents_list << queuemember.mid(6);
            }
        }
    }
    if (m_infoList.contains("Xivo-Conn")) {
        m_infoList["Xivo-Conn"]->setText(QString::number(nagents));
        QString todisp;
        if (nagents)
            todisp = tr("Connected agents : %1").arg(queueagents_list.join(", "));
        m_infoList["Xivo-Conn"]->setToolTip(todisp);
    }


    // number of Talking agents
    nagents = 0;
    queueagents_list.clear();
    foreach (QString queuemember, queueagents.keys()) {
        if (agentfilter.exactMatch(queuemember)) {
            QVariantMap qaprops = queueagents.value(queuemember).toMap();
            if ((qaprops.value("Status").toString() == "3") &&
                (qaprops.value("Paused").toString() == "0")) {
                nagents++;
                queueagents_list << queuemember.mid(6);
            }
        }
    }
    if (m_infoList.contains("Xivo-Talking")) {
        m_infoList["Xivo-Talking"]->setText(QString::number(nagents));
        QString todisp;
        if (nagents)
            todisp = tr("Talking agents : %1").arg(queueagents_list.join(", "));
        m_infoList["Xivo-Talking"]->setToolTip(todisp);
    }

    QVariantMap properties = qinfo->properties();
    QVariantMap channel_list = properties.value("channels").toMap();

    uint oldest = 0;
    int first_item = 1;
    uint current_entrytime;

    foreach (QString channel_name, channel_list.keys()) {
        current_entrytime = channel_list.value(channel_name).toMap().value("entrytime").toUInt();
        if (first_item) {
            oldest = current_entrytime;
            first_item = 0;
        }
        oldest = (oldest < current_entrytime) ? oldest : current_entrytime ;
    }

    uint oldest_waiting_time = (oldest == 0 ) ? oldest : (b_engine->timeServer() - oldest);

    m_longestWait->setProperty("time", oldest_waiting_time);
    m_longestWait->setProperty("running_time", !first_item);

    updateName();
    updateBusyWidget();
}

void QueueRow::updateName()
{
    QString queueName;
    if (xlet->showNumber()) {
        queueName = qinfo->queueName() + " (" + qinfo->queueNumber() + ")";
    } else {
        queueName = qinfo->queueName();
    }

    m_name->setText(queueName);
    setLayoutColumnWidth(m_layout, statItems.count());
}

QWidget* QueueRow::makeTitleRow(XletQueues *parent)
{
    QWidget *row = new QWidget(parent);
    QGridLayout *layout = new QGridLayout(row);
    layout->setSpacing(0);
    row->setLayout(layout);
    QLabel *label;
    QSpacerItem *spacer;
    m_colWidth.clear();

    static struct {
        QString hashname;
        QString name;
        QString tooltip;
    } stats_detail[] =  {
        {   "Holdtime",
            tr("Estimated\nHoldtime"),
            tr("Average waiting time before getting an agent,"
               "calculated by asterisk") },
        {   "Xivo-Conn",
            tr("Connected"),
            tr("Number of agents in this queue") },
        {   "Xivo-Avail",
            tr("Available"),
            tr("Number of available agents in this queue") },
        {   "Xivo-Talking",
            tr("Currently\nTalking"),
            tr("Number of agents in this queue, currently talking") },
        {   "Xivo-Holdtime-avg",
            tr("Average\nHoldtime"),
            tr("Average waiting time before getting an agent") },
        {   "Xivo-Holdtime-max",
            tr("Max\nHoldtime"),
            tr("Maximum waiting time before getting an agent") },
     /* {   "Completed",
            tr("Completed"),
            tr("Completed") },
        {   "Abandoned",
            tr("Abandoned"),
            tr("Abandoned") },
        {   "ServicelevelPerf",
            tr("ServicelevelPerf\n(%)"),
            tr("ServicelevelPerf(%)") },
        {   "Servicelevel",
            tr("Servicelevel"),
            tr("Servicelevel") },
        {   "Max",
            tr("Max"),
            tr("Max") },
        {   "Weight",
            tr("Weight"),
            tr("Weight") }, */
        {   "Xivo-Join",
            tr("Joined"),
            tr("Number of calls this queue has received") },
        {   "Xivo-Link",
            tr("Linked"),
            tr("Number of calls that were answered on this queue") },
        {   "Xivo-Lost",
            tr("Lost"),
            tr("Number of calls where the caller has left "
               "before getting an answer from an agent") },
        {   "Xivo-Rate",
            tr("Efficiency\n(%)"),
            tr("Ratio (Linked) / (Joined) (%)") },
        {   "Xivo-TalkingTime",
            tr("Conversation\nTime"),
            tr("Average length of a conversation") },
        {   "Xivo-Qos",
            tr("Quality of\nService\n(%)"),
            tr("Ratio (Number of calls answered in less than X sec) / "
               "(Number of calls answered) (%)") }
    };

    int i;
    if (statItems.empty()) {
        for (i=0;i<nelem(stats_detail);i++) {
            statItems << stats_detail[i].hashname;
        }

        statsOfDurationType << "Xivo-TalkingTime" << "Xivo-Holdtime-max"
                            << "Xivo-Holdtime-avg";

        statsToRequest << "Xivo-Holdtime-max" << "Xivo-Holdtime-avg" << "Xivo-QoS"
                       << "Xivo-Join" << "Xivo-Lost" << "Xivo-TalkingTime"
                       << "Xivo-Rate" << "Xivo-Link";
    }

    int col = 0;
    label = new QLabel(row);
    label->setText(tr("Queues"));
    layout->addWidget(label, 1, col++);

    label = new QLabel(row);
    label->setText(tr("Number of agents"));
    label->setAlignment(Qt::AlignCenter);
    label->setStyleSheet("background-color:#333;color:#eee;margin-right:1px;");
    layout->addWidget(label, 0, 6, 1, 3);

    label = new QLabel(row);
    label->setText(tr("Stats on Slice"));
    label->setAlignment(Qt::AlignCenter);
    label->setStyleSheet("QLabel { background-color:#333;color:#eee; } ");
    layout->addWidget(label, 0, 9, 1, nelem(stats_detail)-4);

    spacer = new QSpacerItem(25, 1, QSizePolicy::Fixed, QSizePolicy::Fixed);
    layout->addItem(spacer, 1, col++);
    spacer = new QSpacerItem(25, 1, QSizePolicy::Fixed, QSizePolicy::Fixed);
    layout->addItem(spacer, 1, col++);

    label = new QLabel(row);
    label->setText(tr("Busy"));
    label->setFixedSize(100, 30);
    label->setAlignment(Qt::AlignCenter);
    layout->addWidget(label, 1, col++);

    label = new QLabel(row);
    label->setFixedSize(100, 30);
    label->setText(tr("Longest Wait"));
    label->setAlignment(Qt::AlignCenter);
    layout->addWidget(label, 1, col++);

    QString detailCss = "QLabel { background-color:#666; }";
    for (i=0 ; i<nelem(stats_detail) ; i++) {
        label = new QLabel(row);
        label->setText(stats_detail[i].name);
        label->setAlignment(Qt::AlignCenter);
        label->setStyleSheet(detailCss);
        label->setToolTip(stats_detail[i].tooltip);
        layout->addWidget(label, 1, col++);
        m_colWidth.append(-1);
    }
    setLayoutColumnWidth(layout, nelem(stats_detail));

    spacer = new QSpacerItem(1, 1);
    layout->addItem(spacer, 1, col, 1,-1);
    layout->setColumnStretch(col, 1);

    return row;
}

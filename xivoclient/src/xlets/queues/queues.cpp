/* XiVO Client
 * Copyright (C) 2007-2012, Avencall
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

#include <QDebug>
#include <QVBoxLayout>
#include <QTimer>
#include <QMenu>
#include <QContextMenuEvent>
#include <QLabel>
#include <QScrollArea>
#include <QPushButton>
#include <QCheckBox>
#include <QSpinBox>

#include <baseengine.h>

#include "queues.h"

Q_EXPORT_PLUGIN2(xletqueuesplugin, XLetQueuesPlugin);

XLet* XLetQueuesPlugin::newXLetInstance(QWidget *parent)
{
    b_engine->registerTranslation(":/obj/queues_%1");
    return new XletQueues(parent);
}

XletQueues::XletQueues(QWidget *parent)
    : XLet(parent),
      m_configureWindow(NULL)
{
    setTitle(tr("Queues"));

    QStringList xletlist;
    uint nsecs = 30;
    if (b_engine->getConfig().contains("xlet.queues.statsfetchperiod")) {
        nsecs = b_engine->getConfig("xlet.queues.statsfetchperiod").toInt();
    }

    QVBoxLayout *xletLayout = new QVBoxLayout();
    setLayout(xletLayout);
    xletLayout->setSpacing(0);

    registerListener("getqueuesstats");

    m_model = new QueuesModel(this);

    m_proxyModel = new QueuesSortFilterProxyModel(this);
    m_proxyModel->setSourceModel(m_model);
    m_proxyModel->updateFilter();

    QueuesView *view = new QueuesView(this);
    view->setModel(m_proxyModel);
    view->hideColumn(QueuesModel::ID);
    view->sortByColumn(QueuesModel::NAME, Qt::AscendingOrder);
    view->init();

    xletLayout->addWidget(view);

    QTimer * timer_display = new QTimer(this);
    QTimer * timer_request = new QTimer(this);
    connect(timer_request, SIGNAL(timeout()), this, SLOT(askForQueueStats()));
    connect(timer_display, SIGNAL(timeout()),
            m_model, SLOT(increaseWaitTime()));
    timer_request->start(nsecs * 1000);
    timer_display->start(1000);

    connect(m_model, SIGNAL(askForQueueStats()),
            this, SLOT(askForQueueStats()));


    subscribeToQueuesStats();
}

void XletQueues::subscribeToQueuesStats()
{
    QVariantMap command;
    command["class"] = "subscribetoqueuesstats";

    b_engine->sendJsonCommand(command);

}

void XletQueues::parseCommand(const QVariantMap &map)
{
    m_model->eatQueuesStats(map);
}

void XletQueues::openConfigureWindow()
{
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

void XletQueues::askForQueueStats()
{
    QVariantMap _for;

    QVariantMap statConfig = b_engine->getConfig("guioptions.queuespanel").toMap();

    for (int i = 0 ; i < m_model->rowCount(QModelIndex()) ; i++) {
        QModelIndex index = m_model->index(i, QueuesModel::ID);
        QString xqueueid = m_model->data(index).toString();

        QVariantMap _param;
        _param["window"] = statConfig.value("window" + xqueueid, 3600).toString();
        _param["xqos"] = statConfig.value("xqos" + xqueueid, 60).toString();

        QString queueid = xqueueid.section("/", 1, 1);
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

    QLabel *desc = new QLabel(tr(
        "Choose which queue should be displayed, and the\n"
        "queues parameters for the Stats on slice:"), this);

    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setFrameStyle(QFrame::NoFrame);
    scrollArea->setWidget(buildConfigureQueueList(this));

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

    layout->addWidget(new QLabel(tr("Queue"), root), 0, 0, Qt::AlignLeft);
    QLabel *label_qos = new QLabel(tr("Qos - X (s)"), root);
    label_qos->setToolTip(tr(
        "This is the threshold in seconds to consider that the answer to a "
        "call was too late to be accounted as an answer of quality."));
    layout->addWidget(label_qos, 0, 1, Qt::AlignLeft);
    QLabel *label_window = new QLabel(tr("Window (s)"), root);
    label_window->setToolTip(tr(
        "The window is the period of time used to compute the statistics"));
    layout->addWidget(label_window, 0, 2, Qt::AlignLeft);

    QCheckBox *displayQueue;
    QSpinBox *spinBox;
    int row;
    int column;
    QVariantMap statConfig = b_engine->getConfig("guioptions.queuespanel").toMap();
    QString xqueueid;

    row = 1;

    QHashIterator<QString, XInfo *> i = \
        QHashIterator<QString, XInfo *>(b_engine->iterover("queues"));

    while (i.hasNext()) {
        column = 0;
        i.next();
        QueueInfo * queueinfo = (QueueInfo *) i.value();
        xqueueid = queueinfo->xid();

        displayQueue = new QCheckBox(queueinfo->queueName(), root);
        displayQueue->setProperty("xqueueid", xqueueid);
        displayQueue->setProperty("param", "visible");
        displayQueue->setChecked(statConfig.value("visible" + xqueueid, true).toBool());
        layout->addWidget(displayQueue, row, column++);
        connect(displayQueue, SIGNAL(stateChanged(int)),
                this, SLOT(changeQueueStatParam(int)));

        spinBox = new QSpinBox(root);
        spinBox->setAlignment(Qt::AlignCenter);
        spinBox->setMaximum(240);
        spinBox->setProperty("xqueueid", xqueueid);
        spinBox->setProperty("param", "xqos");
        spinBox->setValue(statConfig.value("xqos" + xqueueid, 60).toInt());
        layout->addWidget(spinBox, row, column++);
        connect(spinBox, SIGNAL(valueChanged(int)),
                this, SLOT(changeQueueStatParam(int)));

        spinBox = new QSpinBox(root);
        spinBox->setAlignment(Qt::AlignCenter);
        spinBox->setMaximum(3600*24);
        spinBox->setProperty("xqueueid", xqueueid);
        spinBox->setProperty("param", "window");
        spinBox->setValue(statConfig.value("window" + xqueueid, 3600).toInt());
        layout->addWidget(spinBox, row, column++);
        connect(spinBox, SIGNAL(valueChanged(int)),
                this, SLOT(changeQueueStatParam(int)));

        row++;
    }

    return root;
}

void XletQueuesConfigure::changeQueueStatParam(int v)
{
    QString xqueueid = sender()->property("xqueueid").toString();
    QString param = sender()->property("param").toString();

    QVariantMap qcfg = b_engine->getConfig("guioptions.queuespanel").toMap();
    qcfg[param + xqueueid] = v;

    QVariantMap config;
    config["guioptions.queuespanel"] = qcfg;

    b_engine->setConfig(config);
}

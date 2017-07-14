/* Copyright 2007-2017 The Wazo Authors  (see the AUTHORS file)
 *
 * This program is free software: you can redistribute it and/or modify
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
 * This programm is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QMainWindow>
#include <QDockWidget>
#include <QVBoxLayout>
#include <QListView>
#include <QTimer>

#include <baseengine.h>
#include <xletlib/agents_model.h>

#include "agent_status_dashboard.h"
#include "agent_status_widget_builder.h"
#include "agent_status_widget_storage.h"
#include "filtered_agent_list.h"

XLet* XLetAgentStatusDashboardPlugin::newXLetInstance(QWidget *parent)
{
    b_engine->registerTranslation(":/obj/agent_status_dashboard_%1");
    return new XletAgentStatusDashboard(parent);
}

XletAgentStatusDashboard::XletAgentStatusDashboard(QWidget *parent)
    : XLet(parent, tr("Agent status dashboard"))
{
    this->m_model = new AgentsModel(this);

    this->m_widget_builder = new AgentStatusWidgetBuilder;
    this->m_widget_storage = new AgentStatusWidgetStorage(this->m_widget_builder);
    this->m_delegate = new AgentStatusDelegate(this->m_widget_storage);

    // When embedding QMainWindow in a widget, setParent must be called after instantiation.
    this->m_window = new QMainWindow();
    this->m_window->setParent(this);
    this->m_window->setDockNestingEnabled(true);
    this->m_window->show();

    QVBoxLayout * layout = new QVBoxLayout(this);
    layout->addWidget(this->m_window);

    connect(b_engine, SIGNAL(initialized()),
            this, SLOT(restoreState()));
    connect(b_engine, SIGNAL(delogged()),
            this, SLOT(saveState()));

    connect(b_engine, SIGNAL(updateQueueConfig(const QString &)),
            this, SLOT(updateQueueConfig(const QString &)));

    connect(b_engine, SIGNAL(removeQueueConfig(const QString &)),
            this, SLOT(removeQueueConfig(const QString &)));

    QTimer * timer_display = new QTimer(this);
    connect(timer_display, SIGNAL(timeout()),
            m_model, SLOT(increaseAvailability()));
    timer_display->start(1000);
}

XletAgentStatusDashboard::~XletAgentStatusDashboard()
{
    delete m_delegate;
    delete m_widget_storage;
    delete m_widget_builder;

    foreach(const QString &queue_id, m_filtered_agent_lists.keys()){
        this->destroyQueue(queue_id);
    }
}

void XletAgentStatusDashboard::saveState()
{
    b_engine->setConfig("agent_status_dashboard.main_window_state", m_window->saveState());
}

void XletAgentStatusDashboard::updateQueueConfig(const QString & queue_id)
{
    QDockWidget * dock;
    FilteredAgentList * filtered_agent_list;
    if (m_filtered_agent_lists.contains(queue_id)) {
        dock = this->m_window->findChild<QDockWidget *>(queue_id);
        filtered_agent_list = this->m_filtered_agent_lists.value(queue_id);
    } else {
        dock = this->createDock(queue_id);
        filtered_agent_list = this->createFilteredAgentList(queue_id);
        QWidget * agent_list_view = dynamic_cast<QWidget *>(filtered_agent_list->getView());
        dock->setWidget(agent_list_view);
        dock->show();
    }
    dock->setWindowTitle(filtered_agent_list->getQueueName());
}

QDockWidget * XletAgentStatusDashboard::createDock(const QString & queue_id)
{
    QDockWidget * new_dock = new QDockWidget(this->m_window);
    new_dock->setObjectName(queue_id);
    this->m_window->addDockWidget(Qt::TopDockWidgetArea, new_dock);

    return new_dock;
}

FilteredAgentList * XletAgentStatusDashboard::createFilteredAgentList(const QString & queue_id)
{
    FilteredAgentList * filtered_agent_list = new FilteredAgentList(queue_id,
                                                                    this->m_model,
                                                                    this->m_delegate);

    this->m_filtered_agent_lists.insert(queue_id, filtered_agent_list);

    return filtered_agent_list;
}

void XletAgentStatusDashboard::removeQueueConfig(const QString & queue_id)
{
    this->destroyQueue(queue_id);
}

void XletAgentStatusDashboard::restoreState()
{
    QByteArray main_window_state = b_engine->getConfig("agent_status_dashboard.main_window_state").toByteArray();
    this->m_window->restoreState(main_window_state);
}

void XletAgentStatusDashboard::destroyQueue(const QString & queue_id)
{
    if (this->m_filtered_agent_lists.contains(queue_id)) {
        QDockWidget * dock = this->m_window->findChild<QDockWidget *>(queue_id);
        this->m_window->removeDockWidget(dock);
        delete dock;

        FilteredAgentList * filtered_agent_list = this->m_filtered_agent_lists.take(queue_id);
        delete filtered_agent_list;
    }
}

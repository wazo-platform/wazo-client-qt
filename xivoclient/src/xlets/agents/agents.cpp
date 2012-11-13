/* Copyright (C) 2007-2012, Avencall
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

#include <QVBoxLayout>
#include <QTimer>

#include <baseengine.h>

#include "agents.h"
#include "agents_view.h"
#include "agents_sort_filter_proxy_model.h"
#include "agents_model.h"
#include "agents_controller.h"

Q_EXPORT_PLUGIN2(xletagentsplugin, XLetAgentsPlugin);

XLet* XLetAgentsPlugin::newXLetInstance(QWidget *parent)
{
    b_engine->registerTranslation(":/obj/agents_%1");
    return new XletAgents(parent);
}

XletAgents::XletAgents(QWidget *parent)
    : XLet(parent)
{
    setTitle(tr("Agents' List (plain)"));

    QVBoxLayout *xletLayout = new QVBoxLayout();
    setLayout(xletLayout);
    xletLayout->setSpacing(0);

    m_model = new AgentsModel(this);

    AgentsSortFilterProxyModel * proxy_model = new AgentsSortFilterProxyModel(this);
    proxy_model->setSourceModel(m_model);

    AgentsController *controller = new AgentsController(this, proxy_model);

    m_view = new AgentsView(this);
    m_view->setModel(proxy_model);
    m_view->hideColumn(AgentsModel::ID);
    m_view->sortByColumn(AgentsModel::LASTNAME, Qt::AscendingOrder);

    xletLayout->addWidget(m_view);

    QTimer * timer_display = new QTimer(this);
    connect(timer_display, SIGNAL(timeout()),
            m_model, SLOT(increaseAvailability()));
    timer_display->start(1000);

    connect(m_view, SIGNAL(clicked(const QModelIndex &)),
            controller, SLOT(agentClicked(const QModelIndex &)));
}

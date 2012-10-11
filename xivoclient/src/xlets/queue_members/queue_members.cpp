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

#include <QVBoxLayout>

#include <baseengine.h>

#include "queue_members.h"

Q_EXPORT_PLUGIN2(xletqueuememberplugin, XLetQueueMembersPlugin);

XLet* XLetQueueMembersPlugin::newXLetInstance(QWidget *parent)
{
    b_engine->registerTranslation(":/obj/queue_member_%1");
    return new XletQueueMembers(parent);
}

XletQueueMembers::XletQueueMembers(QWidget *parent)
    : XLet(parent)
{
    setTitle(tr("Queue_Member' List"));

    QVBoxLayout *xletLayout = new QVBoxLayout();
    setLayout(xletLayout);
    xletLayout->setSpacing(0);

    m_model = new QueueMembersModel(this);

    // m_proxy_model = new QueueMemberSortFilterProxyModel(this);
    // m_proxy_model->setSourceModel(m_model);
    // m_proxy_model->setDynamicSortFilter(true);
    // m_proxy_model->updateFilter();

    QueueMembersView *view = new QueueMembersView(this);
    view->setModel(m_model);
    view->hideColumn(QueueMembersModel::ID);
    view->sortByColumn(QueueMembersModel::NUMBER, Qt::AscendingOrder);

    xletLayout->addWidget(view);
}

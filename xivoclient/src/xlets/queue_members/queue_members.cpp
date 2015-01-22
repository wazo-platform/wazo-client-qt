/* XiVO Client
 * Copyright (C) 2007-2015 Avencall
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
#include "queue_members_header.h"
#include "queue_members_view.h"
#include "queue_members_model.h"
#include "queue_members_sort_filter_proxy_model.h"

XLet* XLetQueueMembersPlugin::newXLetInstance(QWidget *parent)
{
    b_engine->registerTranslation(":/obj/queue_members_%1");
    return new XletQueueMembers(parent);
}

XletQueueMembers::XletQueueMembers(QWidget *parent)
    : XLet(parent, tr("Queue Members"))
{
    QVBoxLayout *xletLayout = new QVBoxLayout();
    setLayout(xletLayout);
    xletLayout->setSpacing(0);

    m_model = new QueueMembersModel(this);

    m_proxy_model = new QueueMembersSortFilterProxyModel(this);
    m_proxy_model->setSourceModel(m_model);

    m_view = new QueueMembersView(this);
    m_view->setModel(m_proxy_model);
    m_view->hideColumn(QueueMembersModel::ID);
    m_view->sortByColumn(QueueMembersModel::NUMBER, Qt::AscendingOrder);

    this->addHeader(xletLayout);
    this->addCenteredBody(xletLayout);
}

void XletQueueMembers::addHeader(QVBoxLayout * layout)
{
    QueueMembersHeader *header = new QueueMembersHeader(this);
    header->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    layout->addWidget(header);
}

void XletQueueMembers::addCenteredBody(QVBoxLayout * layout)
{
    QHBoxLayout * centering_layout = new QHBoxLayout();

    centering_layout->insertStretch(0);
    centering_layout->addWidget(m_view);
    centering_layout->insertStretch(2);

    layout->addLayout(centering_layout);
}

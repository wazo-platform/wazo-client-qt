/* XiVO Client
 * Copyright (C) 2007-2014 Avencall
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

#include "conference.h"
#include "conflist.h"
#include "conflist_model.h"
#include "conflist_view.h"
#include "conftab.h"

ConfList::ConfList(ConfTab *parent)
    : QWidget(), m_conf_tab(parent)
{
    QVBoxLayout *vBox = new QVBoxLayout(this);
    setLayout(vBox);

    // this contains the data, unordered
    m_model = new ConfListModel(this);
    m_model->setObjectName("conflist_model");

    QTimer * timer_display = new QTimer(this);
    connect(timer_display, SIGNAL(timeout()),
            m_model, SLOT(updateConfTime()));
    timer_display->start(1000);

    // this maps the indexes between the sorted view and the unordered model
    QSortFilterProxyModel *proxyModel = new QSortFilterProxyModel(this);
    proxyModel->setSourceModel(m_model);
    proxyModel->setDynamicSortFilter(true); /* sorts right on insertion, instead
    of half a second after the window has appeared */

    // this displays the sorted data
    ConfListView *view = new ConfListView(this);
    view->setModel(proxyModel);
    view->hideColumn(ConfListModel::ID);
    view->hideColumn(ConfListModel::MODERATED);
    view->sortByColumn(ConfListModel::NAME, Qt::AscendingOrder);

    vBox->addWidget(view);
}

void ConfList::phoneConfRoom()
{
    const QString &roomNumber = sender()->property("number").toString();
    QVariantMap members = m_model->getMembers(roomNumber);

    b_engine->actionDial(roomNumber);
    m_conf_tab->showConfRoom(roomNumber, members);
}

void ConfList::openConfRoom()
{
    const QString &number = sender()->property("number").toString();
    QVariantMap members = m_model->getMembers(number);

    m_conf_tab->showConfRoom(number, members);
}

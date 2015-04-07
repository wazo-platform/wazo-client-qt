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

#include <QDebug>
#include <QTimer>

#include <xletlib/signal_relayer.h>
#include <baseengine.h>
#include <message_factory.h>

#include "people.h"
#include "people_entry_model.h"
#include "people_entry_sort_filter_proxy_model.h"

#include "ui_people_widget.h"

People::People(QWidget *parent)
    : XLet(parent, tr("People"), ":/images/tab-people.svg"),
      m_proxy_model(NULL),
      m_people_entry_manager(this)
{
    this->ui.setupUi(this);

    ui.menu->addAction(tr("all"));
    ui.menu->setSelectedIndex(0);

    m_proxy_model = new PeopleEntrySortFilterProxyModel(this);
    m_model = new PeopleEntryModel(m_people_entry_manager, this);
    m_proxy_model->setSourceModel(m_model);
    ui.entry_table->setModel(m_proxy_model);
    connect(m_proxy_model, SIGNAL(columnsInserted(const QModelIndex &, int, int)),
            ui.entry_table, SLOT(updateColumnsDelegates(const QModelIndex &, int, int)));
    connect(m_proxy_model, SIGNAL(columnsInserted(const QModelIndex &, int, int)),
            ui.entry_table, SLOT(updateColumnsVisibility(const QModelIndex &, int, int)));
    connect(m_proxy_model, SIGNAL(columnsInserted(const QModelIndex &, int, int)),
            this, SLOT(defaultColumnSort(const QModelIndex &, int, int)));

    connect(this->ui.entry_filter, SIGNAL(textChanged(const QString &)),
            this, SLOT(schedulePeopleLookup(const QString &)));
    connect(signal_relayer, SIGNAL(numberSelectionRequested()),
            this, SLOT(numberSelectionRequested()));
    connect(this->ui.entry_filter, SIGNAL(returnPressed()),
            this, SLOT(focusEntryTable()));
    connect(&m_remote_lookup_timer, SIGNAL(timeout()),
            this, SLOT(searchPeople()));
    connect(this->ui.entry_table, SIGNAL(extensionClicked(const QString &)),
            this, SLOT(dial(const QString &)));
    this->m_remote_lookup_timer.setSingleShot(true);
    this->m_remote_lookup_timer.setInterval(delay_before_lookup);
    b_engine->sendJsonCommand(MessageFactory::getPeopleHeaders());
}

People::~People()
{
}

void People::numberSelectionRequested()
{
    this->ui.entry_filter->setFocus();
    int selection_length = this->ui.entry_filter->text().length();
    this->ui.entry_filter->setSelection(0, selection_length);
}

void People::focusEntryTable()
{
    if (this->m_proxy_model->rowCount() > 0) {
        this->ui.entry_table->selectFirstRow();
    } else {
        signal_relayer->relayNoNumberSelected();
    }
}

void People::schedulePeopleLookup(const QString &lookup_pattern)
{
    m_searched_pattern = lookup_pattern;
    m_remote_lookup_timer.start();
}

void People::searchPeople()
{
    if (m_searched_pattern.length() < min_lookup_length) {
        qDebug() << Q_FUNC_INFO << "ignoring pattern too short" << this->m_searched_pattern;
    } else {
        m_search_history.append(m_searched_pattern);
        b_engine->sendJsonCommand(MessageFactory::peopleSearch(m_searched_pattern));
        qDebug() << Q_FUNC_INFO << "searching" << m_searched_pattern << "...";
    }
}

void People::defaultColumnSort(const QModelIndex &, int, int)
{
    this->m_proxy_model->invalidate();
    int name_column_index = this->m_model->getNameColumnIndex();
    this->m_proxy_model->sort(name_column_index, Qt::AscendingOrder);
    this->ui.entry_table->horizontalHeader()->setSortIndicator(name_column_index, Qt::AscendingOrder);
}

void People::dial(const QString &extension)
{
    b_engine->sendJsonCommand(MessageFactory::dial(extension));
}

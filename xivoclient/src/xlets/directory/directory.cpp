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

#include <QDebug>
#include <QTimer>

#include <xletlib/signal_relayer.h>
#include <baseengine.h>
#include <message_factory.h>

#include "directory.h"
#include "directory_entry_model.h"
#include "directory_entry_sort_filter_proxy_model.h"

#include "ui_directory_widget.h"

Directory::Directory(QWidget *parent)
    : XLet(parent),
      m_proxy_model(NULL),
      m_directory_entry_manager(m_phone_dao, m_user_dao, this)
{
    setTitle(tr("Directory"));

    this->ui.setupUi(this);

    m_proxy_model = new DirectoryEntrySortFilterProxyModel(this);
    m_model = new DirectoryEntryModel(m_directory_entry_manager, this);
    m_proxy_model->setSourceModel(m_model);
    ui.entry_table->setModel(m_proxy_model);

    connect(this->ui.entry_filter, SIGNAL(textChanged(const QString &)),
            m_proxy_model, SLOT(setFilter(const QString &)));
    connect(this->ui.entry_filter, SIGNAL(textChanged(const QString &)),
            &m_directory_entry_manager, SLOT(updateSearch(const QString &)));
    connect(this->ui.entry_filter, SIGNAL(textChanged(const QString &)),
            this, SLOT(scheduleDirectoryLookup(const QString &)));
    connect(signal_relayer, SIGNAL(numberSelectionRequested()),
            this, SLOT(numberSelectionRequested()));
    connect(this->ui.entry_filter, SIGNAL(returnPressed()),
            this, SLOT(focusEntryTable()));
    connect(this->ui.entry_table, SIGNAL(activated(const QModelIndex &)),
            this, SLOT(entrySelectedIndex(const QModelIndex &)));
    connect(&m_remote_lookup_timer, SIGNAL(timeout()),
            this, SLOT(searchDirectory()));
    connect(m_model, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)),
            this, SLOT(dataChanged(const QModelIndex &, const QModelIndex &)));
    this->m_remote_lookup_timer.setSingleShot(true);
    this->m_remote_lookup_timer.setInterval(delay_before_lookup);
    b_engine->sendJsonCommand(MessageFactory::getSwitchboardDirectoryHeaders());
}

Directory::~Directory()
{
}

void Directory::numberSelectionRequested()
{
    this->ui.entry_filter->setFocus();
    int selection_length = this->ui.entry_filter->text().length();
    this->ui.entry_filter->setSelection(0, selection_length);
}

void Directory::focusEntryTable()
{
    if (this->m_proxy_model->rowCount() > 0) {
        this->ui.entry_table->selectFirstRow();
    } else {
        signal_relayer->relayNoNumberSelected();
    }
}

void Directory::entrySelectedIndex(const QModelIndex &index)
{
    const QString &number = m_proxy_model->getNumber(index);
    signal_relayer->relayNumberSelected(number);
}

void Directory::scheduleDirectoryLookup(const QString &lookup_pattern)
{
    if (lookup_pattern.length() >= min_lookup_length) {
        m_searched_pattern = lookup_pattern;
        m_remote_lookup_timer.start();
    }
}

void Directory::searchDirectory()
{
    if (! this->alreadySearched(this->m_searched_pattern)) {
        m_search_history.append(m_searched_pattern);
        b_engine->sendJsonCommand(MessageFactory::switchboardDirectorySearch(m_searched_pattern));
        qDebug() << Q_FUNC_INFO << "searching" << m_searched_pattern << "...";
    }
}

bool Directory::alreadySearched(const QString &search_pattern) const
{
    foreach (const QString &old_search, m_search_history) {
        if (search_pattern.contains(old_search)) {
            return true;
        }
    }
    return false;
}

void Directory::dataChanged(const QModelIndex &, const QModelIndex &)
{
    this->m_proxy_model->invalidate();
    int name_column_index = this->m_model->getNameColumnIndex();
    this->m_proxy_model->sort(name_column_index, Qt::AscendingOrder);
    ui.entry_table->resizeColumnsToContents();
}

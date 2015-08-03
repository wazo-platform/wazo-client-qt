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
#include <QMessageBox>
#include <QPointer>
#include <QTimer>

#include <xletlib/signal_relayer.h>
#include <baseengine.h>
#include <message_factory.h>

#include "contact_dialog.h"
#include "people.h"
#include "people_entry_model.h"
#include "people_entry_sort_filter_proxy_model.h"


People::People(QWidget *parent)
    : XLet(parent, tr("People"), ":/images/tab-people.svg"),
      m_proxy_model(NULL),
      m_model(NULL)
{
    this->ui.setupUi(this);

    m_proxy_model = new PeopleEntrySortFilterProxyModel(this);
    m_model = new PeopleEntryModel(this);
    m_proxy_model->setSourceModel(m_model);
    ui.entry_table->setModel(m_proxy_model);

    QAction *search_action = ui.menu->addAction(tr("all"));
    QAction *favorite_action = ui.menu->addAction(tr("favorites"));
    QAction *my_contacts_action = ui.menu->addAction(tr("my contacts"));

    connect(search_action, SIGNAL(triggered()),
            this, SLOT(searchMode()));
    connect(favorite_action, SIGNAL(triggered()),
            this, SLOT(favoriteMode()));
    connect(my_contacts_action, SIGNAL(triggered()),
            this, SLOT(personalContactsMode()));

    this->ui.menu->setSelectedAction(1);

    connect(m_proxy_model, SIGNAL(columnsInserted(const QModelIndex &, int, int)),
            ui.entry_table, SLOT(updateColumnsDelegates(const QModelIndex &, int, int)));
    connect(m_proxy_model, SIGNAL(columnsInserted(const QModelIndex &, int, int)),
            ui.entry_table, SLOT(updateColumnsVisibility(const QModelIndex &, int, int)));
    connect(m_model, SIGNAL(columnsInserted(const QModelIndex &, int, int)),
            this, SLOT(defaultColumnSort(const QModelIndex &, int, int)));

    connect(this->ui.entry_table, SIGNAL(favoriteToggled(const QVariantMap &)),
            this, SLOT(setFavoriteStatus(const QVariantMap &)));
    connect(this->ui.entry_table, SIGNAL(deleteEntry(const QVariantMap &)),
            this, SLOT(deletePersonalContact(const QVariantMap &)));

    connect(this->ui.entry_filter, SIGNAL(textChanged(const QString &)),
            this, SLOT(schedulePeopleLookup(const QString &)));
    connect(this->ui.entry_filter, SIGNAL(returnPressed()),
            this, SLOT(searchPeople()));

    connect(this->ui.new_contact_button, SIGNAL(clicked()),
            this, SLOT(openNewContactDialog()));

    connect(signal_relayer, SIGNAL(numberSelectionRequested()),
            this, SLOT(numberSelectionRequested()));
    connect(this->ui.entry_filter, SIGNAL(returnPressed()),
            this, SLOT(focusEntryTable()));
    connect(&m_lookup_timer, SIGNAL(timeout()),
            this, SLOT(searchPeople()));
    this->m_lookup_timer.setSingleShot(true);
    this->m_lookup_timer.setInterval(delay_before_lookup);
    b_engine->sendJsonCommand(MessageFactory::getPeopleHeaders());

    this->registerListener("agent_status_update");
    this->registerListener("endpoint_status_update");
    this->registerListener("people_favorite_update");
    this->registerListener("people_favorites_result");
    this->registerListener("people_headers_result");
    this->registerListener("people_personal_contacts_result");
    this->registerListener("people_personal_contact_created");
    this->registerListener("people_personal_contact_deleted");
    this->registerListener("people_search_result");
    this->registerListener("user_status_update");
}

People::~People()
{
}

void People::parseCommand(const QVariantMap &command)
{
    const QString &event = command["class"].toString();

    if (event == "agent_status_update") {
        m_model->parseAgentStatusUpdate(command);
    } else if (event == "endpoint_status_update") {
        m_model->parseEndpointStatusUpdate(command);
    } else if (event == "user_status_update") {
        m_model->parseUserStatusUpdate(command);
    } else if (event == "people_headers_result") {
        m_model->parsePeopleHeadersResult(command);
    } else if (event == "people_search_result") {
        m_model->parsePeopleSearchResult(command);
    } else if (event == "people_favorites_result") {
        m_model->parsePeopleSearchResult(command);
    } else if (event == "people_personal_contacts_result") {
        m_model->parsePeopleSearchResult(command);
    } else if (event == "people_personal_contact_created") {
        this->parsePeoplePersonalContactCreated(command);
    } else if (event == "people_personal_contact_deleted") {
        this->parsePeoplePersonalContactDeleted(command);
    } else if (event == "people_favorite_update") {
        m_model->parsePeopleFavoriteUpdate(command);
    }

}

void People::parsePeoplePersonalContactCreated(const QVariantMap &/*result*/)
{
    if (m_mode == PERSONAL_CONTACT_MODE) {
        b_engine->sendJsonCommand(MessageFactory::personalContacts());
    }
}

void People::parsePeoplePersonalContactDeleted(const QVariantMap &result)
{
    const QVariantMap &data = result["data"].toMap();
    const QString &source = data["source"].toString();
    const QString &source_entry_id = data["source_entry_id"].toString();
    m_model->removeRowFromSourceEntryId(source, source_entry_id);
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
    m_lookup_timer.start();
}

void People::searchPeople()
{
    m_lookup_timer.stop();

    if (m_searched_pattern.length() < min_lookup_length) {
        qDebug() << Q_FUNC_INFO << "ignoring pattern too short" << this->m_searched_pattern;
    } else {
        if (m_mode != SEARCH_MODE) {
            this->ui.menu->setSelectedAction(0);
        }
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

void People::deletePersonalContact(const QVariantMap &unique_source_entry_id)
{
    const QString &source_name = unique_source_entry_id["source"].toString();
    const QString &source_entry_id = unique_source_entry_id["source_entry_id"].toString();
    QPointer<QMessageBox> message = new QMessageBox(QMessageBox::Warning,
                                                    tr("Removing this contact"),
                                                    tr("Removing this contact.\n"
                                                       "Are you sure ?"),
                                                    QMessageBox::Yes|QMessageBox::No,
                                                    this);

    if (message->exec() == QMessageBox::Yes && ! source_entry_id.isEmpty()) {
        b_engine->sendJsonCommand(MessageFactory::deletePersonalContact(source_name, source_entry_id));
    }
    delete message;
}

void People::setFavoriteStatus(const QVariantMap &unique_source_entry_id)
{
    bool enabled = m_model->favoriteStatus(unique_source_entry_id);
    const QString &source_name = unique_source_entry_id["source"].toString();
    const QString &source_entry_id = unique_source_entry_id["source_entry_id"].toString();
    if (source_entry_id.isEmpty()) {
        return;
    }
    b_engine->sendJsonCommand(MessageFactory::setFavoriteStatus(source_name, source_entry_id, !enabled));
}

void People::searchMode()
{
    m_mode = SEARCH_MODE;
    m_model->clearEntries();
}

void People::favoriteMode()
{
    m_mode = FAVORITE_MODE;
    ui.entry_filter->clear();
    m_model->clearEntries();
    b_engine->sendJsonCommand(MessageFactory::favorites());
}

void People::personalContactsMode()
{
    m_mode = PERSONAL_CONTACT_MODE;
    ui.entry_filter->clear();
    m_model->clearEntries();
    b_engine->sendJsonCommand(MessageFactory::personalContacts());
}

void People::openNewContactDialog()
{
    QVariantMap contact_infos;
    QPointer<ContactDialog> contact_dialog = new ContactDialog(this, &contact_infos);
    if (contact_dialog->exec() == QDialog::Accepted && !contact_infos.isEmpty()) {
        b_engine->sendJsonCommand(MessageFactory::createPersonalContact(contact_infos));
    }
    delete contact_dialog;
}

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
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QPointer>
#include <QTextStream>
#include <QTimer>

#include <xletlib/signal_relayer.h>
#include <baseengine.h>
#include <message_factory.h>

#include "contact_dialog.h"
#include "people.h"
#include "people_entry_model.h"
#include "people_entry_sort_filter_proxy_model.h"
#include "people_personal_migration.h"


People::People(QWidget *parent)
    : XLet(parent, tr("People"), ":/images/tab-people.svg"),
      m_proxy_model(NULL),
      m_model(NULL),
      m_waiting_status(NULL)
{
    this->ui.setupUi(this);

    m_waiting_status = new QMovie(":/images/waiting-status.gif", QByteArray(), this);

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
    connect(this->ui.entry_table, SIGNAL(deletePersonalContactClicked(const QVariantMap &)),
            this, SLOT(deletePersonalContact(const QVariantMap &)));
    connect(this->ui.entry_table, SIGNAL(editPersonalContactClicked(const QVariantMap &)),
            this, SLOT(requestEditPersonalContact(const QVariantMap &)));

    connect(this->ui.entry_filter, SIGNAL(textChanged(const QString &)),
            this, SLOT(schedulePeopleLookup(const QString &)));
    connect(this->ui.entry_filter, SIGNAL(returnPressed()),
            this, SLOT(searchPeople()));

    connect(this->ui.new_contact_button, SIGNAL(clicked()),
            this, SLOT(openNewContactDialog()));
    connect(this->ui.import_button, SIGNAL(clicked()),
            this, SLOT(openImportDialog()));
    connect(this->ui.export_button, SIGNAL(clicked()),
            this, SLOT(requestExportPersonalContacts()));
    connect(this->ui.purge_contacts_button, SIGNAL(clicked()),
            this, SLOT(purgePersonalContacts()));

    connect(signal_relayer, SIGNAL(numberSelectionRequested()),
            this, SLOT(numberSelectionRequested()));
    connect(this->ui.entry_filter, SIGNAL(returnPressed()),
            this, SLOT(focusEntryTable()));

    connect(&m_lookup_timer, SIGNAL(timeout()),
            this, SLOT(searchPeople()));
    m_lookup_timer.setSingleShot(true);
    m_lookup_timer.setInterval(delay_before_lookup);

    connect(&m_failure_timer, SIGNAL(timeout()),
            this, SLOT(setFailureStatus()));
    m_failure_timer.setSingleShot(true);
    m_failure_timer.setInterval(delay_before_failure);

    connect(&m_before_waiting_timer, SIGNAL(timeout()),
            this, SLOT(setWaitingStatus()));
    m_before_waiting_timer.setSingleShot(true);
    m_before_waiting_timer.setInterval(delay_before_waiting);

    b_engine->sendJsonCommand(MessageFactory::getPeopleHeaders());

    this->registerListener("people_headers_result");

    this->registerListener("people_search_result");

    this->registerListener("agent_status_update");
    this->registerListener("endpoint_status_update");
    this->registerListener("user_status_update");

    this->registerListener("people_favorite_update");
    this->registerListener("people_favorites_result");

    this->registerListener("people_export_personal_contacts_csv_result");
    this->registerListener("people_import_personal_contacts_csv_result");
    this->registerListener("people_personal_contact_created");
    this->registerListener("people_personal_contact_deleted");
    this->registerListener("people_personal_contact_raw_result");
    this->registerListener("people_personal_contact_raw_update");
    this->registerListener("people_personal_contacts_purged");
    this->registerListener("people_personal_contacts_result");

    if (PeoplePersonalMigration::needMigration()) {
        PeoplePersonalMigration::noticeAndMigratePersonalContacts(this);
    }
}

People::~People()
{
}

void People::parseCommand(const QVariantMap &command)
{
    const QString &event = command["class"].toString();

    if (event != "agent_status_update" &&
        event != "endpoint_status_update" &&
        event != "user_status_update")
    {
        this->setSuccessStatus();
    }

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
    } else if (event == "people_personal_contacts_purged") {
        this->updatePersonalContacts();
    } else if (event == "people_personal_contact_created") {
        this->updatePersonalContacts();
    } else if (event == "people_personal_contact_deleted") {
        this->parsePeoplePersonalContactDeleted(command);
    } else if (event == "people_personal_contact_raw_result") {
        this->parsePeoplePersonalContactRawResult(command);
    } else if (event == "people_personal_contact_raw_update") {
        this->updatePersonalContacts();
    } else if (event == "people_favorite_update") {
        m_model->parsePeopleFavoriteUpdate(command);
    } else if (event == "people_export_personal_contacts_csv_result") {
        this->parsePeopleExportPersonalContactsCSVResult(command);
    } else if (event == "people_import_personal_contacts_csv_result") {
        this->parsePeopleImportPersonalContactsCSVResult(command);
    }
}

void People::parsePeoplePersonalContactDeleted(const QVariantMap &result)
{
    const QString &source = result["source"].toString();
    const QString &source_entry_id = result["source_entry_id"].toString();
    m_model->removeRowFromSourceEntryId(source, source_entry_id);
}

void People::parsePeoplePersonalContactRawResult(const QVariantMap &result)
{
    const QString &source = result["source"].toString();
    const QString &source_entry_id = result["source_entry_id"].toString();
    this->openEditContactDialog(source, source_entry_id, result["contact_infos"].toMap());
}

void People::parsePeopleExportPersonalContactsCSVResult(const QVariantMap &result)
{
    m_csv_contacts = result["csv_contacts"].toByteArray();
    this->openExportDialog();
}

void People::parsePeopleImportPersonalContactsCSVResult(const QVariantMap &result)
{
    const QVariantList &failed_contacts = result["failed"].toList();
    int created_count = result["created_count"].toInt();
    int failed_count = failed_contacts.size();

    QMessageBox *message = new QMessageBox(QMessageBox::Information,
                                           tr("Import Results"),
                                           tr("%1 contacts created\n"
                                              "%2 contacts failed").arg(created_count)
                                                                   .arg(failed_count),
                                           QMessageBox::NoButton,
                                           this);

    if (failed_count > 0) {
        QString failed_formatted = tr("Following lines are errors");
        foreach(const QVariant &failed_contact, failed_contacts) {
            const QVariantMap &entry = failed_contact.toMap();
            failed_formatted.append(tr("\nline %1: ").arg(entry.value("line").toInt()));
            failed_formatted.append(entry.value("errors").toStringList().join(", "));
        }
        message->setDetailedText(failed_formatted);
    }
    if (created_count > 0) {
        this->updatePersonalContacts();
    }

    message->setAttribute(Qt::WA_DeleteOnClose);
    message->show();
}

void People::updatePersonalContacts()
{
    if (m_mode == PERSONAL_CONTACT_MODE) {
        this->waitingStatusAboutToBeStarted();
        b_engine->sendJsonCommand(MessageFactory::personalContacts());
    }
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
        this->waitingStatusAboutToBeStarted();
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
        this->waitingStatusAboutToBeStarted();
        b_engine->sendJsonCommand(MessageFactory::deletePersonalContact(source_name, source_entry_id));
    }
    delete message;
}

void People::purgePersonalContacts()
{
    QPointer<QMessageBox> message = new QMessageBox(QMessageBox::Warning,
                                                    tr("Deleting all personal contacts"),
                                                    tr("Deleting all personal contacts.\n"
                                                       "Are you sure ?"),
                                                    QMessageBox::Yes|QMessageBox::No,
                                                    this);

    if (message->exec() == QMessageBox::Yes) {
        this->waitingStatusAboutToBeStarted();
        b_engine->sendJsonCommand(MessageFactory::purgePersonalContacts());
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
    this->waitingStatusAboutToBeStarted();
    b_engine->sendJsonCommand(MessageFactory::setFavoriteStatus(source_name, source_entry_id, !enabled));
}

void People::searchMode()
{
    m_mode = SEARCH_MODE;
    m_model->clearEntries();
    m_proxy_model->setFilterMode(m_mode);
}

void People::favoriteMode()
{
    m_mode = FAVORITE_MODE;
    ui.entry_filter->clear();
    m_model->clearEntries();
    m_proxy_model->setFilterMode(m_mode);
    this->waitingStatusAboutToBeStarted();
    b_engine->sendJsonCommand(MessageFactory::favorites());
}

void People::personalContactsMode()
{
    m_mode = PERSONAL_CONTACT_MODE;
    ui.entry_filter->clear();
    m_model->clearEntries();
    m_proxy_model->setFilterMode(m_mode);
    this->waitingStatusAboutToBeStarted();
    b_engine->sendJsonCommand(MessageFactory::personalContacts());
}

void People::requestEditPersonalContact(const QVariantMap &unique_source_entry_id)
{
    const QString &source_name = unique_source_entry_id["source"].toString();
    const QString &source_entry_id = unique_source_entry_id["source_entry_id"].toString();
    this->waitingStatusAboutToBeStarted();
    b_engine->sendJsonCommand(MessageFactory::personalContactRaw(source_name, source_entry_id));
}

void People::requestExportPersonalContacts()
{
    this->waitingStatusAboutToBeStarted();
    b_engine->sendJsonCommand(MessageFactory::exportPersonalContactsCSV());
}

void People::openNewContactDialog()
{
    QVariantMap contact_infos;
    ContactDialog *contact_dialog = new ContactDialog(this, contact_infos);
    connect(contact_dialog, SIGNAL(acceptedWithInfos(const QString &, const QString &, const QVariantMap &)),
            this, SLOT(sendCreatePersonalContact(const QString &, const QString &, const QVariantMap &)));

    contact_dialog->setAttribute(Qt::WA_DeleteOnClose);
    contact_dialog->show();
}

void People::sendCreatePersonalContact(const QString &/*source_name*/,
                                       const QString &/*source_entry_id*/,
                                       const QVariantMap &contact_infos)
{
    if (contact_infos.isEmpty()) {
        return;
    }
    this->waitingStatusAboutToBeStarted();
    b_engine->sendJsonCommand(MessageFactory::createPersonalContact(contact_infos));
}

void People::openExportDialog()
{
    QString open_path = QDir::toNativeSeparators(QDir::homePath());
    QFileDialog *file_dialog = new QFileDialog(this,
                                               tr("Save Personal Contacts"),
                                               open_path,
                                               tr("CSV Files (*.csv);;All Files (*)"));
    connect(file_dialog, SIGNAL(fileSelected(const QString &)),
            this, SLOT(savePersonalContactsToFile(const QString &)));

    file_dialog->setAcceptMode(QFileDialog::AcceptSave);
#ifdef Q_OS_UNIX
    file_dialog->setOptions(QFileDialog::DontUseNativeDialog); //QTBUG-38985
#endif
    file_dialog->setAttribute(Qt::WA_DeleteOnClose);
    file_dialog->show();
}

void People::savePersonalContactsToFile(const QString &file_name)
{
    QFile file(file_name);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text) || m_csv_contacts.isEmpty()) {
        return;
    }
    QTextStream stream (&file);
    stream << m_csv_contacts;
}

void People::openImportDialog()
{
    QString open_path = QDir::toNativeSeparators(QDir::homePath());
    QFileDialog *file_dialog = new QFileDialog(this,
                                               tr("Import Personal Contacts"),
                                               open_path,
                                               tr("CSV Files (UTF-8)(*.csv);;All Files (*)"));
    connect(file_dialog, SIGNAL(fileSelected(const QString &)),
            this, SLOT(sendPersonalContactsFromFile(const QString &)));

    file_dialog->setFileMode(QFileDialog::ExistingFile);
#ifdef Q_OS_UNIX
    file_dialog->setOptions(QFileDialog::DontUseNativeDialog); //QTBUG-38985
#endif
    file_dialog->setAttribute(Qt::WA_DeleteOnClose);
    file_dialog->show();
}

void People::sendPersonalContactsFromFile(const QString &file_name)
{
    QFile file(file_name);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }
    this->waitingStatusAboutToBeStarted();
    b_engine->sendJsonCommand(MessageFactory::importPersonalContactsCSV(file.readAll()));
}

void People::openEditContactDialog(const QString &source_name,
                                   const QString &source_entry_id,
                                   const QVariantMap &contact_infos)
{
    ContactDialog *contact_dialog = new ContactDialog(this, contact_infos, source_name, source_entry_id);
    connect(contact_dialog, SIGNAL(acceptedWithInfos(const QString &, const QString &, const QVariantMap &)),
            this, SLOT(sendEditPersonalContact(const QString &, const QString &, const QVariantMap &)));

    contact_dialog->setAttribute(Qt::WA_DeleteOnClose);
    contact_dialog->show();
}

void People::sendEditPersonalContact(const QString &source_name,
                                     const QString &source_entry_id,
                                     const QVariantMap &contact_infos)
{
    if (contact_infos.isEmpty()) {
        return;
    }
    this->waitingStatusAboutToBeStarted();
    b_engine->sendJsonCommand(MessageFactory::editPersonalContact(source_name,
                                                                  source_entry_id,
                                                                  contact_infos));
}

void People::setFailureStatus()
{
    this->ui.status_icon->setPixmap(QPixmap(":/images/dot-red.svg"));
    this->ui.status_icon->setToolTip(tr("Failed"));
}

void People::setSuccessStatus()
{
    m_failure_timer.stop();
    m_before_waiting_timer.stop();
    this->ui.status_icon->clear();
}

void People::waitingStatusAboutToBeStarted()
{
    this->ui.status_icon->clear();
    m_before_waiting_timer.start();
    m_failure_timer.start();
}

void People::setWaitingStatus()
{
    this->ui.status_icon->clear();
    this->ui.status_icon->setMovie(m_waiting_status);
    this->ui.status_icon->movie()->start();
    this->ui.status_icon->setToolTip(tr("Waiting"));
}

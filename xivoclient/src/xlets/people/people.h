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

#ifndef __PEOPLE_H__
#define __PEOPLE_H__

#include <QByteArray>
#include <QObject>
#include <QString>
#include <QTimer>
#include <QVariantMap>
#include <QWidget>

#include <ipbxlistener.h>
#include <xletlib/functests.h>
#include <xletlib/xlet.h>
#include <ui_people_widget.h>

#include "people_enum.h"

class PeopleEntryModel;
class PeopleEntrySortFilterProxyModel;
class QMovie;

class People: public XLet, public IPBXListener
{
    Q_OBJECT
    FUNCTESTED

    private:
        static const int delay_before_waiting = 1*1000;
        static const int delay_before_failure = 30*1000;
        static const int delay_before_lookup = 1000;

    public:
        People(QWidget *parent=0);
        ~People();

    public slots:
        void schedulePeopleLookup(const QString &lookup_pattern);
        void defaultColumnSort(const QModelIndex &, int, int);
        void parseCommand(const QVariantMap &command);

    private slots:
        void deletePersonalContact(const QVariantMap &unique_source_entry_id);
        void purgePersonalContacts();
        void requestEditPersonalContact(const QVariantMap &unique_source_entry_id);
        void requestExportPersonalContacts();
        void setFavoriteStatus(const QVariantMap &unique_source_entry_id);
        void searchMode();
        void searchPeople();
        void favoriteMode();
        void personalContactsMode();
        void setFailureStatus();
        void setWaitingStatus();
        void savePersonalContactsToFile(const QString &file_name);
        void sendPersonalContactsFromFile(const QString &file_name);
        void sendEditPersonalContact(const QString &source_name,
                                     const QString &source_entry_id,
                                     const QVariantMap &contact_infos);
        void sendCreatePersonalContact(const QString &source_name,
                                       const QString &source_entry_id,
                                       const QVariantMap &contact_infos);
        void openNewContactDialog();
        void openImportDialog();
        void openEditContactDialog(const QString &source_name,
                                   const QString &source_entry_id,
                                   const QVariantMap &contact_infos);

    private:
        void parseRelations(const QVariantMap &result) const;
        void parsePeoplePersonalContactDeleted(const QVariantMap &result);
        void parsePeoplePersonalContactRawResult(const QVariantMap &result);
        void parsePeopleExportPersonalContactsCSVResult(const QVariantMap &result);
        void parsePeopleImportPersonalContactsCSVResult(const QVariantMap &result);
        void openExportDialog();
        void setSuccessStatus();
        void updatePersonalContacts();
        void waitingStatusAboutToBeStarted();

        Ui::PeopleWidget ui;
        PeopleEntrySortFilterProxyModel *m_proxy_model;
        PeopleEntryModel *m_model;
        QMovie *m_waiting_status;
        QTimer m_before_waiting_timer;
        QTimer m_failure_timer;
        QTimer m_lookup_timer;
        QString m_searched_pattern;
        PeopleMode m_mode;
        QByteArray m_csv_contacts;

};

#endif /* __PEOPLE_H__ */

/* XiVO Client
 * Copyright (C) 2007-2013, Avencall
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

#ifndef _DIRECTORY_ENTRY_MANAGER_H_
#define _DIRECTORY_ENTRY_MANAGER_H_

#include <QObject>
#include <QString>

#include <ipbxlistener.h>

#include <dao/phonedaoimpl.h>
#include <dao/userdaoimpl.h>

#include <xletlib/directory_entry.h>
#include <xletlib/xletlib_export.h>

class PhoneDAO;
class UserDAO;

class XLETLIB_EXPORT DirectoryEntryManager: public QObject, IPBXListener
{
    Q_OBJECT

    public:
        DirectoryEntryManager(const PhoneDAO &phone_dao,
                              const UserDAO &user_dao,
                              QObject *parent=NULL);
        const DirectoryEntry & getEntry(int entry_index) const;
        int entryCount() const;

    public slots:
        void updatePhone(const QString &phone_xid);
        void removePhone(const QString &phone_xid);

        void updateUser(const QString &user_xid);
        void removeUser(const QString &user_xid);

        void parseCommand(const QVariantMap &command);

    signals:
        void directoryEntryAdded(int entry_index);
        void directoryEntryUpdated(int entry_index);
        void directoryEntryDeleted(int entry_index);

    private:
        template<class T>
        int findEntryBy(const T) const;

        template<class T>
        int hasEntry(const T) const;

        void addEntry(const DirectoryEntry *new_entry);
        void updateEntryAt(int index);
        void removeEntryAt(int index);

        const PhoneDAO &m_phone_dao;
        const UserDAO &m_user_dao;
        QList<const DirectoryEntry *> m_directory_entries;
};

#endif /* _DIRECTORY_ENTRY_MANAGER_H_ */

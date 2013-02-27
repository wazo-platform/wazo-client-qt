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

#ifndef __DIRECTORY_ENTRY_MODEL_H__
#define __DIRECTORY_ENTRY_MODEL_H__

#include <QAbstractTableModel>
#include <QStringList>
#include <ipbxlistener.h>
#include <xletlib/directory_entry.h>

class DirectoryEntryManager;
class DirectoryEntry;

class DirectoryEntryModel : public QAbstractTableModel, IPBXListener
{
    Q_OBJECT

    public:
        DirectoryEntryModel(const DirectoryEntryManager & directory_entry_manager,
                            QObject *parent = NULL);

        int rowCount(const QModelIndex &parent = QModelIndex()) const;
        int columnCount(const QModelIndex &) const;
        int columnCount() const;

        bool removeRows(int, int, const QModelIndex &);

        QVariant data(const QModelIndex&, int role = Qt::DisplayRole) const;
        QVariant headerData(int,
                            Qt::Orientation,
                            int) const;
        int getNumberColumnIndex() const;
        int getNameColumnIndex() const;
    public slots:
        void addDirectoryEntry(int entry_index);
        void updateDirectoryEntry(int entry_index);
        void deleteDirectoryEntry(int entry_index);
        void clearCache();
        void parseCommand(const QVariantMap &command);

    private:
        void refreshEntry(int entry_index);
        QVariant dataDisplay(const DirectoryEntry & entry, int column) const;
        QVariant dataDecoration(const DirectoryEntry & entry, int column) const;
        QVariant dataTooltip(const DirectoryEntry & entry, int column) const;
        QVariant dataSearch(const DirectoryEntry & entry) const;
        void addField(const QString &name, const QString &type);
        QString headerText(int column) const;
        enum ColumnType headerType(int column) const;

        QList< QPair<QString, enum ColumnType> >  m_fields;
        const DirectoryEntryManager & m_directory_entry_manager;
};

#endif

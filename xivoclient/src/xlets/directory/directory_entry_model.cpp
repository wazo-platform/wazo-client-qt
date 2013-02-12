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

#include <QString>
#include <QPixmap>

#include <baseengine.h>
#include <xletlib/directory_entry_manager.h>

#include "directory_entry_model.h"

DirectoryEntryModel::DirectoryEntryModel(const DirectoryEntryManager & directory_entry_manager,
                                         QObject *parent)
    : QAbstractTableModel(parent),
      m_directory_entry_manager(directory_entry_manager)
{
    m_headers[STATUS_ICON] = "";
    m_headers[NAME] = tr("Name");
    m_headers[NUMBER] = tr("Number");

    connect(b_engine, SIGNAL(clearingCache()),
            this, SLOT(clearingCache()));
    connect(&m_directory_entry_manager, SIGNAL(directoryEntryAdded(int)),
            this, SLOT(directoryEntryAdded(int)));
    connect(&m_directory_entry_manager, SIGNAL(directoryEntryUpdated(int)),
            this, SLOT(directoryEntryUpdated(int)));
    connect(&m_directory_entry_manager, SIGNAL(directoryEntryDeleted(int)),
            this, SLOT(directoryEntryDeleted(int)));
}

void DirectoryEntryModel::directoryEntryAdded(int entry_index) {
    int inserted_row = entry_index;
    beginInsertRows(QModelIndex(), inserted_row, inserted_row);
    endInsertRows();
    this->refreshEntry(inserted_row);
}

void DirectoryEntryModel::directoryEntryUpdated(int entry_index) {
    this->refreshEntry(entry_index);
}

void DirectoryEntryModel::directoryEntryDeleted(int entry_index) {
    this->removeRow(entry_index);
}

void DirectoryEntryModel::clearingCache()
{
    this->removeRows(0, this->rowCount(), QModelIndex());
}

void DirectoryEntryModel::refreshEntry(int row_id)
{
    unsigned first_column_index = 0;
    unsigned last_column_index = NB_COL - 1;
    QModelIndex cell_changed_start = createIndex(row_id, first_column_index);
    QModelIndex cell_changed_end = createIndex(row_id, last_column_index);
    emit dataChanged(cell_changed_start, cell_changed_end);
}

int DirectoryEntryModel::rowCount(const QModelIndex&) const
{
    return m_directory_entry_manager.entryCount();
}

int DirectoryEntryModel::columnCount(const QModelIndex&) const
{
    return NB_COL;
}

QVariant DirectoryEntryModel::data(const QModelIndex &index, int role) const
{
    int row = index.row(), column = index.column();
    const DirectoryEntry & entry = m_directory_entry_manager.getEntry(row);

    switch(role) {
    case Qt::DecorationRole:
        return this->dataDecoration(entry, column);
    case Qt::TextAlignmentRole:
        return Qt::AlignCenter;
    case  Qt::DisplayRole:
        return this->dataDisplay(entry, column);
    case Qt::ToolTipRole:
        return this->dataTooltip(entry, column);
    case Qt::UserRole:
        return this->dataSearch(entry);
    default:
        return QVariant();
    }
}

QVariant DirectoryEntryModel::headerData(int column,
                                         Qt::Orientation orientation,
                                         int role) const
{
    if (orientation != Qt::Horizontal) {
        return QVariant();
    }

    switch(role) {
    case  Qt::DisplayRole:
        return this->m_headers[column];
    default:
        return QVariant();
    }
}

QVariant DirectoryEntryModel::dataDisplay(const DirectoryEntry & entry, int column) const
{
    switch (column) {
    case NUMBER:
        return entry.number();
    case NAME:
        return entry.name();
    default :
        return QVariant();
    }
}

QVariant DirectoryEntryModel::dataDecoration(const DirectoryEntry & entry, int column) const
{
    if (column != STATUS_ICON) {
        return QVariant();
    }
    return entry.statusIcon();
}

QVariant DirectoryEntryModel::dataTooltip(const DirectoryEntry & entry, int column) const
{
    if (column != STATUS_ICON) {
        return QVariant();
    }
    return entry.statusText();
}

QVariant DirectoryEntryModel::dataSearch(const DirectoryEntry & entry) const
{
    return entry.searchList();
}

bool DirectoryEntryModel::removeRows(int row, int count, const QModelIndex & index)
{
    if (count > 0) {
        beginRemoveRows(index, row, row + count - 1);
        endRemoveRows();
    }
    return true;
}

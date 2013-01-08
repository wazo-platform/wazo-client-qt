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

#include <QPixmap>
#include <QString>

#include <taintedpixmap.h>
#include "directory_entry_model.h"

DirectoryEntryModel::DirectoryEntryModel(QObject *parent)
    : QAbstractTableModel(parent)
{
    m_headers[STATUS_ICON] = "";
    m_headers[FIRST_NAME] = tr("First name");
    m_headers[LAST_NAME] = tr("Last name");
    m_headers[NUMBER] = tr("Number");

}

int DirectoryEntryModel::rowCount(const QModelIndex&) const
{
    return 1;
}

int DirectoryEntryModel::columnCount(const QModelIndex&) const
{
    return NB_COL;
}

QVariant DirectoryEntryModel::data(const QModelIndex &index, int role) const
{
    int row = index.row(), column = index.column();

    switch(role) {
    case Qt::DecorationRole:
        return this->dataDecoration(row, column);
    case Qt::TextAlignmentRole:
        return Qt::AlignCenter;
    case  Qt::DisplayRole:
        return this->dataDisplay(row, column);
    case Qt::BackgroundRole:
        return this->dataBackground(row, column);
    case Qt::ToolTipRole:
        return this->dataTooltip(row, column);
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

QVariant DirectoryEntryModel::dataDisplay(int row, int column) const
{
    switch (column) {
    case FIRST_NAME:
        return "Alice";
    case LAST_NAME:
        return "Cooper";
    case NUMBER:
        return "666";
    default :
        return QVariant();
    }
}

QVariant DirectoryEntryModel::dataDecoration(int row, int column) const
{
    switch (column) {
    case STATUS_ICON:
        return this->getRedPhone();
    default :
        return QVariant();
    }
}

QPixmap DirectoryEntryModel::getRedPhone() const
{
    return TaintedPixmap(QString(":/images/phone-trans.png"), QColor("red")).getPixmap();
}

QVariant DirectoryEntryModel::dataTooltip(int row, int column) const
{
    switch (column) {
    default :
        return QVariant();
    }
}

QVariant DirectoryEntryModel::dataBackground(int row, int column) const
{
    switch (column) {
    default :
        return QVariant();
    }
}

bool DirectoryEntryModel::removeRows(int row, int count, const QModelIndex & index)
{
    return true;
}

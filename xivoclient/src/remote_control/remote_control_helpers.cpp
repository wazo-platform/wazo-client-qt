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

#ifdef FUNCTESTS

#include "remote_control_helpers.h"

TableWidgetHelper::TableWidgetHelper(const QTableWidget* table)
{
    this->m_table = table;
}

TableWidgetHelper::~TableWidgetHelper()
{

}

bool TableWidgetHelper::hasValue(const QString column, const QString value)
{
    int column_index = this->getColumnIndex(column);
    int nb_rows = this->m_table->rowCount();
    bool found = false;

    for(int count = 0; count < nb_rows && !found; count++) {
        QString item_text = this->m_table->item(count, column_index)->text();
        found = (item_text == value);
    }

    return found;
}

bool TableWidgetHelper::hasLine(const QVariantMap line)
{
    foreach(QString column, line.keys()) {
        QString value = line.value(column).toString();
        if(!this->hasValue(column, value)) {
            return false;
        }
    }

    return true;
}

int TableWidgetHelper::getColumnIndex(const QString column)
{
    int nb_columns = this->m_table->columnCount();
    int index = -1;

    for(int i = 0; i < nb_columns && index < 0; i++) {
        if( this->m_table->horizontalHeaderItem(i)->text() == column) {
            index = i;
        }
    }

    return index;
}

#endif

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

TableWidgetHelper::TableWidgetHelper(QTableWidget* table)
{
    this->m_table = table;
    this->connectSignals();
}

TableWidgetHelper::~TableWidgetHelper()
{

}

void TableWidgetHelper::connectSignals()
{
    connect(this, SIGNAL(itemDoubleClicked(QTableWidgetItem*)),
            this->m_table, SIGNAL(itemDoubleClicked(QTableWidgetItem*)));
}


int TableWidgetHelper::findRow(const QString column, const QString value)
{
    int column_index = this->getColumnIndex(column);
    int nb_rows = this->m_table->rowCount();

    if (column_index < 0) {
        return -1;
    }

    for(int row = 0; row < nb_rows; row++) {
        QString item_text = this->m_table->item(row, column_index)->text();
        if(item_text == value) {
            return row;
        }
    }

    return -1;
}

bool TableWidgetHelper::hasValue(const QString column, const QString value)
{
    return (this->findRow(column, value) >= 0);
}

bool TableWidgetHelper::hasRow(const QVariantMap received_row)
{
    QMap<QString, QString> search_row = toStringMap(received_row);

    int nb_rows = this->m_table->rowCount();

    for(int count = 0; count < nb_rows; count++) {
        QMap<QString, QString> row = rowToMap(count);
        if (row == search_row) {
            return true;
        }
    }

    return false;
}

QMap<QString, QString> TableWidgetHelper::toStringMap(const QVariantMap variant_map)
{
    QMap<QString, QString> converted_map;

    foreach(QString key, variant_map.keys()) {
        QString value = variant_map.value(key).toString();
        converted_map.insert(key, value);
    }

    return converted_map;
}

QMap<QString, QString> TableWidgetHelper::rowToMap(int row)
{
    QMap<QString, QString> map;

    int nb_columns = this->m_table->columnCount();

    for(int column = 0; column < nb_columns; column++) {
        QString key = this->m_table->horizontalHeaderItem(column)->text();
        QString value = this->m_table->item(row, column)->text();
        map.insert(key, value);
    }

    return map;
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

void TableWidgetHelper::doubleClickItem(int row, int column)
{
    QTableWidgetItem* item = this->m_table->item(row, column);
    emit itemDoubleClicked(item);
}

#endif

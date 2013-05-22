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

#include "xlets/remotedirectory-builtin/directorypanel.h"
#include "remote_control.h"


QVariantMap RemoteControl::get_remote_directory_infos()
{
    QVariantMap args;

    DirectoryPanel* xlet = this->get_xlet<DirectoryPanel>("remotedirectory");
    if (xlet == NULL)
        return args;

    int nb_rows = xlet->m_table->rowCount();
    int nb_cols = xlet->m_table->columnCount();

    args["nb_rows"] = nb_rows;

    QVariantList content;
    for (int row = 0; row < nb_rows; row++) {
        QVariantMap header_value;
        for (int col = 0; col < nb_cols; col++) {
            QString key = xlet->m_table->horizontalHeaderItem(col)->text();
            QString value = xlet->m_table->item(row, col)->text();
            header_value[key] = value;
        }
        content.append(header_value);
    }
    args["content"] = content;

    return args;
}

void RemoteControl::set_search_for_remote_directory(const QVariantList & args)
{
    const QString& search = args[0].toString();

    DirectoryPanel* panel = this->get_xlet<DirectoryPanel>("remotedirectory");

    panel->m_searchText->setText(search);
    panel->startSearch();
}

void RemoteControl::exec_double_click_on_number_for_name(const QVariantList &args)
{
    QString name = args[0].toString();

    DirectoryPanel* xlet = this->get_xlet<DirectoryPanel>("remotedirectory");

    connect(this,
            SIGNAL(itemDoubleClicked(QTableWidgetItem*)),
            xlet->m_table,
            SIGNAL(itemDoubleClicked(QTableWidgetItem*)));

    int nb_rows = xlet->m_table->rowCount();
    int nb_cols = xlet->m_table->columnCount();
    for (int row = 0; row < nb_rows; row++) {
        for (int col = 0; col < nb_cols; col++) {
            QString column_name = xlet->m_table->horizontalHeaderItem(col)->text();
            QString value = xlet->m_table->item(row, col)->text();
            if (column_name == "Nom" && name == value) {
                QTableWidgetItem* item = xlet->m_table->item(row, 1);
                emit itemDoubleClicked(item);
                break;
            }
        }
    }
}

#endif

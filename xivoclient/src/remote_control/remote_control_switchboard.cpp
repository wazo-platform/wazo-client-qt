/* XiVO Client
 * Copyright (C) 2013, Avencall
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

#include <xlets/directory/directory.h>

#include "remote_control.h"

void RemoteControl::when_i_search_a_transfer_destination_1(const QVariantList & args)
{
    QString search_string = args[0].toString();

    Directory *xlet = static_cast<Directory*>(m_exec_obj.win->m_xletlist.value("directory"));
    QLineEdit *entry_filter = xlet->findChild<QLineEdit*>(QString("entry_filter"));

    entry_filter->setText(search_string);
    this->pause(1000);
}

void RemoteControl::assert_directory_has_entry(const QVariantList & args)
{
    QString display_name = args[0].toString();
    QString phone_number = args[1].toString();

    Directory *xlet = static_cast<Directory*>(m_exec_obj.win->m_xletlist.value("directory"));
    QAbstractItemModel *model = xlet->ui.entry_table->model();
    bool entry_found = false;
    int row_count = model->rowCount();
    this->assert(row_count > 0, "directory search returned no results");
    for (int row_index = 0; row_index < model->rowCount(); row_index++) {
        if (this->_directory_row_matches(model, row_index, display_name, phone_number)) {
            entry_found = true;
        }
    }
    this->assert(entry_found, QString("Could not find directory entry %1 %2").arg(display_name, phone_number));
}

void RemoteControl::then_i_see_no_transfer_destinations()
{
    Directory *xlet = static_cast<Directory*>(m_exec_obj.win->m_xletlist.value("directory"));
    QAbstractItemModel *model = xlet->ui.entry_table->model();
    int row_count = model->rowCount();
    this->assert(row_count == 0, "Found directory results when none were expected");
}

bool RemoteControl::_directory_row_matches(QAbstractItemModel *model, int row_index, QString display_name, QString phone_number)
{
    QString model_display_name = this->getValueInModel(model, row_index, DirectoryEntryModel::NAME);
    QString model_phone_number = this->getValueInModel(model, row_index, DirectoryEntryModel::NUMBER);

    if (model_display_name != display_name) {
        return false;
    }
    if (model_phone_number != phone_number) {
        return false;
    }
    return true;
}

#endif

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
    QVariantMap entry = args[0].toMap();
    qDebug() << "entry" << entry;

    Directory *xlet = static_cast<Directory*>(m_exec_obj.win->m_xletlist.value("directory"));
    QAbstractItemModel *model = xlet->ui.entry_table->model();
    bool entry_found = false;
    int row_count = model->rowCount();
    this->assert(row_count > 0, "directory search returned no results");
    for (int row_index = 0; row_index < model->rowCount(); row_index++) {
        if (this->_directory_row_matches(model, row_index, entry)) {
            entry_found = true;
        }
    }
    this->assert(entry_found, QString("Could not find directory entry %1").arg(prettyPrintMap(entry)));
}

void RemoteControl::then_i_see_no_transfer_destinations()
{
    Directory *xlet = static_cast<Directory*>(m_exec_obj.win->m_xletlist.value("directory"));
    QAbstractItemModel *model = xlet->ui.entry_table->model();
    int row_count = model->rowCount();
    this->assert(row_count == 0, "Found directory results when none were expected");
}

bool RemoteControl::_directory_row_matches(QAbstractItemModel *model, int row_index, QVariantMap entry)
{
    foreach(const QString &header, entry.keys()) {
        QString entry_value = entry.value(header).toString();

        int header_column = this->findColumnForHeader(model, header);
        this->assert(header_column > 0, QString("header '%1' not found in directory").arg(header));

        QString model_value = this->getValueInModel(model, row_index, header_column);

        if (model_value != entry_value) {
            return false;
        }
    }

    return true;
}

int RemoteControl::findColumnForHeader(QAbstractItemModel *model, QString header)
{
    int nb_cols = model->columnCount();

    for(int col = 0; col < nb_cols; col++) {
        QString value = model->headerData(col, Qt::Horizontal).toString();
        if (value == header) {
            return col;
        }
    }

    return -1;
}

QString RemoteControl::prettyPrintMap(QVariantMap map)
{
    QString prettyprint;
    QDebug debug(&prettyprint);
    debug << map;
    return prettyprint;
}

#endif

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
#include <xletlib/directory_entry.h>

#include "remote_control.h"

QVariantMap RemoteControl::get_switchboard_infos()
{
    QVariantMap args;

    Directory *xlet = this->get_xlet<Directory>("directory");
    QAbstractItemModel *model = xlet->ui.entry_table->model();

    QVariantList content;

    int nb_rows = model->rowCount();
    int nb_cols = model->columnCount();
    for (int row = 0; row < nb_rows; row++) {
        QVariantMap header_value;
        for (int col = 0; col < nb_cols; col++) {
            header_value[getHeaderValueInModel(model, col)] = getValueInModel(model, row, col);
        }
        content.append(header_value);
    }
    args["content"] = content;

    return args;
}

void RemoteControl::set_search_for_directory(const QVariantList & args)
{
    QString search_string = args[0].toString();

    Directory *xlet = this->get_xlet<Directory>("directory");
    QLineEdit *entry_filter = xlet->findChild<QLineEdit*>(QString("entry_filter"));
    if (entry_filter == NULL) {
        qDebug() << "QLineEdit entry_filter is NULL";
    }

    entry_filter->setText(search_string);
}

#endif

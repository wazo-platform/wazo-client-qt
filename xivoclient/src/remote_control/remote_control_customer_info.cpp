/* XiVO Client
 * Copyright (C) 2007-2011, Avencall
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

#include <JsonToVariant.h>

#include "customerinfopanel.h"

#include "remote_control.h"

void RemoteControl::then_i_see_a_sheet_with_variables_and_values(const QVariantList& args)
{
    const QVariantList & variables = args[0].toList();

    CustomerInfoPanel *xlet = static_cast<CustomerInfoPanel*>(m_exec_obj.win->m_xletlist.value("customerinfo"));
    this->assert(xlet != NULL, "xlet null");

    QTabWidget *sheet_tab = xlet->m_tabs;
    this->assert(sheet_tab != NULL, "sheet tab widget null");

    QWidget *current_tab = sheet_tab->currentWidget();
    this->assert(current_tab != NULL, "current sheet tab null");

    int layout_header_offset = 3;

    int widget_index = layout_header_offset;

    foreach (const QVariant &variable, variables) {
        QVariantMap variable_map = variable.toMap();

        const QString &expected_variable_name = variable_map["Variable"].toString();
        const QString &expected_variable_value = variable_map["Value"].toString();

        const QString & name_of_name_label = QString("label_name_%1").arg(widget_index);
        const QString & name_of_value_label = QString("label_value_%1").arg(widget_index);

        QLabel * variable_name_label = current_tab->findChild<QLabel*>(name_of_name_label);
        this->assert(variable_name_label != NULL, QString("can't find label %1").arg(name_of_name_label));
        QLabel * variable_value_label = current_tab->findChild<QLabel*>(name_of_value_label);
        this->assert(variable_value_label != NULL, QString("can't find label %1").arg(name_of_value_label));

        const QString & variable_name = variable_name_label->text();
        const QString & variable_value = variable_value_label->text();

        this->assert(variable_name == expected_variable_name, QString("wrong name %1").arg(variable_name) );
        this->assert(variable_value == expected_variable_value, QString("wrong value %1").arg(variable_value));
        ++widget_index;
    }
}

#endif

/* XiVO Client
 * Copyright (C) 2007-2014 Avencall
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

#include <xlets/customerinfo-builtin/customerinfopanel.h>
#include <xlets/customerinfo-builtin/form_result_extractor.h>

#include "remote_control.h"

QWidget *RemoteControl::_get_current_sheet()
{
    CustomerInfoPanel *xlet = this->get_xlet<CustomerInfoPanel>("customerinfo");
    if (xlet == NULL) {
        throw TestFailedException(QString("xlet customerinfo is NULL"));
    }

    QTabWidget *sheet_tab = xlet->m_tabs;
    if (sheet_tab == NULL) {
        throw TestFailedException(QString("sheet_tab is NULL"));
    }

    QWidget *current_tab = sheet_tab->currentWidget();
    if (current_tab == NULL) {
        throw TestFailedException(QString("current_tab is NULL"));
    }

    return current_tab;
}

QVariantMap RemoteControl::get_sheet_infos()
{
    QVariantMap args;

    QWidget *current_tab = this->_get_current_sheet();
    QVariantMap results;
    bool iter_line_sheet = true;
    int layout_header_offset = 4;
    int i = layout_header_offset;
    while (iter_line_sheet) {
        QString name_of_name_label = QString("label_name_%1").arg(i);
        QString name_of_value_label = QString("label_value_%1").arg(i);

        QLabel *variable_name_label = current_tab->findChild<QLabel*>(name_of_name_label);
        QLabel *variable_value_label = current_tab->findChild<QLabel*>(name_of_value_label);

        if (variable_name_label == NULL || variable_value_label == NULL) {
            iter_line_sheet = false;
            continue;
        }

        results[variable_name_label->text()] = variable_value_label->text();
        i++;
    }
    args["content"] = results;

    return args;
}

QVariantMap RemoteControl::get_infos_in_custom_sheet()
{

    QWidget *sheet = this->_get_current_sheet();
    QVariantMap result = FormResultExtractor::extract_form_result(sheet);
    return result;
}

#endif

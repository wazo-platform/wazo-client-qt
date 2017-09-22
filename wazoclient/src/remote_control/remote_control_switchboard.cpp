/*
 * Copyright 2013-2017 The Wazo Authors  (see the AUTHORS file)
 *
 * This program is free software: you can redistribute it and/or modify
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
 * This programm is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifdef FUNCTESTS

#include <xlets/directory/directory.h>
#include <xlets/switchboard/switchboard.h>
#include <xlets/switchboard/current_call.h>
#include <xletlib/directory_entry.h>
#include <xletlib/queue_entries/queue_entries_model.h>

#include "ui_current_call.h"
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

void RemoteControl::switchboard_answer_incoming_call(const QVariantList & args)
{
    QString cid_name = args[0].toString();
    QString cid_num = args[1].toString();

    Switchboard *xlet = this->get_xlet<Switchboard>("switchboard");
    QueueEntriesView * incoming = xlet->ui.incomingCallsView;
    this->assert(incoming != NULL, "widget for incoming calls is null");

    int row_found = this->findRowWithItem(incoming->model(), QueueEntriesModel::NAME, cid_name);
    this->assert(row_found > -1, "no row with cid_name " + cid_name);

    incoming->selectRow(row_found);
    this->pressEnter(incoming);
}

void RemoteControl::switchboard_hang_up()
{
    Switchboard *xlet = this->get_xlet<Switchboard>("switchboard");
    this->assert(xlet != NULL, "xlet switchboard is null");
    xlet->m_current_call->m_current_call_widget->btn_hangup->click();
}

QVariantMap RemoteControl::get_switchboard_current_call_infos()
{
    QVariantMap result;

    Switchboard *xlet = this->get_xlet<Switchboard>("switchboard");
    this->assert(xlet != NULL, "xlet switchboard is null");
    QString caller_id = xlet->m_current_call->m_caller_id;
    result["caller_id"] = caller_id;

    return result;
}

QVariantMap RemoteControl::get_switchboard_incoming_calls_infos()
{
    QVariantMap result;

    Switchboard *xlet = this->get_xlet<Switchboard>("switchboard");
    QueueEntriesView * incoming = xlet->ui.incomingCallsView;
    this->assert(incoming != NULL, "widget for incoming calls is null");

    QAbstractItemModel *model = incoming->model();

    QVariantList list;

    for (int row = 0; row < model->rowCount(); row++) {
        QVariantMap incoming;
        incoming["position"] = this->getValueInModel(model, row, QueueEntriesModel::POSITION);
        incoming["cid_name"] = this->getValueInModel(model, row, QueueEntriesModel::NAME);
        incoming["cid_num"] = this->getValueInModel(model, row, QueueEntriesModel::NUMBER);
        incoming["time"] = this->getValueInModel(model, row, QueueEntriesModel::TIME);
        incoming["unique_id"] = this->getValueInModel(model, row, QueueEntriesModel::UNIQUE_ID);
        list.append(incoming);
    }

    result["incoming_calls"] = list;
    return result;
}

#endif

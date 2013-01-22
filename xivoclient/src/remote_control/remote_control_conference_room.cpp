
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

#include <QModelIndex>
#include <QVariant>

#include "xlets/conference/conference.h"
#include "xlets/conference/conflist.h"
#include "remote_control.h"

void RemoteControl::assert_conference_room_1_has_number_2_in_xlet(const QVariantList & args)
{
    QString roomName = args[0].toString();
    QString roomNumber = args[1].toString();
    assertValueInConferenceXlet(roomName, ConfListModel::NUMBER, roomNumber);
}

void RemoteControl::assert_conference_room_1_has_pin_code_2_in_xlet(const QVariantList & args)
{
    QString roomName = args[0].toString();
    QString pinCode = args[1].toString();
    assertValueInConferenceXlet(roomName, ConfListModel::PIN_REQUIRED, pinCode);
}

void RemoteControl::assertValueInConferenceXlet(QString roomName, int column, QString value)
{
    qDebug() << "searching for conference room name" << roomName  << "with value" << value;
    QAbstractItemModel* conflist_model = getConflistModel();

    int row = findRowWithItem(conflist_model, ConfListModel::NAME, roomName);
    qDebug() << "row" << row;

    if (row == -1) {
        this->assert(false, QString("conference room with name %1 not found").arg(roomName));
    }

    QString result = getValueInModel(conflist_model, row, column);
    qDebug() << "result" << result;

    this->assert(result == value);
}

QAbstractItemModel* RemoteControl::getConflistModel()
{
    XletConference* conference_xlet = static_cast<XletConference*>(m_exec_obj.win->m_xletlist.value("conference"));

    if (conference_xlet == NULL) {
        this->assert(false, "conference xlet was not found");
    }

    QAbstractItemModel* conflist_model = conference_xlet->findChild<QAbstractItemModel*>("conflist_model");

    if (conflist_model == NULL) {
        this->assert(false, "Conflist model was not found");
    }

    return conflist_model;
}

int RemoteControl::findRowWithItem(QAbstractItemModel* model, int column, QString search) 
{
    int maxRows = model->rowCount(QModelIndex());
    int rowSearch = -1;

    for (int row = 0; row < maxRows && rowSearch < 0; ++row) {

        QString result = getValueInModel(model, row, column);
        if (result == search) {
            rowSearch = row;
        }

    }

    return rowSearch;
}

QString RemoteControl::getValueInModel(QAbstractItemModel* model, int row, int column)
{
    QModelIndex index = model->index(row, column);
    QVariant data = model->data(index, Qt::DisplayRole);
    return data.toString();
}

#endif

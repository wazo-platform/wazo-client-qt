/* XiVO Client
 * Copyright (C) 2007-2015 Avencall
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
#include "xlets/conference/conference_enum.h"
#include "xlets/conference/conference_list_model.h"
#include "remote_control.h"

QVariantMap RemoteControl::get_conference_room_infos()
{
    QVariantMap args;
    Conference* conference_xlet = this->get_xlet<Conference>("conference");
    QAbstractItemModel* conflist_model = conference_xlet->findChild<QAbstractItemModel*>("conference_list_model");

    args["conference_xlet"] = conference_xlet != NULL;
    args["conflist_model"] = conflist_model != NULL;

    QVariantList content;
    QVariantMap header_data;
    header_data["name"] = ConferenceList::COL_NAME;
    header_data["number"] = ConferenceList::COL_NUMBER;
    header_data["pin_required"] = ConferenceList::COL_PIN_REQUIRED;
    header_data["member_count"] = ConferenceList::COL_MEMBER_COUNT;
    header_data["started_since"] = ConferenceList::COL_STARTED_SINCE;

    int nb_rows = conflist_model->rowCount(QModelIndex());
    for (int row = 0; row < nb_rows; row++) {
        QVariantMap header_value;
        QVariantMap::const_iterator i;
        for(i = header_data.begin(); i != header_data.end(); ++i) {
            header_value[i.key()] = getValueInModel(conflist_model, row, i.value().toInt());
        }
        content.append(header_value);
    }
    args["content"] = content;

    return args;
}

#endif

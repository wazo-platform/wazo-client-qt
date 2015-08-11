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

#ifndef __MESSAGE_FACTORY_H__
#define __MESSAGE_FACTORY_H__

#include "baselib_export.h"

#include <QByteArray>
#include <QString>
#include <QVariant>
#include <QVariantMap>

class BASELIB_EXPORT MessageFactory
{
    public:
        MessageFactory();
        ~MessageFactory();
        static QVariantMap subscribeCurrentCalls();
        static QVariantMap answer(const QString &unique_id);
        static QVariantMap callFormResult(const QVariant &info);
        static QVariantMap dial(const QString &number);
        static QVariantMap hangup();
        static QVariantMap holdSwitchboard(const QString &queue_name);
        static QVariantMap resumeSwitchboard(const QString &unique_id);
        static QVariantMap attendedTransfer(const QString &number);
        static QVariantMap directTransfer(const QString &number);
        static QVariantMap completeTransfer();
        static QVariantMap cancelTransfer();
        static QVariantMap directorySearch(const QString &pattern);
        static QVariantMap switchboardDirectorySearch(const QString &pattern);
        static QVariantMap peopleSearch(const QString &pattern);
        static QVariantMap registerAgentStatus(const QVariantList &ids);
        static QVariantMap registerEndpointStatus(const QVariantList &ids);
        static QVariantMap registerUserStatus(const QVariantList &ids);
        static QVariantMap getSwitchboardDirectoryHeaders();
        static QVariantMap getPeopleHeaders();
        static QVariantMap pauseAgentInQueue(const QString &agent_id, const QString &queue_id);
        static QVariantMap unpauseAgentInQueue(const QString &agent_id, const QString &queue_id);
        static QVariantMap pauseAgentInAllQueues(const QString &agent_id, const QString &ipbxid);
        static QVariantMap unpauseAgentInAllQueues(const QString &agent_id, const QString &ipbxid);
        static QVariantMap setUnconditionalForward(bool enabled, QString destination = "");
        static QVariantMap setForwardBusy(bool enabled, QString destination = "");
        static QVariantMap setForwardNoAnswer(bool enabled, QString destination = "");
        static QVariantMap setVoicemail(bool enabled);
        static QVariantMap setCallFiltering(bool enabled);
        static QVariantMap setDoNotDisturb(bool enabled);

        static QVariantMap setFavoriteStatus(const QString &source_name, const QString &source_entry_id, bool enabled);
        static QVariantMap favorites();

        static QVariantMap personalContacts();
        static QVariantMap personalContactRaw(const QString &source_name, const QString &source_entry_id);
        static QVariantMap createPersonalContact(const QVariantMap &contact_infos);
        static QVariantMap deletePersonalContact(const QString &source_name, const QString &source_entry_id);
        static QVariantMap editPersonalContact(const QString &source_name,
                                               const QString &source_entry_id,
                                               const QVariantMap &contact_infos);
        static QVariantMap exportPersonalContactsCSV();
        static QVariantMap importPersonalContactsCSV(const QByteArray &csv_contacts);
    private:
        static QVariantMap baseMessage(const QString &class_name);
        static QVariantMap ipbxcommand(const QString &action_name);
};

#endif /* __MESSAGE_FACTORY_H__ */

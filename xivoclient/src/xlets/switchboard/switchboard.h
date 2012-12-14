/* XiVO Client
 * Copyright (C) 2012, Avencall
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

#ifndef __SWITCHBOARD_H__
#define __SWITCHBOARD_H__

#include <QObject>
#include "xlet.h"
#include "ui_switchboard.h"

class QLabel;
class QueueEntriesModel;
class QueueEntriesSortFilterProxyModel;
class UserInfo;
class CurrentCall;

class Switchboard : public XLet
{
    Q_OBJECT

    public:
        Switchboard(QWidget *parent=0);
        ~Switchboard();
    public slots:
        void updateIncomingHeader(const QString &id, const QVariantList &entries);
        void updateWaitingHeader(const QString &id, const QVariantList &entries);
        void incomingCallClicked(const QModelIndex &index);
        void waitingCallClicked(const QModelIndex &index);
        void keyPressEvent(QKeyEvent *event);
        void queueEntryUpdate(const QString &queue_id, const QVariantList &entry);
        void updatePhoneStatus(const QString &queue_id);
        void postInitializationSetup();
    private:
        void answerIncomingCall() const;
        void retrieveCallOnHold(const QString & call_unique_id) const;
        void focusOnIncomingCalls();
        void focusOnWaitingCalls();
        void handleEnterKeys();
        void incomingCallsUpdated(const QModelIndex &, const QModelIndex &);
        void watch_switchboard_queue();
        void connectPhoneStatus() const;
        void setupUi();
        void subscribeCurrentCalls() const;
        bool isSwitchboardQueue(const QString &queue_id) const;
        bool isSwitchboardHoldQueue(const QString &queue_id) const;
        bool isSwitchboardPhone(const QString &phone_id);

        Ui::SwitchboardPanel ui;
        CurrentCall *m_current_call;

        QueueEntriesModel *m_incoming_call_model;
        QueueEntriesSortFilterProxyModel *m_incoming_call_proxy_model;
        static QString switchboard_queue_name;

        QueueEntriesModel *m_waiting_call_model;
        QueueEntriesSortFilterProxyModel *m_waiting_call_proxy_model;
        static QString switchboard_hold_queue_name;

        const UserInfo *m_switchboard_user;
};

#endif

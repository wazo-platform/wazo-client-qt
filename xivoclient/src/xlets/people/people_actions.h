/* Copyright (C) 2015 Avencall
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


#ifndef __PEOPLE_ACTIONS_H__
#define __PEOPLE_ACTIONS_H__

#include <QAction>
#include <QList>
#include <QObject>
#include <QString>
#include <QVariant>

#include "people_entry.h"
#include "people_entry_model.h"

class PeopleActions: public QObject
{
    Q_OBJECT

    public:
        PeopleActions(const QList<QVariant> &action_items);
        QAction *getCallAction() const;
        const QList<QAction *> &getCallCallableActions() const;
        const QList<QAction *> &getAttendedTransferActions() const;
        const QList<QAction *> &getBlindTransferActions() const;

    public slots:
        void call();
        void attendedTransfer();
        void blindTransfer();

    private:
        QAction *newCallAction(const QString &label, const QString &number);
        QAction *newBlindTransferAction(const QString &label, const QString &number);
        QAction *newAttendedTransferAction(const QString &label, const QString &number);

        QList<QAction *> m_call_callable_actions;
        QList<QAction *> m_blind_transfer_actions;
        QList<QAction *> m_attended_transfer_actions;
        QAction *m_call_action;
};

#endif

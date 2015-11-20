/* Copyright (C) 2015 Avencall
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

#ifndef __PEOPLE_ACTION_GENERATOR_H__
#define __PEOPLE_ACTION_GENERATOR_H__

#include <QAction>
#include <QObject>
#include <QList>

#include <ipbxlistener.h>

#include "people_entry_model.h"
#include "people_enum.h"

class PeopleEntryView;

typedef QPair<QString, QString> QStringPair;

class AttendedTransferAction: public QAction
{
    Q_OBJECT

    public:
        AttendedTransferAction(const QString &title, const QString &number, QWidget *parent);
    public slots:
        void transfer();
    private:
        QString m_number;
};

class BlindTransferAction: public QAction
{
    Q_OBJECT

    public:
        BlindTransferAction(const QString &title, const QString &number, QWidget *parent);
    public slots:
        void transfer();
    private:
        QString m_number;
};

class CallAction: public QAction
{
    Q_OBJECT

    public:
        CallAction(const QString &number, QWidget *parent);
        CallAction(const QString &text, const QString &number, QWidget *parent);
    public slots:
        void call();
    private:
        QString m_number;
};

class ChatAction: public QAction
{
    Q_OBJECT

    public:
        ChatAction(const QString &name, const QString &xivo_uuid, int user_id, QWidget *parent);
    public slots:
        void chat();
    private:
        QString m_name;
        QString m_xivo_uuid;
        int m_user_id;
};

class MailToAction: public QAction
{
    Q_OBJECT

    public:
        MailToAction(const QString &title, const QString &email, QWidget *parent);
    public slots:
        void mailto();
    private:
        QString m_email;
};

class PeopleActionGenerator: public QObject, IPBXListener
{
    Q_OBJECT

    public:
        PeopleActionGenerator(PeopleEntryModel *model, PeopleEntryView *parent);
        virtual ~PeopleActionGenerator();

        QAction *newCallAction(const QModelIndex &index);
        QAction *newChatAction(const QModelIndex &index);
        QList<QAction *> newCallCallableActions(const QModelIndex &index);
        QList<QAction *> newAttendedTransferActions(const QModelIndex &index);
        QList<QAction *> newBlindTransferActions(const QModelIndex &index);
        QList<QAction *> newMailtoActions(const QModelIndex &index);

        bool hasCallCallables(const QModelIndex &index);
        bool hasChat(const QModelIndex &index);
        bool hasTransfers(const QModelIndex &index);
        bool hasMail(const QModelIndex &index);

    public slots:
        void parseCommand(const QVariantMap &command);

    private:
        PeopleActionGenerator() {};
        PeopleEntryModel *model();
        int findColumnOfType(ColumnType type);
        QList<int> findAllColumnOfType(ColumnType type);
        QList<int> columnTypes();
        QVariant dataAt(const QModelIndex &index, int column);
        QVariant headerAt(int column);

        QList<QStringPair> titleValues(ColumnType type, const QModelIndex &index);
        QList<QStringPair> allTitleNumber(const QModelIndex &index);
        QList<QStringPair> allTitleEmail(const QModelIndex &index);
        QList<QStringPair> callableTitleNumber(const QModelIndex &index);

        QWidget *parent();
        bool canTransfer() const;
        bool isConnected(const QModelIndex &index);
        bool isSelf(const QModelIndex &index);
        bool sameXivo(const QModelIndex &index);

        template<typename T>
        QList<QAction*> actionsFromList(QList<QStringPair> pairs) {
            QList<QAction*> actions;
            foreach (const QStringPair pair, pairs) {
                actions.append(new T(pair.first, pair.second, parent()));
            }
            return actions;
        }

        PeopleEntryModel *m_people_entry_model;

        RelationID m_user_id;
        RelationID m_endpoint_id;
        int m_endpoint_status;
};

QString formatColumnNumber(const QString &title, const QString &number);

#endif

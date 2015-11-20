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
        virtual ~CallAction() {};
    public slots:
        void call();
    private:
        QString m_number;
};

class PeopleActionGenerator: public QObject, IPBXListener
{
    Q_OBJECT

    public:
        PeopleActionGenerator(PeopleEntryModel *model, PeopleEntryView *parent);
        virtual ~PeopleActionGenerator();

        QAction *newCallAction(const QModelIndex &index);
        QAction *newChatAction();
        QList<QAction *> newCallCallableActions(const QModelIndex &index);
        QList<QAction *> newAttendedTransferActions(const QModelIndex &index);
        QList<QAction *> newBlindTransferActions(const QModelIndex &index);

        bool hasCallCallables(const QModelIndex &index);
        bool hasChat();
        bool hasTransfers();

    public slots:
        void chat() {};
        void parseCommand(const QVariantMap &command);

    private:
        PeopleActionGenerator() {};
        PeopleEntryModel *model();
        int findColumnOfType(ColumnType type);
        QList<int> findAllColumnOfType(ColumnType type);
        QList<int> columnTypes();
        QVariant dataAt(const QModelIndex &index, int column);
        QVariant headerAt(int column);
        QList<QStringPair> allTitleNumber(const QModelIndex &index);
        QList<QStringPair> callableTitleNumber(const QModelIndex &index);
        QWidget *parent();

        PeopleEntryModel *m_people_entry_model;

        RelationID m_endpoint_id;
        int m_endpoint_status;
};

QString formatColumnNumber(const QString &title, const QString &number);

#endif

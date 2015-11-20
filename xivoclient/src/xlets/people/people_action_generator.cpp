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

#include <QDebug>
#include <baseengine.h>
#include <message_factory.h>

#include "people_entry_view.h"
#include "people_action_generator.h"

#define IN_USE 1

PeopleActionGenerator::PeopleActionGenerator(PeopleEntryModel *model, PeopleEntryView *parent)
    : QObject(parent),
      m_people_entry_model(model)
{
    this->registerListener("relations");
    this->registerListener("endpoint_status_update");
}

PeopleActionGenerator::~PeopleActionGenerator()
{
    qDebug() << Q_FUNC_INFO;
}

QWidget *PeopleActionGenerator::parent()
{
    return reinterpret_cast<QWidget*>(QObject::parent());
}

PeopleEntryModel *PeopleActionGenerator::model()
{
    return m_people_entry_model;
}

void PeopleActionGenerator::parseCommand(const QVariantMap &command)
{
    const QString &event = command["class"].toString();
    const QVariantMap &data = command["data"].toMap();

    RelationID id(data["xivo_uuid"].toString(), data["endpoint_id"].toInt());
    if (event == "relations") {
        m_endpoint_id = id;
    } else if (id == m_endpoint_id) {
        m_endpoint_status = data["status"].toInt();
    }
}

QList<int> PeopleActionGenerator::columnTypes()
{
    QList<int> types;
    for (int i = 0; i < model()->columnCount(); ++i) {
        types.append(model()->headerData(i, Qt::Horizontal, Qt::UserRole).toInt());
    }
    return types;
}

int PeopleActionGenerator::findColumnOfType(ColumnType type)
{
    return columnTypes().indexOf(type);
}

QList<int> PeopleActionGenerator::findAllColumnOfType(ColumnType type)
{
    QList<int> indices;
    QList<int> types = columnTypes();

    for (int i = 0; i < types.size(); ++i) {
        if (types[i] == type) {
            indices.append(i);
        }
    }

    return indices;
}

QVariant PeopleActionGenerator::headerAt(int column)
{
    return model()->headerData(column, Qt::Horizontal, Qt::DisplayRole);
}

QVariant PeopleActionGenerator::dataAt(const QModelIndex &index, int column)
{
    QModelIndex cell = index.child(index.row(), column);
    return model()->data(cell, Qt::DisplayRole);
}

QList<QAction*> PeopleActionGenerator::newBlindTransferActions(const QModelIndex &index)
{
    QList<QAction*> actions;
    if (m_endpoint_status != IN_USE) {
        return actions;
    }

    foreach (const QStringPair &pair, allTitleNumber(index)) {
        actions.append(new BlindTransferAction(pair.first, pair.second, parent()));
    }

    return actions;
}

QList<QAction*> PeopleActionGenerator::newAttendedTransferActions(const QModelIndex &index)
{
    QList<QAction*> actions;
    if (m_endpoint_status != IN_USE) {
        return actions;
    }

    foreach (const QStringPair &pair, allTitleNumber(index)) {
        actions.append(new AttendedTransferAction(pair.first, pair.second, parent()));
    }

    return actions;
}

QAction *PeopleActionGenerator::newCallAction(const QModelIndex &index)
{
    if (findColumnOfType(NUMBER) == -1) {
        return NULL;
    }
    const QString &number = dataAt(index, findColumnOfType(NUMBER)).toString();
    if (number.isEmpty()) {
        return NULL;
    }

    return new CallAction(number, parent());
}

QList<QAction *> PeopleActionGenerator::newCallCallableActions(const QModelIndex &index)
{
    QList<QAction*> actions;
    foreach (const QStringPair &pair, callableTitleNumber(index)) {
        actions.append(new CallAction(formatColumnNumber(pair.first, pair.second), pair.second, parent()));
    }
    return actions;
}

QList<QStringPair> PeopleActionGenerator::allTitleNumber(const QModelIndex &index)
{
    const QString &main_number = dataAt(index, findColumnOfType(NUMBER)).toString();
    const QString &main_title = headerAt(findColumnOfType(NUMBER)).toString();
    QList<QStringPair> pairs = callableTitleNumber(index);
    if (main_number.isEmpty() == false) {
        pairs.prepend(QStringPair(main_title, main_number));
    }
    return pairs;
}

QList<QStringPair> PeopleActionGenerator::callableTitleNumber(const QModelIndex &index)
{
    QList<QStringPair> pairs;
    foreach (int column, findAllColumnOfType(CALLABLE)) {
        const QString &number = dataAt(index, column).toString();
        const QString &header = headerAt(column).toString();
        if (number.isEmpty()) {
            continue;
        }
        pairs.append(QStringPair(header, number));
    }
    return pairs;
}

bool PeopleActionGenerator::hasCallCallables(const QModelIndex &index)
{
    return !callableTitleNumber(index).isEmpty();
}

CallAction::CallAction(const QString &number, QWidget *parent)
    : QAction(tr("Call"), parent),
      m_number(number)
{
    connect(this, SIGNAL(triggered()), this, SLOT(call()));
}

CallAction::CallAction(const QString &text, const QString &number, QWidget *parent)
    : QAction(text, parent),
      m_number(number)
{
    connect(this, SIGNAL(triggered()), this, SLOT(call()));
}

void CallAction::call()
{
    b_engine->sendJsonCommand(MessageFactory::dial(m_number));
}

BlindTransferAction::BlindTransferAction(const QString &title, const QString &number, QWidget *parent)
    : QAction(formatColumnNumber(title, number), parent),
      m_number(number)
{
    connect(this, SIGNAL(triggered()), this, SLOT(transfer()));
}

void BlindTransferAction::transfer()
{
    b_engine->sendJsonCommand(MessageFactory::directTransfer(m_number));
}

AttendedTransferAction::AttendedTransferAction(const QString &title, const QString &number, QWidget *parent)
    : QAction(formatColumnNumber(title, number), parent),
      m_number(number)
{
    connect(this, SIGNAL(triggered()), this, SLOT(transfer()));
}

void AttendedTransferAction::transfer()
{
    b_engine->sendJsonCommand(MessageFactory::attendedTransfer(m_number));
}

QString formatColumnNumber(const QString &title, const QString &number)
{
    return QString("%1 - %2").arg(title).arg(number);
}

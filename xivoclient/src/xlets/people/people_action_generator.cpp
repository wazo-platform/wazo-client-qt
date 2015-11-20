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

#include "people_entry_model.h"
#include "people_entry_view.h"
#include "people_action_generator.h"

PeopleActionGenerator::PeopleActionGenerator(PeopleEntryModel *model, PeopleEntryView *parent)
    : QObject(parent),
      m_people_entry_model(model)
{
    qDebug() << Q_FUNC_INFO;
    m_number_column_index = this->findColumnOfType(NUMBER);
    m_callable_column_indices = this->findAllColumnOfType(CALLABLE);
}

PeopleActionGenerator::~PeopleActionGenerator()
{
    qDebug() << Q_FUNC_INFO;
}

QWidget *PeopleActionGenerator::parent()
{
    return reinterpret_cast<QWidget*>(QObject::parent());
}

PeopleEntryModel *PeopleActionGenerator::model() const
{
    return m_people_entry_model;
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

QString PeopleActionGenerator::formatColumnNumber(const QString &title, const QString &number) const
{
    return QString("%1 - %2").arg(title).arg(number);
}

QAction *PeopleActionGenerator::newCallAction(const QModelIndex &index)
{
    if (m_number_column_index == -1) {
        return NULL;
    }
    const QString &number = dataAt(index, m_number_column_index).toString();
    if (number.isEmpty()) {
        return NULL;
    }

    return new CallAction(number, parent());
}

QList<QStringPair> PeopleActionGenerator::callableTitleNumber(const QModelIndex &index)
{
    QList<QStringPair> pairs;
    foreach (int column, m_callable_column_indices) {
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

QList<QAction *> PeopleActionGenerator::newCallCallableActions(const QModelIndex &index)
{
    QList<QAction*> actions;
    foreach (QStringPair pair, callableTitleNumber(index)) {
        actions.append(new CallAction(formatColumnNumber(pair.first, pair.second), pair.second, parent()));
    }
    return actions;
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

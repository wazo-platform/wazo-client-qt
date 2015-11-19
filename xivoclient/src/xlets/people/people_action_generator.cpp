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

#include <QAction>
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
}

PeopleActionGenerator::~PeopleActionGenerator()
{
    qDebug() << Q_FUNC_INFO;
}

PeopleEntryModel *PeopleActionGenerator::model() const
{
    return m_people_entry_model;
}

int PeopleActionGenerator::findColumnOfType(ColumnType type)
{
    for (int i = 0; i < model()->columnCount(); ++i) {
        if (model()->headerData(i, Qt::Horizontal, Qt::UserRole).toInt() == type) {
            return i;
        }
    }
    return -1;
}

QAction *PeopleActionGenerator::newCallAction(const QModelIndex &index)
{
    QModelIndex cell = index.child(index.row(), m_number_column_index);
    const QString &number = model()->data(cell, Qt::DisplayRole).toString();
    if (number.isEmpty()) {
        return NULL;
    }

    QAction *action = new QAction(tr("Call"), parent());
    action->setData(number);
    connect(action, SIGNAL(triggered()), this, SLOT(call()));
    return action;
}

void PeopleActionGenerator::call()
{
    const QString &number = static_cast<QAction*>(sender())->data().toString();

    b_engine->sendJsonCommand(MessageFactory::dial(number));
}

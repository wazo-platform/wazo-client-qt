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

#include "ColumnFilteringProxyModel.h"

#include <QMenu>
#include <QContextMenuEvent>

ColumnFilteringProxyModel::ColumnFilteringProxyModel(QWidget *parent)
  :QSortFilterProxyModel(parent)
{
}

ColumnFilteringProxyModel::~ColumnFilteringProxyModel()
{
}

bool ColumnFilteringProxyModel::filterAcceptsColumn(int source_column, const QModelIndex &) const
{
    const QString &column_name = this->columnName(source_column);
    return ! this->isHidden(column_name);
}

void ColumnFilteringProxyModel::columnHeaderRightClicked(QContextMenuEvent *e)
{
    QStringList headers = this->getHeaders();
    QWidget *parentWidget = static_cast<QWidget *>(this->parent());
    if (! parentWidget) {
        e->ignore();
        return;
    }
    QMenu *menu = new QMenu(parentWidget);
    if (! menu) {
        e->ignore();
        return;
    }

    foreach (const QString &header, headers) {
        QAction *action = new QAction(header, parentWidget);
        if (! action) {
            continue;
        }
        action->setProperty("header", header);
        action->setCheckable(true);
        action->setChecked(! this->isHidden(header));
        menu->addAction(action);
        connect(action, SIGNAL(triggered()), this, SLOT(handleColumnClicked()));
    }

    menu->popup(e->globalPos());

    e->accept();
}

QStringList ColumnFilteringProxyModel::getHeaders()
{
    QAbstractItemModel *model = this->sourceModel();
    QStringList headers;
    int column_count = model != NULL ? model->columnCount() : 0;

    for (int i = 0; i < column_count; ++i) {
        headers << this->columnName(i);
    }

    return headers;
}

void ColumnFilteringProxyModel::handleColumnClicked()
{
    QObject *sender = this->sender();
    if (! sender) {
        return;
    }

    const QString &column_name = sender->property("header").toString();
    if (this->isHidden(column_name)) {
        m_hidden_column.removeOne(column_name);
    } else {
        m_hidden_column << column_name;
    }
    this->invalidateFilter();
}

bool ColumnFilteringProxyModel::isHidden(const QString &column_name) const
{
    return m_hidden_column.contains(column_name);
}

QString ColumnFilteringProxyModel::columnName(int column) const
{
    QAbstractItemModel *model = this->sourceModel();
    if (! model) {
        return NULL;
    }

    return model->headerData(column, Qt::Horizontal, Qt::DisplayRole).toString();
}

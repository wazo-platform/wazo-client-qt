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

#include <QHeaderView>
#include <QStyleFactory>

#include "abstract_table_view.h"

AbstractTableView::AbstractTableView(QWidget * parent)
    : QTableView(parent)

{
    this->setSortingEnabled(true);
    this->setShowGrid(0);

    this->horizontalHeader()->setCascadingSectionResizes(true);
    this->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    this->horizontalHeader()->setFixedHeight(30);
    this->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    this->horizontalHeader()->setSectionsMovable(true);

    this->verticalHeader()->setDefaultSectionSize(50);
    this->verticalHeader()->hide();

    this->setSelectionMode(QAbstractItemView::NoSelection);

    QStyle *plastique = QStyleFactory::create("cleanlooks");
    this->horizontalHeader()->setStyle(plastique);
    this->verticalHeader()->setStyle(plastique);
}

AbstractTableView::~AbstractTableView()
{
}

QSize AbstractTableView::sizeHint() const
{
    int width = this->horizontalHeader()->length();
    int height = this->verticalHeader()->length();
    return QSize(width, height);
}

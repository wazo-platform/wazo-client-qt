/* XiVO Client
 * Copyright (C) 2007-2011, Proformatique
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

/* $Revision$
 * $Date$
 */

#include <QDebug>
#include <QLabel>
#include <QHBoxLayout>
#include <QTableWidget>

#include "popcaastra.h"
#include "../ui_popcaastra.h"
#include "userinfo.h"

#define DEST_NB_COL 6

/*! \brief Constructor
 *
 * In this example, we just set a simple layout for
 * the widget and add a label displaying "Hello world !" in it.
 */
PopcAastra::PopcAastra(QWidget *parent)
    : XLet(parent), ui(new Ui::PopcAastra)
{
    ui->setupUi(this);
    // set title for the XLet.
    setTitle(tr("POPC Aastra operator"));
    connect(ui->destinationGrid, SIGNAL(cellDoubleClicked(int, int)),
            this, SLOT(destinationClicked(int, int)));
    connect(b_engine, SIGNAL(peersReceived()), this, SLOT(refreshDestination()));
}

void PopcAastra::destinationClicked(int col, int row)
{
    qDebug() << "Cell clicked " << col << "x" << row;
    this->refreshDestination();
}

void PopcAastra::refreshDestination()
{
    QHash<QString, UserInfo*> users = b_engine->users();
    int nb_users = users.size();
    qDebug() << "Nb users = " << nb_users;
    if (nb_users == 0) return;
    int cols = DEST_NB_COL > nb_users ? nb_users : DEST_NB_COL;
    int rows = nb_users / cols;
    if (nb_users % cols) rows++;
    QTableWidget* grid = ui->destinationGrid;
    grid->setColumnCount(cols);
    grid->setRowCount(rows);
    int gridWidth = grid->width();
    int cellWidth = gridWidth / cols;
    for (int i = 0; i < cols; ++i)
        grid->setColumnWidth(i, cellWidth);
    grid->clear();
    QHash<QString, UserInfo*>::const_iterator i = users.constBegin();
    int j = 0;
    while (i != users.constEnd()) {
        qDebug() << "Fullname: " << name;
        grid->setItem(j / cols, j % cols, new QTableWidgetItem(
            QString("%1: %2").arg(j < 26 ? (char)('a' + j) : ' ').arg(i.value()->fullname())));
        ++i;
        ++j;
    }
}

/*! \brief destructor
 */
PopcAastra::~PopcAastra()
{
    qDebug() << Q_FUNC_INFO;
    delete ui;
}


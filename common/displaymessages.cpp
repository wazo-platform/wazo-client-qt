/* XiVO Client
 * Copyright (C) 2007-2010, Proformatique
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

#include <QDateTime>
#include <QDebug>
#include <QHeaderView>
#include <QLabel>
#include <QTableWidget>
#include <QVBoxLayout>

#include "displaymessages.h"
#include "baseengine.h"

/*! \brief Constructor
 *
 * Set up the layout and the table widget.
 */
DisplayMessagesPanel::DisplayMessagesPanel(BaseEngine * engine, QWidget * parent)
    : XLet(engine, parent)
{
    QVBoxLayout * vlayout = new QVBoxLayout(this);
    vlayout->setMargin(0);
    m_table = new QTableWidget( this );
    m_table->setAlternatingRowColors(true);
    m_table->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    m_table->setHorizontalScrollMode( QAbstractItemView::ScrollPerPixel );
    m_table->setColumnCount( 2 );
    QStringList labels;
    labels << tr("Time Stamp");
    labels << tr("Message");
    m_table->setHorizontalHeaderLabels(labels);

    vlayout->addWidget( m_table, 0 );
    // connect signal / slots
    connect( m_engine, SIGNAL(emitTextMessage(const QString &)),
             this, SLOT(addMessage(const QString &)) );
}

/*! \brief add a message to the list
 *
 * add a new message.
 */
void DisplayMessagesPanel::addMessage(const QString & str)
{
    //qDebug() << "DisplayMessagesPanel::addMessage()" << str;
    if(str.size() > 0) {
        QDateTime datetime = QDateTime::currentDateTime();
        m_table->insertRow( 0 );
        QTableWidgetItem * item1 = new QTableWidgetItem( datetime.toString( Qt::ISODate ) );
        item1->setFlags( Qt::ItemIsSelectable | Qt::ItemIsEnabled );
        m_table->setItem( 0, 0, item1 );
        QTableWidgetItem * item2 = new QTableWidgetItem( str );
        item2->setFlags( Qt::ItemIsSelectable | Qt::ItemIsEnabled );
        m_table->setItem( 0, 1, item2 );
        m_table->resizeColumnsToContents();
    }
}

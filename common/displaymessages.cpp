/* XIVO CTI clients
Copyright (C) 2007  Proformatique

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
*/

/* $Revision$
 * $Date$
 */

#include <QDebug>
#include <QHeaderView>
#include <QLabel>
#include <QTableWidget>
#include <QTime>
#include <QVBoxLayout>

#include "searchpanel.h"
#include "peerwidget.h"
#include "displaymessages.h"
#include "baseengine.h"

/*! \brief Constructor
 *
 * Set up the layout and the table widget.
 */
DisplayMessagesPanel::DisplayMessagesPanel(QWidget * parent)
	: QWidget(parent)
{
	QVBoxLayout * vlayout = new QVBoxLayout(this);
	vlayout->setMargin(0);
	// QLabel * lbl = new QLabel( tr("Messages :"), this );
	m_table = new QTableWidget( this );
        m_table->setAlternatingRowColors(true);
        m_table->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
	m_table->setHorizontalScrollMode( QAbstractItemView::ScrollPerPixel );
	m_table->setColumnCount( 2 );
	QStringList labels;
	labels << tr("Time Stamp");
	labels << tr("Message");
	m_table->setHorizontalHeaderLabels(labels);

        //	vlayout->addWidget( lbl, 0, Qt::AlignCenter );
	vlayout->addWidget( m_table, 0 );
}

/*! \brief add a message to the list
 *
 * add a new message.
 */
void DisplayMessagesPanel::addMessage(const QString & str)
{
	//qDebug() << "DisplayMessagesPanel::addMessage()" << str;
	QTime time = QTime::currentTime();
	m_table->insertRow( 0 );
	QTableWidgetItem * item1 = new QTableWidgetItem( time.toString( Qt::ISODate ) );
	item1->setFlags( Qt::ItemIsSelectable | Qt::ItemIsEnabled );
	m_table->setItem( 0, 0, item1 );
	QTableWidgetItem * item2 = new QTableWidgetItem( str );
	item2->setFlags( Qt::ItemIsSelectable | Qt::ItemIsEnabled );
	m_table->setItem( 0, 1, item2 );
	m_table->resizeColumnsToContents();
}


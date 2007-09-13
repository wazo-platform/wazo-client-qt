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

#include <QContextMenuEvent>
#include <QDebug>
#include <QDesktopServices>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QLabel>
#include <QMenu>
#include <QPushButton>
#include <QUrl>
#include <QVBoxLayout>

#include "directorypanel.h"
#include "extendedtablewidget.h"

/*! \brief Constructor
 *
 *  Build layout and child widgets, connect signals/slots.
 */
DirectoryPanel::DirectoryPanel(QWidget * parent)
        : QWidget(parent)
{
	QVBoxLayout * vlayout = new QVBoxLayout(this);
	vlayout->setMargin(0);
	QLabel * titleLbl = new QLabel( tr("Di&rectory"), this );
	vlayout->addWidget( titleLbl, 0, Qt::AlignCenter );
	QHBoxLayout * hlayout = new QHBoxLayout();
	m_searchText = new QLineEdit(this);
	titleLbl->setBuddy(m_searchText);
	connect( m_searchText, SIGNAL(returnPressed()),
	         this, SLOT(startSearch()) );
	hlayout->addWidget( m_searchText );
	m_searchButton = new QPushButton( tr("Search"), this );
	connect( m_searchButton, SIGNAL(clicked()),
	         this, SLOT(startSearch()) );
	hlayout->addWidget( m_searchButton );
	vlayout->addLayout( hlayout );
	m_table = new ExtendedTableWidget( this );
	connect( m_table, SIGNAL(itemClicked(QTableWidgetItem *)),
	         this, SLOT(itemClicked(QTableWidgetItem *)) );
	connect( m_table, SIGNAL(itemDoubleClicked(QTableWidgetItem *)),
	         this, SLOT(itemDoubleClicked(QTableWidgetItem *)) );
	connect( m_table, SIGNAL(emitDial(const QString &)),
	         this, SIGNAL(emitDial(const QString &)) );
	connect( this, SIGNAL(updateMyCalls(const QStringList &, const QStringList &, const QStringList &)),
	         m_table, SLOT(updateMyCalls(const QStringList &, const QStringList &, const QStringList &)) );
	connect( m_table, SIGNAL(transferCall(const QString &, const QString &)),
	         this, SIGNAL(transferCall(const QString &, const QString &)) );
	connect( m_table, SIGNAL(originateCall(const QString &, const QString &)),
	         this, SIGNAL(originateCall(const QString &, const QString &)) );
	vlayout->addWidget(m_table);

	/*
	QStringList labelList;
	labelList << QString("Numero") << QString("Nom");
	m_table->setColumnCount(2);
	m_table->setRowCount(3);
	m_table->setHorizontalHeaderLabels( labelList );
	for(int y = 0; y < 3; y++)
		for(int x = 0; x < 2; x++)
		{
			m_table->setItem( y, x, new QTableWidgetItem("-" + QString::number(x) + "," + QString::number(y)) );
		}
	qDebug() << m_table->columnCount();*/
	//vlayout->addStretch( 1 );
}

/*! \brief useless
 */
void DirectoryPanel::itemClicked(QTableWidgetItem * item)
{
	//qDebug() << item << item->text();
	// check if the string is a number
	QRegExp re_number("\\+?[0-9\\s\\.]+");
	if(re_number.exactMatch(item->text())) {
                //qDebug() << "preparing to dial" << item->text();
                copyNumber(item->text());
        }
}

void DirectoryPanel::itemDoubleClicked(QTableWidgetItem * item)
{
	//qDebug() << item << item->text();
	// check if the string is a number
	QRegExp re_number("\\+?[0-9\\s\\.]+");
	if(re_number.exactMatch(item->text())) {
                //qDebug() << "dialing" << item->text();
                emitDial(item->text());
        }

 	if(item && item->text().contains("@")) {
                QString mailAddr = item->text();
                if(mailAddr.length() > 0) {
                        //qDebug() << "DirectoryPanel::itemDoubleClicked() : mail" << mailAddr;
                        QDesktopServices::openUrl(QUrl("mailto:" + mailAddr));
                }
 	}
}

/*! \brief receive and process search response
 *
 * Parses the response, sets column and row headers,
 * set table cells.
 */
void DirectoryPanel::setSearchResponse(const QString & resp)
{
	int i, x, y;
	//qDebug() << "setSearchResponse()" << resp;
	QStringList items = resp.split(";");
	int ncolumns = items[0].toInt();
	if(ncolumns > 0) {
		int nrows = ((items.size() - 1) / ncolumns) - 1;
                if(nrows >= 0) {
                        m_table->setColumnCount(ncolumns);
                        m_table->setRowCount(nrows);
                        // qDebug() << items.size() << nrows << ncolumns ;
                        QStringList labelList;
                        for(i = 1; i <= ncolumns; i++)
                                labelList << items[i];
                        m_table->setHorizontalHeaderLabels( labelList );
                        for(y = 0; y < nrows; y++) {
                                for(x = 0; x < ncolumns; x++) {
                                        QString it = items[1+(1+y)*ncolumns+x];
                                        QTableWidgetItem * item = new QTableWidgetItem(it);
                                        item->setFlags( Qt::ItemIsSelectable | Qt::ItemIsEnabled ); // Qt::ItemIsDragEnabled

                                        QRegExp re_number("\\+?[0-9\\s\\.]+");
                                        if(it.contains("@"))
                                                item->setToolTip(tr("Double-click to send an E-mail to") + "\n" + it);
                                        else if(re_number.exactMatch(it))
                                                item->setToolTip(tr("Double-click to call") + "\n" + it);
                                        //item->setStatusTip();
                                        // qDebug() << x << y << item->flags();
                                        m_table->setItem( y, x, item );
                                        //qDebug() << m_table->cellWidget( y, x );
                                }
                        }
                }
	}
}

/*! \brief start the search process
 *
 * sends the searchDirectory() signal avec the right argument.
 */
void DirectoryPanel::startSearch()
{
	searchDirectory( m_searchText->text() );
}

/*! \brief stop
 *
 * clear everything.
 */
void DirectoryPanel::stop()
{
	m_table->setRowCount(0);
	m_table->setColumnCount(0);
	m_searchText->setText("");
}


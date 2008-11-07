/* XIVO CTI clients
 * Copyright (C) 2007, 2008  Proformatique
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License version 2 for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Linking the Licensed Program statically or dynamically with other
 * modules is making a combined work based on the Licensed Program. Thus,
 * the terms and conditions of the GNU General Public License version 2
 * cover the whole combination.
 *
 * In addition, as a special exception, the copyright holders of the
 * Licensed Program give you permission to combine the Licensed Program
 * with free software programs or libraries that are released under the
 * GNU Library General Public License version 2.0 or GNU Lesser General
 * Public License version 2.1 or any later version of the GNU Lesser
 * General Public License, and with code included in the standard release
 * of OpenSSL under a version of the OpenSSL license (with original SSLeay
 * license) which is identical to the one that was published in year 2003,
 * or modified versions of such code, with unchanged license. You may copy
 * and distribute such a system following the terms of the GNU GPL
 * version 2 for the Licensed Program and the licenses of the other code
 * concerned, provided that you include the source code of that other code
 * when and as the GNU GPL version 2 requires distribution of source code.
*/

/* $Revision$
 * $Date$
 */

#include <QContextMenuEvent>
#include <QDebug>
#include <QDesktopServices>
#include <QDropEvent>
#include <QFocusEvent>
#include <QHBoxLayout>
#include <QLabel>
#include <QMenu>
#include <QPushButton>
#include <QUrl>
#include <QVBoxLayout>

#include "directorypanel.h"
#include "extendedtablewidget.h"
#include "extendedlineedit.h"
#include "peerchannel.h"
#include "userinfo.h"

/*! \brief Constructor
 *
 *  Build layout and child widgets, connect signals/slots.
 */
DirectoryPanel::DirectoryPanel(const QVariant &, QWidget * parent)
        : QWidget(parent)
{
	QVBoxLayout * vlayout = new QVBoxLayout(this);
	vlayout->setMargin(0);
	QLabel * titleLbl = new QLabel( tr("Di&rectory"), this );
	vlayout->addWidget( titleLbl, 0, Qt::AlignCenter );
	QHBoxLayout * hlayout = new QHBoxLayout();
	m_searchText = new ExtendedLineEdit(this);
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
	connect( m_table, SIGNAL(transferCall(const QString &, const QString &)),
	         this, SIGNAL(transferCall(const QString &, const QString &)) );
	connect( m_table, SIGNAL(originateCall(const QString &, const QString &)),
	         this, SIGNAL(originateCall(const QString &, const QString &)) );
	connect( m_table, SIGNAL(ContextMenuEvent(QContextMenuEvent *)),
	         this, SLOT(contextMenuEvent(QContextMenuEvent *)) );

	vlayout->addWidget(m_table);
        setAcceptDrops(true);
        m_numberToDial = "";

        setFocusPolicy(Qt::StrongFocus);
        setFocusProxy(m_searchText);
}

DirectoryPanel::~DirectoryPanel()
{
}

void DirectoryPanel::setGuiOptions(const QVariant &)
{
}

void DirectoryPanel::setUserInfo(const UserInfo *)
{
}

void DirectoryPanel::focusInEvent(QFocusEvent * event)
{
        qDebug() << "DirectoryPanel::focusInEvent()" << event;
}

void DirectoryPanel::dropEvent(QDropEvent * event)
{
        qDebug() << "DirectoryPanel::dropEvent()" << event;
}

void DirectoryPanel::itemClicked(QTableWidgetItem * item)
{
	//qDebug() << item << item->text();
	// check if the string is a number
	QRegExp re_number("\\+?[0-9\\s\\.]+");
	if(re_number.exactMatch(item->text())) {
                // qDebug() << "DirectoryPanel::itemClicked()" << "preparing to dial" << item->text();
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
                originateCall("user:special:me", "ext:" + item->text());
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
	//qDebug() << "DirectoryPanel::setSearchResponse()" << resp;
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

void DirectoryPanel::contextMenuEvent(QContextMenuEvent * event)
{
	QTableWidgetItem * item = m_table->itemAt( event->pos() );
        if (item == NULL)
                return;

	QRegExp re_number("\\+?[0-9\\s\\.]+");
	if(item && re_number.exactMatch( item->text() )) {
		m_numberToDial = item->text();
                // qDebug() << "DirectoryPanel::contextMenuEvent()" << "preparing to dial" << m_numberToDial;
		QMenu contextMenu(this);
		contextMenu.addAction( tr("&Dial"), this, SLOT(dialNumber()) );
 		if(!m_mychannels.empty()) {
			QMenu * transferMenu = new QMenu(tr("&Transfer"), &contextMenu);
			QListIterator<PeerChannel *> i(m_mychannels);
			while(i.hasNext()) {
				const PeerChannel * channel = i.next();
				transferMenu->addAction(channel->otherPeer(),
				                        channel, SLOT(transfer()));
			}
			contextMenu.addMenu(transferMenu);
		}
		contextMenu.exec( event->globalPos() );
	}

 	if(item && item->text().contains("@")) {
                m_mailAddr = item->text();
                qDebug() << "email addr detection :" << m_mailAddr;
 		QMenu emailContextMenu(this);
                emailContextMenu.addAction( tr("Send an E-mail"), this, SLOT(sendMail()) );
                emailContextMenu.exec( event->globalPos() );
 	}
}

/*! \brief dial the number (when context menu item is toggled)
 */
void DirectoryPanel::dialNumber()
{
	if(m_numberToDial.length() > 0)
		originateCall("user:special:me", "ext:" + m_numberToDial);
}

/*! \brief dial the number (when context menu item is toggled)
 */
void DirectoryPanel::sendMail()
{
        if(m_mailAddr.length() > 0) {
                qDebug() << "ExtendedTableWidget::sendMail()" << m_mailAddr;
                QDesktopServices::openUrl(QUrl("mailto:" + m_mailAddr));
        }
}

/*! \brief update call list for transfer
 */
void DirectoryPanel::updatePeer(UserInfo * /*ui*/,
                                const QString &,
                                const QVariant & chanlist)
{
	while(!m_mychannels.isEmpty())
		delete m_mychannels.takeFirst();

        foreach(QString ref, chanlist.toMap().keys()) {
                QVariant chanprops = chanlist.toMap()[ref];
                PeerChannel * ch = new PeerChannel(chanprops);
 		connect(ch, SIGNAL(transferChan(const QString &)),
 		        this, SLOT(transferChan(const QString &)) );
 		m_mychannels << ch;
        }
}

/*! \brief transfer channel to the number
 */
void DirectoryPanel::transferChan(const QString & chan)
{
	transferCall(chan, m_numberToDial);
}

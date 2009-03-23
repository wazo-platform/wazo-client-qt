/* XIVO CTI clients
 * Copyright (C) 2007-2009  Proformatique
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
#include "baseengine.h"
#include "extendedtablewidget.h"
#include "extendedlineedit.h"
#include "userinfo.h"
#include "phoneinfo.h"
#include "xivoconsts.h"

/*! \brief Constructor
 *
 *  Build layout and child widgets, connect signals/slots.
 */
DirectoryPanel::DirectoryPanel(BaseEngine * engine, QWidget * parent)
    : QWidget(parent), m_engine(engine), m_re_number("\\+?[0-9\\s\\.]+")
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
    m_table = new ExtendedTableWidget( m_engine, this );
    connect( m_table, SIGNAL(itemClicked(QTableWidgetItem *)),
             this, SLOT(itemClicked(QTableWidgetItem *)) );
    connect( m_table, SIGNAL(itemDoubleClicked(QTableWidgetItem *)),
             this, SLOT(itemDoubleClicked(QTableWidgetItem *)) );
    connect( m_table, SIGNAL(actionCall(const QString &, const QString &, const QString &)),
             this, SIGNAL(actionCall(const QString &, const QString &, const QString &)) );
        
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
    // check if the string is a phone number
    if( m_re_number.exactMatch(item->text()) ) {
        // qDebug() << "DirectoryPanel::itemClicked()" << "preparing to dial" << item->text();
        copyNumber(item->text());
    }
}

void DirectoryPanel::itemDoubleClicked(QTableWidgetItem * item)
{
    //qDebug() << item << item->text();
    // check if the string is a number
    if( m_re_number.exactMatch(item->text()) ) {
        //qDebug() << "dialing" << item->text();
        emit actionCall("originate", "user:special:me", "ext:" + item->text()); // Call
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
            m_table->setSortingEnabled(false);
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

                    //QRegExp re_number("\\+?[0-9\\s\\.]+");
                    if(it.contains("@"))
                        item->setToolTip(tr("Double-click to send an E-mail to") + "\n" + it);
                    else if(m_re_number.exactMatch(it))
                        item->setToolTip(tr("Double-click to call") + "\n" + it);
                    //item->setStatusTip();
                    // qDebug() << x << y << item->flags();
                    m_table->setItem( y, x, item );
                    //qDebug() << m_table->cellWidget( y, x );
                }
            }
            m_table->setSortingEnabled(true);
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

#if 0
void DirectoryPanel::contextMenuEvent(QContextMenuEvent * event)
{
    QTableWidgetItem * item = m_table->itemAt( m_table->mapFromParent( event->pos() ) );
    qDebug() << "DirectoryPanel::contextMenuEvent()" << item;
    qDebug() << event->pos() << m_table->pos();
    qDebug() << m_table->mapFromParent( event->pos() );
//    qDebug() << ( event->pos() - m_table->pos() );
//    qDebug() << m_table;
//    qDebug() << m_table->parent();
//    qDebug() << m_table->parent()->parent();
//    qDebug() << this;
    if (item == NULL)
        return;

    if(item && m_re_number.exactMatch( item->text() )) {
        // this is a phone number, offer Dial and Transfer options
        m_numberToDial = item->text();
        // qDebug() << "DirectoryPanel::contextMenuEvent()" << "preparing to dial" << m_numberToDial;
        QMenu contextMenu( this );
        contextMenu.addAction( tr("&Dial"), this, SLOT(dialNumber()) );
        QMenu * transferMenu = new QMenu(tr("&Transfer"), &contextMenu);
        UserInfo * ui = m_engine ? m_engine->getXivoClientUser() : 0;
        if(ui)
        {
            foreach( const QString phone, ui->phonelist() )
            {
                const PhoneInfo * pi = ui->getPhoneInfo( phone );
                if( pi )
                {
                    QMapIterator<QString, QVariant> it( pi->comms() );
                    while( it.hasNext() )
                    {
                        it.next();
                        QMap<QString, QVariant> call = it.value().toMap();
                        // Add the transfer entry with the callerid name and num
                        QString text;
                        if( call.contains("calleridname") )
                        {
                            text.append( call["calleridname"].toString() );
                            text.append(" : ");
                        }
                        text.append( call["calleridnum"].toString() );
                        QAction * transferAction =
                            transferMenu->addAction( text,
                                                     this, SLOT(transfer()) );
//                        transferAction->setProperty( "chan", call["thischannel"] );
                        transferAction->setProperty( "chan", call["peerchannel"] );
                    }
                }
            }
        }
        event->accept();
        if( !transferMenu->isEmpty() )
            contextMenu.addMenu(transferMenu);
        contextMenu.exec( event->globalPos() );
    }

    if(item && item->text().contains("@")) {
        event->accept();
        // this is an email address
        m_mailAddr = item->text();
        qDebug() << "email addr detection :" << m_mailAddr;
        QMenu emailContextMenu( this );
        emailContextMenu.addAction( tr("Send an E-mail"),
                                    this, SLOT(sendMail()) );
        emailContextMenu.exec( event->globalPos() );
    }
}
#endif

/*! \brief dial the number (when context menu item is toggled)
 */
void DirectoryPanel::dialNumber()
{
    if( !m_numberToDial.isEmpty() )
        emit actionCall("originate", "user:special:me", "ext:" + m_numberToDial); // Call
}

/*! \brief dial the number (when context menu item is toggled)
 */
void DirectoryPanel::sendMail()
{
    if(m_mailAddr.length() > 0) {
        //        qDebug() << "ExtendedTableWidget::sendMail()" << m_mailAddr;
        QDesktopServices::openUrl( QUrl("mailto:" + m_mailAddr) );
    }
}


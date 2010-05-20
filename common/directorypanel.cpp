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
    : XLet(engine, parent), m_re_number("\\+?[0-9\\s\\.]+")
{
    setTitle( tr("Directory") );
    setAccessibleName( tr("Directory Panel") );
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

    // connect signal/slots
    connect( this, SIGNAL(searchDirectory(const QString &)),
             m_engine, SLOT(searchDirectory(const QString &)) );
    connect( m_engine, SIGNAL(directoryResponse(const QStringList &, const QStringList &)),
             this, SLOT(setSearchResponse(const QStringList &, const QStringList &)) );
    connect( this, SIGNAL(copyNumber(const QString &)),
             m_engine, SIGNAL(pasteToDialPanel(const QString &)) );
    connect( m_engine, SIGNAL(delogged()),
             this, SLOT(stop()) );
    connect( this, SIGNAL(actionCall(const QString &, const QString &, const QString &)),
             m_engine, SLOT(actionCall(const QString &, const QString &, const QString &)) );
}

/*! \brief does nothing for the moment */
void DirectoryPanel::focusInEvent(QFocusEvent * event)
{
    qDebug() << "DirectoryPanel::focusInEvent()" << event;
}

/*! \brief does nothing for the moment */
void DirectoryPanel::dropEvent(QDropEvent * event)
{
    qDebug() << "DirectoryPanel::dropEvent()" << event;
}

/*! \brief copy number in clicked cell to clipboard
 */
void DirectoryPanel::itemClicked(QTableWidgetItem * item)
{
    //qDebug() << item << item->text();
    // check if the string is a phone number
    if( m_re_number.exactMatch(item->text()) ) {
        //qDebug() << "DirectoryPanel::itemClicked()" << "preparing to dial" << item->text();
        copyNumber(item->text());
    }
}

/*! \brief call the number or send an email to the address */
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
void DirectoryPanel::setSearchResponse(const QStringList & headers, const QStringList & resp)
{
    int x, y;
    //qDebug() << "DirectoryPanel::setSearchResponse()" << headers << resp;
    int ncolumns = headers.size();
    int nrows = resp.size();
    
    m_table->setColumnCount(ncolumns);
    m_table->setRowCount(nrows);
    m_table->setHorizontalHeaderLabels(headers);
    
    if((ncolumns > 0) && (nrows > 0)) {
        m_table->setSortingEnabled(false);
        for(y = 0; y < nrows; y++) {
            QStringList items = resp[y].split(";");
            for(x = 0; x < ncolumns; x++) {
                QString it = items[x];
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
        
        // to remove the headers if ever ...
        // (they are useful to know that a reply has been received)
        // } else {
        // m_table->setColumnCount(0);
        // m_table->setRowCount(0);
    }
    // make columns fit the content
    //if(m_table->rowCount() > 0)
    //    m_table->resizeColumnsToContents();
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


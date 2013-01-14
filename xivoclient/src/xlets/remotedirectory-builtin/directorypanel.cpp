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

#include "directorypanel.h"
#include "phonenumber.h"

/*! \brief Constructor
 *
 *  Build layout and child widgets, connect signals/slots.
 */
DirectoryPanel::DirectoryPanel(QWidget *parent)
    : XLet(parent)
{
    setTitle(tr("Remote Directory"));
    setAccessibleName(tr("Remote Directory Panel"));
    QVBoxLayout *vlayout = new QVBoxLayout(this);
    vlayout->setMargin(0);
    QLabel *titleLbl = new QLabel(tr("Remote Di&rectory"), this);
    vlayout->addWidget(titleLbl, 0, Qt::AlignCenter);
    QHBoxLayout *hlayout = new QHBoxLayout();
    m_searchText = new ExtendedLineEdit(this);
    titleLbl->setBuddy(m_searchText);
    connect(m_searchText, SIGNAL(returnPressed()),
            this, SLOT(startSearch()));
    hlayout->addWidget(m_searchText);
    m_searchButton = new QPushButton(tr("Search"), this);
    connect(m_searchButton, SIGNAL(clicked()),
            this, SLOT(startSearch()));
    hlayout->addWidget(m_searchButton);
    vlayout->addLayout(hlayout);
    m_table = new ExtendedTableWidget(this);
    connect(m_table, SIGNAL(itemClicked(QTableWidgetItem *)),
            this, SLOT(itemClicked(QTableWidgetItem *)));
    connect(m_table, SIGNAL(itemDoubleClicked(QTableWidgetItem *)),
            this, SLOT(itemDoubleClicked(QTableWidgetItem *)));

    vlayout->addWidget(m_table);
    setAcceptDrops(true);

    setFocusPolicy(Qt::StrongFocus);
    setFocusProxy(m_searchText);

    // connect signal/slots
    connect(b_engine, SIGNAL(directoryResponse(const QStringList &, const QStringList &)),
            this, SLOT(setSearchResponse(const QStringList &, const QStringList &)) );
    connect(b_engine, SIGNAL(delogged()),
            this, SLOT(stop()) );
}

/*! \brief does nothing for the moment */
void DirectoryPanel::focusInEvent(QFocusEvent * event)
{
    qDebug() << Q_FUNC_INFO << event;
}

/*! \brief does nothing for the moment */
void DirectoryPanel::dropEvent(QDropEvent * event)
{
    qDebug() << Q_FUNC_INFO << event;
}

/*! \brief copy number in clicked cell to clipboard
 */
void DirectoryPanel::itemClicked(QTableWidgetItem * item)
{
    //qDebug() << Q_FUNC_INFO << item << item->text();
    // check if the string is a phone number
    if( PhoneNumber::phone_re().exactMatch(item->text()) ) {
        //qDebug() << Q_FUNC_INFO << "preparing to dial" << item->text();
        b_engine->pasteToDial(item->text());
        emit selectedText(item->text());
    }
}

/*! \brief call the number or send an email to the address */
void DirectoryPanel::itemDoubleClicked(QTableWidgetItem * item)
{
    //qDebug() << item << item->text();
    // check if the string is a number
    if( PhoneNumber::phone_re().exactMatch(item->text()) ) {
        b_engine->actionDialNumber(item->text()); // Call
    }

    if(item && item->text().contains("@")) {
        QString mailAddr = item->text();
        if(mailAddr.length() > 0) {
            //qDebug() << Q_FUNC_INFO << "mail" << mailAddr;
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
    //qDebug() << Q_FUNC_INFO << headers << resp;
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

                if(it.contains("@"))
                    item->setToolTip(tr("Double-click to send an E-mail to") + "\n" + it);
                else if(PhoneNumber::phone_re().exactMatch(it))
                    item->setToolTip(tr("Double-click to call") + "\n" + it);
                //item->setStatusTip();
                // qDebug() << x << y << item->flags();
                m_table->setItem( y, x, item );
                //qDebug() << m_table->cellWidget( y, x );
            }
        }
        m_table->setSortingEnabled(true);
        m_table->resizeColumnsToContents();

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
 */
void DirectoryPanel::startSearch()
{
    b_engine->searchDirectory(m_searchText->text());
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

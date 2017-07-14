/*
 * Copyright 2007-2017 The Wazo Authors  (see the AUTHORS file)
 *
 * This program is free software: you can redistribute it and/or modify
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
 * This programm is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "directorypanel.h"
#include "phonenumber.h"
#include <message_factory.h>

DirectoryPanel::DirectoryPanel(QWidget *parent)
    : QWidget(parent)
{
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
    connect(m_table, SIGNAL(columnSorted(int, Qt::SortOrder)),
            this, SLOT(saveColumnSorting(int, Qt::SortOrder)));

    vlayout->addWidget(m_table);
    setAcceptDrops(true);

    setFocusPolicy(Qt::StrongFocus);
    setFocusProxy(m_searchText);

    connect(b_engine, SIGNAL(directoryResponse(const QStringList &, const QStringList &)),
            this, SLOT(setSearchResponse(const QStringList &, const QStringList &)) );
    connect(b_engine, SIGNAL(delogged()),
            this, SLOT(stop()) );
}

void DirectoryPanel::focusInEvent(QFocusEvent * event)
{
    qDebug() << Q_FUNC_INFO << event;
}

void DirectoryPanel::dropEvent(QDropEvent * event)
{
    qDebug() << Q_FUNC_INFO << event;
}

void DirectoryPanel::itemClicked(QTableWidgetItem * item)
{
    if( PhoneNumber::phone_re().exactMatch(item->text()) ) {
        b_engine->pasteToDial(item->text());
        emit selectedText(item->text());
    }
}

void DirectoryPanel::itemDoubleClicked(QTableWidgetItem * item)
{
    if (item == NULL) {
        return;
    }

    if( PhoneNumber::phone_re().exactMatch(item->text()) ) {
        b_engine->actionDial(item->text());
    }

    if(item->text().contains("@")) {
        QString mailAddr = item->text();
        if(mailAddr.length() > 0) {
            QDesktopServices::openUrl(QUrl("mailto:" + mailAddr));
        }
    }
}

void DirectoryPanel::setSearchResponse(const QStringList & headers, const QStringList & resp)
{
    int ncolumns = headers.size();
    int nrows = resp.size();

    m_table->clear();
    m_table->setColumnCount(ncolumns);
    m_table->setRowCount(nrows);
    m_table->setHorizontalHeaderLabels(headers);

    if((ncolumns > 0) && (nrows > 0)) {

        m_table->setSortingEnabled(false);

        for(int y = 0; y < nrows; y++) {
            QStringList items = resp[y].split(";");
            for(int x = 0; x < ncolumns; x++) {
                QString it = items[x];
                QTableWidgetItem * item = new QTableWidgetItem(it);
                item->setFlags( Qt::ItemIsSelectable | Qt::ItemIsEnabled );

                if(it.contains("@"))
                    item->setToolTip(tr("Double-click to send an E-mail to") + "\n" + it);
                else if(PhoneNumber::phone_re().exactMatch(it))
                    item->setToolTip(tr("Double-click to call") + "\n" + it);
                m_table->setItem( y, x, item );
            }
        }

        m_table->resizeColumnsToContents();
        m_table->setSortingEnabled(true);
        restoreColumnSorting();
    }

}

void DirectoryPanel::restoreColumnSorting()
{
    int column = b_engine->getConfig(CONFIG_SORT_COLUMN).toInt();
    Qt::SortOrder order = (Qt::SortOrder)b_engine->getConfig(CONFIG_SORT_ORDER).toInt();

    m_table->sortItems(column, order);
}

void DirectoryPanel::saveColumnSorting(int column, Qt::SortOrder order)
{
    if (column < m_table->columnCount()) {
        b_engine->setConfig(CONFIG_SORT_COLUMN, column);
        b_engine->setConfig(CONFIG_SORT_ORDER, order);
    }
}

void DirectoryPanel::startSearch()
{
    const QString &search_pattern = m_searchText->text();
    b_engine->sendJsonCommand(MessageFactory::directorySearch(search_pattern));
}

void DirectoryPanel::stop()
{
    m_table->setRowCount(0);
    m_table->setColumnCount(0);
    m_searchText->setText("");
}

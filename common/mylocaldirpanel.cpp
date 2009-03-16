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

#include <QDebug>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QFileDialog>
#include <QApplication>
#include <QFileInfo>

#include "baseengine.h"
#include "mylocaldirpanel.h"
#include "userinfo.h"
#include "contactdialog.h"
#include "extendedtablewidget.h"
#include "searchdialog.h"
#include "csvstream.h"

/*! \brief Constructor
 */
MyLocalDirPanel::MyLocalDirPanel(BaseEngine * engine, QWidget * parent)
    : QWidget(parent), m_engine(engine), m_ui(NULL)
{
    qDebug() << "MyLocalDirPanel::MyLocalDirPanel()";
        
    m_searchBox = new SearchDialog(this);
    connect(m_searchBox, SIGNAL(findNext()), this, SLOT(findNext()));

    QVBoxLayout * vlayout = new QVBoxLayout(this);
    // button line
    QHBoxLayout * hlayout = new QHBoxLayout();
    QPushButton * addNewBtn = new QPushButton( tr("&New Contact") );
    connect( addNewBtn, SIGNAL(clicked()),
             this, SLOT(openNewContactDialog()) );
    hlayout->addWidget( addNewBtn );
    QPushButton * exportBtn = new QPushButton( tr("&Export Contacts") );
    connect( exportBtn, SIGNAL(clicked()),
             this, SLOT(exportContacts()) );
    hlayout->addWidget( exportBtn );
    QPushButton * importBtn = new QPushButton( tr("&Import Contacts") );
    connect( importBtn, SIGNAL(clicked()),
             this, SLOT(importContacts()) );
    hlayout->addWidget( importBtn );
    QPushButton * searchBtn = new QPushButton( tr("&Search") );
    connect( searchBtn, SIGNAL(clicked()),
             m_searchBox, SLOT(show()) );
    hlayout->addWidget( searchBtn );

    vlayout->addLayout(hlayout);

    m_table = new ExtendedTableWidget( m_engine );
    m_table->setEditable( true );
    connect( m_table, SIGNAL(actionCall(const QString &, const QString &, const QString &)),
             this, SIGNAL(actionCall(const QString &, const QString &, const QString &)) );
    QStringList columnNames;
    columnNames.append( tr("First Name") );
    columnNames.append( tr("Last Name") );
    columnNames.append( tr("Phone Number") );
    columnNames.append( tr("Email Address") );
    columnNames.append( tr("Company") );
    m_table->setColumnCount( 5 );
    m_table->setHorizontalHeaderLabels( columnNames );
    m_table->setSortingEnabled( true );
    vlayout->addWidget(m_table);
    QFile file(getSaveFile());
    loadFromFile( file );
}

/*! \brief Destructor
 */
MyLocalDirPanel::~MyLocalDirPanel()
{
    qDebug() << "MyLocalDirPanel::~MyLocalDirPanel()";
    QFile file(getSaveFile());
    saveToFile( file );
}

QString MyLocalDirPanel::getSaveFile() const
{
    qDebug() << "MyLocalDirPanel::getSaveFile()" << qApp->applicationDirPath() << m_engine->getSettings()->fileName();
    //QDir dir( qApp->applicationDirPath() );
    QFileInfo fi( m_engine->getSettings()->fileName() );
    QDir dir( fi.canonicalPath() );
    return dir.absoluteFilePath("localdir.csv");
}

/*! Does nothing
 */
void MyLocalDirPanel::setGuiOptions(const QVariant &)
{
}

/*! store current user
 */
void MyLocalDirPanel::setUserInfo(const UserInfo * ui)
{
    m_ui = ui;
    // qDebug() << "MyLocalDirPanel::setUserInfo()" << m_ui->fullname();
}

/*! open the dialog box used to enter a new contact
 */
void MyLocalDirPanel::openNewContactDialog()
{
    ContactDialog dialog;
    int r = dialog.exec();
    //qDebug() << r;
    if(r)
    {
        bool saveSorting = m_table->isSortingEnabled();
        m_table->setSortingEnabled( false );
        int row = m_table->rowCount();
        m_table->setRowCount( row + 1 );
        QTableWidgetItem * itemFirstName = new QTableWidgetItem( dialog.firstname() );
        m_table->setItem( row, 0, itemFirstName );
        QTableWidgetItem * itemLastName = new QTableWidgetItem( dialog.lastname() );
        m_table->setItem( row, 1, itemLastName );
        QTableWidgetItem * itemNumber = new QTableWidgetItem( dialog.number() );
        m_table->setItem( row, 2, itemNumber );
        QTableWidgetItem * itemEmail = new QTableWidgetItem( dialog.email() );
        m_table->setItem( row, 3, itemEmail );
        QTableWidgetItem * itemCompany = new QTableWidgetItem( dialog.company() );
        m_table->setItem( row, 4, itemCompany );
        m_table->setSortingEnabled( saveSorting );
    }
}

/*! \brief import contacts from a .csv file
 *
 * open a "Open File" dialog and then call
 * loadFromFile()
 */
void MyLocalDirPanel::importContacts()
{
    //qDebug() << "MyLocalDirPanel::importContacts()";
    QString fileName = QFileDialog::getOpenFileName(this,
                         tr("Open Contacts File"),
                         QString(),
                         tr("Comma Separated Value (*.csv)"));
    if(fileName.isEmpty())
        return;
    QFile file(fileName);
    loadFromFile( file );
}

/*! \brief export contact to a .csv file
 *
 * open a "Save File" dialog then call saveToFile()
 */
void MyLocalDirPanel::exportContacts()
{
    //qDebug() << "MyLocalDirPanel::exportContacts()";
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Contacts File"),
                         QString(),
                         tr("Comma Separated Value (*.csv)"));
    qDebug() << fileName;
    if(fileName.isEmpty())
        return;
    QFile file(fileName);
    saveToFile( file );
}

/*! \brief save the contact list to a .csv file
 *
 * iterate through entries and save them to the .csv file.
 */
void MyLocalDirPanel::saveToFile(QFile & file)
{
    QChar separator = QChar(',');
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;
    CsvStream out(&file);
    // write header line
    out << ( QStringList()
             << tr("First Name")
             << tr("Last Name")
             << tr("Phone Number")
             << tr("Email Address")
             << tr("Company") );
    // write all entries
    for(int i = 0; i < m_table->rowCount(); i++)
    {
        QStringList records;
        for(int j = 0; j < 5; j++)
        {
            QTableWidgetItem * item = m_table->item(i, j);
            QString text;
            if(item)
                text = item->text();
            records.append(text);
        }
        out << records;
    }
    out.flush();
}

void MyLocalDirPanel::loadFromFile(QFile & file)
{
    //QChar separator = QChar(',');
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    CsvStream in(&file);
    //QString headerLine = in.readLine();
    //if(headerLine.isEmpty())
    //    return;
    bool saveSorting = m_table->isSortingEnabled();
    m_table->setSortingEnabled( false );
    //QStringList headers = headerLine.split(separator);
    //if(headers.count() == 1)
    //{
    //    separator = QChar(';');
    //    headers = headerLine.split(separator);
    //}
    QStringList headers = in.readRecords();
    int firstNameCol = findCol(headers, QStringList()
                                        << tr("First Name")
                                        << QString("First Name") );
    int lastNameCol = findCol(headers, QStringList()
                                        << tr("Last Name")
                                        << QString("Last Name") );
    int maxCol = qMax(firstNameCol, lastNameCol);
    int numberCol = findCol(headers, QStringList()
                                        << tr("Phone Number")
                                        << QString("Phone Number")
                                        << tr("Number")
                                        << QString("Number") );
    maxCol = qMax(maxCol, numberCol);
    int emailCol = findCol(headers, QStringList()
                                    << tr("Email Address")
                                    << tr("Email")
                                    << QString("Email Address")
                                    << QString("Email")
                                    << QString("Primary Email") );
    maxCol = qMax(maxCol, emailCol);
    int companyCol = findCol(headers, QStringList()
                                      << tr("Company")
                                      << QString("Company") );
    maxCol = qMax(maxCol, companyCol);
    qDebug() << "MyLocalDirPanel::loadFromFile"
             << firstNameCol << lastNameCol << numberCol
             << emailCol << companyCol;
    while (!in.atEnd())
    {
        QStringList record = in.readRecords();
        if(record.size() <= maxCol)
            continue;
        int row = m_table->rowCount();
        m_table->setRowCount( row + 1 );
        if(firstNameCol >= 0)
        {
            QTableWidgetItem * itemFirstName = new QTableWidgetItem( record[firstNameCol] );
            m_table->setItem( row, 0, itemFirstName );
        }
        if(lastNameCol >= 0)
        {
            QTableWidgetItem * itemLastName = new QTableWidgetItem( record[lastNameCol] );
            m_table->setItem( row, 1, itemLastName );
        }
        if(numberCol >= 0)
        {
            QTableWidgetItem * itemNumber = new QTableWidgetItem( record[numberCol] );
            m_table->setItem( row, 2, itemNumber );
        }
        if(emailCol >= 0)
        {
            QTableWidgetItem * itemEmail = new QTableWidgetItem( record[emailCol] );
            m_table->setItem( row, 3, itemEmail );
        }
        if(companyCol >= 0)
        {
            QTableWidgetItem * itemCompany = new QTableWidgetItem( record[companyCol] );
            m_table->setItem( row, 4, itemCompany );
        }
    }
    m_table->setSortingEnabled( saveSorting );
}

/*! \brief find the column index
 *
 */
int MyLocalDirPanel::findCol(QStringList headers, QStringList labels)
{
    int n = headers.size();
    foreach(QString label, labels)
    {
        for(int i = 0; i < n; i++)
        {
            QString header = headers[i].trimmed();
            if( label.compare(header, Qt::CaseInsensitive) == 0 )
                return i;
        }
    }
    return -1;
}

/*! \brief select next match
 *
 * use the text from the searchbox to find all matching cells
 * and select the next one.
 */
void MyLocalDirPanel::findNext()
{
    if(!m_searchBox)
        return;
    QString searchText = m_searchBox->text();
    qDebug() << "MyLocalDirPanel::findNext()" << searchText;
    QTableWidgetItem * currentItem = m_table->currentItem();
    QList<QTableWidgetItem *> items
        = m_table->findItems( searchText, Qt::MatchContains | Qt::MatchFixedString);
    if(items.size() == 0)
        return;
    if(currentItem) {
        int i = items.indexOf( currentItem );
        i++;
        if( i >= items.size() )
            i = 0;
        currentItem = items[i];
    }
    else
    {
        currentItem = items[0];
    }
    m_table->setCurrentItem( currentItem );
}


/* XiVO Client
 * Copyright (C) 2007-2011, Avencall
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

#include <QApplication>
#include <QDate>
#include <QDebug>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QFileInfo>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QSettings>
#include <QVBoxLayout>

#include "baseengine.h"
#include "mylocaldirpanel.h"
#include "userinfo.h"
#include "contactdialog.h"
#include "extendedtablewidget.h"
#include "searchdialog.h"
#include "csvstream.h"

/*! \brief Constructor
 */
MyLocalDirPanel::MyLocalDirPanel(QWidget * parent)
    : XLet(parent)
{
    setTitle( tr("Personal Directory") );

    m_headers << "firstname"
              << "lastname"
              << "phonenumber"
              << "emailaddress"
              << "company"
              << "faxnumber"
              << "mobilenumber";
    m_max_lengths << 40
                  << 40
                  << 20
                  << 60
                  << 40
                  << 20
                  << 20;

    m_searchBox = new SearchDialog(this);
    connect(m_searchBox, SIGNAL(findNext()), this, SLOT(findNext()));

    QVBoxLayout *vlayout = new QVBoxLayout(this);
    // button line
    QHBoxLayout *hlayout = new QHBoxLayout();
    QPushButton *addNewBtn = new QPushButton(tr("&New Contact"));
    connect(addNewBtn, SIGNAL(clicked()),
            this, SLOT(openNewContactDialog()));
    hlayout->addWidget( addNewBtn );
    QPushButton *exportBtn = new QPushButton(tr("&Export Contacts"));
    connect(exportBtn, SIGNAL(clicked()),
            this, SLOT(exportContacts()) );
    hlayout->addWidget( exportBtn );
    QPushButton *importBtn = new QPushButton(tr("&Import Contacts"));
    connect(importBtn, SIGNAL(clicked()),
            this, SLOT(importContacts()) );
    hlayout->addWidget( importBtn );
    QPushButton *searchBtn = new QPushButton(tr("&Search"));
    connect(searchBtn, SIGNAL(clicked()),
            m_searchBox, SLOT(show()) );
    hlayout->addWidget( searchBtn );
    QPushButton *removeAllBtn = new QPushButton(tr("&Remove all Contacts"));
    connect(removeAllBtn, SIGNAL(clicked()),
            this, SLOT(removeAllContacts()) );
    hlayout->addWidget( removeAllBtn );

    vlayout->addLayout(hlayout);

    m_table = new ExtendedTableWidget;
    m_table->setEditable( true );
    QStringList columnNames;
    columnNames.append(tr("First Name"));
    columnNames.append(tr("Last Name"));
    columnNames.append(tr("Phone Number"));
    columnNames.append(tr("Email Address"));
    columnNames.append(tr("Company"));
    columnNames.append(tr("Fax Number"));
    columnNames.append(tr("Mobile Number"));
    m_table->setColumnCount(m_headers.size());
    m_table->setHorizontalHeaderLabels(columnNames);
    m_table->setSortingEnabled(true);
    vlayout->addWidget(m_table);

    connect(qApp, SIGNAL(commitDataRequest(QSessionManager &)),
            this, SLOT(syncWithSaveFile()));

    QFile file(getSaveFile());
    file.copy(getBackupFile());
    loadFromFile(file);
}


/*! \brief Destructor
 */
MyLocalDirPanel::~MyLocalDirPanel()
{
    syncWithSaveFile();
}

/*! \brief get the saved file
 */
QString MyLocalDirPanel::getSaveFile() const
{
    // qDebug() << Q_FUNC_INFO << qApp->applicationDirPath() << b_engine->getSettings()->fileName();
    // QDir dir( qApp->applicationDirPath() );
    QFileInfo fi( b_engine->getSettings()->fileName() );
    QDir dir( fi.canonicalPath() );
    return dir.absoluteFilePath("localdir.csv");
}

/*! \brief get the backup file
 */
QString MyLocalDirPanel::getBackupFile() const
{
    // qDebug() << Q_FUNC_INFO << qApp->applicationDirPath() << b_engine->getSettings()->fileName();
    // QDir dir( qApp->applicationDirPath() );
    QFileInfo fi( b_engine->getSettings()->fileName() );
    QDir dir( fi.canonicalPath() );
    return dir.absoluteFilePath(QString("localdir_backup.%1.csv").arg(QDate::currentDate().toString("yyyy.MM.dd")));
}

/*! open the dialog box used to enter a new contact
 */
void MyLocalDirPanel::openNewContactDialog()
{
    ContactDialog dialog;
    int r = dialog.exec();
    // qDebug() << r;
    if(r) {
        bool saveSorting = m_table->isSortingEnabled();
        m_table->setSortingEnabled( false );
        int row = m_table->rowCount();
        m_table->setRowCount( row + 1 );

        QTableWidgetItem * itemFirstName = new QTableWidgetItem( dialog.firstname() );
        m_table->setItem( row, 0, itemFirstName );
        QTableWidgetItem * itemLastName = new QTableWidgetItem( dialog.lastname() );
        m_table->setItem( row, 1, itemLastName );
        QTableWidgetItem * itemPhoneNumber = new QTableWidgetItem( dialog.phonenumber() );
        m_table->setItem( row, 2, itemPhoneNumber );
        QTableWidgetItem * itemEmail = new QTableWidgetItem( dialog.email() );
        m_table->setItem( row, 3, itemEmail );
        QTableWidgetItem * itemCompany = new QTableWidgetItem( dialog.company() );
        m_table->setItem( row, 4, itemCompany );
        QTableWidgetItem * itemFaxNumber = new QTableWidgetItem( dialog.faxnumber() );
        m_table->setItem( row, 5, itemFaxNumber );
        QTableWidgetItem * itemMobileNumber = new QTableWidgetItem( dialog.mobilenumber() );
        m_table->setItem( row, 6, itemMobileNumber );

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
    //qDebug() << Q_FUNC_INFO;
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
    // qDebug() << Q_FUNC_INFO;
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Contacts File"),
                                                    QString(),
                                                    tr("Comma Separated Value (*.csv)"));
    if(fileName.isEmpty())
        return;
    QFile file(fileName);
    saveToFile( file );
}

/*! \brief Write contacts in the dedicated file
 */
void MyLocalDirPanel::syncWithSaveFile()
{
    QFile save_file(getSaveFile());
    saveToFile(save_file);
}

/*! \brief save the contact list to a .csv file
 *
 * iterate through entries and save them to the .csv file.
 */
void MyLocalDirPanel::saveToFile(QFile & file)
{
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;
    CsvStream out(&file);
    // write header line
    out << m_headers;

    // write all entries
    for(int i = 0; i < m_table->rowCount(); i++)
    {
        QStringList records;
        for(int j = 0; j < m_headers.size(); j++)
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
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    CsvStream in(&file);
    bool saveSorting = m_table->isSortingEnabled();

    QStringList headers = in.readRecords();
    int firstNameCol = findCol(headers, QStringList()
                               << "firstname"
                               << tr("First Name")
                               << QString("First Name") );
    int lastNameCol = findCol(headers, QStringList()
                              << "lastname"
                              << tr("Last Name")
                              << QString("Last Name") );
    int maxCol = qMax(firstNameCol, lastNameCol);
    int phonenumberCol = findCol(headers, QStringList()
                                 << "phonenumber"
                                 << tr("Phone Number")
                                 << QString("Phone Number")
                                 << tr("Number")
                                 << QString("Number") );
    maxCol = qMax(maxCol, phonenumberCol);
    int emailCol = findCol(headers, QStringList()
                           << "emailaddress"
                           << tr("Email Address")
                           << tr("E-mail Address")
                           << tr("Email")
                           << QString("Email Address")
                           << QString("E-mail Address")
                           << QString("Email")
                           << QString("Primary Email") );
    maxCol = qMax(maxCol, emailCol);
    int companyCol = findCol(headers, QStringList()
                             << "company"
                             << tr("Company")
                             << QString("Company") );
    maxCol = qMax(maxCol, companyCol);
    int faxnumberCol = findCol(headers, QStringList()
                               << "faxnumber"
                               << tr("Fax Number")
                               << tr("Fax")
                               << QString("Fax Number")
                               << QString("Fax") );
    maxCol = qMax(maxCol, faxnumberCol);
    int mobilenumberCol = findCol(headers, QStringList()
                                  << "mobilenumber"
                                  << tr("Mobile Number")
                                  << tr("Mobile")
                                  << QString("Mobile Number")
                                  << QString("Mobile") );
    maxCol = qMax(maxCol, mobilenumberCol);
    if(maxCol < 0) {
        // no header recognized... exiting
        return;
    }
    m_table->setSortingEnabled( false );
    while (!in.atEnd())
    {
        QStringList record = in.readRecords();
        while (record.size() <= maxCol)
            record.append("");
        int row = m_table->rowCount();
        m_table->setRowCount( row + 1 );

        for (unsigned int i = 0; i < record.size(); ++i) {
            record[i].truncate(m_max_lengths[i]);
        }

        int col = 0;
        addCell(row, col++, record[firstNameCol]);
        addCell(row, col++, record[lastNameCol]);
        addCell(row, col++, record[phonenumberCol]);
        addCell(row, col++, record[emailCol]);
        addCell(row, col++, record[companyCol]);
        addCell(row, col++, record[faxnumberCol]);
        addCell(row, col++, record[mobilenumberCol]);
    }
    m_table->setSortingEnabled( saveSorting );
}

void MyLocalDirPanel::addCell(int row, int col, const QString &data)
{
    QTableWidgetItem *item = new QTableWidgetItem(data);
    m_table->setItem(row, col, item);
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
    qDebug() << Q_FUNC_INFO << searchText;
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

/** \brief ask and then remove all contacts
 */
void  MyLocalDirPanel::removeAllContacts()
{
    int ret;
    ret = QMessageBox::warning(this, tr("Removing all contacts"),
                               tr("Removing all contacts.\n"
                                  "Are you sure ?"),
                               QMessageBox::Yes|QMessageBox::No);
    if(ret == QMessageBox::Yes) {
        //m_table->clearContents(); // doesn't resize the table (remove rows)
        // remove all rows 1 by 1 until none left.
        while(m_table->rowCount() > 0) {
            m_table->removeRow( 0 );
        }
    }
}

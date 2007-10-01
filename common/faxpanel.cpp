/*
XIVO CTI clients
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
#include <QHBoxLayout>
#include <QGroupBox>
#include <QCheckBox>
#include <QFileDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSettings>
#include <QVBoxLayout>

#include "faxpanel.h"
#include "dirdialog.h"

FaxPanel::FaxPanel(BaseEngine * engine, QWidget * parent)
        : QWidget(parent), m_mainwindow(parent), m_engine(engine)
{
        QSettings settings;
        // qDebug() << "FaxPanel::FaxPanel()" << parent;
        Qt::CheckState previous_hide = (Qt::CheckState) settings.value("faxhistory/hidenumber", 0).toInt();

	QVBoxLayout * vlayout = new QVBoxLayout(this);

        //
	QGroupBox * groupBox1 = new QGroupBox( tr("1. Choose Destination Number") );
	groupBox1->setAlignment( Qt::AlignLeft );
	QHBoxLayout * hbox1 = new QHBoxLayout( groupBox1 );

	QLabel * lbluncond = new QLabel(tr("Fax Number"), this);
	m_destination = new QLineEdit(this);
        QPushButton * directory = new QPushButton( tr("Directory"), this);
        connect(directory, SIGNAL(clicked()),
                this, SLOT(dirLookup()));
        hbox1->addWidget(lbluncond);
	hbox1->addWidget(m_destination);
        hbox1->addWidget(directory);
        lbluncond->setObjectName("fax");

        //
	QGroupBox * groupBox2 = new QGroupBox( tr("2. Choose File to Send") );
	groupBox2->setAlignment( Qt::AlignLeft );
	QHBoxLayout * hbox2 = new QHBoxLayout( groupBox2 );

        m_openFileNameLabel = new QLineEdit("", this);
        QPushButton * openFileNamesButton = new QPushButton( tr("Browse"), this);
        connect(openFileNamesButton, SIGNAL(clicked()),
                this, SLOT(setOpenFileName()));
	hbox2->addWidget(m_openFileNameLabel);
	hbox2->addWidget(openFileNamesButton);

        //
        QGroupBox * groupBox3 = new QGroupBox( tr("3. Hide the Number ?") );
	groupBox3->setAlignment( Qt::AlignLeft );
	QHBoxLayout * hbox3 = new QHBoxLayout( groupBox3 );

        m_maskornot = new QCheckBox(tr("Hide Number"), this);
        m_maskornot->setCheckState(previous_hide);
 	hbox3->addStretch(1);
 	hbox3->addWidget(m_maskornot);
 	hbox3->addStretch(1);

        //
        QGroupBox * groupBox4 = new QGroupBox( tr("4. Send your File") );
	groupBox4->setAlignment( Qt::AlignLeft );
	QHBoxLayout * hbox4 = new QHBoxLayout( groupBox4 );

        QPushButton * sendButton = new QPushButton( tr("Send"), this);
        connect(sendButton, SIGNAL(clicked()),
                this, SLOT(sendFax()));
 	hbox4->addStretch(1);
 	hbox4->addWidget(sendButton);
 	hbox4->addStretch(1);

        vlayout->addWidget(groupBox1);
        vlayout->addWidget(groupBox2);
        vlayout->addWidget(groupBox3);
        vlayout->addWidget(groupBox4);
        vlayout->addStretch(1);

}


FaxPanel::~FaxPanel()
{
        // qDebug() << "FaxPanel::~FaxPanel()";
}

void FaxPanel::setOpenFileName()
{
        QFileDialog::Options options;
        /*        if (!native->isChecked())*/
        options |= QFileDialog::DontUseNativeDialog;
        QString selectedFilter;
        QString fileName = QFileDialog::getOpenFileName(this,
                                                        tr("Open Fax File"),
                                                        m_openFileNameLabel->text(),
                                                        tr("PDF Files (*.pdf);;TIFF Files (*.tif, *.tiff);;Both (*.pdf, *.tif, *.tiff);;All Files (*)"),
                                                        &selectedFilter,
                                                        options);
        if (!fileName.isEmpty())
                m_openFileNameLabel->setText(fileName);
}


void FaxPanel::sendFax()
{
        QSettings settings;
	settings.setValue("faxhistory/hidenumber", m_maskornot->checkState());

        if ((! m_openFileNameLabel->text().isEmpty()) && (! m_destination->text().isEmpty())) {
                qDebug() << "FaxPanel::sendFax()"
                         << m_openFileNameLabel->text()
                         << m_destination->text()
                         << m_maskornot->checkState();
                faxSend(m_openFileNameLabel->text(),
                        m_destination->text(),
                        m_maskornot->checkState());
        }
}

void FaxPanel::dirLookup()
{
        // qDebug() << "FaxPanel::dirLookup()";
        m_dirw = new DirDialog(m_engine, m_mainwindow);
        m_dirw->exec();
        // qDebug() << "FaxPanel::dirLookup() DirDialog exec'ed";
        QString retstr = m_dirw->faxnumber();
        if(retstr.size() > 0)
                m_destination->setText(retstr);
        delete m_dirw;
}

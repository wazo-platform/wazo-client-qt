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
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QCheckBox>
#include <QFileDialog>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>

#include "faxpanel.h"

FaxPanel::FaxPanel(QWidget * parent)
        : QWidget(parent)
{
        qDebug() << "FaxPanel::FaxPanel()";
	QVBoxLayout * vlayout = new QVBoxLayout(this);

        //
	QGroupBox * groupBox1 = new QGroupBox( tr("1. Choose Destination Number") );
	groupBox1->setAlignment( Qt::AlignLeft );
	QHBoxLayout * hbox1 = new QHBoxLayout( groupBox1 );

	QLabel * lbluncond = new QLabel(tr("Fax Number"), this);
	m_destination = new QLineEdit(this);
        QPushButton * directory = new QPushButton( tr("Directory"), this);
        hbox1->addWidget(lbluncond);
	hbox1->addWidget(m_destination);
        hbox1->addWidget(directory);
        lbluncond->setObjectName("fax");

        //
	QGroupBox * groupBox2 = new QGroupBox( tr("2. Choose File to Send") );
	groupBox2->setAlignment( Qt::AlignLeft );
	QHBoxLayout * hbox2 = new QHBoxLayout( groupBox2 );

        m_openFileNameLabel = new QLineEdit("/tmp", this);
        QPushButton * openFileNamesButton = new QPushButton( tr("Browse"), this);
        connect(openFileNamesButton, SIGNAL(clicked()),
                this, SLOT(setOpenFileName()));
	hbox2->addWidget(m_openFileNameLabel);
	hbox2->addWidget(openFileNamesButton);

        //
        QGroupBox * groupBox3 = new QGroupBox( tr("3. Hide the Number ?") );
	groupBox3->setAlignment( Qt::AlignLeft );
	QHBoxLayout * hbox3 = new QHBoxLayout( groupBox3 );

        QCheckBox * maskornot = new QCheckBox(tr("Hide Number"), this);
 	hbox3->addStretch(1);
 	hbox3->addWidget(maskornot);
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


void FaxPanel::setOpenFileName()
{
        QFileDialog::Options options;
        /*        if (!native->isChecked())*/
        options |= QFileDialog::DontUseNativeDialog;
        QString selectedFilter;
        QString fileName = QFileDialog::getOpenFileName(this,
                                                        tr("Open Fax File"),
                                                        m_openFileNameLabel->text(),
                                                        //tr("All Files (*)"),
                                                        tr("PDF Files (*.pdf);;TIFF Files (*.tif, *.tiff);;Both (*.pdf, *.tif, *.tiff);;All Files (*)"),
                                                        &selectedFilter,
                                                        options);
        if (!fileName.isEmpty())
                m_openFileNameLabel->setText(fileName);
}


void FaxPanel::sendFax()
{
        qDebug() << "FaxPanel::sendFax()"
                 << m_openFileNameLabel->text()
                 << m_destination->text();
        if (! m_openFileNameLabel->text().isEmpty())
                faxSend(m_openFileNameLabel->text(), m_destination->text());
}

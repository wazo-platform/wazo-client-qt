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

#include <QDebug>
#include <QFileDialog>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

#include "callcampaignpanel.h"
#include "userinfo.h"

/*! \brief Constructor
 */
CallCampaignPanel::CallCampaignPanel(QWidget * parent)
        : QWidget(parent), m_ui(NULL)
{
        qDebug() << "CallCampaignPanel::CallCampaignPanel()";
        
	m_vlayout = new QVBoxLayout(this);
	m_hlayout = new QHBoxLayout();
	m_glayout = new QGridLayout();

	QLabel * label = new QLabel(tr("File to load"), this);
        m_openFileNameLabel = new QLineEdit("", this);
//         connect(m_openFileNameLabel, SIGNAL(textChanged(const QString &)),
//                 this, SLOT(fileNameChanged(const QString &)));
        QPushButton * openFileNamesButton = new QPushButton( tr("Browse"), this);
        connect(openFileNamesButton, SIGNAL(clicked()),
                this, SLOT(setOpenFileName()));
        QPushButton * loadFile = new QPushButton( tr("Load"), this);
        connect(loadFile, SIGNAL(clicked()),
                this, SLOT(loadFileClicked()));
        QPushButton * listFromServer = new QPushButton( tr("Server"), this);
        connect(listFromServer, SIGNAL(clicked()),
                this, SLOT(getServerClicked()));
        m_hlayout->addWidget(label);
        m_hlayout->addWidget(m_openFileNameLabel);
        m_hlayout->addWidget(openFileNamesButton);
        m_hlayout->addWidget(loadFile);
        m_hlayout->addWidget(listFromServer);
        
        m_vlayout->addLayout(m_hlayout);
        m_vlayout->addLayout(m_glayout);
        m_vlayout->addStretch();
        
        m_glayout->setColumnStretch(1, 1);
}

void CallCampaignPanel::setUserInfo(const UserInfo * ui)
{
        m_ui = ui;
        qDebug() << "CallCampaignPanel::setUserInfo()" << m_ui->fullname();
}

void CallCampaignPanel::setOpenFileName()
{
        qDebug() << "CallCampaignPanel::setOpenFileName()";
        QFileDialog::Options options;
        /*        if (!native->isChecked())*/
        options |= QFileDialog::DontUseNativeDialog;
        QString selectedFilter;
        QString fileName = QFileDialog::getOpenFileName(this,
                                                        tr("Open PhoneList File"),
                                                        m_openFileNameLabel->text(),
                                                        tr("CSV Files (*.csv);;All Files (*)"),
                                                        &selectedFilter,
                                                        options);
        if (!fileName.isEmpty())
                m_openFileNameLabel->setText(fileName);
}


void CallCampaignPanel::addNumber(const QString & line)
{
        if(line.size() > 0) {
                QString numbertoadd = line.split(";")[0].trimmed();
                if(m_numbers.keys().contains(numbertoadd))
                        qDebug() << numbertoadd << "already exists";
                else {
                        m_numbers[numbertoadd] = "todo";
                        QLabel * ql = new QLabel(numbertoadd);
                        QPushButton * qpbstart = new QPushButton();
                        qpbstart->setIcon(QIcon(":/images/add.png"));
                        qpbstart->setIconSize(QSize(10, 10));
                        QPushButton * qpbstop  = new QPushButton();
                        qpbstop->setIcon(QIcon(":/images/cancel.png"));
                        qpbstop->setIconSize(QSize(10, 10));
                        m_glayout->addWidget(ql, m_numbers.size(), 0);
                        m_glayout->addWidget(qpbstart, m_numbers.size(), 2);
                        m_glayout->addWidget(qpbstop, m_numbers.size(), 3);
                }
        }
}

void CallCampaignPanel::fileNameChanged(const QString &)
{
//         if ((! m_openFileNameLabel->text().isEmpty()) && (! m_destination->text().isEmpty())) {
//                 m_sendButton->setEnabled(true);
//         } else {
//                 m_sendButton->setEnabled(false);
//         }
}

void CallCampaignPanel::requestFileListResult(const QStringList & qsl)
{
        qDebug() << "CallCampaignPanel::requestFileListResult()" << qsl;
        foreach(QString number, qsl) {
                addNumber(number);
        }
}

void CallCampaignPanel::getServerClicked()
{
        qDebug() << "CallCampaignPanel::getServerClicked()";
        requestFileList();
}

void CallCampaignPanel::loadFileClicked()
{
        qDebug() << "CallCampaignPanel::loadFileClicked()" << m_openFileNameLabel->text();
        if(m_openFileNameLabel->text().size() == 0)
                return;
        
        QFile * qf = new QFile(m_openFileNameLabel->text());
        qf->open(QFile::ReadOnly);
        QString line = "dummy";
        int nlines = 0;
        while(line.size() > 0) {
                QByteArray data = qf->readLine();
                line = QString::fromUtf8(data);
                addNumber(line);
                nlines ++;
        }
        qDebug() << nlines << "read";
        
//         QByteArray * filedata = new QByteArray();
//         filedata->append(qf->readAll());
        qf->close();
        //        qDebug() << "CallCampaignPanel::fileNameChanged() size = " << filedata->size();
}

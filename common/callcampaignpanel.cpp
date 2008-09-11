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
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

#include "callcampaignpanel.h"
#include "userinfo.h"

/*! \brief Constructor
 */
CallCampaignPanel::CallCampaignPanel(QWidget * parent)
        : QWidget(parent), m_ui(NULL)
{
        qDebug() << "CallCampaignPanel::CallCampaignPanel()";
        
	QHBoxLayout * hlayout = new QHBoxLayout(this);
        m_openFileNameLabel = new QLineEdit("", this);
        connect(m_openFileNameLabel, SIGNAL(textChanged(const QString &)),
                this, SLOT(fileNameChanged(const QString &)));
        QPushButton * openFileNamesButton = new QPushButton( tr("Browse"), this);
        connect(openFileNamesButton, SIGNAL(clicked()),
                this, SLOT(setOpenFileName()));
        hlayout->addWidget(m_openFileNameLabel);
        hlayout->addWidget(openFileNamesButton);
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
                                                        tr("Open Fax File"),
                                                        m_openFileNameLabel->text(),
                                                        tr("CSV Files (*.csv);;All Files (*)"),
                                                        &selectedFilter,
                                                        options);
        if (!fileName.isEmpty())
                m_openFileNameLabel->setText(fileName);
}

void CallCampaignPanel::fileNameChanged(const QString &)
{
        qDebug() << "FaxPanel::fileNameChanged()";
//         if ((! m_openFileNameLabel->text().isEmpty()) && (! m_destination->text().isEmpty())) {
//                 m_sendButton->setEnabled(true);
//         } else {
//                 m_sendButton->setEnabled(false);
//         }
}

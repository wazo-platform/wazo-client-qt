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
	m_hlayout1 = new QHBoxLayout();
	m_hlayout2 = new QHBoxLayout();
	m_glayout = new QGridLayout();

	QLabel * label1 = new QLabel(tr("File to load"), this);
        m_openFileNameLabel = new QLineEdit("", this);
//         connect(m_openFileNameLabel, SIGNAL(textChanged(const QString &)),
//                 this, SLOT(fileNameChanged(const QString &)));
        QPushButton * openFileNamesButton = new QPushButton( tr("Browse"), this);
        connect(openFileNamesButton, SIGNAL(clicked()),
                this, SLOT(setOpenFileName()));
        QPushButton * loadFile = new QPushButton( tr("Load"), this);
        connect(loadFile, SIGNAL(clicked()),
                this, SLOT(loadFileClicked()));

	QLabel * label2 = new QLabel(tr("Server"), this);
        QPushButton * getCallsButton = new QPushButton( tr("Get"), this);
        QPushButton * startCallsButton = new QPushButton( tr("Start"), this);
        QPushButton * stopCallsButton = new QPushButton( tr("Stop"), this);
        QPushButton * clearCallsButton = new QPushButton( tr("Clear"), this);
        connect(getCallsButton, SIGNAL(clicked()),
                this, SLOT(getCalls()));
        connect(startCallsButton, SIGNAL(clicked()),
                this, SLOT(startCalls()));
        connect(stopCallsButton, SIGNAL(clicked()),
                this, SLOT(stopCalls()));
        connect(clearCallsButton, SIGNAL(clicked()),
                this, SLOT(clearCalls()));
        
        m_hlayout1->addWidget(label1);
        m_hlayout1->addWidget(m_openFileNameLabel);
        m_hlayout1->addWidget(openFileNamesButton);
        m_hlayout1->addWidget(loadFile);
        
        m_hlayout2->addWidget(label2);
        m_hlayout2->addWidget(getCallsButton);
        m_hlayout2->addWidget(startCallsButton);
        m_hlayout2->addWidget(stopCallsButton);
        m_hlayout2->addWidget(clearCallsButton);
        
        m_vlayout->addLayout(m_hlayout1);
        m_vlayout->addLayout(m_hlayout2);
        m_vlayout->addLayout(m_glayout);
        m_vlayout->addStretch();
        
        m_glayout->setColumnStretch(2, 1);
}

void CallCampaignPanel::setGuiOptions(const QVariant &)
{
}

void CallCampaignPanel::setUserInfo(const UserInfo * ui)
{
        m_ui = ui;
        // qDebug() << "CallCampaignPanel::setUserInfo()" << m_ui->fullname();
}

void CallCampaignPanel::setOpenFileName()
{
        // qDebug() << "CallCampaignPanel::setOpenFileName()";
        QString selectedFilter;
        QString fileName = QFileDialog::getOpenFileName(this,
                                                        tr("Open PhoneList File"),
                                                        m_openFileNameLabel->text(),
                                                        tr("CSV Files (*.csv);;All Files (*)"),
                                                        &selectedFilter);
        if (!fileName.isEmpty())
                m_openFileNameLabel->setText(fileName);
}


void CallCampaignPanel::addNumber(const QString & line)
{
        if(line.size() > 0) {
                QString numbertoadd = line.split(";")[0].trimmed();
                if(! m_numbers.keys().contains(numbertoadd)) {
                        m_numbers[numbertoadd] = "new";
                        m_numlabel[numbertoadd] = new QLabel(numbertoadd);
                        m_qpbstart[numbertoadd] = new QPushButton();
                        m_qpbstart[numbertoadd]->setIcon(QIcon(":/images/bookmark.png"));
                        m_qpbstart[numbertoadd]->setIconSize(QSize(16, 16));
                        int nlines = m_glayout->rowCount();
                        m_glayout->addWidget(m_numlabel[numbertoadd], nlines, 0);
                        m_glayout->addWidget(m_qpbstart[numbertoadd], nlines, 1);
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

void CallCampaignPanel::requestFileListResult(const QVariant & result)
{
        // qDebug() << "CallCampaignPanel::requestFileListResult()" << result;
        QString action = result.toMap()["command"].toString();
        
        if(action == "fetchlist") {
                foreach(QString number, result.toMap()["list"].toStringList())
                        addNumber(number);
        } else if(action == "callstarted") {
                QString number = result.toMap()["number"].toString();
                m_qpbstart[number]->setIcon(QIcon(":/images/reload.png"));
                m_numbers[number] = "ongoing";
        } else if(action == "callstopped") {
                QString number = result.toMap()["number"].toString();
                m_qpbstart[number]->setIcon(QIcon(":/images/button_ok.png"));
                m_numbers[number] = "stopped";
        } else if(action == "callnext") {
                QString number = result.toMap()["number"].toString();
                m_qpbstart[number]->setIcon(QIcon(":/images/button_ok.png"));
                m_numbers[number] = "stopped";
                checkStatuses();
        }
}

void CallCampaignPanel::checkStatuses()
{
        QString toexec;
        QStringList keys = m_numbers.keys();
        keys.sort();
        foreach(QString key, keys) {
                if(m_numbers[key] == "ongoing") {
                        break;
                } else if(m_numbers[key] == "new") {
                        toexec = key;
                        break;
                }
        }
        if(toexec.size() > 0) {
                qDebug() << "CallCampaignPanel::checkStatuses()" << toexec;
                requestFileList("startcall " + toexec);
        }
}

void CallCampaignPanel::getCalls()
{
        // qDebug() << "CallCampaignPanel::getCalls()";
        requestFileList("fetchlist");
}

void CallCampaignPanel::startCalls()
{
        // qDebug() << "CallCampaignPanel::startCalls()";
        checkStatuses();
        // requestFileList("fetchlist");
}

void CallCampaignPanel::stopCalls()
{
        // qDebug() << "CallCampaignPanel::stopCalls()";
        QString toexec;
        QStringList keys = m_numbers.keys();
        keys.sort();
        foreach(QString key, keys) {
                if(m_numbers[key] == "ongoing") {
                        toexec = key;
                        break;
                }
        }
        if(toexec.size() > 0) {
                requestFileList("stopcall " + toexec);
        }
}

void CallCampaignPanel::clearCalls()
{
        // qDebug() << "CallCampaignPanel::clearCalls()";
        QStringList keys = m_numbers.keys();
        foreach(QString key, m_numbers.keys()) {
                if(m_numbers[key] == "stopped") {
                        m_numbers.remove(key);
                        delete m_qpbstart[key];
                        m_qpbstart.remove(key);
                        delete m_numlabel[key];
                        m_numlabel.remove(key);
                }
        }
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

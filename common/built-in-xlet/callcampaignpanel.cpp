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

#include "callcampaignpanel.h"

/*! \brief Constructor
 */
CallCampaignPanel::CallCampaignPanel(QWidget *parent)
    : XLet(parent)
{
    qDebug() << Q_FUNC_INFO;
    setTitle(tr("Call Campaign"));
        
    m_vlayout = new QVBoxLayout(this);
    m_hlayout1 = new QHBoxLayout();
    m_hlayout2 = new QHBoxLayout();
    m_glayout = new QGridLayout();

    QLabel * label1 = new QLabel(tr("File to load"), this);
    m_openFileNameLabel = new QLineEdit("", this);
    //         connect(m_openFileNameLabel, SIGNAL(textChanged(const QString &)),
    //                 this, SLOT(fileNameChanged(const QString &)));
    QPushButton *openFileNamesButton = new QPushButton(tr("Browse"), this);
    connect(openFileNamesButton, SIGNAL(clicked()),
            this, SLOT(setOpenFileName()));
    QPushButton *loadFile = new QPushButton(tr("Load"), this);
    connect(loadFile, SIGNAL(clicked()),
            this, SLOT(loadFileClicked()));

    QLabel *label2 = new QLabel(tr("Server"), this);
    QPushButton *getCallsButton = new QPushButton(tr("Get"), this);
    QPushButton *startCallsButton = new QPushButton(tr("Start"), this);
    QPushButton *stopCallsButton = new QPushButton(tr("Stop"), this);
    QPushButton *clearCallsButton = new QPushButton(tr("Clear"), this);
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

    // connects signals/slots with engine
    connect(this, SIGNAL(requestFileList(const QString &)),
            b_engine, SLOT(requestFileList(const QString &)));
    connect(b_engine, SIGNAL(requestFileListResult(const QVariant &)),
            this, SLOT(requestFileListResult(const QVariant &)));
}

void CallCampaignPanel::setOpenFileName()
{
    // qDebug() << Q_FUNC_INFO;
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
    // qDebug() << Q_FUNC_INFO << result;
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
        qDebug() << Q_FUNC_INFO << toexec;
        requestFileList("startcall " + toexec);
    }
}

void CallCampaignPanel::getCalls()
{
    // qDebug() << Q_FUNC_INFO;
    requestFileList("fetchlist");
}

void CallCampaignPanel::startCalls()
{
    // qDebug() << Q_FUNC_INFO;
    checkStatuses();
    // requestFileList("fetchlist");
}

void CallCampaignPanel::stopCalls()
{
    // qDebug() << Q_FUNC_INFO;
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
    // qDebug() << Q_FUNC_INFO;
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
    qDebug() << Q_FUNC_INFO << m_openFileNameLabel->text();
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
    qDebug() << Q_FUNC_INFO << nlines << "read";
        
    //         QByteArray * filedata = new QByteArray();
    //         filedata->append(qf->readAll());
    qf->close();
    //        qDebug() << Q_FUNC_INFO << "size = " << filedata->size();
}

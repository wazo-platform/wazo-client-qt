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


#include "faxpanel.h"
#include "phonenumber.h"

FaxPanel::FaxPanel(QWidget *parent)
    : XLet(parent), m_mainwindow(parent)
{
    // qDebug() << Q_FUNC_INFO << parent;
    setTitle( tr("Fax") );

    QVBoxLayout * vlayout = new QVBoxLayout(this);

    //
    QGroupBox * groupBox1 = new QGroupBox( tr("1. Choose Destination Number") );
    groupBox1->setAlignment( Qt::AlignLeft );
    QHBoxLayout * hbox1 = new QHBoxLayout( groupBox1 );

    QLabel * lblfax = new QLabel(tr("Fax Number"), this);
    m_destination = new QLineEdit(this);
    QPushButton * directory = new QPushButton( tr("Directory"), this);
    connect(directory, SIGNAL(clicked()),
            this, SLOT(dirLookup()));
    connect(m_destination, SIGNAL(textChanged(const QString &)),
            this, SLOT(destNumberChanged(const QString &)));
    hbox1->addWidget(lblfax);
    hbox1->addWidget(m_destination);
    hbox1->addWidget(directory);
    lblfax->setObjectName("fax");

    //
    QGroupBox * groupBox2 = new QGroupBox( tr("2. Choose File to Send") );
    groupBox2->setAlignment( Qt::AlignLeft );
    QHBoxLayout * hbox2 = new QHBoxLayout( groupBox2 );
    m_openFileNameLabel = new FileNameLineEdit(this);
    connect(m_openFileNameLabel, SIGNAL(textChanged(const QString &)),
            this, SLOT(fileNameChanged(const QString &)));
    QPushButton * openFileNamesButton = new QPushButton( tr("Browse"), this);
    connect(openFileNamesButton, SIGNAL(clicked()),
            this, SLOT(setOpenFileName()));
    hbox2->addWidget(m_openFileNameLabel);
    hbox2->addWidget(openFileNamesButton);

    //
    QGroupBox * groupBox3 = new QGroupBox( tr("3. Hide the Number ?") );
    groupBox3->setAlignment( Qt::AlignLeft );
    QHBoxLayout * hbox3 = new QHBoxLayout( groupBox3 );

    int previous_hide = b_engine->getSettings()->value("faxhistory/hidenumber", 0).toUInt();
    m_maskornot = new QCheckBox(tr("Hide Number"), this);
    m_maskornot->setCheckState((Qt::CheckState)previous_hide);
    hbox3->addStretch(1);
    hbox3->addWidget(m_maskornot);
    hbox3->addStretch(1);

    //
    QGroupBox * groupBox4 = new QGroupBox( tr("4. Send your File") );
    groupBox4->setAlignment( Qt::AlignLeft );
    QHBoxLayout * hbox4 = new QHBoxLayout( groupBox4 );

    m_sendButton = new QPushButton( tr("Send"), this);
    m_sendButton->setEnabled(false);
    connect(m_sendButton, SIGNAL(clicked()),
            this, SLOT(sendFax()));
    hbox4->addStretch(1);
    hbox4->addWidget(m_sendButton);
    hbox4->addStretch(1);

    vlayout->addWidget(groupBox1);
    vlayout->addWidget(groupBox2);
    vlayout->addWidget(groupBox3);
    vlayout->addWidget(groupBox4);
    vlayout->addStretch(1);

    // connect signals / slots
    connect( b_engine, SIGNAL(ackFax(const QString &, const QString &)),
             this, SLOT(popupMsg(const QString &, const QString &)) );
}


FaxPanel::~FaxPanel()
{
    // qDebug() << Q_FUNC_INFO;
    b_engine->getSettings()->setValue("faxhistory/hidenumber", m_maskornot->checkState());
}

void FaxPanel::destNumberChanged(const QString &/* ext*/)
{
    // qDebug() << Q_FUNC_INFO << ext;
    if ((! m_openFileNameLabel->text().isEmpty()) && (! m_destination->text().isEmpty())) {
        m_sendButton->setEnabled(true);
    } else {
        m_sendButton->setEnabled(false);
    }
}

void FaxPanel::fileNameChanged(const QString &)
{
    // qDebug() << Q_FUNC_INFO << ext;
    if ((! m_openFileNameLabel->text().isEmpty()) && (! m_destination->text().isEmpty())) {
        m_sendButton->setEnabled(true);
    } else {
        m_sendButton->setEnabled(false);
    }
}

void FaxPanel::setOpenFileName()
{
    // There has been an option set as QFileDialog::DontUseNativeDialog
    // previously, probably because of a too quick copy/paste from an example.
    // While on Linux platforms, it makes no difference, on MacOS it is worth
    // not setting it, in order for special places like "Volumes" to be seen.
    QString open_path = m_openFileNameLabel->text();
    if (open_path.isEmpty()) {
        open_path = QDir::toNativeSeparators(QDir::homePath());
    }
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Open Fax File"),
                                                    open_path,
                                                    tr("PDF Files (*.pdf);;All Files (*)"));
    if (!fileName.isEmpty())
        m_openFileNameLabel->setText(fileName);
}


void FaxPanel::sendFax()
{
    if ((! m_openFileNameLabel->text().isEmpty()) && (! m_destination->text().isEmpty())) {
        // qDebug() << Q_FUNC_INFO
        // << m_openFileNameLabel->text()
        // << m_destination->text()
        // << m_maskornot->checkState();
        // m_sendButton->setEnabled(false);
        m_dest_string = m_destination->text();
        m_file_string = m_openFileNameLabel->text();
        m_destination->clear();
        m_openFileNameLabel->clear();
        b_engine->sendFaxCommand(m_file_string,
                                 m_dest_string,
                                 m_maskornot->checkState());
    }
}

void FaxPanel::dirLookup()
{
    // qDebug() << Q_FUNC_INFO;
    DirDialog dirdialog(m_mainwindow);
    connect(dirdialog.dirpanel(), SIGNAL(selectedText(const QString &)),
            m_destination, SLOT(setText(const QString &)));
    QString old_destination = m_destination->text();
    int ret = dirdialog.exec();
    if (ret == QDialog::Rejected)
        m_destination->setText(old_destination);
}

void FaxPanel::popupMsg(const QString & status, const QString & reason)
{
    static QMessageBox msgbox;
    QMessageBox::Icon icon;
    QString text;

    // qDebug() << Q_FUNC_INFO << status << reason;

    if(status == "ok") {
        icon = QMessageBox::Information;
        text = tr("Your Fax (file %1)\n"
                  "was successfully sent to %2.").arg(m_file_string, m_dest_string);
    } else if (status == "queued") {
        icon = QMessageBox::Information;
        text = tr("Your Fax (file %1)\n"
                  "is being processed and will be sent soon.").arg(m_file_string);
    } else {
        QString faxreason;
        if(reason == "filenotfound")
            faxreason = tr("File not found");
        else if(reason == "fileempty")
            faxreason = tr("Empty file");
        else if(reason == "orig")
            faxreason = tr("Problem when dialing the number");
        else if(reason == "convert-pdftif")
            faxreason = tr("Unable to convert your PDF to TIFF");
        else if(reason == "filetype")
            faxreason = tr("Document not a PDF");
        else if(reason == "AMI")
            faxreason = tr("Dialog between CTI server and IPBX");
        else if(reason == "exists-pathspool")
            faxreason = tr("(Server) missing directory");
        else if(reason == "unknown")
            faxreason = tr("Unknown");
        else
            faxreason = tr("Not given");
        icon = QMessageBox::Critical;
        text = tr("Your Fax (file %1)\n"
                  "was NOT sent to %2.\n"
                  "Reason given : %3.").arg(m_file_string, m_dest_string, faxreason);
        m_destination->setText(m_dest_string);
        m_openFileNameLabel->setText(m_file_string);
    }

    msgbox.setWindowTitle("XiVO CTI (Fax)");
    msgbox.setIcon(icon);
    msgbox.setText(text);
    msgbox.show();
}

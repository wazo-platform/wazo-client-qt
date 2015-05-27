/* XiVO Client
 * Copyright (C) 2007-2015 Avencall
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

#include <QMovie>

#include <baseengine.h>

#include "fax.h"
#include "phonenumber.h"

Fax::Fax(QWidget *parent)
    : XLet(parent, tr("Fax"), ":/images/tab-fax.svg"),
      m_mainwindow(parent),
      m_waiting_status(NULL)
{
    this->ui.setupUi(this);

    m_waiting_status = new QMovie(":/images/waiting-status.gif");

    connect( this->ui.file_name_input, SIGNAL(textChanged(const QString &)),
             this, SLOT(fileNameChanged(const QString &)) );
    connect( this->ui.file_browse_button, SIGNAL(clicked()),
             this, SLOT(setOpenFileName()) );

    connect( this->ui.fax_number_search_button, SIGNAL(clicked()),
             this, SLOT(dirLookup()) );
    connect( this->ui.fax_number_input, SIGNAL(textChanged(const QString &)),
             this, SLOT(destNumberChanged(const QString &)) );

    connect( this->ui.send_fax_button, SIGNAL(clicked()),
             this, SLOT(sendFax()) );

    registerListener("faxstatus");
}

void Fax::parseCommand(const QVariantMap &map)
{
    QString status = map.value("status").toString();
    int nb_pages_sent = map.value("pages").toInt();
    QString pages_sent = tr("%1 pages sent").arg(nb_pages_sent);

    if (status == "SUCCESS") {
        this->ui.fax_status->setPixmap(QPixmap(":/images/button_ok.png"));
        this->ui.fax_status->setToolTip(tr("SUCCESS - %1").arg(pages_sent));
    } else {
        this->ui.fax_status->setPixmap(QPixmap(":/images/cancel.png"));
        this->ui.fax_status->setToolTip(tr("FAILED"));
    }
    this->setEnabledFaxWidget(true);
}

void Fax::destNumberChanged(const QString &/* ext*/)
{
    if ((! this->ui.file_name_input->text().isEmpty()) && (! this->ui.fax_number_input->text().isEmpty())) {
        this->ui.send_fax_button->setEnabled(true);
    } else {
        this->ui.send_fax_button->setEnabled(false);
    }
}

void Fax::fileNameChanged(const QString &)
{
    if ((! this->ui.file_name_input->text().isEmpty()) && (! this->ui.fax_number_input->text().isEmpty())) {
        this->ui.send_fax_button->setEnabled(true);
    } else {
        this->ui.send_fax_button->setEnabled(false);
    }
}

void Fax::setOpenFileName()
{
    // There has been an option set as QFileDialog::DontUseNativeDialog
    // previously, probably because of a too quick copy/paste from an example.
    // While on Linux platforms, it makes no difference, on MacOS it is worth
    // not setting it, in order for special places like "Volumes" to be seen.
    QString open_path = this->ui.file_name_input->text();
    if (open_path.isEmpty()) {
        open_path = QDir::toNativeSeparators(QDir::homePath());
    }
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Open Fax File"),
                                                    open_path,
                                                    tr("PDF Files (*.pdf);;All Files (*)"));
    if (!fileName.isEmpty())
        this->ui.file_name_input->setText(fileName);
}


void Fax::sendFax()
{
    if ((! this->ui.file_name_input->text().isEmpty()) && (! this->ui.fax_number_input->text().isEmpty())) {
        m_dest_string = this->ui.fax_number_input->text();
        m_file_string = this->ui.file_name_input->text();
        this->ui.fax_number_input->clear();
        this->ui.file_name_input->clear();
        b_engine->sendFaxCommand(m_file_string,
                                 m_dest_string);
        this->setWaitingStatus();
    }
}

void Fax::setWaitingStatus()
{
    this->ui.fax_status->setMovie(m_waiting_status);
    this->ui.fax_status->movie()->start();
    this->ui.fax_status->setToolTip(tr("Sending"));
    this->setEnabledFaxWidget(false);
}

void Fax::setEnabledFaxWidget(bool enabled)
{
    this->ui.file_name_input->setEnabled(enabled);
    this->ui.fax_number_input->setEnabled(enabled);
    this->ui.file_browse_button->setEnabled(enabled);
    this->ui.fax_number_search_button->setEnabled(enabled);
}

void Fax::dirLookup()
{
    DirDialog dirdialog(m_mainwindow);
    connect(dirdialog.dirpanel(), SIGNAL(selectedText(const QString &)),
            this->ui.fax_number_input, SLOT(setText(const QString &)));
    QString old_destination = this->ui.fax_number_input->text();
    int ret = dirdialog.exec();
    if (ret == QDialog::Rejected)
        this->ui.fax_number_input->setText(old_destination);
}

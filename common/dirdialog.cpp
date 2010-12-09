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

#include "dirdialog.h"

DirDialog::DirDialog(QWidget *parent)
    : QDialog(parent)
{
    restoreGeometry(b_engine->getSettings()->value("faxhistory/geometry").toByteArray());
    // the object will be destroyed when closed
    setWindowTitle(tr("Directory"));

    QVBoxLayout *vlayout = new QVBoxLayout(this);
    m_directory = new DirectoryPanel(this);
    connect(m_directory, SIGNAL(searchDirectory(const QString &)),
            b_engine, SLOT(searchDirectory(const QString &)));
    connect(m_directory, SIGNAL(copyNumber(const QString &)),
            this, SLOT(copyNumber(const QString &)));
    connect(m_directory, SIGNAL(emitDial(const QString &, bool)),
            this, SLOT(copyNumberAndQuit(const QString &, bool)));

    m_btnbox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this);
    connect(m_btnbox, SIGNAL(accepted()),
            this, SLOT(saveAndClose()));
    connect(m_btnbox, SIGNAL(rejected()),
            this, SLOT(close()));
    m_btnbox->button(QDialogButtonBox::Cancel)->setDefault(false);
    m_btnbox->button(QDialogButtonBox::Ok)->setDefault(false);

    vlayout->addWidget(m_directory);
    vlayout->addWidget(m_btnbox);

    m_faxnumber = "";
    m_retfaxnumber = "";
}

/*! \brief Destructor */
DirDialog::~DirDialog()
{
    // qDebug() << Q_FUNC_INFO;
    b_engine->getSettings()->setValue("faxhistory/geometry", saveGeometry() );
}

const QString & DirDialog::faxnumber() const
{
    return m_retfaxnumber;
}

void DirDialog::copyNumber(const QString & number)
{
    m_faxnumber = number;
}

void DirDialog::copyNumberAndQuit(const QString & number, bool)
{
    m_faxnumber = number;
    saveAndClose();
}

void DirDialog::saveAndClose()
{
    m_retfaxnumber = m_faxnumber;
    if(m_retfaxnumber.size() > 0)
        close();
}

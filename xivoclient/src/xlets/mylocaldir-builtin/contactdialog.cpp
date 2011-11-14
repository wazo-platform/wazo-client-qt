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

#include <QFormLayout>
#include <QDialogButtonBox>
#include "contactdialog.h"

ContactDialog::ContactDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Contact"));
    QFormLayout *layout = new QFormLayout(this);
    m_firstname = new QLineEdit();
    layout->addRow(tr("First Name"), m_firstname);
    m_lastname = new QLineEdit();
    layout->addRow(tr("Last Name"), m_lastname);
    m_phonenumber = new QLineEdit();
    layout->addRow(tr("Phone Number"), m_phonenumber);
    m_email = new QLineEdit();
    layout->addRow(tr("Email Address"), m_email);
    m_company = new QLineEdit();
    layout->addRow(tr("Company"), m_company);
    m_faxnumber = new QLineEdit();
    layout->addRow(tr("Fax Number"), m_faxnumber);
    m_mobilenumber = new QLineEdit();
    layout->addRow(tr("Mobile Number"), m_mobilenumber);
    QDialogButtonBox *buttonBox \
        = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    layout->addRow(buttonBox);
}

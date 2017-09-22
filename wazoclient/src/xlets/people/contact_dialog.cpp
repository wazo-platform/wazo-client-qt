/*
 * Copyright 2015-2017 The Wazo Authors  (see the AUTHORS file)
 *
 * This program is free software: you can redistribute it and/or modify
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
 * This programm is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "contact_dialog.h"


ContactDialog::ContactDialog(QWidget *parent,
                             const QVariantMap &contact_infos,
                             const QString &source_name,
                             const QString &source_entry_id)
    : QDialog(parent),
      m_source_entry_id(source_entry_id),
      m_source_name(source_name),
      m_contact_infos(contact_infos)
{
    this->ui.setupUi(this);
    this->setSizeGripEnabled(true);
    connect(this->ui.confirm_button_box, SIGNAL(accepted()),
            this, SLOT(accept()));
    connect(this->ui.confirm_button_box, SIGNAL(rejected()),
           this, SLOT(reject()));

    this->ui.firstname_input->setText(m_contact_infos.value("firstname").toString());
    this->ui.lastname_input->setText(m_contact_infos.value("lastname").toString());
    this->ui.phone_number_input->setText(m_contact_infos.value("number").toString());
    this->ui.mobile_number_input->setText(m_contact_infos.value("mobile").toString());
    this->ui.fax_input->setText(m_contact_infos.value("fax").toString());
    this->ui.email_input->setText(m_contact_infos.value("email").toString());
    this->ui.company_input->setText(m_contact_infos.value("company").toString());
}

ContactDialog::~ContactDialog()
{
}

void ContactDialog::accept()
{
    m_contact_infos.insert("firstname", this->ui.firstname_input->text());
    m_contact_infos.insert("lastname",  this->ui.lastname_input->text());
    m_contact_infos.insert("number", this->ui.phone_number_input->text());
    m_contact_infos.insert("mobile", this->ui.mobile_number_input->text());
    m_contact_infos.insert("fax", this->ui.fax_input->text());
    m_contact_infos.insert("email", this->ui.email_input->text());
    m_contact_infos.insert("company", this->ui.company_input->text());
    emit acceptedWithInfos(m_source_name, m_source_entry_id, m_contact_infos);
    QDialog::accept();
}

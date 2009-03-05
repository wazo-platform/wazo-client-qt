/* XIVO CTI clients
 * Copyright (C) 2007-2009  Proformatique
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
#include <QDialogButtonBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSettings>
#include <QVBoxLayout>

#include "baseengine.h"
#include "dirdialog.h"
#include "directorypanel.h"

/*! \brief constructor
 */
DirDialog::DirDialog(BaseEngine * engine, QWidget * parent)
    : QDialog(parent), m_engine(engine)
{
    restoreGeometry(m_engine->getSettings()->value("faxhistory/geometry").toByteArray());
    // the object will be destroyed when closed
    setWindowTitle(tr("Directory"));

    QVBoxLayout * vlayout = new QVBoxLayout(this);
    m_directory = new DirectoryPanel( m_engine, this);
    connect( m_directory, SIGNAL(searchDirectory(const QString &)),
             m_engine, SLOT(searchDirectory(const QString &)) );
    connect( m_engine, SIGNAL(directoryResponse(const QString &)),
             m_directory, SLOT(setSearchResponse(const QString &)) );
    connect( m_directory, SIGNAL(copyNumber(const QString &)),
             this, SLOT(copyNumber(const QString &)) );
    connect( m_directory, SIGNAL(emitDial(const QString &, bool)),
             this, SLOT(copyNumberAndQuit(const QString &, bool)) );

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

DirDialog::~DirDialog()
{
    // qDebug() << "DirDialog::~DirDialog()";
    m_engine->getSettings()->setValue("faxhistory/geometry", saveGeometry() );
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

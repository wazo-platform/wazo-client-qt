/* XIVO CTI clients
Copyright (C) 2007  Proformatique

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
*/

/* $Revision$
   $Date$
*/

#include <QDebug>
#include <QDialogButtonBox>
#include <QLabel>
#include <QLineEdit>
#include <QSettings>
#include <QVBoxLayout>

#include "dirdialog.h"
#include "directorypanel.h"
#include "mainwidget.h"

/*! \brief constructor
 */
DirDialog::DirDialog(BaseEngine * engine, QWidget * parent)
        : QDialog(parent), m_engine(engine)
{
        QSettings settings;
	restoreGeometry(settings.value("faxhistory/geometry").toByteArray());
	// the object will be destroyed when closed
	setWindowTitle(tr("Directory"));

	QVBoxLayout * vlayout = new QVBoxLayout(this);
        m_directory = new DirectoryPanel(this);
        connect( m_directory, SIGNAL(searchDirectory(const QString &)),
                 m_engine, SLOT(searchDirectory(const QString &)) );
        connect( m_engine, SIGNAL(directoryResponse(const QString &)),
                 m_directory, SLOT(setSearchResponse(const QString &)) );
        connect( m_directory, SIGNAL(copyNumber(const QString &)),
                 this, SLOT(copyNumber(const QString &)) );
        connect( m_directory, SIGNAL(emitDial(const QString &)),
                 this, SLOT(copyNumberAndQuit(const QString &)) );

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
        QSettings settings;
	settings.setValue("faxhistory/geometry", saveGeometry() );
}

const QString & DirDialog::faxnumber() const
{
	return m_retfaxnumber;
}

void DirDialog::copyNumber(const QString & number)
{
        m_faxnumber = number;
}

void DirDialog::copyNumberAndQuit(const QString & number)
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

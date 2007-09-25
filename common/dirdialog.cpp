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

#include <QCheckBox>
#include <QComboBox>
#include <QDebug>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QVBoxLayout>

#include "dirdialog.h"
#include "mainwidget.h"
#include "directorypanel.h"

/*! \brief constructor
 */
DirDialog::DirDialog(QWidget * parent)
        : QDialog(parent)
{
	int line = 0;
	setModal(true);
	// the object will be destroyed when closed
	setAttribute(Qt::WA_DeleteOnClose);
	setWindowTitle(tr("Directory"));

        DirectoryPanel * directory = new DirectoryPanel(this);


// 	vlayout->addLayout(gridlayout);
//         vlayout->addStretch(1);
// 	vlayout->addWidget(m_btnbox);
}

/*!
 * This slot saves the configuration (which is stored in displayed
 * widgets) to the BaseEngine object
 * and also to the main window object and then call close()
 */
void DirDialog::saveAndClose()
{
        close();
}

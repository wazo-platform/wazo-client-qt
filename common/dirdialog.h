/* XIVO CTI clients
Copyright (C) 2007, 2008  Proformatique

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
 * $Date$
 */

#ifndef __DIRDIALOG_H__
#define __DIRDIALOG_H__

#include <QCheckBox>
#include <QComboBox>
#include <QDialog>
#include <QLineEdit>
#include <QSpinBox>

class QDialogButtonBox;

class BaseEngine;
class DirectoryPanel;

//class MainWidget;

/*! \brief Configuration Window
 *
 * This Widget enables the user to edit the connection
 * parameters to the identification server */
/* could be a QDialog instead of QWidget */
//class DirDialog: public QWidget
class DirDialog: public QDialog
{
	Q_OBJECT
public:
	/*! \brief Constructor
	 *
	 * Construct the widget and its layout.
	 * Fill widgets with values got from the BaseEngine object.
	 * Once constructed, the Widget is ready to be shown.
	 * \param engine	related BaseEngine object where parameters will be modified
	 * \param parent	parent QWidget
	 */
	DirDialog(BaseEngine *, QWidget *);
        ~DirDialog();
        const QString & faxnumber() const;
private slots:
	//! Save the configuration to the BaseEngine object and close
	void saveAndClose();
        void copyNumber(const QString &);
        void copyNumberAndQuit(const QString &);
private:
        BaseEngine * m_engine;
        DirectoryPanel * m_directory;
        QDialogButtonBox * m_btnbox;	//!< Buttons box
        QString m_faxnumber;
        QString m_retfaxnumber;
};
#endif

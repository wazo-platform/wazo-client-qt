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
	DirDialog(QWidget *);
	//DirDialog(BaseEngine *engine, QWidget *parent = 0);
private slots:
	//! Save the configuration to the BaseEngine object and close
	void saveAndClose();
private:
	QLineEdit * m_serverhost;	//!< IP/hostname of the server
	QLineEdit * m_sbport;		//!< server port (switchboard)
	QLineEdit * m_loginport;	//!< port of the server

	QCheckBox * m_autoconnect;	//!< Auto connect checkbox
	QCheckBox * m_trytoreconnect;	//!< "Try to reconnect" Checkbox
	QSpinBox  * m_tryinterval_sbox;	//!< "Try to reconnect" interval
	QSpinBox  * m_kainterval_sbox;	//!< Keep alive interval

	QLineEdit * m_asterisk;		//!< id name of the Asterisk server
	QComboBox * m_protocombo;	//!< Protocol SIP/IAX
	QLineEdit * m_userid;		//!< user login
	QLineEdit * m_passwd;		//!< user password

	QSpinBox  * m_history_sbox;	//!< History size
	QSpinBox  * m_tablimit_sbox;	//!< Maximum number of tabs
	QCheckBox * m_tcpmode;		//!< Use outgoing TCP connection
	QCheckBox * m_lastconnwins;	//!< The last connected user wins => disconnects the other
	QCheckBox * m_presence;		//!< connect to presence server checkbox
	QCheckBox * m_cinfo;		//!< customer info checkbox

        QDialogButtonBox * m_btnbox;	//!< Buttons box
};
#endif

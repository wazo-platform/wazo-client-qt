/* XIVO CTI clients
 * Copyright (C) 2007, 2008  Proformatique
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

#ifndef __CONFWIDGET_H__
#define __CONFWIDGET_H__

#include <QDialog>
#include <QHash>

class QCheckBox;
class QComboBox;
class QDialogButtonBox;
class QLabel;
class QLineEdit;
class QSpinBox;
class QTabWidget;

class BaseEngine;
class MainWidget;

/*! \brief Configuration Window
 *
 * This Widget enables the user to edit the connection
 * parameters to the identification server */
/* could be a QDialog instead of QWidget */
//class ConfWidget: public QWidget
class ConfWidget: public QDialog
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
	ConfWidget(BaseEngine *,
                   MainWidget *);
	~ConfWidget();

private slots:
	//! Save the configuration to the BaseEngine object and close
	void saveAndClose();
        void loginKindChanged(int);

private:
	BaseEngine * m_engine;		//!< BaseEngine object parameters are commited to
	MainWidget * m_mainwindow;	//!< MainWidget where some parameters are commited to

	QLineEdit * m_serverhost;	//!< IP/hostname of the server
	QLineEdit * m_sbport;		//!< server port (switchboard)

	QCheckBox * m_autoconnect;	//!< "Auto connect" checkbox
	QCheckBox * m_trytoreconnect;	//!< "Try to reconnect" Checkbox
	QCheckBox * m_systrayed;	//!< "Systray at startup" Checkbox
	QSpinBox  * m_tryinterval_sbox;	//!< "Try to reconnect" interval
	QSpinBox  * m_kainterval_sbox;	//!< Keep alive interval

	QLineEdit * m_company;		//!< name of the company
	QLineEdit * m_passwd;		//!< user password
	QCheckBox * m_keeppass;		//!< keep password ?
	QLineEdit * m_userid;		//!< user login
	QCheckBox * m_loginkind;	//!< login kind (user or agent)
	QLineEdit * m_phonenumber;	//!< agent's phone number

        QLabel * m_lblulogin;
        QLabel * m_lblcompany;
        QLabel * m_lblpasswd;
        QLabel * m_lblphone;

	QCheckBox * m_presence;		//!< connect to presence server checkbox
	QCheckBox * m_cinfo;		//!< customer info checkbox
	QCheckBox * m_autourl;		//!< auto url allow checkbox
	QSpinBox  * m_history_sbox;	//!< History size

        QHash<QString, QSpinBox *> m_queuelevels;

	QSpinBox  * m_contactssize_sbox;	//!< Displayed contacts' size
	QSpinBox  * m_contactswidth_sbox;	//!< Displayed contacts' width
	QSpinBox  * m_tablimit_sbox;	//!< Maximum number of tabs
	QCheckBox * m_lastconnwins;	//!< The last connected user wins => disconnects the other

        QDialogButtonBox * m_btnbox;	//!< Buttons box
        QTabWidget * m_tabwidget;
};
#endif

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
#include <QSettings>
#include <QSpinBox>
#include <QVBoxLayout>

#include "confwidget.h"
#include "mainwidget.h"
#include "baseengine.h"

/*! \brief constructor
 */
ConfWidget::ConfWidget(BaseEngine * engine,
                       MainWidget * parent)
        : QDialog(parent), m_engine(engine), m_mainwindow(parent),
          m_presence(NULL), m_cinfo(NULL)
{
	int line = 0;
	// the object will be destroyed when closed
	setAttribute(Qt::WA_DeleteOnClose);
	setWindowTitle(tr("Configuration"));

	QVBoxLayout * vlayout = new QVBoxLayout(this);
	QTabWidget * tabwidget = new QTabWidget();
        
        //
        // Connection Tab
        //
        
	QGridLayout * gridlayout1 = new QGridLayout();
        QWidget * widget_connection = new QWidget();
        widget_connection->setLayout(gridlayout1);

        //      QStringList childgroups = m_engine->getSettings()->childGroups();
        // 	QLabel * lblprofile = new QLabel(tr("Profile"), this);
        // 	QComboBox * profile = new QComboBox(this);
        // 	profile->addItem(QString(tr("Default")));
        //      profile->setCurrentIndex(0);
        // 	gridlayout1->addWidget(lblprofile, line, 0);
        // 	gridlayout1->addWidget(profile, line++, 1);

	QLabel * lblhost = new QLabel(tr("Server Host"), this);
	m_serverhost = new QLineEdit(m_engine->serverip(), this);
	gridlayout1->addWidget(lblhost, line, 0);
	gridlayout1->addWidget(m_serverhost, line++, 1);

        QLabel * lblsbport = new QLabel(tr("Login Port"), this);
        m_sbport = new QLineEdit( QString::number(m_engine->sbPort()), this);
        m_sbport->setInputMask("99999");
        gridlayout1->addWidget(lblsbport, line, 0);
        gridlayout1->addWidget(m_sbport, line++, 1);
        m_sbport->setEnabled(true);

 	gridlayout1->setRowStretch( line, 1 );
        
        //
        // Functions Tab
        //
        
        QVBoxLayout * vbox = new QVBoxLayout();
        QWidget * widget_functions = new QWidget();
        widget_functions->setLayout(vbox);

        QStringList qsl = m_engine->getSettings()->value("display/capas", "").toString().split(",");
        if(qsl.contains("presence")) {
                m_presence = new QCheckBox(tr("Presence reporting"));
                m_presence->setCheckState(m_engine->checkedPresence() ? Qt::Checked : Qt::Unchecked);
                vbox->addWidget( m_presence );
        }

        if(qsl.contains("customerinfo")) {
                m_cinfo = new QCheckBox(tr("Customer Info"));
                m_cinfo->setCheckState(m_engine->checkedCInfo() ? Qt::Checked : Qt::Unchecked);
                vbox->addWidget( m_cinfo );
        }

        m_autourl = new QCheckBox(tr("Allow the Automatic Opening of URLs"));
        m_autourl->setCheckState(m_engine->checkedAutoUrl() ? Qt::Checked : Qt::Unchecked);
        vbox->addWidget( m_autourl );

 	vbox->addStretch(1);

        // Box for Connection Definition
        //	QGroupBox * groupBox_conn = new QGroupBox( tr("Identification"), this );
        //	groupBox_conn->setAlignment( Qt::AlignHCenter );
        //	QVBoxLayout * vbox_conn = new QVBoxLayout( groupBox_conn );

        
        //
        // User Account's Tab
        //
        
	QGridLayout * gridlayout3 = new QGridLayout();
        QWidget * widget_user = new QWidget();
        widget_user->setLayout(gridlayout3);

	m_lblulogin = new QLabel(tr("User Login"), this);
	m_userid = new QLineEdit(m_engine->userId(), this);
	gridlayout3->addWidget(m_lblulogin, line, 0);
	gridlayout3->addWidget(m_userid, line++, 1);

	m_lblpasswd = new QLabel(tr("Password"), this);
	m_passwd = new QLineEdit(m_engine->password(), this);
	m_passwd->setEchoMode(QLineEdit::Password);
	gridlayout3->addWidget(m_lblpasswd, line, 0);
	gridlayout3->addWidget(m_passwd, line++, 1);

        QLabel * lblloginkind = new QLabel(tr("Login as Agent"), this);
	m_loginkind = new QCheckBox(this);
	m_loginkind->setCheckState((m_engine->loginkind() == 2) ? Qt::Checked : Qt::Unchecked);
 	gridlayout3->addWidget(lblloginkind, line, 0);
	gridlayout3->addWidget(m_loginkind, line++, 1);

	connect( m_loginkind, SIGNAL(stateChanged(int)),
	         this, SLOT(loginKindChanged(int)) );

        QFrame * qhline3 = new QFrame(this);
        qhline3->setFrameShape(QFrame::HLine);
        gridlayout3->addWidget(qhline3, line++, 0, 1, 0);

        m_lblasterisk = new QLabel(tr("XIVO Id"), this);
	m_asterisk = new QLineEdit(m_engine->serverast(), this);
        gridlayout3->addWidget(m_lblasterisk, line, 0);
        gridlayout3->addWidget(m_asterisk, line++, 1);

        QFrame * qhline4 = new QFrame(this);
        qhline4->setFrameShape(QFrame::HLine);
        gridlayout3->addWidget(qhline4, line++, 0, 1, 0);

	m_lblphone = new QLabel(tr("Phone Number"), this);
        m_phonenumber = new QLineEdit(m_engine->phonenumber(), this);
        gridlayout3->addWidget(m_lblphone, line, 0);
        gridlayout3->addWidget(m_phonenumber, line++, 1);

	QLabel * lblproto = new QLabel(tr("Protocol"), this);
	m_protocombo = new QComboBox(this);
	m_protocombo->addItem(QString("SIP"));
	m_protocombo->addItem(QString("IAX"));
	if(m_engine->protocol().toLower() == QString("sip"))
		m_protocombo->setCurrentIndex(0);
	else
		m_protocombo->setCurrentIndex(1);
	gridlayout3->addWidget(lblproto, line, 0);
	gridlayout3->addWidget(m_protocombo, line++, 1);


        QFrame * qhline5 = new QFrame(this);
        qhline5->setFrameShape(QFrame::HLine);
        gridlayout3->addWidget(qhline5, line++, 0, 1, 0);

	m_lblalogin = new QLabel(tr("Agent Id"), this);
	m_agentid = new QLineEdit(m_engine->agentId(), this);
	gridlayout3->addWidget(m_lblalogin, line, 0);
	gridlayout3->addWidget(m_agentid, line++, 1);

        this->loginKindChanged(m_loginkind->checkState());

 	gridlayout3->setRowStretch( line, 1 );

        //
        // GUI Settings
        //

	QGridLayout * gridlayout4 = new QGridLayout();
        QWidget * widget_gui = new QWidget();
        widget_gui->setLayout(gridlayout4);

        QString lastconn = tr("The last connected one takes on the login");
        //m_lastconnwins = new QCheckBox(tr("The last connected one takes on the login"), this);
 	//m_lastconnwins->setCheckState(m_engine->lastconnwins() ? Qt::Checked : Qt::Unchecked);
        //gridlayout4->addWidget(m_lastconnwins, line++, 0, 1, 0);

	m_autoconnect = new QCheckBox(tr("Autoconnect at startup"), this);
	m_autoconnect->setCheckState(m_engine->autoconnect() ? Qt::Checked : Qt::Unchecked);
	gridlayout4->addWidget(m_autoconnect, line++, 0, 1, 0);

	m_trytoreconnect = new QCheckBox(tr("Try to reconnect") + "\n" + \
                                         tr("Checking this box disables the Error Popups"), this);
	m_trytoreconnect->setCheckState(m_engine->trytoreconnect() ? Qt::Checked : Qt::Unchecked);
	gridlayout4->addWidget(m_trytoreconnect, line++, 0, 1, 0);
	gridlayout4->addWidget(new QLabel(tr("Try to reconnect interval"), this), line, 0);
	m_tryinterval_sbox = new QSpinBox(this);
	m_tryinterval_sbox->setRange(1, 120);
	m_tryinterval_sbox->setValue(m_engine->trytoreconnectinterval() / 1000);
	gridlayout4->addWidget(m_tryinterval_sbox, line++, 1);

	gridlayout4->addWidget(new QLabel(tr("Keep alive interval"), this), line, 0);
	m_kainterval_sbox = new QSpinBox(this);
	m_kainterval_sbox->setRange(1, 120);
	m_kainterval_sbox->setValue(m_engine->keepaliveinterval() / 1000);
	gridlayout4->addWidget(m_kainterval_sbox, line++, 1);

	gridlayout4->addWidget(new QLabel(tr("Tab limit"), this), line, 0);
	m_tablimit_sbox = new QSpinBox(this);
	m_tablimit_sbox->setRange(0, 99);
	m_tablimit_sbox->setValue(m_mainwindow->tablimit());
	gridlayout4->addWidget(m_tablimit_sbox, line++, 1);

	gridlayout4->addWidget(new QLabel(tr("History size"), this), line, 0);
	m_history_sbox = new QSpinBox(this);
	m_history_sbox->setRange(1, 20);
	m_history_sbox->setValue(m_engine->historySize());
	gridlayout4->addWidget(m_history_sbox, line++, 1);

	gridlayout4->addWidget(new QLabel(tr("Contacts' size"), this), line, 0);
	m_contacts_sbox = new QSpinBox(this);
	m_contacts_sbox->setRange(1, 50);
	m_contacts_sbox->setValue(m_engine->contactsSize());
	gridlayout4->addWidget(m_contacts_sbox, line++, 1);

        if(m_engine->isASwitchboard() == false) {
                m_systrayed = new QCheckBox(tr("Systrayed at startup"), this);
                m_systrayed->setCheckState(m_engine->systrayed() ? Qt::Checked : Qt::Unchecked);
                gridlayout4->addWidget(m_systrayed, line++, 0, 1, 0);
        }

 	gridlayout4->setRowStretch( line, 1 );

	m_btnbox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this);
	connect(m_btnbox, SIGNAL(accepted()),
                this, SLOT(saveAndClose()));
	connect(m_btnbox, SIGNAL(rejected()),
                this, SLOT(close()));
	m_btnbox->button(QDialogButtonBox::Ok)->setDefault(true);

        tabwidget->addTab(widget_connection, tr("Connection"));
        tabwidget->addTab(widget_user, tr("Users"));
        tabwidget->addTab(widget_functions, tr("Functions"));
        tabwidget->addTab(widget_gui, tr("GUI Settings"));

	vlayout->addWidget(tabwidget);
	vlayout->addWidget(m_btnbox);
}


void ConfWidget::loginKindChanged(int index)
{
        if(index == 0) {
                m_lblalogin->hide();
                m_agentid->hide();
        } else {
                m_lblalogin->show();
                m_agentid->show();
        }
}


/*!
 * This slot saves the configuration (which is stored in displayed
 * widgets) to the BaseEngine object
 * and also to the main window object and then call close()
 */
void ConfWidget::saveAndClose()
{
        qDebug() << "ConfWidget::saveAndClose()";
        m_engine->setAddress(m_serverhost->text(), m_sbport->text().toUShort());
	m_engine->setServerip(m_serverhost->text());

	m_engine->setServerAst(m_asterisk->text());
	m_engine->setProtocol(m_protocombo->currentText().toLower());
        m_engine->setLoginKind(m_loginkind->checkState());

	m_engine->setUserId(m_userid->text());
	m_engine->setAgentId(m_agentid->text());
	m_engine->setPhonenumber(m_phonenumber->text());

	m_engine->setPassword(m_passwd->text());

	m_engine->setAutoconnect(m_autoconnect->checkState() == Qt::Checked);
	m_engine->setTrytoreconnect(m_trytoreconnect->checkState() == Qt::Checked);
	m_engine->setTrytoreconnectinterval(m_tryinterval_sbox->value()*1000);
	m_engine->setKeepaliveinterval(m_kainterval_sbox->value()*1000);

        if(m_presence)
                m_engine->setCheckedPresence(m_presence->checkState() == Qt::Checked);
        if(m_cinfo)
                m_engine->setCheckedCInfo(m_cinfo->checkState() == Qt::Checked);
        m_engine->setCheckedAutoUrl(m_autourl->checkState() == Qt::Checked);

	m_engine->setHistorySize(m_history_sbox->value());
	m_engine->setContactsSize(m_contacts_sbox->value());
        if(m_engine->isASwitchboard() == false)
                m_engine->setSystrayed(m_systrayed->checkState() == Qt::Checked);
        // m_engine->setLastConnWins(m_lastconnwins->checkState() == Qt::Checked);
	m_mainwindow->setTablimit(m_tablimit_sbox->value());

	m_engine->saveSettings();
        close();
}

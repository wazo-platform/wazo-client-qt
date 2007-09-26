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

#include "confwidget.h"
#include "mainwidget.h"
#include "baseengine.h"

/*! \brief constructor
 */
ConfWidget::ConfWidget(BaseEngine * engine,
                       MainWidget * parent)
        : QDialog(parent), m_engine(engine), m_mainwindow(parent)
{
	int line = 0;
	// the object will be destroyed when closed
	setAttribute(Qt::WA_DeleteOnClose);
	setWindowTitle(tr("Configuration"));

	QVBoxLayout * vlayout = new QVBoxLayout(this);
	/* grid layout for the editable values */
	QGridLayout * gridlayout = new QGridLayout();

	QLabel * lblhost = new QLabel(tr("Server Host"), this);
	m_serverhost = new QLineEdit(m_engine->serverip(), this);
	gridlayout->addWidget(lblhost, line, 0);
	gridlayout->addWidget(m_serverhost, line++, 1);

	m_tcpmode = new QCheckBox(tr("TCP Mode (for NAT traversal)"), this);
	m_tcpmode->setCheckState(m_engine->tcpmode() ? Qt::Checked : Qt::Unchecked);
	gridlayout->addWidget(m_tcpmode, line++, 0, 1, 0);
        
	QLabel * lbllport = new QLabel(tr("UDP Login Port"), this);
	m_loginport = new QLineEdit(QString::number(m_engine->loginPort()), this);
	m_loginport->setInputMask("99999");
	gridlayout->addWidget(lbllport, line, 0);
	gridlayout->addWidget(m_loginport, line++, 1);

        QLabel * lblsbport = new QLabel(tr("TCP Login Port"), this);
        m_sbport = new QLineEdit( QString::number(m_engine->sbPort()), this);
        m_sbport->setInputMask("99999");
        gridlayout->addWidget(lblsbport, line, 0);
        gridlayout->addWidget(m_sbport, line++, 1);

        m_loginport->setDisabled(m_tcpmode->checkState() == Qt::Checked);
        m_sbport->setEnabled(m_tcpmode->checkState() == Qt::Checked);
	connect( m_tcpmode,   SIGNAL(toggled(bool)),
	         m_loginport, SLOT(setDisabled(bool)) );
	connect( m_tcpmode,   SIGNAL(toggled(bool)),
	         m_sbport,    SLOT(setEnabled(bool)) );

        // Box for Enabled Functions Definition
	QGroupBox * groupBox = new QGroupBox( tr("Functions") );
	groupBox->setAlignment( Qt::AlignHCenter );
	QVBoxLayout * vbox = new QVBoxLayout( groupBox );

	m_presence = new QCheckBox(tr("Presence reporting"));
	m_presence->setCheckState(m_engine->checkedPresence() ? Qt::Checked : Qt::Unchecked);
	m_cinfo = new QCheckBox(tr("Customer Info"));
	m_cinfo->setCheckState(m_engine->checkedCInfo() ? Qt::Checked : Qt::Unchecked);
	vbox->addWidget( m_presence );
	vbox->addWidget( m_cinfo );
	gridlayout->addWidget(groupBox, line++, 0, 1, 0);

        // Box for Connexion Definition
        //	QGroupBox * groupBox_conn = new QGroupBox( tr("Identification"), this );
        //	groupBox_conn->setAlignment( Qt::AlignHCenter );
        //	QVBoxLayout * vbox_conn = new QVBoxLayout( groupBox_conn );

	QLabel * lblasterisk = new QLabel(tr("Asterisk Id Name"), this);
	m_asterisk = new QLineEdit(m_engine->serverast(), this);
	gridlayout->addWidget(lblasterisk, line, 0);
	gridlayout->addWidget(m_asterisk, line++, 1);

	QLabel * lblproto = new QLabel(tr("Protocol"), this);
	m_protocombo = new QComboBox(this);
	m_protocombo->addItem(QString("SIP"));
	m_protocombo->addItem(QString("IAX"));
	if(m_engine->protocol().toLower() == QString("sip"))
		m_protocombo->setCurrentIndex(0);
	else
		m_protocombo->setCurrentIndex(1);
	gridlayout->addWidget(lblproto, line, 0);
	gridlayout->addWidget(m_protocombo, line++, 1);

	QLabel * lbllogin = new QLabel(tr("Login"), this);
	m_userid = new QLineEdit(m_engine->userId(), this);
	gridlayout->addWidget(lbllogin, line, 0);
	gridlayout->addWidget(m_userid, line++, 1);

	QLabel * lblpasswd = new QLabel(tr("Password"), this);
	m_passwd = new QLineEdit(m_engine->password(), this);
	m_passwd->setEchoMode(QLineEdit::Password);
	gridlayout->addWidget(lblpasswd, line, 0);
	gridlayout->addWidget(m_passwd, line++, 1);


        QString lastconn = tr("The last connected one takes on the login");
        //m_lastconnwins = new QCheckBox(tr("The last connected one takes on the login"), this);
 	//m_lastconnwins->setCheckState(m_engine->lastconnwins() ? Qt::Checked : Qt::Unchecked);
        //gridlayout->addWidget(m_lastconnwins, line++, 0, 1, 0);

	m_autoconnect = new QCheckBox(tr("Autoconnect at startup"), this);
	m_autoconnect->setCheckState(m_engine->autoconnect() ? Qt::Checked : Qt::Unchecked);
	gridlayout->addWidget(m_autoconnect, line++, 0, 1, 0);

	m_trytoreconnect = new QCheckBox(tr("Try to reconnect") + "\n" + \
                                         tr("Checking this box disables the Error Popups"), this);
	m_trytoreconnect->setCheckState(m_engine->trytoreconnect() ? Qt::Checked : Qt::Unchecked);
	gridlayout->addWidget(m_trytoreconnect, line++, 0, 1, 0);
	gridlayout->addWidget(new QLabel(tr("Try to reconnect interval"), this), line, 0);
	m_tryinterval_sbox = new QSpinBox(this);
	m_tryinterval_sbox->setRange(1, 120);
	m_tryinterval_sbox->setValue(m_engine->trytoreconnectinterval() / 1000);
	gridlayout->addWidget(m_tryinterval_sbox, line++, 1);

	gridlayout->addWidget(new QLabel(tr("Keep alive interval"), this), line, 0);
	m_kainterval_sbox = new QSpinBox(this);
	m_kainterval_sbox->setRange(1, 120);
	m_kainterval_sbox->setValue(m_engine->keepaliveinterval() / 1000);
	gridlayout->addWidget(m_kainterval_sbox, line++, 1);

	QLabel * lbltablimit = new QLabel(tr("Tab limit"), this);
	gridlayout->addWidget(lbltablimit, line, 0);
	m_tablimit_sbox = new QSpinBox(this);
	m_tablimit_sbox->setRange(0, 99);
	m_tablimit_sbox->setValue(m_mainwindow->tablimit());
	gridlayout->addWidget(m_tablimit_sbox, line++, 1);

	gridlayout->addWidget(new QLabel(tr("History size"), this), line, 0);
	m_history_sbox = new QSpinBox(this);
	m_history_sbox->setRange(1, 20);
	m_history_sbox->setValue(m_engine->historysize());
	gridlayout->addWidget(m_history_sbox, line++, 1);

	m_btnbox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this);
	connect(m_btnbox, SIGNAL(accepted()),
                this, SLOT(saveAndClose()));
	connect(m_btnbox, SIGNAL(rejected()),
                this, SLOT(close()));
	m_btnbox->button(QDialogButtonBox::Ok)->setDefault(true);

	vlayout->addLayout(gridlayout);
        vlayout->addStretch(1);
	vlayout->addWidget(m_btnbox);
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
	m_engine->setLoginPort(m_loginport->text().toUShort());

	m_engine->setServerAst(m_asterisk->text());
	m_engine->setProtocol(m_protocombo->currentText().toLower());
	m_engine->setUserId(m_userid->text());
	m_engine->setPassword(m_passwd->text());

	m_engine->setAutoconnect(m_autoconnect->checkState() == Qt::Checked);
	m_engine->setTrytoreconnect(m_trytoreconnect->checkState() == Qt::Checked);
	m_engine->setTrytoreconnectinterval(m_tryinterval_sbox->value()*1000);
	m_engine->setKeepaliveinterval(m_kainterval_sbox->value()*1000);

	m_engine->setCheckedPresence(m_presence->checkState() == Qt::Checked);
	m_engine->setCheckedCInfo(m_cinfo->checkState() == Qt::Checked);

	m_engine->setHistorySize(m_history_sbox->value());
	m_engine->setTcpmode(m_tcpmode->checkState() == Qt::Checked);
        // m_engine->setLastConnWins(m_lastconnwins->checkState() == Qt::Checked);
	m_mainwindow->setTablimit(m_tablimit_sbox->value());

	m_engine->saveSettings();
        close();
}

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
#include <QFormLayout>

#include "configwidget.h"
#include "baseengine.h"
#include "xivoconsts.h"

QStringList queuelevel_colors = (QStringList() << "green" << "orange" << "red");

/*! \brief constructor
 */
ConfigWidget::ConfigWidget(BaseEngine * engine,
                           QWidget * parent)
    : QDialog(parent), m_engine(engine),
      m_history_sbox(NULL)
{
    int line = 0;
    setWindowTitle(tr("Configuration"));

    QVBoxLayout * vlayout = new QVBoxLayout(this);
    m_tabwidget = new QTabWidget();
        
    QVariantMap opts = m_engine->getGuiOptions("user").toMap();
    //
    // Connection Tab
    //
        
    QGridLayout * gridlayout1 = new QGridLayout();
    QWidget * widget_connection = new QWidget();
    widget_connection->setLayout(gridlayout1);

    //      QStringList childgroups = m_engine->getSettings()->childGroups();
    //         QLabel * lblprofile = new QLabel(tr("Profile"), this);
    //         QComboBox * profile = new QComboBox(this);
    //         profile->addItem(QString(tr("Default")));
    //      profile->setCurrentIndex(0);
    //         gridlayout1->addWidget(lblprofile, line, 0);
    //         gridlayout1->addWidget(profile, line++, 1);

    QLabel * lblhost = new QLabel(tr("Server Host"), this);
    m_serverhost = new QLineEdit(m_engine->serverip(), this);
    gridlayout1->addWidget(lblhost, line, 0);
    gridlayout1->addWidget(m_serverhost, line++, 1);

    QLabel * lblsbport = new QLabel(tr("Login Port"), this);
    m_ctiport = new QSpinBox(this);
    m_ctiport->setRange(1, 65535);
    m_ctiport->setValue(m_engine->sbPort());
    gridlayout1->addWidget(lblsbport, line, 0);
    gridlayout1->addWidget(m_ctiport, line++, 1);
        
    gridlayout1->setRowStretch( line, 1 );
    gridlayout1->setColumnStretch( 2, 1 );
        
    //
    // Functions Tab
    //
        
    QGridLayout * gridlayout2 = new QGridLayout();
    QWidget * widget_functions = new QWidget();
    widget_functions->setLayout(gridlayout2);
    line = 0;
    int width = 4;
        
    QHash<QString, QString> func_legend;
    func_legend["presence"] = tr("Presence reporting");
    func_legend["customerinfo"] = tr("Customer Info");
        
    foreach(QString function, CheckFunctions) {
        m_function[function] = new QCheckBox(func_legend[function]);
        m_function[function]->setCheckState(m_engine->checkedFunction(function) ? Qt::Checked : Qt::Unchecked);
        gridlayout2->addWidget( m_function[function], line++, 0, 1, width );
    }
        
    m_autourl_allowed = new QCheckBox(tr("Allow the Automatic Opening of URL's"));
    m_autourl_allowed->setCheckState(opts["autourl_allowed"].toUInt() == 2 ? Qt::Checked : Qt::Unchecked);
    gridlayout2->addWidget( m_autourl_allowed, line++, 0, 1, width );
        
    gridlayout2->addWidget(new QLabel(tr("History size"), this), line, 0);
    m_history_sbox = new QSpinBox(this);
    m_history_sbox->setRange(1, 20);
    m_history_sbox->setValue(m_engine->historySize());
    gridlayout2->addWidget(m_history_sbox, line++, 1);
        
    gridlayout2->addWidget(new QLabel(tr("Contacts' max number"), this), line, 0);
    m_contactssize_sbox = new QSpinBox(this);
    m_contactssize_sbox->setRange(1, 500);
    m_contactssize_sbox->setValue(opts["contacts-max"].toUInt());
    gridlayout2->addWidget(m_contactssize_sbox, line++, 1);
        
    gridlayout2->addWidget(new QLabel(tr("Contacts' width"), this), line, 0);
    m_contactswidth_sbox = new QSpinBox(this);
    m_contactswidth_sbox->setRange(1, 20);
    m_contactswidth_sbox->setValue(opts["contacts-width"].toUInt());
    gridlayout2->addWidget(m_contactswidth_sbox, line++, 1);
        
    gridlayout2->addWidget(new QLabel(tr("Queue Display"), this), line, 0);
    int ncol = 1;
    foreach(QString color, queuelevel_colors) {
        m_queuelevels[color] = new QSpinBox(this);
        m_queuelevels[color]->setRange(0, 100);
        m_queuelevels[color]->setValue(opts["queuelevels"].toMap()[color].toUInt());
        gridlayout2->addWidget(m_queuelevels[color], line, ncol ++);
    }
    line++;
        
    gridlayout2->setRowStretch( line, 1 );
    gridlayout2->setColumnStretch( width, 1 );

    // Box for Connection Definition
    //        QGroupBox * groupBox_conn = new QGroupBox( tr("Identification"), this );
    //        groupBox_conn->setAlignment( Qt::AlignHCenter );
    //        QVBoxLayout * vbox_conn = new QVBoxLayout( groupBox_conn );

        
    //
    // User Account's Tab
    //
        
    QGridLayout * gridlayout3 = new QGridLayout();
    QWidget * widget_user = new QWidget();
    widget_user->setLayout(gridlayout3);
    line = 0;

    m_company = new QLineEdit(m_engine->company(), this);
    gridlayout3->addWidget( new QLabel(tr("Company"), this), line, 0);
    gridlayout3->addWidget(m_company, line++, 1);
    
    m_userid = new QLineEdit(m_engine->userId(), this);
    gridlayout3->addWidget( new QLabel(tr("User Login"), this), line, 0);
    gridlayout3->addWidget(m_userid, line++, 1);
    
    m_password = new QLineEdit(m_engine->password(), this);
    m_password->setEchoMode(QLineEdit::Password);
    gridlayout3->addWidget( new QLabel(tr("Password"), this), line, 0);
    gridlayout3->addWidget(m_password, line++, 1);
    
    m_keeppass = new QCheckBox(tr("Keep Password"));
    m_keeppass->setCheckState((m_engine->keeppass() == 2) ? Qt::Checked : Qt::Unchecked);
    gridlayout3->addWidget(m_keeppass, line++, 0, 1, 2);
    
    m_showagselect = new QCheckBox(tr("Show the Agent options (like the\nones beneath) on first window"));
    m_showagselect->setCheckState((m_engine->showagselect() == 2) ? Qt::Checked : Qt::Unchecked);
    gridlayout3->addWidget(m_showagselect, line++, 0, 1, 2);
    
    QFrame * qhline3 = new QFrame(this);
    qhline3->setFrameShape(QFrame::HLine);
    gridlayout3->addWidget(qhline3, line++, 0, 1, 2);
        
    m_loginkind = new QComboBox(this);
    m_loginkind->addItem(QString(tr("No Agent")));
    m_loginkind->addItem(QString(tr("Agent (unlogged)")));
    m_loginkind->addItem(QString(tr("Agent (logged)")));
    m_loginkind->setCurrentIndex(opts["loginkind"].toUInt());
    gridlayout3->addWidget(m_loginkind, line++, 0, 1, 2);
    connect( m_loginkind, SIGNAL(currentIndexChanged(int)),
             this, SLOT(loginKindChanged(int)) );
        
    m_lblphone = new QLabel(tr("Phone Number"), this);
    m_phonenumber = new QLineEdit(m_engine->phonenumber(), this);
    gridlayout3->addWidget(m_lblphone, line, 0);
    gridlayout3->addWidget(m_phonenumber, line++, 1);
        
    loginKindChanged(m_loginkind->currentIndex());
        
    gridlayout3->setRowStretch( line, 1 );
    gridlayout3->setColumnStretch( 2, 1 );
        
    //
    // GUI Settings
    //
        
    QFormLayout * layout4 = new QFormLayout();
    QWidget * widget_gui = new QWidget();
    widget_gui->setLayout(layout4);

    //        QString lastconn = tr("The last connected one takes on the login");
    //m_lastconnwins = new QCheckBox(tr("The last connected one takes on the login"), this);
    //m_lastconnwins->setCheckState(m_engine->lastconnwins() ? Qt::Checked : Qt::Unchecked);
    //gridlayout4->addWidget(m_lastconnwins, line++, 0, 1, 0);

    m_autoconnect = new QCheckBox(tr("Autoconnect at startup"), this);
    m_autoconnect->setCheckState(m_engine->autoconnect() ? Qt::Checked : Qt::Unchecked);
    layout4->addRow( m_autoconnect );

    m_trytoreconnect = new QCheckBox(tr("Try to reconnect") + "\n" + \
                                     tr("Checking this box disables the Error Popups"), this);
    m_trytoreconnect->setCheckState(m_engine->trytoreconnect() ? Qt::Checked : Qt::Unchecked);
    layout4->addRow( m_trytoreconnect );

    m_tryinterval_sbox = new QSpinBox(this);
    m_tryinterval_sbox->setRange(1, 120);
    m_tryinterval_sbox->setValue(m_engine->trytoreconnectinterval() / 1000);
    layout4->addRow( tr("Try to reconnect interval"), m_tryinterval_sbox );

    m_kainterval_sbox = new QSpinBox(this);
    m_kainterval_sbox->setRange(1, 120);
    m_kainterval_sbox->setValue(m_engine->keepaliveinterval() / 1000);
    layout4->addRow( tr("Keep alive interval"), m_kainterval_sbox );

    QFrame * qhline5 = new QFrame(this);
    qhline5->setFrameShape(QFrame::HLine);
    layout4->addRow( qhline5 );

    m_tablimit_sbox = new QSpinBox(this);
    m_tablimit_sbox->setRange(0, 99);
    m_tablimit_sbox->setValue(opts["sheet-tablimit"].toUInt());
    layout4->addRow( tr("Tab limit"), m_tablimit_sbox );
        
    m_systrayed = new QCheckBox(tr("Systrayed at startup"), this);
    m_systrayed->setCheckState(m_engine->systrayed() ? Qt::Checked : Qt::Unchecked);
    layout4->addRow( m_systrayed );

    m_comboswitchboard = new QComboBox(this);
    m_comboswitchboard->addItem( tr("Small"), QString("small") );
    m_comboswitchboard->addItem( tr("Detailed"), QString("detailed") );
    for(int i = 0; i < m_comboswitchboard->count(); i++)
        {
            if(opts["switchboard-elt-type"] == m_comboswitchboard->itemData(i))
                m_comboswitchboard->setCurrentIndex(i);
        }
    layout4->addRow( tr("Apparence of SwitchBoard elements"),
                     m_comboswitchboard);

    m_maxWidthWanted = new QSpinBox(this);
    m_maxWidthWanted->setRange(50, 250);
    int maxwidthwanted = opts["maxwidthwanted"].toInt();
    if(maxwidthwanted < 50)
        maxwidthwanted = 200;
    m_maxWidthWanted->setValue( maxwidthwanted );
    layout4->addRow( tr("Maximum width for small SwitchBoard elements"),
                     m_maxWidthWanted );

    m_btnbox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this);
    connect(m_btnbox, SIGNAL(accepted()),
            this, SLOT(saveAndClose()));
    connect(m_btnbox, SIGNAL(rejected()),
            this, SLOT(close()));
    m_btnbox->button(QDialogButtonBox::Ok)->setDefault(true);

    m_tabwidget->addTab(widget_user, tr("Account"));
    m_tabwidget->addTab(widget_connection, tr("Connection"));
    m_tabwidget->addTab(widget_functions, tr("Functions"));
    m_tabwidget->addTab(widget_gui, tr("GUI Settings"));

    m_tabwidget->setCurrentIndex(m_engine->getSettings()->value("display/configtab", 0).toInt());

    vlayout->addWidget(m_tabwidget);
    vlayout->addWidget(m_btnbox);
}

ConfigWidget::~ConfigWidget()
{
//    qDebug() << "ConfigWidget::~ConfigWidget()" << m_engine;
    m_engine->getSettings()->setValue("display/configtab", m_tabwidget->currentIndex());
}

/*! \brief hide/show phonenumber input according to loginKind
 */
void ConfigWidget::loginKindChanged(int index)
{
    // qDebug() << "ConfigWidget::loginKindChanged()" << index;
    if(index == 0) {
        m_lblphone->hide();
        m_phonenumber->hide();
    } else {
        m_lblphone->show();
        m_phonenumber->show();
    }
}

/*!
 * This slot saves the configuration (which is stored in displayed
 * widgets) to the BaseEngine object
 * and also to the main window object and then call close()
 */
void ConfigWidget::saveAndClose()
{
    // qDebug() << "ConfigWidget::saveAndClose()";
    m_engine->setAddress(m_serverhost->text(), m_ctiport->value());
    m_engine->setServerip(m_serverhost->text());

    m_engine->setCompany(m_company->text());
    m_engine->setKeepPass(m_keeppass->checkState());
    m_engine->setShowAgentSelect(m_showagselect->checkState());

    m_engine->setUserId(m_userid->text());
    m_engine->setPhonenumber(m_phonenumber->text());
    m_engine->setFullId();

    m_engine->setPassword(m_password->text());

    m_engine->setAutoconnect(m_autoconnect->checkState() == Qt::Checked);
    m_engine->setTrytoreconnect(m_trytoreconnect->checkState() == Qt::Checked);
    m_engine->setTrytoreconnectinterval(m_tryinterval_sbox->value()*1000);
    m_engine->setKeepaliveinterval(m_kainterval_sbox->value()*1000);

    foreach(QString function, CheckFunctions)
        m_engine->setCheckedFunction(function, m_function[function]->checkState() == Qt::Checked);
    if(m_history_sbox)
        m_engine->setHistorySize(m_history_sbox->value());
        
    m_engine->setSystrayed(m_systrayed->checkState() == Qt::Checked);
        
    QVariantMap opts;
    QVariantMap qvm;
    foreach(QString color, queuelevel_colors)
        qvm[color] = QVariant(m_queuelevels[color]->value());
    opts["queuelevels"] = qvm;
    
    // BaseEngine::setLoginKind() sets the user option if needed.
    m_engine->setLoginKind(m_loginkind->currentIndex());
    opts["loginkind"] = m_loginkind->currentIndex();
    
    opts["contacts-max"] = m_contactssize_sbox->value();
    opts["contacts-width"] = m_contactswidth_sbox->value();
    opts["sheet-tablimit"] = m_tablimit_sbox->value();
    opts["autourl_allowed"] = m_autourl_allowed->checkState();
    opts["switchboard-elt-type"] = m_comboswitchboard->itemData( m_comboswitchboard->currentIndex() ).toString();
    opts["maxwidthwanted"] = m_maxWidthWanted->value();
    m_engine->setGuiOption("user", opts);
    
    m_engine->saveSettings();
    emit confUpdated();
    close();
}

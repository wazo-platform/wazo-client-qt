/* XiVO Client
 * Copyright (C) 2007-2011, Proformatique
 *
 * This file is part of XiVO Client.
 *
 * XiVO Client is free software: you can redistribute it and/or modify
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
 * XiVO Client is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with XiVO Client.  If not, see <http://www.gnu.org/licenses/>.
 */

/* $Revision$
 * $Date$
 */

#include <QCheckBox>
#include <QComboBox>
#include <QDebug>
#include <QKeyEvent>
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
#include <QPixmap>

#include "configwidget.h"
#include "baseengine.h"
#include "xivoconsts.h"

static const QStringList queuelevel_colors = (QStringList() << "green" << "orange");
QHash<QString, QString> func_legend;

/*! \brief constructor */
ConfigWidget::ConfigWidget(QWidget *parent)
    : QDialog(parent),
      m_currentKeyChange(-1)
{
    setWindowTitle(tr("Configuration"));

    QVBoxLayout * vlayout = new QVBoxLayout(this);
    m_tabwidget = new QTabWidget();

    m_opts = b_engine->getGuiOptions("client_gui");
    m_forcedopts = b_engine->getGuiOptions("server_gui");
    m_config = b_engine->getConfig();

    _insert_connection_tab();
    _insert_account_tab();
    _insert_guisetting_tab();
    _insert_function_tab();
    _insert_advanced_tab();

    m_tabwidget->setCurrentIndex(b_engine->getSettings()->value("display/configtab", 0).toInt());
    vlayout->addWidget(m_tabwidget);
    
    m_btnbox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this);
    connect(m_btnbox, SIGNAL(accepted()), this, SLOT(saveAndClose()));
    connect(m_btnbox, SIGNAL(rejected()), this, SLOT(close()));
    m_btnbox->button(QDialogButtonBox::Ok)->setDefault(true);
    vlayout->addWidget(m_btnbox);
}

/*! \brief connection config tab */
void ConfigWidget::_insert_connection_tab()
{
    QFormLayout * layout1 = new QFormLayout;
    QWidget * widget_connection = new QWidget;

    widget_connection->setLayout(layout1);

    m_cti_address = new QLineEdit(m_config["cti_address"].toString(), this);
    layout1->addRow(tr("Server Host"), m_cti_address);

    m_cti_port = new QSpinBox(this);
    m_cti_port->setRange(1, 65535);
    m_cti_port->setValue(m_config["cti_port"].toUInt());
    layout1->addRow(tr("Login Port"), m_cti_port);

    m_cti_encrypt = new QCheckBox(tr("Encrypt Connection"));
    m_cti_encrypt->setCheckState(m_config["cti_encrypt"].toBool() ? Qt::Checked : Qt::Unchecked);
    layout1->addRow(m_cti_encrypt);
    
    m_trytoreconnect = new QCheckBox(tr("Try to reconnect") + "\n" + \
                                     tr("Checking this box disables the Error Popups"), this);
    m_trytoreconnect->setCheckState(m_config["trytoreconnect"].toBool() ? Qt::Checked : Qt::Unchecked);
    layout1->addRow(m_trytoreconnect);

    m_tryinterval_sbox = new QSpinBox(this);
    m_tryinterval_sbox->setRange(1, 120);
    m_tryinterval_sbox->setValue(m_config["trytoreconnectinterval"].toUInt() / 1000);
    layout1->addRow(tr("Try to reconnect interval"), m_tryinterval_sbox);

    m_kainterval_sbox = new QSpinBox(this);
    m_kainterval_sbox->setRange(1, 120);
    m_kainterval_sbox->setValue(m_config["keepaliveinterval"].toUInt() / 1000);
    layout1->addRow(tr("Keep alive interval"), m_kainterval_sbox);

    m_tabwidget->addTab(widget_connection, tr("Connection"));
}

/*! \brief function config tab */
void ConfigWidget::_insert_function_tab()
{
    QVBoxLayout *layout2 = new QVBoxLayout();
    QWidget *widget_functions = new QWidget();
    widget_functions->setLayout(layout2);
    
    // Please don't split this loop, because it is much simpler to add new functions this way
    func_legend["presence"] = tr("Presence reporting");
    func_legend["customerinfo"] = tr("Customer Info");

    foreach(QString function, func_legend.keys()) {
        m_function[function] = new QCheckBox(func_legend[function]);
        m_function[function]->setCheckState(b_engine->checkedFunction(function) ? Qt::Checked : Qt::Unchecked);
        layout2->addWidget(m_function[function]);
    }
    
    m_function_tabwidget = new QTabWidget();
    
        QWidget * widget_presence = new QWidget() ;
        QFormLayout * layout21 = new QFormLayout() ;
        widget_presence->setLayout(layout21);
        
        m_presenceIndicatorSize = new QSpinBox(this);
        m_presenceIndicatorSize->setRange(1, 20);
        int presenceIndicatorSize = m_opts.value("presenceindicatorsize").toInt();
        if ((presenceIndicatorSize <= 0) || (presenceIndicatorSize > 20))
            presenceIndicatorSize = 5;
        m_presenceIndicatorSize->setValue(presenceIndicatorSize);
        layout21->addRow(tr("Presence indicator size (in pixels)"), m_presenceIndicatorSize);
        
    m_function_tabwidget->addTab(widget_presence, tr("Presence reporting"));
    
        QWidget * widget_customerinfo = new QWidget() ;
        QFormLayout * layout22 = new QFormLayout() ;
        widget_customerinfo->setLayout(layout22);
        
        m_autourl_allowed = new QCheckBox(tr("Allow the Automatic Opening of URL's"));
        m_autourl_allowed->setCheckState(m_opts.value("autourl_allowed").toUInt() == 2 ?
                                         Qt::Checked : Qt::Unchecked);
        layout22->addRow(m_autourl_allowed);
        
        m_tablimit_sbox = new QSpinBox(this);
        m_tablimit_sbox->setRange(0, 99);
        m_tablimit_sbox->setValue(m_opts.value("sheet-tablimit").toUInt());
        layout22->addRow(tr("Tab limit"), m_tablimit_sbox);
        
    m_function_tabwidget->addTab(widget_customerinfo, tr("Customer Info"));
    
        QWidget * widget_history = new QWidget() ;
        QFormLayout * layout23 = new QFormLayout() ;
        widget_history->setLayout(layout23);
        
        m_history_sbox = new QSpinBox(this);
        m_history_sbox->setRange(1, 20);
        m_history_sbox->setValue(m_config["historysize"].toUInt());
        layout23->addRow(tr("History size"), m_history_sbox);
        
    m_function_tabwidget->addTab(widget_history, tr("History"));
        
        QWidget * widget_contacts = new QWidget() ;
        QFormLayout * layout24 = new QFormLayout() ;
        widget_contacts->setLayout(layout24);
        
        qDebug() << m_forcedopts;
        m_contactssize_sbox = new QSpinBox(this);
        m_contactssize_sbox->setRange(1, 500);
        m_contactssize_sbox->setValue(m_opts.value("contacts-max").toUInt());
        layout24->addRow(tr("Contacts' max number"), m_contactssize_sbox);
        
        m_contactswidth_sbox = new QSpinBox(this);
        m_contactswidth_sbox->setRange(1, 20);
        m_contactswidth_sbox->setValue(m_opts.value("contacts-width").toUInt());
        layout24->addRow(tr("Contacts' width"), m_contactswidth_sbox);
        
    m_function_tabwidget->addTab(widget_contacts, tr("Contacts"));
    
        QWidget * widget_queues = new QWidget() ;
        QGridLayout * layout25 = new QGridLayout() ;
        layout25->setAlignment(Qt::AlignTop|Qt::AlignHCenter);
        widget_queues->setLayout(layout25);
        
        int line = 0;

        layout25->addWidget(new QLabel(tr("Queue Display"), this), line, 0);
        int ncol = 1;
        foreach(QString color, queuelevel_colors) {
            m_queuelevels[color] = new QSpinBox(this);
            m_queuelevels[color]->setRange(0, 100);
            m_queuelevels[color]->setValue(m_opts.value("queuelevels").toMap().value(color).toUInt());
            layout25->addWidget(m_queuelevels[color], line, ncol++);
        }
        line++;

        m_queue_longestwait = new QCheckBox(tr("Queue Display (Longest Wait)"), this);
        m_queue_longestwait->setCheckState(m_opts.value("queue_longestwait").toBool() ? Qt::Checked : Qt::Unchecked);
        layout25->addWidget(m_queue_longestwait, line, 0);
        ncol = 1;
        foreach(QString color, queuelevel_colors) {
            m_queuelevels_wait[color] = new QSpinBox(this);
            m_queuelevels_wait[color]->setRange(0, 3600);
            m_queuelevels_wait[color]->setValue(m_opts.value("queuelevels_wait").toMap().value(color).toUInt());
            layout25->addWidget(m_queuelevels_wait[color], line, ncol++);
        }

        line++;
        m_queue_displaynu = new QCheckBox(tr("Queue Display number"), this);
        m_queue_displaynu->setCheckState(m_opts.value("queue_displaynu").toBool() ? Qt::Checked : Qt::Unchecked);
        layout25->addWidget(m_queue_displaynu, line, 0);
    
    m_function_tabwidget->addTab(widget_queues, tr("Queues"));
    
        QWidget * widget_switchboard = new QWidget() ;
        QFormLayout * layout26 = new QFormLayout() ;
        widget_switchboard->setLayout(layout26);
    
        m_comboswitchboard = new QComboBox(this);
        m_comboswitchboard->addItem(tr("Small"), QString("small"));
        m_comboswitchboard->addItem(tr("Detailed"), QString("detailed"));
        for(int i = 0; i < m_comboswitchboard->count(); i++) {
            if(m_opts.value("switchboard-elt-type") == m_comboswitchboard->itemData(i))
                m_comboswitchboard->setCurrentIndex(i);
        }
        layout26->addRow(tr("Appearance of SwitchBoard elements"), m_comboswitchboard);

        m_maxWidthWanted = new QSpinBox(this);
        m_maxWidthWanted->setRange(50, 250);
        int maxwidthwanted = m_opts.value("maxwidthwanted").toInt();
        if(maxwidthwanted < 50)
            maxwidthwanted = 200;
        m_maxWidthWanted->setValue(maxwidthwanted);
        layout26->addRow(tr("Maximum width for small SwitchBoard elements"), m_maxWidthWanted);
    
    m_function_tabwidget->addTab(widget_switchboard, tr("Switchboard"));
    
    _insert_operator_functiontab();
    
    layout2->addWidget(m_function_tabwidget);
    
    m_tabwidget->addTab(widget_functions, tr("Functions"));
}

/*! \brief account config tab */
void ConfigWidget::_insert_account_tab()
{
    QFormLayout *layout3 = new QFormLayout();
    QWidget *widget_user = new QWidget();
    widget_user->setLayout(layout3);

    m_context = new QLineEdit(m_config["company"].toString(), this);
    layout3->addRow(tr("Context"), m_context);

    m_userid = new QLineEdit(m_config["userlogin"].toString(), this);
    layout3->addRow(tr("User Login"), m_userid);

    m_password = new QLineEdit(m_config["password"].toString(), this);
    m_password->setEchoMode(QLineEdit::Password);
    layout3->addRow(tr("Password"), m_password);

    m_keeppass = new QCheckBox(tr("Keep Password"));
    m_keeppass->setCheckState((m_config["keeppass"].toUInt() == 2) ? Qt::Checked : Qt::Unchecked);
    layout3->addRow(m_keeppass);
    
    m_autoconnect = new QCheckBox(tr("Autoconnect at startup"), this);
    m_autoconnect->setCheckState(m_config["autoconnect"].toBool() ? Qt::Checked : Qt::Unchecked);
    layout3->addRow(m_autoconnect);

    m_showagselect = new QCheckBox(tr("Show the Agent options (like the\nones beneath) on first window"));
    m_showagselect->setCheckState((m_config["showagselect"].toUInt() == 2) ? Qt::Checked : Qt::Unchecked);
    layout3->addRow(m_showagselect);

    m_loginkind = new QComboBox(this);
    m_loginkind->addItem(QString(tr("No Agent")));
    m_loginkind->addItem(QString(tr("Agent (unlogged)")));
    m_loginkind->addItem(QString(tr("Agent (logged)")));
    m_loginkind->setCurrentIndex(m_opts.value("loginkind").toUInt());
    layout3->addRow(tr("Agent options"), m_loginkind);
    connect(m_loginkind, SIGNAL(currentIndexChanged(int)),
            this, SLOT(loginKindChanged(int)));

    m_agentphonenumber = new QLineEdit(m_config["agentphonenumber"].toString(), this);
    layout3->addRow(tr("Phone Number"), m_agentphonenumber);

    loginKindChanged(m_loginkind->currentIndex());

    m_tabwidget->addTab(widget_user, tr("Account"));
}

/*! \brief gui settings config tab */
void ConfigWidget::_insert_guisetting_tab()
{
    QFormLayout *layout4 = new QFormLayout();
    QWidget *widget_gui = new QWidget();
    widget_gui->setLayout(layout4);

    m_locale_cbox = new QComboBox(this);
    m_locale_cbox->addItem(tr("System Language"), QString("default"));
    m_locale_cbox->addItem(tr("English"), QString("en_US"));
    m_locale_cbox->addItem(tr("French"), QString("fr_FR"));
    m_locale_cbox->addItem(tr("Nederlands"), QString("nl_NL"));
    m_locale_cbox->addItem(tr("German"), QString("de_DE"));
    for (int i = 0; i < m_locale_cbox->count(); i++) {
        if (m_config["forcelocale"].toString() == m_locale_cbox->itemData(i))
            m_locale_cbox->setCurrentIndex(i);
    }
    layout4->addRow(tr("Language"), new WarningWidget(m_locale_cbox));

    QFrame *qhline5 = new QFrame(this);
    qhline5->setFrameShape(QFrame::HLine);
    layout4->addRow(qhline5);

    m_systrayed = new QCheckBox(tr("Systrayed at startup"), this);
    m_systrayed->setCheckState(m_config["systrayed"].toBool() ? Qt::Checked : Qt::Unchecked);
    layout4->addRow(m_systrayed);
    
    // The value displayed is the inverse of the bool in memory
    m_unique = new QCheckBox(tr("Allow multiple instances of XiVO Client"), this);
    m_unique->setCheckState(m_config["uniqueinstance"].toBool() ? Qt::Unchecked : Qt::Checked);
    layout4->addRow(new WarningWidget(m_unique));
    
    /*!
     * \todo make a list of the qss files and fill a combobox ?
     */
    m_qss = new QLineEdit();
    m_qss->setText(m_config["qss"].toString());
    layout4->addRow(tr("Interface style"), new WarningWidget(m_qss));
    
    m_clipboard = new QCheckBox(tr("Enable the clipboard")) ;
    m_clipboard->setCheckState(m_config["enableclipboard"].toBool() ? Qt::Checked : Qt::Unchecked);
    layout4->addRow(new WarningWidget(m_clipboard));
    
    m_displayprofile = new QCheckBox(tr("Display the configuration profile")) ;
    m_displayprofile->setCheckState(m_config["displayprofile"].toBool() ? Qt::Checked : Qt::Unchecked);
    layout4->addRow(new WarningWidget(m_displayprofile));
    
    m_activate_on_tel = new QCheckBox(tr("Activate on incoming call")) ;
    m_activate_on_tel->setCheckState(m_config["activate_on_tel"].toBool() ? Qt::Checked : Qt::Unchecked);
    layout4->addRow(new WarningWidget(m_activate_on_tel));
    
    m_tabwidget->addTab(widget_gui, tr("GUI Settings"));
}

/*! \brief advanced config tab */
void ConfigWidget::_insert_advanced_tab()
{
    QFormLayout * layout5 = new QFormLayout();
    QWidget * widget_adv = new QWidget();
    widget_adv->setLayout(layout5);
    
    m_logtofile = new QCheckBox(tr("Enable logging of program actions"));
    m_logtofile->setCheckState(m_config["logtofile"].toBool() ? Qt::Checked : Qt::Unchecked);
    layout5->addRow(new WarningWidget(m_logtofile));
    
    m_logfilename = new QLineEdit();
    m_logfilename->setText(m_config["logfilename"].toString());
    layout5->addRow(tr("Logfile name"), new WarningWidget(m_logfilename));
    
    m_tabwidget->addTab(widget_adv, tr("Advanced"));
}

void ConfigWidget::changeOperatorKey(bool a)
{
  QPushButton *button = qobject_cast<QPushButton *>(sender());
  int index = button->property("i").toInt();
  static QPushButton *old = NULL;

  if (a) {
      if ((old!=NULL)&&(old!=button))
          old->toggle();
      old = button;
      button->setText(tr("(current: %0) use your keyboard")
                      .arg(QKeySequence(m_opts.value("xlet_operator_key" + m_operator_action[index].action).toInt()).toString()));
      m_currentKeyChange = index;
  } else {
    button->setText(tr("(current: %0) click to change")
                    .arg(QKeySequence(m_opts.value("xlet_operator_key" + m_operator_action[index].action).toInt()).toString()));
    old = NULL;
  }
}

/*! \brief operator xlet config tab */
void ConfigWidget::_insert_operator_functiontab()
{
    m_operator_action[0].action = QString("answer");
    m_operator_action[0].translation = tr("Answer");
    m_operator_action[1].action = QString("hangup");
    m_operator_action[1].translation = tr("Hangup");
    m_operator_action[2].action = QString("dtransfer");
    m_operator_action[2].translation = tr("D. Transfer");
    m_operator_action[3].action = QString("itransfer");
    m_operator_action[3].translation = tr("I. Transfer");
    m_operator_action[4].action = QString("ilink");
    m_operator_action[4].translation = tr("I. Link");
    m_operator_action[5].action = QString("icancel");
    m_operator_action[5].translation = tr("I. Cancel");
    m_operator_action[6].action = QString("park");
    m_operator_action[6].translation = tr("Park");
    m_operator_action[7].action = QString("atxferfinalize");
    m_operator_action[7].translation = tr("Finalize Transfer");
    m_operator_action[8].action = QString("atxfercancel");
    m_operator_action[8].translation = tr("Cancel Transfer");

    int i;

    QGridLayout *glayout = new QGridLayout();
    QWidget *root_widget = new QWidget();
    root_widget->setLayout(glayout);
    QPushButton *selectKey;



    glayout->addWidget(new QLabel(tr("Operator action")), 0, 1);
    glayout->addWidget(new QLabel(tr("Key binding")), 0, 2);

    for(i=0;i<9;i++) {
        selectKey = new QPushButton(tr("(current: %0) click to change")
                                    .arg(QKeySequence(m_opts.value("xlet_operator_key" + m_operator_action[i].action).toInt()).toString()),
                                    root_widget);
        selectKey->setCheckable(true);
        selectKey->setProperty("i", i);
        m_operator_action[i].button = selectKey;
        glayout->addWidget(new QLabel(m_operator_action[i].translation), i+1 ,1);
        glayout->addWidget(selectKey, i+1, 2);

        connect(selectKey, SIGNAL(toggled(bool)), this, SLOT(changeOperatorKey(bool)));
    }


    m_operator_answer_work = new QCheckBox(tr("Display an answer action"));
    m_operator_answer_work->setCheckState((m_opts.value("xlet_operator_answer_work", 1).toInt()) ? Qt::Checked : Qt::Unchecked);
    glayout->addWidget(m_operator_answer_work, ++i , 1, 1, 2);

    glayout->addWidget(new QLabel(tr("Any change here requires an application restart to be effective")), ++i, 1, 1, 2);

    m_function_tabwidget->addTab(root_widget, tr("Operator Xlet"));
}


ConfigWidget::~ConfigWidget()
{
//    qDebug() << Q_FUNC_INFO << b_engine;
    b_engine->getSettings()->setValue("display/configtab", m_tabwidget->currentIndex());
}

void ConfigWidget::keyPressEvent(QKeyEvent *e)
{
    if ((m_currentKeyChange == -1) || (e->modifiers() != Qt::NoModifier))
        return;

    int i, already_bound = -1;

    for (i=0;i<9;i++) {
        if ((m_opts.value("xlet_operator_key" + m_operator_action[i].action).toInt() == e->key()) &&
            (i != m_currentKeyChange)) {
            already_bound = i;
            break;
        }
    }

    if (already_bound != -1) {
        QMessageBox::information(this,
            tr("This key is already bound"),
            tr("Please press another key, this one has already been bound for action '%0'")
                .arg(m_operator_action[i].translation));

        return ;
    }

    m_opts["xlet_operator_key" + m_operator_action[m_currentKeyChange].action] = e->key();
    m_operator_action[m_currentKeyChange].button->toggle();
}

/*! \brief hide/show agentphonenumber input according to loginKind */
void ConfigWidget::loginKindChanged(int index)
{
    // qDebug() << Q_FUNC_INFO << index;
    if (index == 0) {
        m_agentphonenumber->setEnabled(false);
    } else {
        m_agentphonenumber->setEnabled(true);
    }
}

/*!
 * This slot saves the configuration (which is stored in displayed
 * widgets) to the BaseEngine object
 * and also to the main window object and then call close()
 */
void ConfigWidget::saveAndClose()
{
    int i;
    // qDebug() << Q_FUNC_INFO;
    m_config["cti_address"] = m_cti_address->text();
    m_config["cti_port"] = m_cti_port->value();
    m_config["cti_encrypt"] = m_cti_encrypt->checkState() == Qt::Checked;
    m_config["company"] = m_context->text();
    m_config["keeppass"] = m_keeppass->checkState();
    m_config["showagselect"] = m_showagselect->checkState();
    m_config["userlogin"] = m_userid->text();
    m_config["agentphonenumber"] = m_agentphonenumber->text();
    m_config["password"] = m_password->text();
    /*!
     * \todo replace with QString::currentText() ?
     */
    m_config["forcelocale"] = m_locale_cbox->itemData(m_locale_cbox->currentIndex()).toString();
    m_config["autoconnect"] = m_autoconnect->checkState() == Qt::Checked;
    m_config["trytoreconnect"] = m_trytoreconnect->checkState() == Qt::Checked;
    m_config["trytoreconnectinterval"] = m_tryinterval_sbox->value() * 1000;
    m_config["keepaliveinterval"] = m_kainterval_sbox->value() * 1000;
    m_config["historysize"] = m_history_sbox->value();
    m_config["systrayed"] = m_systrayed->checkState() == Qt::Checked;
    m_config["uniqueinstance"] = m_unique->checkState() == Qt::Unchecked;
    m_config["qss"] = m_qss->text();
    m_config["enableclipboard"] = m_clipboard->checkState() == Qt::Checked;
    m_config["logtofile"] = m_logtofile->checkState() == Qt::Checked;
    m_config["logfilename"] = m_logfilename->text();
    m_config["displayprofile"] = m_displayprofile->checkState() == Qt::Checked;
    m_config["activate_on_tel"] = m_activate_on_tel->checkState() == Qt::Checked;
    b_engine->setConfig(m_config);

    foreach(QString function, func_legend.keys())
        b_engine->setCheckedFunction(function, m_function[function]->checkState() == Qt::Checked);

    QVariantMap opts_saved;
    QVariantMap qvm, qvm2;

    for(i=0;i<9;i++) {
        opts_saved["xlet_operator_key" + m_operator_action[i].action] =
            m_opts.value("xlet_operator_key" + m_operator_action[i].action).toInt();
    }
    opts_saved["xlet_operator_answer_work"] = m_operator_answer_work->checkState() == Qt::Checked;


    foreach(QString color, queuelevel_colors)
        qvm[color] = QVariant(m_queuelevels[color]->value());
    opts_saved["queuelevels"] = qvm;

    foreach(QString color, queuelevel_colors)
        qvm2[color] = QVariant(m_queuelevels_wait[color]->value());
    opts_saved["queuelevels_wait"] = qvm2;

    opts_saved["loginkind"] = m_loginkind->currentIndex();

    opts_saved["contacts-max"] = m_contactssize_sbox->value();
    opts_saved["contacts-width"] = m_contactswidth_sbox->value();
    opts_saved["sheet-tablimit"] = m_tablimit_sbox->value();
    opts_saved["autourl_allowed"] = m_autourl_allowed->checkState();
    opts_saved["queue_longestwait"] = m_queue_longestwait->checkState() == Qt::Checked;
    opts_saved["queue_displaynu"] = m_queue_displaynu->checkState() == Qt::Checked;
    opts_saved["switchboard-elt-type"] = m_comboswitchboard->itemData(m_comboswitchboard->currentIndex()).toString();
    opts_saved["maxwidthwanted"] = m_maxWidthWanted->value();
    opts_saved["presenceindicatorsize"] = m_presenceIndicatorSize->value();
    b_engine->setGuiOption("client_gui", opts_saved);
    /*!
     * \todo Don't put any settings below, fix todo in baseEngine::setGuiSettings() first.
     */

    emit confUpdated();
    close();
}

WarningWidget::WarningWidget(QWidget * widget = NULL, QString tooltip) : QWidget() {
    
    layout = new QHBoxLayout();
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->setAlignment(Qt::AlignLeft);
    setLayout(layout);
    
    QLabel * label = new QLabel();
    label->setPixmap (QPixmap(":/images/warning.png").scaledToHeight(18, Qt::SmoothTransformation));
    if (tooltip.isEmpty()) {
        tooltip = tr("You must restart the program for this setting to apply");
    }
    label->setToolTip(tooltip);
    
    layout->addWidget(widget);
    layout->addWidget(label);
}

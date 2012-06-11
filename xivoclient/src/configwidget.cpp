/* XiVO Client
 * Copyright (C) 2007-2011, Avencall
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
#include <QSpinBox>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QPixmap>
#include <QDir>
#include <QFrame>

#include <baseengine.h>
#include <xivoconsts.h>
#include <warningwidget.h>

#include "configwidget.h"

QHash<QString, QString> func_legend;
QString reboot_message;

/*! \brief constructor */
ConfigWidget::ConfigWidget(QWidget *parent)
    : QDialog(parent),
      m_currentKeyChange(-1)
{
    reboot_message = tr("You must restart the program for this setting to apply.");
    m_parent = parent;
    setWindowTitle(tr("Configuration"));
    setAttribute(Qt::WA_DeleteOnClose);

    QVBoxLayout * vlayout = new QVBoxLayout(this);
    m_tabwidget = new QTabWidget();

    m_config = b_engine->getConfig();
    
    m_dblclick_actions["call"] = tr("Call");
    // XXX: disable atxfer until they work in asterisk 1.8
    // m_dblclick_actions["atxfer"] = "Indirect transfer";

    createColors();

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

void ConfigWidget::createColors()
{
    ColorLevelStruct color;
    color.id="green";
    color.label=tr("Green");
    m_queue_colors.append(color);
    color.id="orange";
    color.label=tr("Orange");
    m_queue_colors.append(color);
}

/*! \brief connection config tab */
void ConfigWidget::_insert_connection_tab()
{
    QGridLayout * servers_layout = new QGridLayout;
    servers_layout->setHorizontalSpacing(5);
    servers_layout->setVerticalSpacing(5);

    // Headers
    servers_layout->addWidget(new QLabel(tr("Host address")), 0, 1);
    servers_layout->addWidget(new QLabel(tr("CTI port")), 0, 2);

    // Main server
    servers_layout->addWidget(new QLabel(tr("Main server")), 1, 0);

    m_main_server_address_input = new QLineEdit(m_config["cti_address"].toString(), this);
    servers_layout->addWidget(m_main_server_address_input, 1, 1);

    m_main_server_port_input = new QSpinBox(this);
    m_main_server_port_input->setRange(1, 65535);
    m_main_server_port_input->setValue(m_config["cti_port"].toUInt());
    servers_layout->addWidget(m_main_server_port_input, 1, 2);

    m_main_server_encrypt_input = new QCheckBox(tr("Encrypt Connection"));
    m_main_server_encrypt_input->setChecked(m_config["cti_encrypt"].toBool());
    servers_layout->addWidget(m_main_server_encrypt_input, 1, 3);

    // Backup server
    servers_layout->addWidget(new QLabel(tr("Backup server")), 2, 0);

    m_backup_server_address_input = new QLineEdit(m_config["cti_backup_address"].toString(), this);
    servers_layout->addWidget(m_backup_server_address_input, 2, 1);

    m_backup_server_port_input = new QSpinBox(this);
    m_backup_server_port_input->setRange(1, 65535);
    m_backup_server_port_input->setValue(m_config["cti_backup_port"].toUInt());
    servers_layout->addWidget(m_backup_server_port_input, 2, 2);

    m_backup_server_encrypt_input = new QCheckBox(tr("Encrypt Connection"));
    m_backup_server_encrypt_input->setChecked(m_config["cti_backup_encrypt"].toBool());
    servers_layout->addWidget(m_backup_server_encrypt_input, 2, 3);

    // Rest of the widget
    QFormLayout * form_layout = new QFormLayout;
    form_layout->setHorizontalSpacing(5);
    form_layout->setVerticalSpacing(5);

    m_trytoreconnect = new QCheckBox(tr("Try to reconnect") + "\n" + \
                                     tr("Checking this box disables the Error Popups"), this);
    m_trytoreconnect->setChecked(m_config["trytoreconnect"].toBool());
    form_layout->addRow(m_trytoreconnect);

    m_tryinterval_sbox = new QSpinBox(this);
    m_tryinterval_sbox->setRange(1, 120);
    m_tryinterval_sbox->setValue(m_config["trytoreconnectinterval"].toUInt() / 1000);
    form_layout->addRow(tr("Try to reconnect interval"), m_tryinterval_sbox);

    m_keepalive_input = new QSpinBox(this);
    m_keepalive_input->setRange(1, 999);
    m_keepalive_input->setValue(m_config["keepaliveinterval"].toUInt() / 1000);
    form_layout->addRow(tr("Keep alive interval"), m_keepalive_input);

    QVBoxLayout * connection_layout = new QVBoxLayout;
    connection_layout->addItem(servers_layout);
    QFrame* horizontal_separator = new QFrame();
    horizontal_separator->setFrameShape(QFrame::HLine);
    horizontal_separator->setFrameShadow(QFrame::Sunken);
    connection_layout->addWidget(horizontal_separator);
    connection_layout->addItem(form_layout);
    connection_layout->addStretch();

    QWidget * connection_widget = new QWidget;
    connection_widget->setLayout(connection_layout);

    m_tabwidget->addTab(connection_widget, tr("Connection"));
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
        m_function[function]->setChecked(m_config["checked_function." + function].toBool());
        layout2->addWidget(m_function[function]);
    }

    m_function_tabwidget = new QTabWidget();

        QWidget * widget_presence = new QWidget() ;
        QFormLayout * layout_presence = new QFormLayout() ;
        widget_presence->setLayout(layout_presence);
        
        m_presenceIndicatorSize = new QSpinBox(this);
        m_presenceIndicatorSize->setRange(1, 20);
        int presenceIndicatorSize = m_config["guioptions.presenceindicatorsize"].toInt();
        if ((presenceIndicatorSize <= 0) || (presenceIndicatorSize > 20))
            presenceIndicatorSize = 5;
        m_presenceIndicatorSize->setValue(presenceIndicatorSize);
        layout_presence->addRow(tr("Presence indicator size (in pixels)"), m_presenceIndicatorSize);
        
    m_function_tabwidget->addTab(widget_presence, tr("Presence reporting"));

        QWidget * widget_customerinfo = new QWidget() ;
        QFormLayout * layout_customerinfo = new QFormLayout() ;
        widget_customerinfo->setLayout(layout_customerinfo);
        
        m_autourl_allowed = new QCheckBox(tr("Allow the Automatic Opening of URL's"));
        m_autourl_allowed->setChecked(m_config["guioptions.autourl_allowed"].toUInt() == Qt::Checked);
        layout_customerinfo->addRow(m_autourl_allowed);
        
        m_tablimit_sbox = new QSpinBox(this);
        m_tablimit_sbox->setRange(0, 99);
        m_tablimit_sbox->setValue(m_config["guioptions.sheet-tablimit"].toUInt());
        layout_customerinfo->addRow(tr("Tab limit"), m_tablimit_sbox);
        
    m_function_tabwidget->addTab(widget_customerinfo, tr("Customer Info"));

        QWidget * widget_dial = new QWidget() ;
        QFormLayout * layout_dial = new QFormLayout() ;
        widget_dial->setLayout(layout_dial);
        
        m_dial_history_size = new QSpinBox(this);
        m_dial_history_size->setRange(0, 20);
        int dial_history_size = m_config["dialpanel.history_length"].toInt();
        if ((dial_history_size < 0) || (dial_history_size > 20))
            dial_history_size = 5;
        m_dial_history_size->setValue(dial_history_size);
        layout_dial->addRow(tr("Lines of call history saved"), m_dial_history_size);

    m_function_tabwidget->addTab(widget_dial, tr("Dialer"));

        QWidget * widget_history = new QWidget() ;
        QFormLayout * layout_history = new QFormLayout() ;
        widget_history->setLayout(layout_history);
        
        m_history_sbox = new QSpinBox(this);
        m_history_sbox->setRange(1, 20);
        m_history_sbox->setValue(m_config["historysize"].toUInt());
        layout_history->addRow(tr("History size"), m_history_sbox);
        
    m_function_tabwidget->addTab(widget_history, tr("History"));
        
        QWidget * widget_contacts = new QWidget() ;
        QFormLayout * layout_contacts = new QFormLayout() ;
        widget_contacts->setLayout(layout_contacts);
        
        m_contactssize_sbox = new QSpinBox(this);
        m_contactssize_sbox->setRange(1, 500);
        m_contactssize_sbox->setValue(m_config["guioptions.contacts-max"].toUInt());
        layout_contacts->addRow(tr("Contacts' max number"), m_contactssize_sbox);
        
        m_contactswidth_sbox = new QSpinBox(this);
        m_contactswidth_sbox->setRange(0, 20);
        m_contactswidth_sbox->setValue(m_config["guioptions.contacts-width"].toUInt());
        layout_contacts->addRow(tr("Contacts per row (0 = auto)"), m_contactswidth_sbox);
        
        m_contacts_dblclick = new QComboBox(this);
        foreach (QString key, m_dblclick_actions.keys()) {
            m_contacts_dblclick->addItem(m_dblclick_actions[key], key);
        }
        int i_contacts_dblclick = m_contacts_dblclick->findData (m_config["doubleclick.searchpanel"]);
        m_contacts_dblclick->setCurrentIndex(i_contacts_dblclick);
        layout_contacts->addRow(tr("Double-click action"), m_contacts_dblclick);
        
    m_function_tabwidget->addTab(widget_contacts, tr("Contacts"));

        QWidget * widget_queues = new QWidget() ;
        QGridLayout * layout_queues = new QGridLayout() ;
        layout_queues->setAlignment(Qt::AlignTop|Qt::AlignHCenter);
        widget_queues->setLayout(layout_queues);
        
        int line = 0;
        int ncol = 1;
        foreach(ColorLevelStruct color, m_queue_colors) {
            layout_queues->addWidget(new QLabel(color.label, this), line, ncol++);
        }
        line++;

        layout_queues->addWidget(new QLabel(tr("Queue thresholds (waiting calls)"), this), line, 0);
        ncol = 1;
        foreach(ColorLevelStruct color, m_queue_colors) {
            m_queuelevels[color.id] = new QSpinBox(this);
            m_queuelevels[color.id]->setRange(0, 100);
            m_queuelevels[color.id]->setValue(m_config["guioptions.queuelevels"].toMap().value(color.id).toUInt());
            m_queuelevels[color.id]->setToolTip(tr("Thresholds to change the color of the queue, in number of waiting calls"));
            layout_queues->addWidget(m_queuelevels[color.id], line, ncol++);
        }
        line++;

        m_queue_longestwait = new QCheckBox(tr("Display queue's longest wait"), this);
        m_queue_longestwait->setChecked(m_config["guioptions.queue_longestwait"].toBool());
        layout_queues->addWidget(m_queue_longestwait, line, 0);
        line++;

        QLabel *queue_longestwait_thresholds
            = new QLabel(tr("Queue thresholds (longest wait)"), this);
        layout_queues->addWidget(queue_longestwait_thresholds, line, 0);
        ncol = 1;
        foreach(ColorLevelStruct color, m_queue_colors) {
            m_queuelevels_wait[color.id] = new QSpinBox(this);
            m_queuelevels_wait[color.id]->setRange(0, 3600);
            m_queuelevels_wait[color.id]->setValue(m_config["guioptions.queuelevels_wait"].toMap().value(color.id).toUInt());
            m_queuelevels_wait[color.id]->setToolTip(tr("Thresholds to change the color of the queue, in seconds of longest wait"));
            layout_queues->addWidget(m_queuelevels_wait[color.id], line, ncol++);
        }

        line++;
        m_queue_displaynu = new QCheckBox(tr("Display queue number"), this);
        m_queue_displaynu->setChecked(m_config["guioptions.queue_displaynu"].toBool());
        layout_queues->addWidget(m_queue_displaynu, line, 0);

    m_function_tabwidget->addTab(widget_queues, tr("Queues"));

        QWidget * widget_switchboard = new QWidget() ;
        QFormLayout * layout_switchboard = new QFormLayout() ;
        widget_switchboard->setLayout(layout_switchboard);

        m_comboswitchboard = new QComboBox(this);
        m_comboswitchboard->addItem(tr("Small"), QString("small"));
        m_comboswitchboard->addItem(tr("Detailed"), QString("detailed"));
        for(int i = 0; i < m_comboswitchboard->count(); i++) {
            if(m_config["guioptions.switchboard-elt-type"] == m_comboswitchboard->itemData(i))
                m_comboswitchboard->setCurrentIndex(i);
        }
        layout_switchboard->addRow(tr("Appearance of SwitchBoard elements"), m_comboswitchboard);

        m_maxWidthWanted = new QSpinBox(this);
        m_maxWidthWanted->setRange(50, 250);
        int maxwidthwanted = m_config["guioptions.maxwidthwanted"].toInt();
        if(maxwidthwanted < 50)
            maxwidthwanted = 200;
        m_maxWidthWanted->setValue(maxwidthwanted);
        layout_switchboard->addRow(tr("Maximum width for small SwitchBoard elements"), m_maxWidthWanted);
        
        m_switchboard_dblclick = new QComboBox(this);
        foreach (QString key, m_dblclick_actions.keys()) {
            m_switchboard_dblclick->addItem(m_dblclick_actions[key], key);
        }
        int i_switchboard_dblclick = m_switchboard_dblclick->findData (m_config["doubleclick.switchboard"].toString());
        m_switchboard_dblclick->setCurrentIndex(i_switchboard_dblclick);
        layout_switchboard->addRow(tr("Double-click action"), m_switchboard_dblclick);

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
    m_keeppass->setChecked(m_config["keeppass"].toBool());
    layout3->addRow(m_keeppass);

    m_autoconnect = new QCheckBox(tr("Autoconnect at startup"), this);
    m_autoconnect->setChecked(m_config["autoconnect"].toBool());
    layout3->addRow(m_autoconnect);

    m_showagselect = new QCheckBox(tr("Show the Agent options (like the\nones beneath) on first window"));
    m_showagselect->setChecked(m_config["showagselect"].toBool());
    layout3->addRow(m_showagselect);

    m_loginkind = new QComboBox(this);
    m_loginkind->addItem(QString(tr("No Agent")));
    m_loginkind->addItem(QString(tr("Agent (unlogged)")));
    m_loginkind->addItem(QString(tr("Agent (logged)")));
    m_loginkind->setCurrentIndex(m_config["guioptions.loginkind"].toUInt());
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
    m_locale_cbox->addItem(tr("Italian"), QString("it_IT"));
    m_locale_cbox->addItem(tr("Portuguese") + QString(" (%1)").arg(tr("incomplete")), QString("pt_BR"));
    m_locale_cbox->addItem(tr("Dutch")      + QString(" (%1)").arg(tr("incomplete")), QString("nl_NL"));
    m_locale_cbox->addItem(tr("Hungarian")  + QString(" (%1)").arg(tr("incomplete")), QString("hu_HU"));
    m_locale_cbox->addItem(tr("German")     + QString(" (%1)").arg(tr("incomplete")), QString("de_DE"));
    m_locale_cbox->addItem(tr("Japanese")   + QString(" (%1)").arg(tr("incomplete")), QString("ja_JP"));
    m_locale_cbox->addItem(tr("Spanish")    + QString(" (%1)").arg(tr("incomplete")), QString("es_ES"));
    for (int i = 0; i < m_locale_cbox->count(); i++) {
        if (m_config["forcelocale"].toString() == m_locale_cbox->itemData(i))
            m_locale_cbox->setCurrentIndex(i);
    }
    layout4->addRow(tr("Language"), new WarningWidget<QComboBox>(m_locale_cbox, reboot_message));

    QFrame *qhline5 = new QFrame(this);
    qhline5->setFrameShape(QFrame::HLine);
    layout4->addRow(qhline5);

    m_systrayed = new QCheckBox(tr("Systrayed at startup"), this);
    m_systrayed->setChecked(m_config["systrayed"].toBool());
    layout4->addRow(m_systrayed);

    // The value displayed is the inverse of the bool in memory
    m_unique = new QCheckBox(tr("Allow multiple instances of XiVO Client"), this);
    m_unique->setChecked(!m_config["uniqueinstance"].toBool());
    layout4->addRow(new WarningWidget<QCheckBox>(m_unique, reboot_message));

    m_qss = new QComboBox();
    m_qss->addItem(tr("Default style"), QString("none"));
    QDir qss_dir(":/") ;
    QStringList qss_files = qss_dir.entryList(QStringList("*.qss"));
    foreach(QString qss_file, qss_files) {
        qss_file.truncate(qss_file.length()-4);
        m_qss->addItem(qss_file, qss_file);
    }
    int qss_index = m_qss->findData(m_config["qss"]);
    if(qss_index != -1) {
        m_qss->setCurrentIndex(qss_index);
    }
    layout4->addRow(tr("Interface style"), new WarningWidget<QComboBox>(m_qss, reboot_message));

    m_clipboard = new QCheckBox(tr("Enable the clipboard")) ;
    m_clipboard->setChecked(m_config["enableclipboard"].toBool());
    layout4->addRow(new WarningWidget<QCheckBox>(m_clipboard, reboot_message));

    m_displayprofile = new QCheckBox(tr("Display the configuration profile")) ;
    m_displayprofile->setChecked(m_config["displayprofile"].toBool());
    layout4->addRow(m_displayprofile);

    m_activate_on_tel = new QCheckBox(tr("Activate the window when calling from external application")) ;
    m_activate_on_tel->setChecked(m_config["activate_on_tel"].toBool());
    layout4->addRow(new WarningWidget<QCheckBox>(m_activate_on_tel, reboot_message));

    m_reset_gui = new QPushButton(tr("Reset"));
    connect(m_reset_gui, SIGNAL(pressed()),
            m_parent, SLOT(resetState()));
    layout4->addRow(tr("Reset docks position"), m_reset_gui);

    m_tabwidget->addTab(widget_gui, tr("GUI Settings"));
}

/*! \brief advanced config tab */
void ConfigWidget::_insert_advanced_tab()
{
    QFormLayout * layout5 = new QFormLayout();
    QWidget * widget_adv = new QWidget();
    widget_adv->setLayout(layout5);

    m_logtofile = new QCheckBox(tr("Enable logging of program actions"));
    m_logtofile->setChecked(m_config["logtofile"].toBool());
    layout5->addRow(new WarningWidget<QCheckBox>(m_logtofile, reboot_message));

    m_logfilename = new QLineEdit();
    m_logfilename->setText(m_config["logfilename"].toString());
    layout5->addRow(tr("Logfile name"), new WarningWidget<QLineEdit>(m_logfilename, reboot_message));

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
                      .arg(QKeySequence(m_config["guioptions.xlet_operator_key" + m_operator_action[index].action].toInt()).toString()));
      m_currentKeyChange = index;
  } else {
    button->setText(tr("(current: %0) click to change")
                    .arg(QKeySequence(m_config["guioptions.xlet_operator_key" + m_operator_action[index].action].toInt()).toString()));
    old = NULL;
  }
}

/*! \brief operator xlet config tab */
void ConfigWidget::_insert_operator_functiontab()
{
     m_operator_action[ANSWER].action = QString("answer");
     m_operator_action[ANSWER].translation = tr("Answer");
     m_operator_action[HANGUP].action = QString("hangup");
     m_operator_action[HANGUP].translation = tr("Hangup");
     m_operator_action[DTXFER].action = QString("dtransfer");
     m_operator_action[DTXFER].translation = tr("D. Transfer");
     m_operator_action[ITXFER].action = QString("itransfer");
     m_operator_action[ITXFER].translation = tr("I. Transfer");
     m_operator_action[ILINK].action = QString("ilink");
     m_operator_action[ILINK].translation = tr("I. Link");
     m_operator_action[ICANCEL].action = QString("icancel");
     m_operator_action[ICANCEL].translation = tr("I. Cancel");
     m_operator_action[ATXFER_FINAL].action = QString("atxferfinalize");
     m_operator_action[ATXFER_FINAL].translation = tr("Finalize Transfer");
     m_operator_action[CANCEL_TXFER].action = QString("atxfercancel");
     m_operator_action[CANCEL_TXFER].translation = tr("Cancel Transfer");
     m_operator_action[PARK].action = QString("park");
     m_operator_action[PARK].translation = tr("Park");
    int i;

    QGridLayout *glayout = new QGridLayout();
    QWidget *root_widget = new QWidget();
    root_widget->setLayout(glayout);
    QPushButton *selectKey;

    glayout->addWidget(new QLabel(tr("Operator action")), 0, 1);
    glayout->addWidget(new QLabel(tr("Key binding")), 0, 2);

    for(i=0;i<NB_OP_ACTIONS;i++) {
        selectKey = new QPushButton(tr("(current: %0) click to change")
                                    .arg(QKeySequence(m_config["guioptions.xlet_operator_key" + m_operator_action[i].action].toInt()).toString()),
                                    root_widget);
        selectKey->setCheckable(true);
        selectKey->setProperty("i", i);
        m_operator_action[i].button = selectKey;
        glayout->addWidget(new QLabel(m_operator_action[i].translation), i+1 ,1);
        glayout->addWidget(selectKey, i+1, 2);

        connect(selectKey, SIGNAL(toggled(bool)), this, SLOT(changeOperatorKey(bool)));
    }


    m_operator_answer_work = new QCheckBox(tr("Display an answer action"));
    m_operator_answer_work->setChecked(m_config["guioptions.xlet_operator_answer_work"].toInt());
    glayout->addWidget(m_operator_answer_work, ++i , 1, 1, 2);

    glayout->addWidget(new QLabel(tr("Any change here requires an application restart to be effective")), ++i, 1, 1, 2);

    m_function_tabwidget->addTab(root_widget, tr("Operator"));
}


ConfigWidget::~ConfigWidget()
{
//    qDebug() << Q_FUNC_INFO << b_engine;
}

void ConfigWidget::keyPressEvent(QKeyEvent *e)
{
    if ((m_currentKeyChange == -1) || (e->modifiers() != Qt::NoModifier))
        return;

    int i, already_bound = -1;

    for(i=0;i<NB_OP_ACTIONS;i++) {
        if ((m_config["guioptions.xlet_operator_key" + m_operator_action[i].action].toInt() == e->key()) &&
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

    m_config["guioptions.xlet_operator_key" + m_operator_action[m_currentKeyChange].action] = e->key();
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
    m_config["cti_address"] = m_main_server_address_input->text();
    m_config["cti_port"] = m_main_server_port_input->value();
    m_config["cti_encrypt"] = m_main_server_encrypt_input->isChecked();
    m_config["cti_backup_address"] = m_backup_server_address_input->text();
    m_config["cti_backup_port"] = m_backup_server_port_input->value();
    m_config["cti_backup_encrypt"] = m_backup_server_encrypt_input->isChecked();
    m_config["company"] = m_context->text();
    m_config["keeppass"] = m_keeppass->isChecked();
    m_config["showagselect"] = m_showagselect->isChecked();
    m_config["userlogin"] = m_userid->text();
    m_config["agentphonenumber"] = m_agentphonenumber->text();
    m_config["password"] = m_password->text();
    m_config["forcelocale"] = m_locale_cbox->itemData(m_locale_cbox->currentIndex()); // not currentText()
    m_config["autoconnect"] = m_autoconnect->isChecked();
    m_config["trytoreconnect"] = m_trytoreconnect->isChecked();
    m_config["trytoreconnectinterval"] = m_tryinterval_sbox->value() * 1000;
    m_config["keepaliveinterval"] = m_keepalive_input->value() * 1000;
    m_config["historysize"] = m_history_sbox->value();
    m_config["systrayed"] = m_systrayed->isChecked();
    m_config["uniqueinstance"] = !m_unique->isChecked();
    m_config["qss"] = m_qss->itemData(m_qss->currentIndex()); // not currentText()
    m_config["enableclipboard"] = m_clipboard->isChecked();
    m_config["logtofile"] = m_logtofile->isChecked();
    m_config["logfilename"] = m_logfilename->text();
    m_config["displayprofile"] = m_displayprofile->isChecked();
    m_config["activate_on_tel"] = m_activate_on_tel->isChecked();
    m_config["dialpanel.history_length"] = m_dial_history_size->value();
    m_config["doubleclick.searchpanel"] = m_contacts_dblclick->itemData(m_contacts_dblclick->currentIndex());
    m_config["doubleclick.switchboard"] = m_switchboard_dblclick->itemData(m_switchboard_dblclick->currentIndex());

    foreach(QString function, func_legend.keys())
        m_config["checked_function." + function] = m_function[function]->isChecked();

    for(i=0;i<NB_OP_ACTIONS;i++) {
        m_config["guioptions.xlet_operator_key" + m_operator_action[i].action] =
            m_config["guioptions.xlet_operator_key" + m_operator_action[i].action].toInt();
    }
    m_config["guioptions.xlet_operator_answer_work"] = m_operator_answer_work->isChecked();

    QVariantMap qvm, qvm2;

    foreach(ColorLevelStruct color, m_queue_colors) {
        qvm[color.id] = QVariant(m_queuelevels[color.id]->value());
        qvm2[color.id] = QVariant(m_queuelevels_wait[color.id]->value());
    }
    m_config["guioptions.queuelevels"] = qvm;
    m_config["guioptions.queuelevels_wait"] = qvm2;

    m_config["guioptions.loginkind"] = m_loginkind->currentIndex();

    m_config["guioptions.contacts-max"] = m_contactssize_sbox->value();
    m_config["guioptions.contacts-width"] = m_contactswidth_sbox->value();
    m_config["guioptions.sheet-tablimit"] = m_tablimit_sbox->value();
    /*!
     * \todo Store autourl_allowed as bool in m_config, not int
     */
    m_config["guioptions.autourl_allowed"] = m_autourl_allowed->checkState();
    m_config["guioptions.queue_longestwait"] = m_queue_longestwait->isChecked();
    m_config["guioptions.queue_displaynu"] = m_queue_displaynu->isChecked();
    m_config["guioptions.switchboard-elt-type"] = m_comboswitchboard->itemData(m_comboswitchboard->currentIndex()); // not currentText()
    m_config["guioptions.maxwidthwanted"] = m_maxWidthWanted->value();
    m_config["guioptions.presenceindicatorsize"] = m_presenceIndicatorSize->value();

    b_engine->setConfig(m_config);

    close();
}

/*!
 * Save the current tab and close
 */

bool ConfigWidget::close()
{
    b_engine->getSettings()->setValue("display/configtab", m_tabwidget->currentIndex());
    return QDialog::close();
}

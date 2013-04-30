/* XiVO Client
 * Copyright (C) 2007-2013, Avencall
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
#include <QSettings>
#include <QSpinBox>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QPixmap>
#include <QDir>
#include <QFrame>

#include <baseengine.h>
#include <xivoconsts.h>
#include <xletlib/warningwidget.h>

#include "config_widget.h"

QHash<QString, QString> func_legend;
QString reboot_message;

ConfigWidget::ConfigWidget(QWidget *parent)
    : QDialog(parent)
{
    reboot_message = tr("You must restart the program for this setting to apply.");
    m_parent = parent;
    setWindowTitle(tr("Configuration"));
    setAttribute(Qt::WA_DeleteOnClose);

    // OLD
    QVBoxLayout * vlayout = new QVBoxLayout();
    m_tabwidget = new QTabWidget();

    m_config = b_engine->getConfig();

    m_dblclick_actions["call"] = tr("Call");

    // NEW
    QVBoxLayout * config_layout = new QVBoxLayout();
    QWidget * config_widget = new QWidget(this);
    this->ui.setupUi(config_widget);
    config_layout->addWidget(config_widget);

    this->load_values();
    this->ui.tabWidget->setCurrentIndex(b_engine->getSettings()->value("display/configtab", 0).toInt());
    connect(this->ui.buttonBox, SIGNAL(accepted()), this, SLOT(saveAndClose()));
    connect(this->ui.buttonBox, SIGNAL(rejected()), this, SLOT(close()));

    // TEMP
    QHBoxLayout * both_versions_layout = new QHBoxLayout(this);
    both_versions_layout->addLayout(vlayout);
    both_versions_layout->addLayout(config_layout);
}

ConfigWidget::~ConfigWidget()
{
}

void ConfigWidget::load_values()
{
    // User
    this->ui.language->addItem(tr("System Language"), QString("default"));
    this->ui.language->addItem(tr("Dutch"), QString("nl_NL"));
    this->ui.language->addItem(tr("English"), QString("en_US"));
    this->ui.language->addItem(tr("French"), QString("fr_FR"));
    this->ui.language->addItem(tr("Italian"), QString("it_IT"));
    this->ui.language->addItem(tr("German") + QString(" (%1)").arg(tr("incomplete")), QString("de_DE"));
    this->ui.language->addItem(tr("Hungarian") + QString(" (%1)").arg(tr("incomplete")), QString("hu_HU"));
    this->ui.language->addItem(tr("Japanese") + QString(" (%1)").arg(tr("incomplete")), QString("ja_JP"));
    this->ui.language->addItem(tr("Portuguese") + QString(" (%1)").arg(tr("incomplete")), QString("pt_BR"));
    this->ui.language->addItem(tr("Spanish") + QString(" (%1)").arg(tr("incomplete")), QString("es_ES"));
    for (int i = 0; i < this->ui.language->count(); i++) {
        if (this->m_config["forcelocale"].toString() == this->ui.language->itemData(i))
            this->ui.language->setCurrentIndex(i);
    }

    this->ui.style->addItem(tr("Default style"), QString("none"));
    QDir qss_dir(":/") ;
    QStringList qss_files = qss_dir.entryList(QStringList("*.qss"));
    foreach(QString qss_file, qss_files) {
        qss_file.truncate(qss_file.length()-4);
        this->ui.style->addItem(qss_file, qss_file);
    }
    int qss_index = this->ui.style->findData(this->m_config["qss"]);
    if(qss_index != -1) {
        this->ui.style->setCurrentIndex(qss_index);
    }

    this->ui.auto_reconnect->setChecked(this->m_config["trytoreconnect"].toBool());
    this->ui.startup_connect->setChecked(this->m_config["autoconnect"].toBool());
    this->ui.start_systrayed->setChecked(this->m_config["systrayed"].toBool());
    this->ui.show_window_on_call->setChecked(this->m_config["activate_on_tel"].toBool());
    this->ui.enable_clipboard->setChecked(this->m_config["enableclipboard"].toBool());

    // Features
    /// Presence
    this->ui.enable_presence_reporting->setChecked(this->m_config["checked_function.presence"].toBool());

    /// Screen popup
    this->ui.enable_screen_popup->setChecked(this->m_config["checked_function.customerinfo"].toBool());
    this->ui.enable_auto_open_url->setChecked(this->m_config["guioptions.autourl_allowed"].toBool());
    this->ui.max_screen_popups->setValue(this->m_config["guioptions.sheet-tablimit"].toUInt());

    /// History
    this->ui.history_size->setValue(this->m_config["historysize"].toUInt());

    // Contact
    this->ui.max_contacts_displayed->setValue(this->m_config["guioptions.contacts-max"].toUInt());

    /// Queue list
    this->ui.show_longest_wait_time->setChecked(this->m_config["guioptions.queue_longestwait"].toBool());
    this->ui.longest_wait_time_green->setValue(this->m_config["guioptions.queuelevels_wait"].toMap().value("green").toUInt());
    this->ui.longest_wait_time_orange->setValue(this->m_config["guioptions.queuelevels_wait"].toMap().value("orange").toUInt());
    this->ui.waiting_calls_green->setValue(this->m_config["guioptions.queuelevels"].toMap().value("green").toUInt());
    this->ui.waiting_calls_orange->setValue(this->m_config["guioptions.queuelevels"].toMap().value("orange").toUInt());

    /// Queue members
    this->ui.hide_unlogged_agents->setChecked(this->m_config["guioptions.queue_members_hide_unlogged_agents"].toBool());


    // Administration
    this->ui.server->setText(this->m_config["cti_address"].toString());
    this->ui.port->setValue(this->m_config["cti_port"].toUInt());
    this->ui.encrypted->setChecked(this->m_config["cti_encrypt"].toBool());
    this->ui.backup_server->setText(this->m_config["cti_backup_address"].toString());
    this->ui.backup_port->setValue(this->m_config["cti_backup_port"].toUInt());
    this->ui.backup_encrypted->setChecked(this->m_config["cti_backup_encrypt"].toBool());
    this->ui.show_agent_options->setChecked(this->m_config["showagselect"].toBool());


    // Advanced
    this->ui.auto_reconnect_interval->setValue(this->m_config["trytoreconnectinterval"].toUInt() / 1000);
    this->ui.keepalive_interval->setValue(this->m_config["keepaliveinterval"].toUInt() / 1000);
    this->ui.allow_multiple_instances->setChecked(!this->m_config["uniqueinstance"].toBool());
}

void ConfigWidget::loginKindChanged(int index)
{
    if (index == 0) {
        m_agentphonenumber->setEnabled(false);
    } else {
        m_agentphonenumber->setEnabled(true);
    }
}

void ConfigWidget::saveAndClose()
{
    // User
    m_config["forcelocale"] = this->ui.language->itemData(this->ui.language->currentIndex());
    m_config["qss"] = this->ui.style->itemData(this->ui.style->currentIndex());
    m_config["trytoreconnect"] = this->ui.auto_reconnect->isChecked();
    m_config["autoconnect"] = this->ui.startup_connect->isChecked();
    m_config["systrayed"] = this->ui.start_systrayed->isChecked();
    m_config["activate_on_tel"] = this->ui.show_window_on_call->isChecked();
    m_config["enableclipboard"] = this->ui.enable_clipboard->isChecked();


    // Features
    /// Presence
    m_config["checked_function.presence"] = this->ui.enable_presence_reporting->isChecked();

    /// Screen popup
    m_config["checked_function.customerinfo"] = this->ui.enable_screen_popup->isChecked();
    m_config["guioptions.autourl_allowed"] = this->ui.enable_auto_open_url->isChecked();
    m_config["guioptions.sheet-tablimit"] = this->ui.max_screen_popups->value();

    /// History
    m_config["historysize"] = this->ui.history_size->value();

    // Contact
    m_config["guioptions.contacts-max"] = this->ui.max_contacts_displayed->value();

    /// Queue list
    m_config["guioptions.queue_longestwait"] = this->ui.show_longest_wait_time->isChecked();

    QVariantMap qvm, qvm2;
    qvm["green"] = QVariant(this->ui.longest_wait_time_green->value());
    qvm["orange"] = QVariant(this->ui.longest_wait_time_orange->value());
    qvm2["green"] = QVariant(this->ui.longest_wait_time_green->value());
    qvm2["orange"] = QVariant(this->ui.longest_wait_time_orange->value());
    m_config["guioptions.queuelevels"] = qvm;
    m_config["guioptions.queuelevels_wait"] = qvm2;

    /// Queue members
    m_config["guioptions.queue_members_hide_unlogged_agents"] = this->ui.hide_unlogged_agents->isChecked();


    // Administration
    m_config["cti_address"] = this->ui.server->text();
    m_config["cti_port"] = this->ui.port->value();
    m_config["cti_encrypt"] = this->ui.encrypted->isChecked();
    m_config["cti_backup_address"] = this->ui.backup_server->text();
    m_config["cti_backup_port"] = this->ui.backup_port->value();
    m_config["cti_backup_encrypt"] = this->ui.backup_encrypted->isChecked();
    m_config["showagselect"] = this->ui.show_agent_options->isChecked();


    // Advanced
    m_config["trytoreconnectinterval"] = this->ui.auto_reconnect_interval->value() * 1000;
    m_config["keepaliveinterval"] = this->ui.keepalive_interval->value() * 1000;
    m_config["uniqueinstance"] = !this->ui.allow_multiple_instances->isChecked();

/*
    m_config["keeppass"] = m_keeppass->isChecked();
    m_config["userlogin"] = m_userid->text();
    m_config["agentphonenumber"] = m_agentphonenumber->text();
    m_config["password"] = m_password->text();
    m_config["logtofile"] = m_logtofile->isChecked();
    m_config["logfilename"] = m_logfilename->text();
    m_config["displayprofile"] = m_displayprofile->isChecked();
    m_config["dialpanel.history_length"] = m_dial_history_size->value();
    m_config["doubleclick.searchpanel"] = m_contacts_dblclick->itemData(m_contacts_dblclick->currentIndex());
    m_config["switchboard_queue_name"] = m_switchboard_queue_name->text();
    m_config["switchboard_hold_queue_name"] = m_switchboard_hold_queue_name->text();

    m_config["guioptions.loginkind"] = m_loginkind->currentIndex();
    m_config["guioptions.contacts-width"] = m_contactswidth_sbox->value();
    m_config["guioptions.queue_displaynu"] = m_queue_displaynu->isChecked();
    m_config["guioptions.presenceindicatorsize"] = m_presenceIndicatorSize->value();
*/
    b_engine->setConfig(m_config);

    close();
}

bool ConfigWidget::close()
{
    b_engine->getSettings()->setValue("display/configtab", m_tabwidget->currentIndex());
    return QDialog::close();
}

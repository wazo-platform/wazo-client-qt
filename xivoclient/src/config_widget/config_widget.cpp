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

#include <assembler.h>
#include <baseengine.h>
#include <xivoconsts.h>
#include <xletlib/warningwidget.h>

#include "config_widget.h"

const QString reboot_message = "You must restart the program for this setting to apply.";

ConfigWidget::ConfigWidget(MainWindow *parent)
    : QDialog(parent)
{
    this->ui.setupUi(this);
    this->connect(this->ui.buttonBox, SIGNAL(accepted()), SLOT(accept()));
    this->connect(this->ui.buttonBox, SIGNAL(rejected()), SLOT(reject()));
    parent->connect(this->ui.reset_docks, SIGNAL(pressed()), SLOT(restoreDefaultState()));
    this->connect(parent, SIGNAL(initialized()), SLOT(initialize()));
}

ConfigWidget::~ConfigWidget()
{
}

void ConfigWidget::initialize()
{
    this->m_config = b_engine->getConfig();
    this->load_values();
    this->setModal(true);
    this->ui.tabWidget->setCurrentIndex(b_engine->getSettings()->value("display/configtab", 0).toInt());
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

    /// Composer
    this->ui.composer_history_size->setValue(this->m_config["dialpanel.history_length"].toUInt());

    /// History
    this->ui.history_size->setValue(this->m_config["historysize"].toUInt());

    /// Contact
    this->ui.max_contacts_displayed->setValue(this->m_config["guioptions.contacts-max"].toUInt());

    /// Queue list
    this->ui.show_longest_wait_time->setChecked(this->m_config["guioptions.queue_longestwait"].toBool());
    this->ui.longest_wait_time_green->setValue(this->m_config["guioptions.queuelevels_wait"].toMap().value("green").toUInt());
    this->ui.longest_wait_time_orange->setValue(this->m_config["guioptions.queuelevels_wait"].toMap().value("orange").toUInt());
    this->ui.waiting_calls_green->setValue(this->m_config["guioptions.queuelevels"].toMap().value("green").toUInt());
    this->ui.waiting_calls_orange->setValue(this->m_config["guioptions.queuelevels"].toMap().value("orange").toUInt());

    /// Queue members
    this->ui.hide_unlogged_agents->setChecked(this->m_config["guioptions.queue_members_hide_unlogged_agents"].toBool());

    /// Switchboard
    this->ui.switchboard_queue_name->setText(this->m_config["switchboard_queue_name"].toString());
    this->ui.switchboard_call_on_hold_queue_name->setText(this->m_config["switchboard_hold_queue_name"].toString());


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
    this->ui.show_displayprofile->setChecked(this->m_config["displayprofile"].toBool());
}

void ConfigWidget::accept()
{
    // User
    this->m_config["forcelocale"] = this->ui.language->itemData(this->ui.language->currentIndex());
    this->m_config["qss"] = this->ui.style->itemData(this->ui.style->currentIndex());
    this->m_config["trytoreconnect"] = this->ui.auto_reconnect->isChecked();
    this->m_config["autoconnect"] = this->ui.startup_connect->isChecked();
    this->m_config["systrayed"] = this->ui.start_systrayed->isChecked();
    this->m_config["activate_on_tel"] = this->ui.show_window_on_call->isChecked();
    this->m_config["enableclipboard"] = this->ui.enable_clipboard->isChecked();


    // Features
    /// Presence
    this->m_config["checked_function.presence"] = this->ui.enable_presence_reporting->isChecked();

    /// Screen popup
    this->m_config["checked_function.customerinfo"] = this->ui.enable_screen_popup->isChecked();
    this->m_config["guioptions.autourl_allowed"] = this->ui.enable_auto_open_url->isChecked();
    this->m_config["guioptions.sheet-tablimit"] = this->ui.max_screen_popups->value();

    /// Composer
    this->m_config["dialpanel.history_length"] = this->ui.composer_history_size->value();

    /// History
    this->m_config["historysize"] = this->ui.history_size->value();

    /// Contact
    this->m_config["guioptions.contacts-max"] = this->ui.max_contacts_displayed->value();

    /// Queue list
    this->m_config["guioptions.queue_longestwait"] = this->ui.show_longest_wait_time->isChecked();

    QVariantMap qvm, qvm2;
    qvm["green"] = QVariant(this->ui.waiting_calls_green->value());
    qvm["orange"] = QVariant(this->ui.waiting_calls_orange->value());
    qvm2["green"] = QVariant(this->ui.longest_wait_time_green->value());
    qvm2["orange"] = QVariant(this->ui.longest_wait_time_orange->value());
    this->m_config["guioptions.queuelevels"] = qvm;
    this->m_config["guioptions.queuelevels_wait"] = qvm2;

    /// Queue members
    this->m_config["guioptions.queue_members_hide_unlogged_agents"] = this->ui.hide_unlogged_agents->isChecked();

    /// Switchboard
    this->m_config["switchboard_queue_name"] = this->ui.switchboard_queue_name->text();
    this->m_config["switchboard_hold_queue_name"] = this->ui.switchboard_call_on_hold_queue_name->text();


    // Administration
    this->m_config["cti_address"] = this->ui.server->text();
    this->m_config["cti_port"] = this->ui.port->value();
    this->m_config["cti_encrypt"] = this->ui.encrypted->isChecked();
    this->m_config["cti_backup_address"] = this->ui.backup_server->text();
    this->m_config["cti_backup_port"] = this->ui.backup_port->value();
    this->m_config["cti_backup_encrypt"] = this->ui.backup_encrypted->isChecked();
    this->m_config["showagselect"] = this->ui.show_agent_options->isChecked();


    // Advanced
    this->m_config["trytoreconnectinterval"] = this->ui.auto_reconnect_interval->value() * 1000;
    this->m_config["keepaliveinterval"] = this->ui.keepalive_interval->value() * 1000;
    this->m_config["uniqueinstance"] = !this->ui.allow_multiple_instances->isChecked();
    this->m_config["displayprofile"] = this->ui.show_displayprofile->isChecked();

    b_engine->getSettings()->setValue("display/configtab", this->ui.tabWidget->currentIndex());

    b_engine->setConfig(m_config);

    QDialog::accept();
}

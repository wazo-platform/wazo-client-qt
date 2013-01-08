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

#ifndef __CONFIGWIDGET_H__
#define __CONFIGWIDGET_H__

#include <QDialog>
#include <QHash>
#include <QVariantMap>
#include <QMessageBox>
#include <QHBoxLayout>

#include <functests.h>

class QCheckBox;
class QComboBox;
class QDialogButtonBox;
class QLabel;
class QLineEdit;
class QSpinBox;
class QTabWidget;

class BaseEngine;

/*If you want to update the settings window, you need to look for every
 * setting wanted in the application.
 * First you can get every QSettings variable with a command like :
 *     grep -ron '[[:space:]]QSettings[^;]*;' xivoclient common baselib
 * Then you can get every setting by typing a command like :
 *     egrep -ron '(variables)->value\("[^)]*\)' xivoclient common baselib
 * where variables stands for the list of the variables obtained by the
 * previous command, separated by |
 */

class ConfigWidget: public QDialog
{
    Q_OBJECT
    FUNCTESTED

    public:
        ConfigWidget(QWidget *parent=0);
        QSize sizeHint();
        ~ConfigWidget();

    public slots:
        bool close();

    private slots:
        void saveAndClose();
        void loginKindChanged(int);

    private:
        void createColors();
        void _insert_connection_tab();
        void _insert_account_tab();
        void _insert_guisetting_tab();
        void _insert_function_tab();
        void _insert_advanced_tab();

        QWidget * m_parent;

        QVariantMap m_config;

        struct ColorLevelStruct {
            QString id;
            QString label;
        };
        QList<ColorLevelStruct> m_queue_colors;

        QMap<QString, QString> m_dblclick_actions;

        QLineEdit * m_main_server_address_input;
        QSpinBox  * m_main_server_port_input;
        QCheckBox * m_main_server_encrypt_input;
        QCheckBox * m_backup_server_enabled;
        QLineEdit * m_backup_server_address_input;
        QSpinBox  * m_backup_server_port_input;
        QCheckBox * m_backup_server_encrypt_input;
        QCheckBox * m_trytoreconnect;
        QSpinBox  * m_tryinterval_sbox;
        QSpinBox  * m_keepalive_input;

        QLineEdit * m_context;
        QLineEdit * m_userid;
        QLineEdit * m_password;
        QCheckBox * m_keeppass;
        QCheckBox * m_autoconnect;
        QCheckBox * m_showagselect;
        QComboBox * m_loginkind;
        QLineEdit * m_agentphonenumber;

        QComboBox * m_locale_cbox;
        QCheckBox * m_systrayed;
        QCheckBox * m_unique;
        QComboBox * m_qss;
        QCheckBox * m_clipboard;

        QHash<QString, QCheckBox *> m_function;

        QTabWidget * m_function_tabwidget;

        QSpinBox * m_presenceIndicatorSize;

        QCheckBox * m_autourl_allowed;
        QSpinBox  * m_tablimit_sbox;

        QSpinBox  * m_dial_history_size;

        QSpinBox  * m_history_sbox;

        QSpinBox  * m_contactssize_sbox;
        QSpinBox  * m_contactswidth_sbox;
        QComboBox * m_contacts_dblclick;

        QCheckBox * m_hide_unlogged_agents;

        QHash<QString, QSpinBox *> m_queuelevels;
        QHash<QString, QSpinBox *> m_queuelevels_wait;
        QCheckBox * m_queue_longestwait;
        QCheckBox * m_queue_displaynu;

        QDialogButtonBox * m_btnbox;
        QTabWidget * m_tabwidget;

        QCheckBox * m_logtofile;
        QLineEdit * m_logfilename;

        QCheckBox * m_displayprofile;
        QCheckBox * m_activate_on_tel;

        QPushButton * m_reset_gui;
};

#endif

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

#include <xletlib/functests.h>

#include <ui_config_widget.h>

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
        void load_values();

        QWidget * m_parent;
        QTabWidget * m_tabwidget;
        QVariantMap m_config;
        QLineEdit * m_userid;
        QLineEdit * m_password;
        QCheckBox * m_keeppass;
        QCheckBox * m_autoconnect;
        QCheckBox * m_showagselect;
        QComboBox * m_loginkind;
        QLineEdit * m_agentphonenumber;

        Ui::ConfigWidget ui;
};

#endif

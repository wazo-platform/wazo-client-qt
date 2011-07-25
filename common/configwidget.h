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

#ifndef __CONFIGWIDGET_H__
#define __CONFIGWIDGET_H__

#include <QDialog>
#include <QHash>
#include <QVariantMap>
#include <QMessageBox>
#include <QHBoxLayout>

class QCheckBox;
class QComboBox;
class QDialogButtonBox;
class QLabel;
class QLineEdit;
class QSpinBox;
class QTabWidget;

class BaseEngine;

/*! \brief Configuration Window
 *
 * This Widget enables the user to edit the connection
 * parameters to the identification server.
 * 
 * If you want to update the settings window, you need to look for every
 * setting wanted in the application.
 * First you can get every QSettings variable with a command like :
 * \code $ grep -ron '[[:space:]]QSettings[^;]*;' xivoclient common baselib \endcode
 * Then you can get every setting by typing a command like :
 * \code $ egrep -ron '(<variables>)->value\("[^)]*\)' xivoclient common baselib \endcode
 * where <variable> stands for the list of the variables obtained by the 
 * previous command, separated by |*/
class ConfigWidget: public QDialog
{
    Q_OBJECT

    public:
        ConfigWidget(QWidget *parent=0);
        QSize sizeHint();
        ~ConfigWidget();

    signals:
        void confUpdated();

    private slots:
        //! Save the configuration to the BaseEngine object and close
        void saveAndClose();
        void changeOperatorKey(bool);
        void loginKindChanged(int);
    protected:
        virtual void keyPressEvent(QKeyEvent *);

    private:
        void _insert_connection_tab();
        void _insert_account_tab();
        void _insert_guisetting_tab();
        void _insert_function_tab();
        void _insert_operator_functiontab();
        void _insert_advanced_tab();

        QVariantMap m_opts;
        QVariantMap m_forcedopts;
        QVariantMap m_config;

        BaseEngine * m_engine;          //!< BaseEngine object parameters are commited to

        QLineEdit * m_cti_address;      //!< IP/hostname of the server
        QSpinBox  * m_cti_port;         //!< server port
        QCheckBox * m_cti_encrypt;      //!< encrypt connection
        QCheckBox * m_trytoreconnect;   //!< "Try to reconnect" Checkbox
        QSpinBox  * m_tryinterval_sbox; //!< "Try to reconnect" interval
        QSpinBox  * m_kainterval_sbox;  //!< Keep alive interval

        QLineEdit * m_context;          //!< context name (related to the company)
        QLineEdit * m_userid;           //!< user login
        QLineEdit * m_password;         //!< user password
        QCheckBox * m_keeppass;         //!< keep password ?
        QCheckBox * m_autoconnect;      //!< "Auto connect" checkbox
        QCheckBox * m_showagselect;     //!< show agent select on main window ?
        QComboBox * m_loginkind;        //!< login kind (user or agent)
        QLineEdit * m_agentphonenumber; //!< agent's phone number

        QComboBox * m_locale_cbox;      //!< Locale selectbox
        QCheckBox * m_systrayed;        //!< "Systray at startup" Checkbox
        QCheckBox * m_unique;           //!< Can multiple instances of the program run simultaneously?
        QLineEdit * m_qss;              //!< Qt style sheet file. Only the name of the file, without qss.
        QCheckBox * m_clipboard;        //!< Clipboard enabled?

        QHash<QString, QCheckBox *> m_function; //!< connect to functions checkboxes
        
        QTabWidget * m_function_tabwidget; //!< Contains the settings for the xlets
        
        QSpinBox * m_presenceIndicatorSize; //!< size of the presence indicator for basic peer widgets
        
        QCheckBox * m_autourl_allowed;  //!< Allow automatic opening of urls
        QSpinBox  * m_tablimit_sbox;    //!< Maximum number of tabs
        
        QSpinBox  * m_history_sbox;     //!< History size
        
        QSpinBox  * m_contactssize_sbox;  //!< Displayed contacts' size
        QSpinBox  * m_contactswidth_sbox; //!< Number of contacts displayed on one line

        QHash<QString, QSpinBox *> m_queuelevels;      //!< For queue display
        QHash<QString, QSpinBox *> m_queuelevels_wait; //!< For queue display
        QCheckBox * m_queue_longestwait;               //!< should we display the longest wait in the queue xlet ?
        QCheckBox * m_queue_displaynu;                 //!< should we display the queue number in queue name ?

        QComboBox * m_comboswitchboard; //!< Appearance of switchboard
        QSpinBox * m_maxWidthWanted;    //!< Maximum width for small items in switchboard

        QDialogButtonBox * m_btnbox;    //!< Buttons box
        QTabWidget * m_tabwidget;       //!< Tabs to access configuration widgets

        struct {
            QString action;
            QString translation;
            QPushButton *button;
        } m_operator_action[9];

        QCheckBox * m_operator_answer_work; //!< should we display the answer key in operator xlet ?

        int m_currentKeyChange;
        
        QCheckBox * m_logtofile;
        QLineEdit * m_logfilename;
        
        QCheckBox * m_displayprofile;
};

/*! \brief Wrapper to display a warning icon beside a QWidget*/
class WarningWidget : public QWidget
{
    Q_OBJECT
    
    private:
        QHBoxLayout * layout;
    public:
        WarningWidget(QWidget *, QString s = "");
};

#endif

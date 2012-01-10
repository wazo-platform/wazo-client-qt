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
 * \code $ egrep -ron '(variables)->value\("[^)]*\)' xivoclient common baselib \endcode
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
        //! Save the configuration to the BaseEngine object and close
        void saveAndClose();
        void changeOperatorKey(bool);
        void changeEncrypted(bool);
        void loginKindChanged(int);
    protected:
        virtual void keyPressEvent(QKeyEvent *);

    private:
        void createColors();
        void _insert_connection_tab();
        void _insert_account_tab();
        void _insert_guisetting_tab();
        void _insert_function_tab();
        void _insert_operator_functiontab();
        void _insert_advanced_tab();

        QWidget * m_parent;

        QVariantMap m_config;

        struct ColorLevelStruct {
            QString id;
            QString label;
        };
        QList<ColorLevelStruct> m_queue_colors;

        QMap<QString, QString> m_dblclick_actions;

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
        QComboBox * m_qss;              //!< Qt style sheet file. Only the name of the file, without qss.
        QCheckBox * m_clipboard;        //!< Clipboard enabled?

        QHash<QString, QCheckBox *> m_function; //!< connect to functions checkboxes

        QTabWidget * m_function_tabwidget; //!< Contains the settings for the xlets

        QSpinBox * m_presenceIndicatorSize; //!< size of the presence indicator for basic peer widgets

        QCheckBox * m_autourl_allowed;  //!< Allow automatic opening of urls
        QSpinBox  * m_tablimit_sbox;    //!< Maximum number of tabs

        QSpinBox  * m_dial_history_size; //!< Dial history saved

        QSpinBox  * m_history_sbox;     //!< History size

        QSpinBox  * m_contactssize_sbox;  //!< Displayed contacts' size
        QSpinBox  * m_contactswidth_sbox; //!< Number of contacts displayed on one line
        QComboBox * m_contacts_dblclick;  //!< Action to do when a contact is double clicked

        QHash<QString, QSpinBox *> m_queuelevels;      //!< For queue display
        QHash<QString, QSpinBox *> m_queuelevels_wait; //!< For queue display
        QCheckBox * m_queue_longestwait;               //!< should we display the longest wait in the queue xlet ?
        QCheckBox * m_queue_displaynu;                 //!< should we display the queue number in queue name ?

        QComboBox * m_comboswitchboard; //!< Appearance of switchboard
        QSpinBox * m_maxWidthWanted;    //!< Maximum width for small items in switchboard
        QComboBox * m_switchboard_dblclick;  //!< Action to do when a switchboard item is double clicked

        QDialogButtonBox * m_btnbox;    //!< Buttons box
        QTabWidget * m_tabwidget;       //!< Tabs to access configuration widgets

        enum operator_actions {
            ANSWER,
            HANGUP,
            DTXFER,
            ITXFER,
            ILINK,
            ICANCEL,
            ATXFER_FINAL,
            CANCEL_TXFER,
            NB_OP_ACTIONS
        };

        struct {
            QString action;
            QString translation;
            QPushButton *button;
        } m_operator_action[NB_OP_ACTIONS];

        QCheckBox * m_operator_answer_work; //!< should we display the answer key in operator xlet ?

        int m_currentKeyChange;

        QCheckBox * m_logtofile;
        QLineEdit * m_logfilename;

        QCheckBox * m_displayprofile;
        QCheckBox * m_activate_on_tel;

        QPushButton * m_reset_gui;
};

#endif

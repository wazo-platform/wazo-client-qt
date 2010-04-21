/* XiVO Client
 * Copyright (C) 2007-2010, Proformatique
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
 * parameters to the identification server */
/* could be a QDialog instead of QWidget */
//class ConfigWidget: public QWidget
class ConfigWidget: public QDialog
{
    Q_OBJECT
 public:
    /*! \brief Constructor
     *
     * Construct the widget and its layout.
     * Fill widgets with values got from the BaseEngine object.
     * Once constructed, the Widget is ready to be shown.
     * \param engine        related BaseEngine object where parameters will be modified
     * \param parent        parent QWidget
     */
    ConfigWidget(BaseEngine * engine, QWidget * parent = 0);
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
    void _insert_function_tab();
    void _insert_account_tab();
    void _insert_guisetting_tab();
    void _insert_operatorxlet_tab();

    QVariantMap opts;
    QVariantMap forcedopts;

    BaseEngine * m_engine;          //!< BaseEngine object parameters are commited to
        
    QLineEdit * m_serverhost;       //!< IP/hostname of the server
    QSpinBox  * m_ctiport;          //!< server port (switchboard)
    
    QCheckBox * m_autoconnect;      //!< "Auto connect" checkbox
    QCheckBox * m_trytoreconnect;   //!< "Try to reconnect" Checkbox
    QCheckBox * m_systrayed;        //!< "Systray at startup" Checkbox
    QSpinBox  * m_tryinterval_sbox; //!< "Try to reconnect" interval
    QSpinBox  * m_kainterval_sbox;  //!< Keep alive interval
    
    QLineEdit * m_context;          //!< context name (related to the company)
    QLineEdit * m_password;         //!< user password
    QCheckBox * m_keeppass;         //!< keep password ?
    QCheckBox * m_showagselect;     //!< show agent select on main window ?
    QLineEdit * m_userid;           //!< user login
    QComboBox * m_loginkind;        //!< login kind (user or agent)
    QLineEdit * m_agentphonenumber; //!< agent's phone number
    
    QLabel * m_lblphone;    //!< label "Phone Number"
    
    QHash<QString, QCheckBox *> m_function;        //!< connect to functions checkboxes
    QCheckBox * m_autourl_allowed;  //!< Allow automatic opening of urls
    QSpinBox  * m_history_sbox;        //!< History size
    
    QHash<QString, QSpinBox *> m_queuelevels;   //!< For queue display
    QHash<QString, QSpinBox *> m_queuelevels_wait;   //!< For queue display
    
    QSpinBox  * m_contactssize_sbox;        //!< Displayed contacts' size
    QSpinBox  * m_contactswidth_sbox;        //!< Displayed contacts' width
    QSpinBox  * m_tablimit_sbox;        //!< Maximum number of tabs
    QCheckBox * m_lastconnwins;        //!< The last connected user wins => disconnects the other
    
    QComboBox * m_comboswitchboard; //!< Apparence of SwitchBoard
    QSpinBox * m_maxWidthWanted;    //!< maximum width for small items in swich board
    QSpinBox * m_presenceIndicatorSize; //<! size of the presence indicator for basic peer widgets
    
    QDialogButtonBox * m_btnbox;        //!< Buttons box
    QTabWidget * m_tabwidget;       //!< Tabs to access configuration widgets
    QCheckBox * m_queue_longestwait;  //!< should we display the longest wait in the queue xlet ?
    QCheckBox * m_queue_displaynu;  //!< should we display the queue number in queue name ?

    struct {
      QString action;
      QString translation;
      QPushButton *button;
    } m_operator_action[9];

    int m_currentKeyChange;
    
};
#endif

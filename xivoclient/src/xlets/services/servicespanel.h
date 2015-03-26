/* XiVO Client
 * Copyright (C) 2007-2014 Avencall
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

#ifndef __SERVICESPANEL_H__
#define __SERVICESPANEL_H__

#include <QObject>

#include <xletlib/xletinterface.h>
#include <xletlib/xlet.h>
#include <ui_services_widget.h>
#include <message_factory.h>

#include "waitingwidget.h"

class QLineEdit;
class QCheckBox;
class QRadioButton;
class QButtonGroup;
class QAbstractButton;

class UserInfo;
class BaseEngine;

struct CallForwardStruct {
    bool enabled;
    QString destination;
};

class ServicesPanel : public XLet
{
    Q_OBJECT

    public:
        ServicesPanel(QWidget *parent=0);

    public slots:
       // void syncOpt(const QString &);
       void updateUserConfig(const QString &, const QVariantMap &);
       // void updatePhoneConfig(const QString &);

    private slots:
       // void chkoptToggled(bool);
       // void forwardToggled(bool);
       // void forwardLostFocus();
       // void servicePutIsOK(const QString &, const QString &);
       // void forwardModeChanged();

        void on_call_filtering_checkbox_stateChanged(int state);
        void on_dnd_checkbox_stateChanged(int state);
        void on_fwdna_checkbox_stateChanged(int state);
        void on_fwdbusy_checkbox_stateChanged(int state);

        void on_nofwd_radiobutton_toggled(bool checked);
        void on_fwdunc_radiobutton_toggled(bool checked);
        void on_fwdsimple_radiobutton_toggled(bool checked);

        void on_fwdunc_input_returnPressed();
        void on_fwdna_input_returnPressed();
        void on_fwdbusy_input_returnPressed();

    private:
        //void updateCheckboxEnabled(const QString &);
        //void updateTextboxEnabled(const QString &);
        CallForwardStruct localCallForward(const QString &);
        void toggledSimpleFwd(bool checked);

        Ui::ServicesWidget ui;
        //QStringList m_capas;
        bool m_nofwd_sended;
        //QHash<QString, QString> m_capalegend;
        //QHash<QString, WaitingWidget<QCheckBox> *> m_chkopt;
        //QHash<QString, WaitingWidget<QAbstractButton> *> m_forward;
        //QHash<QString, WaitingWidget<QLineEdit> *> m_forwarddest;
        QHash<QString, QString>                    m_replyids;
        //QRadioButton * m_nofwd, * m_uncfwd, * m_otherfwd;
        //QButtonGroup * m_fwdmode;
};

class XLetServicesPlugin : public QObject, XLetInterface
{
    Q_OBJECT
    Q_INTERFACES(XLetInterface)
    Q_PLUGIN_METADATA(IID "com.avencall.Plugin.XLetInterface/1.2" FILE "xletservices.json")

    public:
        XLet* newXLetInstance(QWidget *parent=0);
};

#endif

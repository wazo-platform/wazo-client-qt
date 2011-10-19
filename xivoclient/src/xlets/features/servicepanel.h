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

#ifndef __SERVICEPANEL_H__
#define __SERVICEPANEL_H__

#include <QtGui>
#include <xletinterface.h>
#include "xlet.h"

class QCheckBox;
class QLineEdit;

class UserInfo;
class BaseEngine;

class ServicePanel : public XLet
{
    Q_OBJECT

    public:
        ServicePanel(QWidget *parent=0);

    public slots:
        void setOpt(const QString &);
        void setForward(const QString &);
        void Connect();
        void DisConnect();
        void Reset();
        void updateUserConfig(const QString &);
        void updatePhoneConfig(const QString &);

    private slots:
        void chkoptToggled(bool);
        void Toggled(bool);
        void toggleIfAllowed(const QString &);
        void forwardLostFocus();

    private:
        QStringList m_capas;
        QHash<QString, QString> m_capalegend;
        QHash<QString, QCheckBox *> m_chkopt;
        QHash<QString, QCheckBox *> m_forward;
        QHash<QString, QLineEdit *> m_forwarddest;
};

class XLetFeaturePlugin : public QObject, XLetInterface
{
    Q_OBJECT
    Q_INTERFACES(XLetInterface)

    public:
        XLet* newXLetInstance(QWidget *parent=0);
};

#endif

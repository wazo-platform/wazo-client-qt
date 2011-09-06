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

#ifndef __IDENTITYDISPLAY_H__
#define __IDENTITYDISPLAY_H__

#include <QtGui>
#include <xletinterface.h>
#include "xlet.h"

class UserInfo;
class IdentityAgent;
class IdentityPhone;
class IdentityVoiceMail;

/*! \brief Identity Display
 */
class IdentityDisplay : public XLet
{
    Q_OBJECT

    public:
        IdentityDisplay(QWidget *parent=0);
        void doGUIConnects(QWidget *);

    public slots:
        void setGuiOptions(const QVariantMap &);
        void idxChanged(const QString &);
        void updatePresence();
        void setOpt();
        void setForward(const QString &, const QVariant &);
        void updateUserConfig(const QString &);
        void updateUserStatus(const QString &);
        void updatePhoneConfig(const QString &);

    signals:
        void setAvailState(const QString &, bool);
        void changeWatchedAgent(const QString &, bool);
        void setSystrayIcon(const QString &);

    private:
        void svcSummary();
        void setupIcons();

        QGridLayout * m_glayout;
        QLabel *m_icon_user;
        QFrame *m_qvline1;
        QFrame *m_qvline2;
        QLabel *m_user;
        QLabel *m_phonenum;
        QComboBox * m_presencevalue;
        IdentityAgent * m_agent;
        QHash<QString, IdentityPhone *> m_identityphones;
        IdentityVoiceMail * m_voicemail;
        QHash<QString, QString> m_presence_names;
        int m_col_user;
        int m_col_agent;
        int m_col_phone;
        int m_col_vm;
        int m_col_last;
        Qt::Alignment m_iconAlign;
        Qt::Alignment m_textAlignVCenter;
        QFont m_gui_font;
        quint32 m_gui_buttonsize;
        quint32 m_loginkind;
        QStringList m_functions;
        QVariantMap m_svcstatus;
};

class XLetIdentityPlugin : public QObject, XLetInterface
{
    Q_OBJECT
    Q_INTERFACES(XLetInterface)

    public:
        XLet* newXLetInstance(QWidget *parent=0);
};

#endif

/* XiVO Client
 * Copyright (C) 2007-2015 Avencall
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

#ifndef __IDENTITYDISPLAY_H__
#define __IDENTITYDISPLAY_H__

#include <QtGui>
#include <QPushButton>

#include <xletlib/xletinterface.h>
#include <xletlib/xlet.h>

#include <ui_identity.h>

class UserInfo;
class IdentityAgent;
class IdentityPhone;
class IdentityVoiceMail;

class IdentityDisplay : public XLet
{
    Q_OBJECT

    public:
        IdentityDisplay(QWidget *parent=0);
        void doGUIConnects(QWidget *);

    public slots:
        void setGuiOptions();
        void updatePresenceList();
        void updatePresenceVisibility();
        void updateAgentVisibility();
        void setOpt();
        void setForward(const QString &, const QVariant &);
        void updateUserConfig(const QString &);
        void updateAgentStatus(const QString &);
        void updatePhoneConfig(const QString &);
        void foldToggle(bool fold);

    private slots:
        void setPresence(const QString &new_presence);
        void login();
        void logout();
        void pause();
        void unpause();

    signals:
        void setAvailState(const QString &, bool);
        void changeWatchedAgent(const QString &, bool);
        void setAppIcon(const QString &);

    private:
        void svcSummary();
        void updateNameTooltip();

        QGridLayout * m_glayout;
        QLabel *m_icon_user;
        QFrame *m_qvline1;
        QFrame *m_qvline2;
        QLabel *m_user;
        QLabel *m_phonenum;
        QComboBox * m_presencevalue;
        QHash<QString, IdentityPhone *> m_identityphones;
        IdentityVoiceMail * m_voicemail;
        int m_col_phone;
        Qt::Alignment m_iconAlign;
        Qt::Alignment m_textAlignVCenter;
        QFont m_gui_font;
        quint32 m_gui_buttonsize;
        QVariantMap m_svcstatus;
        QPushButton m_fold_button;

        static QIcon m_hide_icon;
        static QIcon m_show_icon;

        Ui::IdentityWidget ui;
        QMenu *m_agent_menu;
        QSignalMapper *m_presence_mapper;
        QMenu *m_presence_menu;

        bool m_logged_with_agent;
};

class XLetIdentityPlugin : public QObject, XLetInterface
{
    Q_OBJECT
    Q_INTERFACES(XLetInterface)
    Q_PLUGIN_METADATA(IID "com.avencall.Plugin.XLetInterface/1.2" FILE "xletidentity.json")

    public:
        XLet* newXLetInstance(QWidget *parent=0);
};

#endif

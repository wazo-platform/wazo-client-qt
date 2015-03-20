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

#include <xletlib/xletinterface.h>
#include <xletlib/xlet.h>

#include <ui_identity.h>

class VoiceMailInfo;

class IdentityDisplay : public XLet
{
    Q_OBJECT

    public:
        IdentityDisplay(QWidget *parent=0);
        void doGUIConnects(QWidget *main_window);

    public slots:
        void updatePhoneConfig(const QString &phone_id);
        void updateUserConfig(const QString &user_id);
        void updateVoiceMailConfig(const QString &voicemail_id);

        void updateAgentStatus(const QString &agent_id);
        void updateUserStatus(const QString &user_id);
        void updateVoiceMailStatus(const QString &voicemail_id);

        void updatePresenceList();
        void updatePresenceVisibility();
        void setPresence(const QString &new_presence);

        void on_fold_button_toggled(bool fold);
        void on_voicemail_button_clicked();
        void on_dial_input_returnPressed();
        void on_dial_search_button_clicked();

        void login();
        void logout();
        void pause();
        void unpause();

    signals:
        void setAppIcon(const QString &);

    private:
        void addPresence(const QString &);
        void dial();
        void fillAgentMenu(QMenu *menu);
        QPixmap newMessagesIcon(int message_count);
        QPixmap presenceIcon(const QColor &color);
        void requestVoicemailMessageCount(const VoiceMailInfo *voicemail);
        void updateAgentVisibility();
        void updateCurrentPresence();
        void updateNameTooltip();
        void updateOptions();
        void updateVoiceMailVisibility();

        QIcon m_hide_icon;
        QIcon m_show_icon;

        Ui::IdentityWidget ui;
        QSignalMapper *m_presence_mapper;
        QMenu *m_agent_menu;
        QMenu *m_presence_menu;
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

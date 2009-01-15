/* XIVO CTI clients
 * Copyright (C) 2007, 2008  Proformatique
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License version 2 for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Linking the Licensed Program statically or dynamically with other
 * modules is making a combined work based on the Licensed Program. Thus,
 * the terms and conditions of the GNU General Public License version 2
 * cover the whole combination.
 *
 * In addition, as a special exception, the copyright holders of the
 * Licensed Program give you permission to combine the Licensed Program
 * with free software programs or libraries that are released under the
 * GNU Library General Public License version 2.0 or GNU Lesser General
 * Public License version 2.1 or any later version of the GNU Lesser
 * General Public License, and with code included in the standard release
 * of OpenSSL under a version of the OpenSSL license (with original SSLeay
 * license) which is identical to the one that was published in year 2003,
 * or modified versions of such code, with unchanged license. You may copy
 * and distribute such a system following the terms of the GNU GPL
 * version 2 for the Licensed Program and the licenses of the other code
 * concerned, provided that you include the source code of that other code
 * when and as the GNU GPL version 2 requires distribution of source code.
*/

/* $Revision$
 * $Date$
 */

#ifndef __IDENTITYDISPLAY_H__
#define __IDENTITYDISPLAY_H__

#include <QHash>
#include <QLabel>
#include <QList>
#include <QObject>
#include <QVariant>
#include <QWidget>

class QComboBox;
class QContextMenuEvent;
class QProgressBar;

class ExtendedLabel;
class UserInfo;

/*! \brief Identity Display
 */
class IdentityDisplay : public QWidget
{
	Q_OBJECT
public:
	IdentityDisplay(const QVariant &,
                        QWidget * parent = 0);
public slots:
        void setGuiOptions(const QVariant &);
        void setUserInfo(const UserInfo *);
        void setAgentList(int, const QVariant &);
	void setQueueList(const QVariant &);
        void updatePeer(UserInfo *,
                        const QString &,
                        const QVariant &);
	void updatePeerAgent(int,
                             const QString &,
                             const QString &,
                             const QVariant &);
        void doAgentLogActions();
        void doAgentPauseActions();
        void doAgentUnPauseActions();
        void idxChanged(const QString &);
        void updatePresence(const QVariant &);
private slots:
        void contextMenuEvent(QContextMenuEvent *);
        void contextMenuAction();
private:
        void showAgentProps();
        void hideAgentProps();
        void setStatusColors(int, int);
        
        ExtendedLabel * m_icon_user;
        ExtendedLabel * m_icon_agent;
        ExtendedLabel * m_icon_voicemail;
        
        QFrame * m_qvline1;
        QFrame * m_qvline2;
        
        QLabel * m_user;
        QLabel * m_voicemail_old;
        QLabel * m_voicemail_new;
        QLabel * m_voicemail_name;
        QLabel * m_phonenum;
        QComboBox * m_presencevalue;
        QLabel * m_agent;
        QLabel * m_agentstatus;
        QLabel * m_agentpause;
        
        QHash<QString, QString> m_presence_names;
        const UserInfo * m_ui;
        
        QFont m_gui_font;
        quint32 m_gui_buttonsize;
        quint32 m_loginkind;
        QStringList m_functions;
        bool m_allow_logagent;
        bool m_allow_pauseagent;
signals:
        void agentAction(const QString &);
        void setAvailState(const QString &, bool);
        void changeWatchedAgent(const QString &, bool);
        void setSystrayIcon(const QString &);
};

#endif

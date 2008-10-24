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

#ifndef __AGENTSPANEL_H__
#define __AGENTSPANEL_H__

#include <QHash>
#include <QList>
#include <QObject>
#include <QWidget>

class QFrame;
class QGridLayout;
class QLabel;
class QPushButton;
class QScrollArea;
class QVBoxLayout;

class UserInfo;

/*! \brief Identity Display
 */
class AgentsPanel : public QWidget
{
	Q_OBJECT
public:
	AgentsPanel(const QMap<QString, QVariant> &,
                    QWidget * parent = 0);
	~AgentsPanel();
signals:
        void changeWatchedAgent(const QString &, bool);
        void agentAction(const QString &);
public slots:
        void setUserInfo(const UserInfo *);
        void setAgentList(const QMap<QString, QVariant> &);
        void setAgentStatus(const QString &);
        void updatePeerAgent(const QString &,
                             const QString &,
                             const QStringList &);
        void updateAgentPresence(const QString &, const QString &, const QColor &);
        void setGuiOptions(const QMap<QString, QVariant> &);
private slots:
        void agentClicked();
private:
        QFont m_gui_font;
        quint32 m_gui_buttonsize;
        
        QGridLayout * m_gridlayout;
	QVBoxLayout * m_layout;
        QScrollArea * m_scrollarea;
	QWidget * m_widget;
        const UserInfo * m_userinfo;

        QHash<QString, QLabel *>      m_agent_labels;
        QHash<QString, QPushButton *> m_agent_more;
        QHash<QString, QPushButton *> m_agent_record;
        QHash<QString, QPushButton *> m_agent_listen;
        QHash<QString, QLabel *>      m_agent_busy;
        QHash<QString, QLabel *>      m_agent_presence;
        QHash<QString, QLabel *>      m_agent_logged_status;
        QHash<QString, QPushButton *> m_agent_logged_action;
        QHash<QString, QLabel *>      m_agent_joined_number;
        QHash<QString, QLabel *>      m_agent_joined_status;
        QHash<QString, QLabel *>      m_agent_paused_number;
        QHash<QString, QLabel *>      m_agent_paused_status;
        
        QHash<QString, QStringList> m_agent_joined_list;
        QHash<QString, QStringList> m_agent_paused_list;
        
        QLabel * m_title1;
        QLabel * m_title2;
        QLabel * m_title3;
        QLabel * m_title4;
        QLabel * m_title5;
        QLabel * m_title6;
        QLabel * m_title7;
        QLabel * m_title8;
};

#endif /* __AGENTSPANEL_H__ */

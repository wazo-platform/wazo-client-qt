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

/* $Revision: 4659 $
 * $Date: 2008-11-13 12:10:04 +0100 (jeu, 13 nov 2008) $
 */

#ifndef __AGENTSPANELNEXT_H__
#define __AGENTSPANELNEXT_H__

#include <QHash>
#include <QList>
#include <QObject>
#include <QVariant>
#include <QWidget>

class QFrame;
class QGridLayout;
class QHBoxLayout;
class QLabel;
class QPushButton;
class QVBoxLayout;

class UserInfo;

/*! \brief Identity Display
 */
class AgentsPanelNext : public QWidget
{
	Q_OBJECT
public:
	AgentsPanelNext(const QVariant &,
                    QWidget * parent = 0);
	~AgentsPanelNext();
signals:
        void changeWatchedAgent(const QString &, bool);
        void agentAction(const QString &);
public slots:
        void setGuiOptions(const QVariant &);
        void setUserInfo(const UserInfo *);
        void setAgentList(const QVariant &);
        void updatePeerAgent(const QString &,
                             const QString &,
                             const QVariant &);
        void updateAgentPresence(const QString &, const QVariant &);
private slots:
        void agentClicked();
        void setAgentProps(const QString &);
private:
        QFont m_gui_font;
        quint32 m_gui_buttonsize;
        
	QHBoxLayout * m_hlayout;
        QHash<QString, QVBoxLayout *> m_vlayout;
        QHash<QString, QLabel *> m_title;
        QHash<QString, QPushButton *> m_titleedit;
        const UserInfo * m_userinfo;
        
        QHash<QString, QPushButton *> m_agent_labels;
        QHash<QString, QVariant> m_agent_props;
};

#endif /* __AGENTSPANEL_H__ */

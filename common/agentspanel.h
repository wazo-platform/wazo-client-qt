/* XiVO Client
 * Copyright (C) 2007-2009, Proformatique
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

#ifndef __AGENTSPANEL_H__
#define __AGENTSPANEL_H__

#include <QHash>
#include <QList>
#include <QObject>
#include <QVariant>
#include <QWidget>

class QFrame;
class QGridLayout;
class QLabel;
class QPushButton;

class BaseEngine;
class UserInfo;

/*! \brief Display a list of agents
 */
class AgentsPanel : public QWidget
{
    Q_OBJECT
 public:
    AgentsPanel(BaseEngine *,
                QWidget * parent = 0);
    ~AgentsPanel();
 signals:
    void changeWatchedAgent(const QString &, bool);
    void agentAction(const QString &);
    void shouldNotOccur(const QString &, const QString &); //!< signal to log tricky situations
 public slots:
    void setGuiOptions(const QVariantMap &);
    void setUserInfo(const UserInfo *) {};
    void newAgentList(const QStringList &);
    void newQueueList(const QStringList &);
    void updateAgentPresence(const QString &, const QString &, const QVariant &);
    void statusRecord(const QString &, const QString &, const QString &);
    void statusListen(const QString &, const QString &, const QString &);
 private slots:
    void agentClicked();
 private:
    void newAgentLine(const QString &);
    void updateAgentLineAdmin(const QString &, const QString &, const QString &);
    void updateAgentLineEvent(const QString &);
    void displayLine(const QString &, int);
    void updateAgentStatus(const QString &, const QVariantMap &);
    
    QFont m_gui_font;   //!< font
    quint32 m_gui_buttonsize;   //!< button size
    
    BaseEngine * m_engine;
    QGridLayout * m_gridlayout;     //!< Layout
    
    QHash<QString, QLabel *>      m_agent_labels;
    QHash<QString, QPushButton *> m_agent_more;
    QHash<QString, QPushButton *> m_agent_record;
    QHash<QString, QPushButton *> m_agent_listen;
    QHash<QString, QLabel *>      m_agent_busy;
    QHash<QString, QLabel *>      m_agent_presence;
    QHash<QString, QLabel *>      m_agent_logged_status;
    QHash<QString, QPushButton *> m_agent_logged_action;
    QHash<QString, QLabel *>      m_agent_joined_number;
    QHash<QString, QLabel *>      m_agent_paused_status;
    QHash<QString, QPushButton *> m_agent_paused_action;
    QHash<QString, QLabel *>      m_agent_paused_number;
    
    QHash<QString, QStringList> m_agent_joined_list;
    QHash<QString, QStringList> m_agent_paused_list;
    
    QLabel * m_title1;  //!< "Agent"
    QLabel * m_title2;  //!< "Record"
    QLabel * m_title3;  //!< "Listen"
    QLabel * m_title4;  //!< "On Line"
    QLabel * m_title5;  //!< "Presence"
    QLabel * m_title_logged;    //!< "Logged"
    QLabel * m_title_paused;    //!< "Paused"
    QLabel * m_title_njoined;   //!< "Joined queues"
    QLabel * m_title_npaused;   //!< "paused queues"
};

#endif /* __AGENTSPANEL_H__ */

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

#ifndef __QUEUEDETAILSPANEL_H__
#define __QUEUEDETAILSPANEL_H__

#include <QDateTime>
#include <QHash>
#include <QObject>
#include <QVariant>
#include "xlet.h"

class QGridLayout;
class QLabel;
class QPushButton;
class QScrollArea;
class QVBoxLayout;

class AgentInfo;
class UserInfo;

/*! \brief Display details about a queue
 *
 * The agents in this queue and details are displayed
 */
class QueuedetailsPanel : public XLet
{
    Q_OBJECT

    public:
        QueuedetailsPanel(QWidget *parent=0);

    public slots:
        void newAgentList(const QStringList &);
        void newQueueList(const QStringList &);
        void monitorThisQueue(const QString &);

    private slots:
        void agentClicked();

    private:
        void clearPanel();
        void updatePanel();
        void setAgentLookProps(const QString &);
        void setAgentQueueProps(const QString &, const QVariant &);
        void setAgentProps(const QString &, const AgentInfo *);
        void setAgentQueueSignals(const QString &);
        void fillAgent(int, const QString &);
        
        QGridLayout *m_gridlayout; //!< Layout
        
        QString m_monitored_queueid;  //!< queue id
        QLabel *m_queuelegend_agentid; //!< "Agent" label
        QLabel *m_queuelegend_status;  //!< "Status" label
        QLabel *m_queuelegend_paused;  //!< "Paused" label
        QLabel *m_queuelegend_callstaken;  //!< "Call Taken" label
        QLabel *m_queuelegend_lastcall;  //!< "Call Taken" label
        QLabel *m_queuelegend_penalty;  //!< "Call Taken" label
        QLabel *m_queuedescription;       //!< queue name label
        
        QHash<QString, QLabel *> m_agent_labels; //!< agents name
        QHash<QString, QPushButton *> m_agent_more;  //!< "+" buttons
        QHash<QString, QLabel *> m_agent_join_status; //!< agents status
        QHash<QString, QLabel *> m_agent_pause_status; //!< agents paused
        QHash<QString, QLabel *> m_agent_callstaken; //!< agents number of calls
        QHash<QString, QLabel *> m_agent_lastcall; //!< agents last call
        QHash<QString, QLabel *> m_agent_penalty; //!< agents penalty
};

#endif /* __QUEUEDETAILSPANEL_H__ */

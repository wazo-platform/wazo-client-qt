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

#ifndef __AGENTDETAILSPANEL_H__
#define __AGENTDETAILSPANEL_H__

#include <QtGui>
#include <xletinterface.h>
#include <xlet.h>

#include "baseengine.h"
#include "queue_agent_status.h"


/*! \brief Display details about an agent
 */
class XletAgentDetails : public XLet
{
    Q_OBJECT

    public:
        XletAgentDetails(QWidget *parent);

    protected:
        void contextMenuEvent(QContextMenuEvent *);

    signals:
        void changeWatchedQueue(const QString &);
        void setFileName(const QString &);

    public slots:
        void setGuiOptions(const QVariantMap &);
        void newAgentList(const QStringList &);
        void monitorThisAgent(const QString &);
        void saveToFile(const QString &, void*);
        void statusRecord(const QString &, const QString &, const QString &);
        void statusListen(const QString &, const QString &, const QString &);

    private slots:
        void queueClicked();
        void actionClicked();
        void serverFileList(const QStringList &);
        void getFile();

    private:
        void clearPanel();
        void updatePanel();
        void setQueueLookProps(const QString &);
        void setQueueAgentProps(const QString &, const QVariant &);
        void setQueueProps(const QString &, const QueueInfo *);
        void setQueueAgentSignals(const QString &);
        void fillQueue(int, const QString &);

        QGridLayout *m_gridlayout; //!< layout
        int m_linenum;  //!< line number ?

        QString m_monitored_agentid;    //!< monitored agent id
        QLabel *m_agentstatus; //!< to display agent name and status
        QLabel *m_agentlegend_qname;   //!< "Queues"
        QLabel *m_agentlegend_joined;  //!< "Joined"
        QLabel *m_agentlegend_paused;  //!< "Paused"
        QLabel *m_agentlegend_njoined; //!< number joined
        QLabel *m_agentlegend_npaused; //!< number paused
        QHash<QString, QLabel *> m_actionlegends;   //!< Label Login/Logout
        QHash<QString, QPushButton *> m_action; //!< buttons cancel/ok

        QHash<QString, QLabel *> m_queue_labels;
        QHash<QString, QPushButton *> m_queue_more;
        QHash<QString, QPushButton *> m_queue_join_action;
        QHash<QString, QPushButton *> m_queue_pause_action;
        QHash<QString, QLabel *> m_queue_join_status;
        QHash<QString, QLabel *> m_queue_pause_status;

        QPoint m_eventpoint;
        QVariantMap m_optionsMap;
};

class XLetAgentDetailsPlugin : public QObject, XLetInterface
{
    Q_OBJECT
    Q_INTERFACES(XLetInterface)

    public:
        XLet *newXLetInstance(QWidget *parent=0);
};


#endif /* __AGENTDETAILSPANEL_H__ */

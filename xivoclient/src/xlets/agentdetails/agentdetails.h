/* XiVO Client
 * Copyright (C) 2007-2014 Avencall
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
 */

#ifndef __AGENTDETAILSPANEL_H__
#define __AGENTDETAILSPANEL_H__

#include <QtGui>

#include <baseengine.h>
#include <storage/queue_agent_status.h>
#include <xletlib/xletinterface.h>
#include <xletlib/xlet.h>

class QGridLayout;
class QLabel;
class QPushButton;

/*! \brief Display details about an agent
 */
class XletAgentDetails : public XLet
{
    Q_OBJECT

    public:
        XletAgentDetails(QWidget *parent);

    signals:
        void changeWatchedQueue(const QString &);

    public slots:
        void updateAgentConfig(const QString &);
        void updateAgentStatus(const QString &);
        void monitorThisAgent(const QString &);

    private slots:
        void queueClicked();
        void actionClicked();
        void updatePanel();
        void onRemoveQueueConfig();
        void updateHeader();
        void updateAvailability();

    private:
        void clearPanel();
        void setQueueLookProps(const QString &);
        void setQueueAgentProps(const QString &, const QString &);
        void setQueueProps(const QString &);
        void setQueueAgentSignals(const QString &);
        void fillQueue(int, const QString &);
        QString getQueueLabelText(const QString &);

        QGridLayout *m_gridlayout; //!< layout
        int m_linenum;  //!< line number ?

        QString m_monitored_agentid;    //!< monitored agent id
        QLabel *m_agent_header;
        QLabel *m_agent_availability;
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
};

class XLetAgentDetailsPlugin : public QObject, XLetInterface
{
    Q_OBJECT
    Q_INTERFACES(XLetInterface)
    Q_PLUGIN_METADATA(IID "com.avencall.Plugin.XLetInterface/1.2" FILE "xletagentdetails.json")

    public:
        XLet *newXLetInstance(QWidget *parent=0);
};

#endif /* __AGENTDETAILSPANEL_H__ */

/* Copyright (C) 2007-2013, Avencall
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

#ifndef __AGENT_STATUS_DASHBOARD_H__
#define __AGENT_STATUS_DASHBOARD_H__

#include <xletlib/xletinterface.h>
#include <xletlib/xlet.h>

#include "agent_status_delegate.h"

class AgentsModel;
class AgentStatusDelegate;
class AgentStatusWidgetBuilder;
class AgentStatusWidgetStorage;
class QVBoxLayout;

class XletAgentStatusDashboard : public XLet
{
    Q_OBJECT
    public:
        XletAgentStatusDashboard(QWidget *parent);
        ~XletAgentStatusDashboard();

    private slots:
        void updateQueueConfig(const QString & queue_id);

    private:
        QString getQueueName(QString queue_id);

        AgentsModel * m_model;
        AgentStatusDelegate * m_delegate;
        AgentStatusWidgetBuilder * m_widget_builder;
        AgentStatusWidgetStorage * m_widget_storage;
        QVBoxLayout * m_layout;
};

class XLetAgentStatusDashboardPlugin : public QObject, XLetInterface
{
    Q_OBJECT
    Q_INTERFACES(XLetInterface)

    public:
        XLet *newXLetInstance(QWidget *parent=0);
};


#endif /* __AGENT_STATUS_DASHBOARD_H__ */

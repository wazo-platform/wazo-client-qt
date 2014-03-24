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
 * along with XiVO Client.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __QUEUESPANEL_H__
#define __QUEUESPANEL_H__

#include <xletlib/xlet.h>
#include <xletlib/xletinterface.h>
#include <ipbxlistener.h>

#include "queuesmodel.h"
#include "queuessortfilterproxymodel.h"
#include "queuesview.h"

class UserInfo;
class XletQueues;
class QueueInfo;

class XletQueuesConfigure : public QWidget
{
    Q_OBJECT

    public:
        XletQueuesConfigure(XletQueues *xlet);
        QWidget* buildConfigureQueueList(QWidget *);

    private slots:
        void changeQueueStatParam(int);
};

class XletQueues : public XLet, IPBXListener
{
    Q_OBJECT

    public:
        XletQueues(QWidget *parent=0);
        void parseCommand(const QVariantMap &);

    protected:
        virtual void contextMenuEvent(QContextMenuEvent *);

    private:
        void openConfigureWindow();
        void subscribeToQueuesStats();

    public slots:
        void askForQueueStats();

    private:
        XletQueuesConfigure *m_configureWindow;

        QueuesModel *m_model;
        QueuesSortFilterProxyModel *m_proxyModel;
};

class XLetQueuesPlugin : public QObject, XLetInterface
{
    Q_OBJECT
    Q_INTERFACES(XLetInterface)
    Q_PLUGIN_METADATA(IID "com.avencall.Plugin.XLetInterface/1.2" FILE "xletqueues.json")

    public:
        XLet *newXLetInstance(QWidget *parent=0);
};

#endif

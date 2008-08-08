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
#include <QWidget>

class QComboBox;
class QProgressBar;
class QPushButton;

class BaseEngine;
class UserInfo;

/*! \brief Identity Display
 */
class IdentityDisplay : public QWidget
{
	Q_OBJECT
public:
	IdentityDisplay(QWidget * parent = 0);
        void setEngine(BaseEngine *);
public slots:
        void setUserInfo(const UserInfo *);
        void setAgentList(const QString &);
	void setQueueList(bool, const QString &);
        void updatePeer(UserInfo *,
                        const QString &,
                        const QHash<QString, QStringList> &);
	void updatePeerAgent(const QString &,
                             const QString &,
                             const QStringList &);
	void setQueueStatus(const QString &);
        void doAgentAction();
        void doQueueAction();
        void doQueueJoinAll();
        void doQueueLeaveAll();
        void idxChanged(const QString &);
private:
        BaseEngine * m_engine;
        QLabel * m_user;
        QLabel * m_info1;
        QLabel * m_info2;
        QLabel * m_info3;
        QLabel * m_info4;
        QLabel * m_info5;
        QLabel * m_info6;
        QLabel * m_info7;
        QLabel * m_agent;
        QFrame * m_qf;
        QPushButton * m_agentaction;
        QPushButton * m_queueaction;
        QPushButton * m_queuejoinall;
        QPushButton * m_queueleaveall;
        QComboBox * m_queuelist;
        QProgressBar * m_queuebusy;
        QHash<QString, int> m_queuesindexes;
        QHash<QString, bool> m_queuesstatuses;
        QHash<QString, QString> m_queuesbusyness;

        const UserInfo * m_ui;
        bool m_agentstatus;
        bool m_queuechangeallow;
        int m_maxqueues;
signals:
        void agentAction(const QString &);
};


class SizeableLabel : public QLabel
{
        Q_OBJECT
public:
        SizeableLabel(const QString &text, const QSize &size, QWidget *parent = 0);
        QSize sizeHint() const;
private:
        QSize m_size;
};

#endif

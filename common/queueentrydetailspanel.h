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

#ifndef __QUEUEENTRYDETAILSPANEL_H__
#define __QUEUEENTRYDETAILSPANEL_H__

#include <QDateTime>
#include <QHash>
#include <QList>
#include <QObject>
#include <QVariant>
#include <QWidget>

class QGridLayout;
class QLabel;
class QScrollArea;
class QVBoxLayout;

class BaseEngine;
class UserInfo;

/*! \brief Display entries of a queue
 */
class QueueentrydetailsPanel : public QWidget
{
    Q_OBJECT
 public:
    QueueentrydetailsPanel(BaseEngine *,
                           const QVariant &,
                           QWidget * parent = 0);
    ~QueueentrydetailsPanel();
 signals:
    void changeWatchedAgent(const QString &);   //!< change watched agent
 protected:
    void timerEvent(QTimerEvent *);
 public slots:
    void setGuiOptions(const QVariant &) {};
    void setUserInfo(const UserInfo *) {};
    void newAgentList(const QStringList &);
    void newQueueList(const QStringList &);
    void monitorThisQueue(const QString &);
 private:
    void updateEntryChannel(const QString &);
    void clearPanel();
    void updatePanel();
    
    BaseEngine * m_engine;        //!< BaseEngine object
    QGridLayout * m_gridlayout; //!< Layout
    
    QString m_monitored_queueid;  //!< queue id
    QString m_monitored_astid;    //!< asterisk id
    QString m_monitored_context;  //!< context
    QString m_monitored_queuename;  //!< queue name
    QLabel * m_queuedescription;   //!< label for displaying queue name
    QHash<QString, QLabel *> m_entrypos;    //!< display entry
    QHash<QString, QLabel *> m_entrytime;   //!< display call duration ?
};

#endif /* __QUEUEENTRYDETAILSPANEL_H__ */

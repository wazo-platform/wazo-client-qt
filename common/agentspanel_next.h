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

#ifndef __AGENTSPANELNEXT_H__
#define __AGENTSPANELNEXT_H__

#include <QDateTime>
#include <QHash>
#include <QList>
#include <QObject>
#include <QVariant>
#include <QWidget>

class QComboBox;
class QContextMenuEvent;
class QGridLayout;
class QLabel;
class QPushButton;

class BaseEngine;
class ExtendedLabel;
class UserInfo;

/*! \brief Identity Display
 */
class AgentsPanelNext : public QWidget
{
    Q_OBJECT
public:
    AgentsPanelNext(BaseEngine *,
                    QWidget * parent = 0);
    ~AgentsPanelNext();
signals:
    void changeWatchedAgent(const QString &, bool);
    void agentAction(const QString &);
    void saveQueueGroups(const QVariant &);
    void loadQueueGroups();
    void logAction(const QString &);
    void shouldNotOccur(const QString &, const QString &); //!< signal to log tricky situations
public slots:
    void setGuiOptions(const QVariantMap &);
    void setUserInfo(const UserInfo *) {};
    void newAgentList(const QStringList &);
    void newQueueList(const QStringList &);
    void setQueueOrder(const QVariant &);
private slots:
    void renameQueueGroup();
    void removeQueueGroup();
    void agentClicked(QMouseEvent *);
    void setAgentProps(const QString &);
    void removeQueueFromGroup();
    void removeQueuesFromGroup();
    void addQueueToGroup();
    void addQueuesToGroup();
    void setQueueGroups(const QVariant &);
    void newGroup();
    void actionclicked();
protected slots:
    void contextMenuEvent(QContextMenuEvent *);
protected:
    void mouseReleasedEvent(QMouseEvent *);
    void timerEvent(QTimerEvent *);
private:
    void newQueue(const QString &, const QString &, const QVariant &);
    void refreshContents();
    void refreshDisplay();
    void saveGroups();
    
    QFont m_gui_font;
    QGridLayout * m_glayout;
    QMap<QString, ExtendedLabel *> m_title;
    BaseEngine * m_engine;
    
    int m_blinktime;
    
    QHash<QString, QString> m_groups;
    QHash<QString, ExtendedLabel *> m_agent_labels;
    QStringList m_queuelist;
    QVariant m_queueorder;
    
    QComboBox * m_queue_chose;
};

#endif /* __AGENTSPANEL_H__ */

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

#ifndef __QUEUESPANEL_H__
#define __QUEUESPANEL_H__

#include <QHash>
#include <QLabel>
#include <QList>
#include <QObject>
#include <QVariant>
#include "xlet.h"

class QCheckBox;
class QGridLayout;
class QLabel;
class QProgressBar;
class QPushButton;

class UserInfo;

/*! \brief Displays queues and their status
 */
class QueuesPanel : public XLet
{
    Q_OBJECT
public:
    QueuesPanel(BaseEngine *,
                QWidget * parent = 0);
    ~QueuesPanel();
protected:
    void update(const QStringList &);
private:
    void addQueue(const QString &, const QString &, const QString &);
signals:
    void changeWatchedQueue(const QString &);   //!< Watch this queue
    void saveQueueOrder(const QVariant &);      //!< Save Queue order (in settings)
    void loadQueueOrder();                      //!< request load of queue order
public slots:
    void setGuiOptions(const QVariantMap &);
    void setUserInfo(const UserInfo *) {};
    void updateCounter(const QVariant &);
    void removeQueues(const QString &, const QStringList &);
    void newQueueList(const QStringList &);
    void newAgentList(const QStringList &);
    void setQueueOrder(const QVariant &);
private slots:
    void queueClicked();
private:
    bool updateQueue(const QString &, const QString &, const QVariant &);
    void affWidgets();
    
    QFont m_gui_font;
    quint32 m_gui_buttonsize;
    bool m_gui_showqueuenames;
    bool m_gui_showmore;
    
    QGridLayout * m_gridlayout; //!< Layout
    QStringList m_statitems;    //!< list of stats items which are reported for each queue
    QHash<QString, QString> m_statlegends_short;  //!< text displayed on top of each column
    QHash<QString, QString> m_statlegends_long;  //!< text displayed on top of each column
    
    QHash<QString, QLabel *> m_queuelabels; //!< QLabel used to display the names of queues
    QHash<QString, QPushButton *> m_queuemore;  //!< Button to display queue details
    QHash<QString, QPushButton *> m_queuemove;  //!< Button to change the order in which the queues are displayed
    QHash<QString, QProgressBar *> m_queuebusies;   //!< Widgets to display the queues busy level
    QHash<QString, QHash<QString, QLabel *> > m_queueinfos; //!< display details about queues
    QStringList m_queue_lines;  //!< store the order of queues
    
    quint32 m_maxbusy;  //!< Maximum value for busy level
    
    QLabel * m_busytitle;   //!< displayed on top of the column of busy levels
    QLabel * m_qtitle;      //!< global title
    QHash<QString, QLabel *> m_title_infos; //!< To display text on top of each column
    QVariantMap m_optionsMap;
};

#endif /* __QUEUESPANEL_H__ */

/* XIVO CTI clients
 * Copyright (C) 2007-2009  Proformatique
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

#ifndef __QUEUESPANEL_H__
#define __QUEUESPANEL_H__

#include <QHash>
#include <QLabel>
#include <QList>
#include <QObject>
#include <QVariant>
#include <QWidget>

class QCheckBox;
class QGridLayout;
class QLabel;
class QProgressBar;
class QPushButton;

class UserInfo;

/*! \brief Displays queues and their status
 */
class QueuesPanel : public QWidget
{
        Q_OBJECT
public:
        QueuesPanel(const QVariant &,
                    QWidget * parent = 0);
        ~QueuesPanel();
protected:
        void update();
private:
        void addQueue(const QString &, const QString &, const QString &, bool);
signals:
        void changeWatchedQueue(const QString &);   //!< Watch this queue
        void saveQueueOrder(const QVariant &);      //!< Save Queue order (in settings)
        void loadQueueOrder();                      //!< request load of queue order
public slots:
        void setGuiOptions(const QVariant &);
        void setUserInfo(const UserInfo *);
        void updateCounter(const QVariant &);
        void removeQueues(const QString &, const QStringList &);
        void setQueueList(const QVariant &);
        void setQueueStatus(const QVariant &);
        void updatePeerAgent(double,
                             const QString &,
                             const QString &,
                             const QVariant &);
        void setQueueOrder(const QVariant &);
private slots:
        void queueClicked();
private:
        void affWidgets(bool);
        
        const UserInfo * m_userinfo;
        QFont m_gui_font;
        quint32 m_gui_buttonsize;
        bool m_gui_showqueuenames;
        bool m_gui_showmore;
        
        QGridLayout * m_gridlayout; //!< Layout
        QStringList m_statitems;    //!< list of stats items which are reported for each queue
        QHash<QString, QString> m_statlegends;  //!< text displayed on top of each column
        
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
        QVariant m_options;
};

#endif /* __QUEUESPANEL_H__ */

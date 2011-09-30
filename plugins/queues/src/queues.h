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

#ifndef __QUEUESPANEL_H__
#define __QUEUESPANEL_H__

#include <QtGui>


#include <xletinterface.h>
#include <xlet.h>

#include "queueinfo.h"
#include "baseengine.h"
#include "userinfo.h"
#include "queueinfo.h"


class UserInfo;
class XletQueues;

/*! \brief to configure if the queue should be shown and the queue
 *  stats parameters
 */
class XletQueuesConfigure : public QWidget
{
    Q_OBJECT

    public:
        XletQueuesConfigure(XletQueues *xlet);
        QWidget* buildConfigureQueueList(QWidget *);

    protected:
        virtual void closeEvent(QCloseEvent *);

    private slots:
        void changeQueueStatParam(int);
};

class QueueRow : public QWidget {
    Q_OBJECT

    public:
        QueueRow(const QueueInfo *info, XletQueues *xlet);
        void updateRow();
        void updateSliceStat(const QString &stat, const QString &value);
        void updateLongestWaitWidget(int display, uint greenlevel, uint orangelevel);
        void updateBusyWidget();
        void updateName();

        static QWidget* makeTitleRow(XletQueues *parent);
        static void getLayoutColumnsWidth(QGridLayout *layout);
        static void setLayoutColumnWidth(QGridLayout *layout, int nbStat);

    private:
        QLabel *m_name;  //!< to display the name of queue
        QPushButton *m_more;  //!< to display queue details
        QLabel *m_longestWait;  //!< to display the longuest waiting time for each queue
        QPushButton *m_move;  //!< to change the order in which the queue are displayed
        QProgressBar *m_busy;  //!< to display the queues busy level
        QHash<QString, QLabel *> m_infoList; //!< the stats info
        static uint m_maxbusy;  //!< Maximum value for busy level
        QGridLayout * m_layout;
        const QueueInfo * m_queueinfo;
        XletQueues * m_xlet;
        static QList<int> m_colWidth;

};

/*! \brief Displays queues and their status
 */
class XletQueues : public XLet, IPBXListener
{
    Q_OBJECT

    public:
        XletQueues(QWidget *parent=0);
        void eatQueuesStats(const QVariantMap &);
        void parseCommand(const QVariantMap &);
        bool showMoreQueueDetailButton() { return m_showMore; };
        bool showNumber() { return m_showNumber; };

    protected:
        virtual void contextMenuEvent(QContextMenuEvent *);

    private:
        void openConfigureWindow();
        void saveQueueOrder(const QStringList &);
        void setQueueOrder(const QStringList &);
        void loadQueueOrder();                      //!< request load of queue order

    signals:
        void changeWatchedQueue(const QString &);   //!< Watch this queue

    public slots:
        void removeQueues(const QString &, const QStringList &);
        void updateQueueConfig(const QString &);
        void updateQueueStatus(const QString &);
        void settingsChanged();
        void updateLongestWaitWidgets();
        void askForQueueStats();
        void queueClicked();
        void display();

    private:
        bool m_showMore;
        bool m_showNumber;
        XletQueuesConfigure *m_configureWindow;

        QVBoxLayout *m_layout;
        QWidget *m_titleRow;
        QHash<QString, QueueRow *> m_queueList;
};

class XLetQueuesPlugin : public QObject, XLetInterface
{
    Q_OBJECT
    Q_INTERFACES(XLetInterface)

    public:
        XLet *newXLetInstance(QWidget *parent=0);
};


#endif

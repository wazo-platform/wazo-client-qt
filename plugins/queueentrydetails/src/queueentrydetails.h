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

#ifndef __QUEUEENTRYDETAILSPANEL_H__
#define __QUEUEENTRYDETAILSPANEL_H__

#include <QtGui>
#include "xlet.h"
#include "xletinterface.h"

#include <baseengine.h>

/*! \brief Display entries of a queue
 */
class XLetQueueEntryDetails : public XLet
{
    Q_OBJECT

    public:
        XLetQueueEntryDetails(QWidget *parent=0);

    signals:
        void changeWatchedAgent(const QString &);   //!< change watched agent

    protected:
        void timerEvent(QTimerEvent *);

    public slots:
        void setGuiOptions(const QVariantMap &) {};
        void updateAgentConfig(const QString &);
        void updateAgentStatus(const QString &);
        void updateQueueConfig(const QString &);
        void updateQueueStatus(const QString &);
        void monitorThisQueue(const QString &);

    private:
        void updateEntryChannel(const QString &);
        void clearPanel();
        void updatePanel();

        QGridLayout *m_gridlayout; //!< Layout

        QString m_monitored_queueid;  //!< queue id
        QLabel *m_queuedescription;   //!< label for displaying queue name
        QHash<QString, QLabel *> m_entrypos;    //!< display entry
        QHash<QString, QLabel *> m_entrytime;   //!< display call duration ?
};

class XLetQueueEntryDetailsPlugin : public QObject, XLetInterface
{
    Q_OBJECT
    Q_INTERFACES(XLetInterface)

    public:
        XLet *newXLetInstance(QWidget *parent=0);
};


#endif /* __QUEUEENTRYDETAILSPANEL_H__ */

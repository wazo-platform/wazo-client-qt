/* XiVO Client
 * Copyright (C) 2007-2012, Avencall
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

#ifndef __QUEUEENTRIES_H__
#define __QUEUEENTRIES_H__

#include <QHash>
#include <QVariantMap>
#include <QVariantList>
#include "xlet.h"
#include "xletinterface.h"

class QTimerEvent;
class QueueInfo;
class QGridLayout;
class QLabel;
class QString;

class QueueEntries : public XLet
{
    Q_OBJECT

    public:
        QueueEntries(QWidget *parent=0);

    protected:
        void timerEvent(QTimerEvent *);

    public slots:
        void setGuiOptions(const QVariantMap &) {};
        void updateQueueConfig(const QString &);
        void monitorThisQueue(const QString &);
        void subscribeQueueEntry(const QString &);
        void queueEntryUpdate(const QString &, const QVariantList &);
    private:
        void clearPanel();
        void updatePanel();
        void updateDescription(const QueueInfo *, int);
        void showEntries();

        QGridLayout *m_gridlayout;

        QString m_monitored_queueid;
        QLabel *m_queuedescription;
        QHash<int, QLabel *> m_entrypos;
        QVariantList m_queue_entries;
};

class QueueEntriesPlugin : public QObject, XLetInterface
{
    Q_OBJECT
    Q_INTERFACES(XLetInterface)

    public:
        XLet *newXLetInstance(QWidget *parent=0);
};


#endif /* __QUEUEENTRIES_H__ */

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

#include <QDebug>
#include <QVariant>
#include <QTime>

#include "JsonToVariant.h"
#include "VariantToJson.h"

#include "jsondecodethread.h"

JsonDecodeThread::JsonDecodeThread(QObject * parent)
        : QThread(parent)
{
}

void JsonDecodeThread::setLine(const QString & line)
{
        // qDebug() << "JsonDecodeThread::setLine()";
        m_queue.enqueue(line);
        m_waitcond_queueloop.wakeAll();
}

void JsonDecodeThread::run()
{
        while(true) {
                m_mtx_queueloop.lock();
                m_waitcond_queueloop.wait(& m_mtx_queueloop);
                while(! m_queue.isEmpty()) {
                        QString line = m_queue.dequeue();
                        QVariant data;
                        QTime t0 = QTime::currentTime();
                        try {
                                data = JsonQt::JsonToVariant::parse(line.trimmed());
                        }
                        catch(JsonQt::ParseException) {
                                qDebug() << "JsonDecodeThread::run() exception catched for" << line.trimmed();
                                data = QVariant(QVariant::Invalid);
                        }
                        QTime t1 = QTime::currentTime();
                        int nmsecs = t0.msecsTo(t1);
                        if(nmsecs > 0)
                                qDebug() << "JsonDecodeThread::run()" << data.toMap()["class"].toString()
                                         << line.size() << nmsecs << (line.size() / nmsecs) << "bytes/ms";
                        m_mtx_data.lock();
                        setProperty("parsed", data);
                        gotit();
                        m_waitcond_data.wait(& m_mtx_data);
                        m_mtx_data.unlock();
                }
                m_mtx_queueloop.unlock();
        }
}

void JsonDecodeThread::lock()
{
        m_mtx_data.lock();
}

void JsonDecodeThread::unlock_and_wake()
{
        m_mtx_data.unlock();
        m_waitcond_data.wakeAll();
}

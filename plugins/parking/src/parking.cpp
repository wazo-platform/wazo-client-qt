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


#include "parking.h"
#include "parkingwidget.h"

Q_EXPORT_PLUGIN2(xletparkingplugin, XLetParkingPlugin);

XLet* XLetParkingPlugin::newXLetInstance(QWidget *parent)
{
    b_engine->registerTranslation(":/parking_%1");
    return new XletParking(parent);
}


XletParking::XletParking(QWidget *parent)
    : XLet(parent)
{
    setTitle(tr("Parking"));
    m_parkinglayout = new QVBoxLayout(this);
    m_parkinglayout->setMargin(0);
    m_deltasec = 0.5;
    m_timerid = startTimer(m_deltasec * 1000);
    // connect signal / slots
    connect(b_engine, SIGNAL(updateParkinglotConfig(const QString &)),
            this, SLOT(updateParkinglotConfig(const QString &)));
    connect(b_engine, SIGNAL(updateParkinglotStatus(const QString &)),
            this, SLOT(updateParkinglotStatus(const QString &)));
}

XletParking::~XletParking()
{
    // qDebug() << Q_FUNC_INFO;
    killTimer(m_timerid);
}

/*! \brief receives configuration updates for parkinglots
 * \param xid The parkinglot's xivo id
 */
void XletParking::updateParkinglotConfig(const QString & xid)
{
    // qDebug() << Q_FUNC_INFO << xid;
    if (! m_parkinglots.contains(xid)) {
        ParkingWidget * w = new ParkingWidget(xid, this);
        if (w) {
            m_parkinglots.insert(xid, w);
            m_parkinglayout->addWidget(w);
            connect(w, SIGNAL(itemClicked(const QString &)),
                    this, SLOT(itemClicked(const QString &)));
            connect(w, SIGNAL(itemDoubleClicked(const QString &)),
                    this, SLOT(itemDoubleClicked(const QString &)));
        }
    }
}

/*! \brief receives status updates for parkinglots
 * \param xid The parking lot's xivo id
 */
void XletParking::updateParkinglotStatus(const QString & xid)
{
    // qDebug() << Q_FUNC_INFO << xid;
    if (m_parkinglots.contains(xid)) {
        m_parkinglots[xid]->refresh();
    }
}

/*! \brief Receives item clicks from parkingwidgets */
void XletParking::itemClicked(const QString & number)
{
    // qDebug() << Q_FUNC_INFO << number;
    b_engine->pasteToDial(number);
}

void XletParking::itemDoubleClicked(const QString & number)
{
    // qDebug() << Q_FUNC_INFO << number;
    b_engine->actionCall("originate", "user:special:me", "ext:" + number);
}

void XletParking::timerEvent(QTimerEvent * event)
{
    int timerId = event->timerId();
    if (timerId == m_timerid) {
        foreach (ParkingWidget * p, m_parkinglots) {
            if (p && p->count() > 0)
                p->refreshTimer();
        }
    }
}

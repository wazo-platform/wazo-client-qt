/* Copyright 2013-2017 The Wazo Authors  (see the AUTHORS file)
 *
 * This program is free software: you can redistribute it and/or modify
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
 * This programm is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QTimer>

#include "event_waiter.h"

/* should_wait prevents the waiting loop to run and timeout, in case the
   expected event already occurred
*/

EventWaiter::EventWaiter()
    : should_wait(true),
      stop_reason(TIMEOUT)
{
}

EventWaiter::~EventWaiter()
{
}

void EventWaiter::waitWithTimeout(int timeout_millisecs)
{
    QTimer timeout;
    timeout.setSingleShot(true);

    connect(&timeout, SIGNAL(timeout()), &(this->loop), SLOT(quit()));

    if (should_wait) {
        timeout.start(timeout_millisecs);
        loop.exec();
    }

    if (stop_reason == TIMEOUT) {
        throw TimeoutException();
    }
}

void EventWaiter::stopWaiting()
{
    this->should_wait = false;
    this->stop_reason = EXPLICIT_STOP;
    this->loop.quit();
}

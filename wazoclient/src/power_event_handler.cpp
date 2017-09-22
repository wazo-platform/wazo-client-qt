/*
 * Copyright 2007-2017 The Wazo Authors  (see the AUTHORS file)
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

#include <QCoreApplication>
#include <QVariant>

#ifdef Q_OS_WIN
#define NOGDI
#include <windef.h>
#include <winbase.h>
#include <winuser.h>
#include <pbt.h>
#endif

#include "power_event_handler.h"

bool PowerEventHandler::nativeEventFilter(const QByteArray & eventType, void * msg, long * result)
{
    if (eventType == "windows_generic_MSG") {
#ifdef Q_OS_WIN
        MSG * windows_message = (MSG *) msg;
        if(windows_message->message == WM_POWERBROADCAST) {
            /*
              PBT_APMSUSPEND System is suspending operation.
              - only since windows 2000:
              PBT_APMQUERYSUSPEND Request for permission to suspend
              PBT_APMQUERYSUSPENDFAILED Suspension request denied.
              PBT_APMPOWERSTATUSCHANGE Power status has changed. (switch battery/AC or battery under threshold)
            */
            if(windows_message->wParam == PBT_APMSUSPEND) {
                this->setProperty("stopper", "standby");
                emit standBy();
                *result = TRUE;
                return true;
            }
            /*
              PBT_APMRESUMEAUTOMATIC Operation is resuming automatically from a low-power state.
                                     This message is sent every time the system resumes.
              PBT_APMRESUMESUSPEND   Operation is resuming from a low-power state.
                                     This message is sent after PBT_APMRESUMEAUTOMATIC if the resume is triggered
                                     by user input, such as pressing a key.
            */
            if(windows_message->wParam == PBT_APMRESUMESUSPEND) {
                emit resume();
                *result = TRUE;
                return true;
            }
        }
#else
        Q_UNUSED(msg);
        Q_UNUSED(result);
#endif
    }
    return false;
}

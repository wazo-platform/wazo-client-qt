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

/* $Revision: 5882 $
 * $Date: 2009-04-07 12:44:04 +0200 (mar 07 avr 2009) $
 */

#include <QCoreApplication>
#include <QVariant>

#include "powerawareapplication.h"
#ifdef Q_OS_WIN
#define NOGDI
#include <windef.h>
#include <winbase.h>
#include <winuser.h>
#include <pbt.h>
#endif

/*! \brief constructor */
PowerAwareApplication::PowerAwareApplication(int & argc, char ** argv)
    : QtSingleApplication(argc, argv)
    //: QApplication(argc, argv)
{
}

#ifdef Q_OS_WIN
bool PowerAwareApplication::winEventFilter(MSG * msg, long * result)
{
    if(msg->message == WM_POWERBROADCAST) {
/*
 PBT_APMSUSPEND System is suspending operation.
 - only since windows 2000:
 PBT_APMQUERYSUSPEND Request for permission to suspend
 PBT_APMQUERYSUSPENDFAILED Suspension request denied.
*/
/*      if(msg->wParam == PBT_APMQUERYSUSPEND) {
            standBy();
            *result = TRUE; // BROADCAST_QUERY_DENY would block & deny/abort the Suspend Operation
            return true;
        } */
        powerEvent(QString("pbt-%1").arg(msg->wParam));
        
        if(msg->wParam == PBT_APMSUSPEND) {
            setProperty("stopper", "standby");
            standBy();
            *result = TRUE;
            return true;
        }
/*
 PBT_APMRESUMEAUTOMATIC Operation is resuming automatically from a low-power state. This message is sent every time the system resumes.
 PBT_APMRESUMESUSPEND   Operation is resuming from a low-power state. This message is sent after PBT_APMRESUMEAUTOMATIC if the resume is triggered by user input, such as pressing a key.
*/
        if(msg->wParam == PBT_APMRESUMESUSPEND) {
            resume();
            *result = TRUE;
            return true;
        }
    }
    return false;
}
#endif

void PowerAwareApplication::commitData(QSessionManager &)
{
    powerEvent("sessionclosed");
}

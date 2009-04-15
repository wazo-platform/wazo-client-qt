/* XIVO client
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
    : QApplication(argc, argv)
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
        powerEvent(msg->wParam);
        
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

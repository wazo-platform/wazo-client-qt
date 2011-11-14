/* XiVO Client
 * Copyright (C) 2011, Avencall
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

#ifndef __AASTRA_SIPNOTIFY_H__
#define __AASTRA_SIPNOTIFY_H__

#include <QVariantMap>

#define SPECIAL_ME "user:special:me"

namespace aastra_notify {

enum AastraKey {
    LINE, KEYPAD, KEYPAD_STAR, KEYPAD_POUND, SOFT_KEY, PRG_KEY,
    TOP_SOFT_KEY, EXP_MOD_SOFT_KEY, VOL_UP, VOL_DOWN, HEADSET, SPEAKER, MUTE,
    XFER, CONF, HOLD, REDIAL, CALLERS, SERVICES, INTERCOM, DIRECTORY, OPTIONS,
    SAVE, DELETE, GOODBYE, VOICEMAIL, NAV_UP, NAV_DOWN, NAV_LEFT, NAV_RIGHT
};

/*! \brief Creates a QVariantMap to simulate a click on an aastra phone
    
    \param type of key that is going to be pressed
    \param channel identifying the phone -- SIP/ewn1j9
    \param num1 used to identify a key when using a type with multiple possibilities
    \param num2 used when multiple numbers are needed
    \return the QVariantMap to emit to send the SIPnotify for that key press
 */
QVariantMap GetKey(AastraKey type, const QString & channel, int num1=0, int num2=0);
/*! \brief Create a QVariantMap to call from an aastra phone

    \param number is the number to call
    \param channel used to dial -- SIP/ewn1j9
    \param interrupt will interrupt ongoing call if true
    \return the QVariantMap to emit the dial SIPnotify
*/
QVariantMap GetDial(const QString & number, const QString & channel, bool interrupt=false);
QVariantMap Build(const QList<QString> &, const QString &);
QString GetKeyUri(AastraKey key, int num1=0, int num2=0);
/*! \brief Add the uris for a number to a given command */
void AppendNumber(const QString & number, QStringList & commands);

} // namespace aastra_notify

#endif // __AASTRA_SIPNOTIFY__


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

#include <QRegExp>
#include <QDebug>

#include "phonenumber.h"

namespace PhoneNumber
{
    /*!
     * \return regexp whose pattern is phone_pattern
     */
    QRegExp phone_re()
    {
        return QRegExp(PhoneNumber::phone_pattern);
    }

    /*!
     * see http://people.w3.org/~dom/archives/2005/09/integrating-a-new-uris-scheme-handler-to-gnome-and-firefox/
     * to learn how to handle "tel:0123456" uri scheme
     *
     * tel:number is in RFC 3966
     * callto:number is unofficial (read 7.3. in RFC 3966) and used by Skype
     * we support tel:number and callto:number
     * \return true if the parameter matches RFC 3966
     */
    bool isURI(const QString &string)
    {
        QRegExp re("^(tel|callto):" + phone_pattern, Qt::CaseInsensitive);
        return (re.indexIn(string) < 0);
    }

    /*!
     * \return the last contained phone number cleaned, i.e. with only numbers left
     */
    QString extract(const QString &string, const QString &prefix, const QString &suffix)
    {
        QRegExp re(prefix + phone_pattern + suffix);
        re.indexIn(string);                           // Apply the regexp
        QString ret = re.cap(0);                      // Get the entire match
        // qDebug() << string << re.pattern() << ret;
        ret.truncate(ret.length() - suffix.length()); // Chop suffix
        ret.remove(0, prefix.length());               // Chop prefix
        ret.remove('.').remove(' ').remove('-');      // Whistle while you work
        return ret;
    }

    /*!
     * \return true if the parameter contains a phone number that can be extracted
     */
    bool contains(const QString &string)
    {
        QRegExp re(phone_pattern);
        return (re.indexIn(string) < 0);
    }
}

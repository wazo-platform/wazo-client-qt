/* XiVO Client
 * Copyright (C) 2007-2011, Avencall
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

#ifndef __MEETMEINFO_H__
#define __MEETMEINFO_H__

#include "baselib_export.h"
#include <QStringList>
#include <QHash>
#include <QVariant>
#include "xinfo.h"

/*! \brief for storing meetme (conference room) infos
 *
 * All attributes use implicitly shared data
 * so it would be not too costy to copy/return this
 * class.
 */
class BASELIB_EXPORT MeetmeInfo : public XInfo
{
    public:
        MeetmeInfo(const QString &, const QString &);
        bool updateConfig(const QVariantMap &);
        bool updateStatus(const QVariantMap &);
        const QString & name() const;  //! conference room name
        const QString & number() const;  //! conference room number

        bool paused() const;  //! conference room paused status
        const QVariantMap & channels() const;  //! conference room channels
        bool pin_needed() const { return m_pin_needed; };
        const QString & admin_moderationmode() const { return m_admin_moderationmode; };
        const QString & pseudochan() const { return m_pseudochan; };

    private:
        QString m_context;  //!< room context
        QString m_name;  //!< room access name
        QString m_number;  //!< room access number (if any)
        QString m_pseudochan;
        QString m_admin_moderationmode;
        bool m_pin_needed;
        
        bool m_paused;  //!< is the conference room paused ?
        QVariantMap m_channels;  //!< channels in this conference room
};

#endif /* __MEETMEINFO_H__ */

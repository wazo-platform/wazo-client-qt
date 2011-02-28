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

#ifndef __PEERITEM_H__
#define __PEERITEM_H__

#include "baselib_export.h"

#include <QtGui>

#include <basicpeerwidget.h>

class PeerWidget;
class UserInfo;

/*! \brief PeerItem object, linking to a PeerWidget
 */
class BASELIB_EXPORT PeerItem
{
    public:
        PeerItem(UserInfo *);
        PeerItem(const PeerItem &);
        PeerItem();

        UserInfo* userinfo();

        void setWidget(BasePeerWidget * widget) { m_peerwidget = widget; };
        BasePeerWidget * getWidget() { return m_peerwidget; };
        void updateStatus();
        void updateAgentStatus(const QVariant &);
        void updateName(const QString &);
        void updateDisplayedStatus();
        void updateDisplayedName();
    private:
        BasePeerWidget *m_peerwidget;  //!< related PeerWidget

        UserInfo * m_ui;  // Properties of each peer
        QString m_vmstatus;
        QVariant m_agentstatus;
        QString m_pausestatus;
        QStringList m_queuelist;
};

#endif

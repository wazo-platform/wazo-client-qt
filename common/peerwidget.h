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

#ifndef __PEERWIDGET_H__
#define __PEERWIDGET_H__

#include <QHash>
#include <QWidget>
#include <QPoint>

#include "basepeerwidget.h"

class QLabel;
class QPixmap;

class BaseEngine;
class PeerChannel;
class ExtendedLabel;
class UserInfo;

/*! \brief Widget to display a Peer status
 *
 * Display Icons for the state and the peer name.
 */
class PeerWidget : public BasePeerWidget
{
    Q_OBJECT
        public:
    PeerWidget(BaseEngine *, UserInfo *, const QVariant &);
    ~PeerWidget();
    void setName(const QString &);
    void setEngine(BaseEngine *);
    void setAgentToolTip(const QString &, const QStringList &);
    void setAgentState(const QString & color);
    void updatePresence();  //!< update presence information displayed
    void updatePhonesStates();
 private:
    QHash<QString, QLabel *> m_lblphones;        //!< phone labels
        
    QLabel * m_availlbl;        //!< Peer state display from XIVO CTI Client
    ExtendedLabel * m_agentlbl;
    QLabel * m_voicelbl;
    QLabel * m_fwdlbl;
    QLabel * m_textlbl;                //!< text label : to display peer name
        
    static QHash<QString, QPixmap> m_phones;        //!< phone icons
    static QHash<QString, QPixmap> m_persons;        //!< person icons
    static QHash<QString, QPixmap> m_agents;        //!< agent icons
    QStringList m_functions;
};

#endif

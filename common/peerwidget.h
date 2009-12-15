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

/* $Revision$
 * $Date$
 */

#ifndef __PEERWIDGET_H__
#define __PEERWIDGET_H__

#include <QHash>
#include <QWidget>
#include <QPoint>

#include "basepeerwidget.h"
#include "taintedpixmap.h"

class QLabel;
class QPixmap;

class BaseEngine;
class PeerChannel;
class UserInfo;

/*! \brief Widget to display a Peer status
 *
 * Display Icons for the state and the peer name.
 */
class PeerWidget : public BasePeerWidget
{
    Q_OBJECT
public:
    PeerWidget(BaseEngine *, UserInfo *);
    ~PeerWidget();
    void setName(const QString &);
    void setEngine(BaseEngine *);
    void setAgentToolTip(const QString &, const QStringList &);
    void setAgentState(const QString & color);
    void setMobileState(const QString & color);
    void updatePresence();  //!< update presence information displayed
    void updatePhonesStates();
private:
    QHash<QString, QLabel *> m_lblphones;        //!< phone labels
        
    QLabel * m_availlbl;        //!< Peer state display from XIVO CTI Client
    QLabel * m_agentlbl;        //!< agent state label
    QLabel * m_mobilelbl;       //!< mobile phone label
    QLabel * m_voicelbl;
    QLabel * m_fwdlbl;
    QLabel * m_textlbl;                //!< text label : to display peer name
        
    static QHash<QString, QPixmap> m_phones;        //!< phone icons
    static QHash<QString, QPixmap> m_agents;        //!< agent icons
    static QHash<QString, QPixmap> m_mobiles;        //!< mobil phone icons
};

#endif

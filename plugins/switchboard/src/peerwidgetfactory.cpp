/* XiVO Client
 * Copyright (C) 2007-2010, Proformatique
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

/* $Revision: 8992 $
 * $Date: 2010-08-24 05:59:38 -0400 (mar 24 aoû 2010) $
 */


#include "peerwidgetfactory.h"
#include <basicpeerwidget.h>
#include <peerwidget.h>
#include <detailedexternalphonepeerwidget.h>
#include <externalphonepeerwidget.h>

QString PeerWidgetFactory::getSwitchBoardEltType()
{
    return b_engine->getGuiOptions("merged_gui").value("switchboard-elt-type").toString();
}

/*! \brief return an "External phone" widget
 *
 * return an instance of ExternalPhonePeerWidget or DetailedExternalPhonePeerWidget
 * depending on the value of the "switchboard-elt-type" gui setting of the BaseEngine.
 */
BasePeerWidget* PeerWidgetFactory::newExternalPhonePeerWidget(
    const QString &label,
    const QString &number)
{
    BasePeerWidget *w;

    if(getSwitchBoardEltType() == "small"){
        w = new ExternalPhonePeerWidget(label, number);
    }else{
        w = new DetailedExternalPhonePeerWidget(label, number);
    }
    
    return w;
}

/*! \brief return an "Internal phone" widget
 *
 * return an instance of BasicPeerWidget or PeerWidget
 * depending on the value of the "switchboard-elt-type" gui setting of the BaseEngine.
 */
BasePeerWidget* PeerWidgetFactory::newPeerWidget(UserInfo * ui)
{
    BasePeerWidget *w;

    if(getSwitchBoardEltType() == "small"){
        w = new BasicPeerWidget(ui);
    }else{
        w = new PeerWidget(ui);
    }

    return w;
}


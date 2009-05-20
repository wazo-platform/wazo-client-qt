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


#include "peerwidgetfactory.h"
#include "baseengine.h"
#include "basicpeerwidget.h"
#include "peerwidget.h"
#include "detailedexternalphonepeerwidget.h"
#include "externalphonepeerwidget.h"

/*! \brief Constructor */
PeerWidgetFactory::PeerWidgetFactory(BaseEngine * engine, QObject * parent)
    : QObject(parent), m_engine(engine)
{
}

/*! \brief Destructor */
QString PeerWidgetFactory::getSwitchBoardEltType() const
{
    return m_engine->getGuiOptions("client_gui").toMap()["switchboard-elt-type"].toString();
}

/*! \brief return an "External phone" widget
 *
 * return an instance of ExternalPhonePeerWidget or DetailedExternalPhonePeerWidget
 * depending on the value of the "switchboard-elt-type" gui setting of the BaseEngine.
 * The actionCall() signal is already connected to the BaseEngine.
 */
BasePeerWidget * PeerWidgetFactory::newExternalPhonePeerWidget(const QString & label, const QString & number)
{
    BasePeerWidget * w;
    if(getSwitchBoardEltType() == "small")
    {
        w = new ExternalPhonePeerWidget( m_engine, label, number );
    }
    else
    {
        w = new DetailedExternalPhonePeerWidget( m_engine, label, number );
    }
    connect( w, SIGNAL(actionCall(const QString &,
                                  const QString &,
                                  const QString &)),
             m_engine, SLOT(actionCall(const QString &,
                                       const QString &,
                                       const QString &)) );
    return w;
}

/*! \brief return an "Internal phone" widget
 *
 * return an instance of BasicPeerWidget or PeerWidget
 * depending on the value of the "switchboard-elt-type" gui setting of the BaseEngine.
 * The actionCall() signal is already connected to the BaseEngine.
 */
BasePeerWidget * PeerWidgetFactory::newPeerWidget(UserInfo * ui)
{
    BasePeerWidget * w;
    if(getSwitchBoardEltType() == "small")
    {
        w = new BasicPeerWidget( m_engine, ui );
    }
    else
    {
        w = new PeerWidget( m_engine, ui );
    }
    connect( w, SIGNAL(actionCall(const QString &,
                                  const QString &,
                                  const QString &)),
             m_engine, SLOT(actionCall(const QString &,
                                       const QString &,
                                       const QString &)) );
    return w;
}


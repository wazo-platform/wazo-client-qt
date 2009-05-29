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

#include <QApplication>
#include <QPluginLoader>

#include "xletfactory.h"
#include "xletinterface.h"

#include "agentspanel.h"
#include "agentspanel_next.h"
#include "agentdetailspanel.h"
#include "callcampaignpanel.h"
#include "callstackwidget.h"
#include "conferencepanel.h"
#include "customerinfopanel.h"
#include "datetimepanel.h"
#include "dialpanel.h"
#include "directorypanel.h"
#include "displaymessages.h"
#include "identitydisplay.h"
#include "faxpanel.h"
#include "logwidget.h"
#include "mylocaldirpanel.h"
#ifdef USE_OUTLOOK
#include "outlook_panel.h"
#include "outlook_engine.h"
#endif /* USE_OUTLOOK */
#include "parkingpanel.h"
#include "queuespanel.h"
#include "queuedetailspanel.h"
#include "queueentrydetailspanel.h"
#include "searchpanel.h"
#include "servicepanel.h"
#include "statuspanel.h"
#include "switchboardwindow.h"
#include "videopanel.h"

/*! \brief Constructor 
 */
XLetFactory::XLetFactory(BaseEngine * engine, QObject * parent)
    : QObject(parent), m_engine(engine)
{
}

/*! \brief build a new XLet depending on the type wanted */
XLet * XLetFactory::newXLet(const QString & id, QWidget * topwindow) const
{
    XLet * xlet = 0;
    if(id == "history") {
        xlet = new LogWidget(m_engine, topwindow);
    } else if(id == "identity") {
        xlet = new IdentityDisplay(m_engine, topwindow);
    } else if(id == "agents") {
        xlet = new AgentsPanel(m_engine, topwindow);
    } else if(id == "agentsnext") {
        xlet = new AgentsPanelNext(m_engine, topwindow);
    } else if(id == "agentdetails") {
        xlet = new AgentdetailsPanel(m_engine, topwindow);
    } else if(id == "conference") {
        xlet = new ConferencePanel(m_engine, topwindow);
    } else if(id == "customerinfo") {
        xlet = new CustomerInfoPanel(m_engine, topwindow);
    } else if(id == "queues") {
        xlet = new QueuesPanel(m_engine, topwindow);
    } else if(id == "queuedetails") {
        xlet = new QueuedetailsPanel(m_engine, topwindow);
    } else if(id == "queueentrydetails") {
        xlet = new QueueentrydetailsPanel(m_engine, topwindow);
    } else if(id == "datetime") {
        xlet = new DatetimePanel(m_engine, topwindow);
    } else if(id == "dial") {
        xlet = new DialPanel(m_engine, topwindow);
    } else if(id == "video") {
        xlet = new PlayerWidget(m_engine, topwindow);
    } else if(id == "operator") {
        xlet = new StatusPanel(m_engine, topwindow);
    } else if(id == "messages") {
        xlet = new DisplayMessagesPanel(m_engine, topwindow);
    } else if(id == "switchboard") {
        xlet = new SwitchBoardWindow(m_engine, topwindow);
    } else if(id == "parking") {
        xlet = new ParkingPanel(m_engine, topwindow);
    } else if(id == "fax") {
        xlet = new FaxPanel(m_engine, topwindow);
    } else if(id == "search") {
        xlet = new SearchPanel(m_engine, topwindow);
    } else if(id == "features") {
        xlet = new ServicePanel(m_engine, topwindow);
    } else if(id == "directory") {
        xlet = new DirectoryPanel(m_engine, topwindow);
#ifdef USE_OUTLOOK
    } else if(id == "outlook") {
        xlet = new OutlookPanel(m_engine, topwindow);
#endif /* USE_OUTLOOK */
    } else if(id == "callcampaign") {
        xlet = new CallCampaignPanel(m_engine, topwindow);
    } else if(id == "mylocaldir") {
        xlet = new MyLocalDirPanel(m_engine, topwindow);
    } else if(id == "calls") {
        xlet = new CallStackWidget(m_engine, topwindow);
    }
    return xlet;
}

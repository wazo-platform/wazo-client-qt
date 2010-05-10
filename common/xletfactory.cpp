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

/* $Revision$
 * $Date$
 */

#include <QDebug>
#include <QApplication>
#include <QPluginLoader>
#include <QDir>

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
#include "parkingpanel.h"
#include "queuespanel.h"
#include "queuedetailspanel.h"
#include "queueentrydetailspanel.h"
#include "searchpanel.h"
#include "servicepanel.h"
#include "statuspanel.h"
#include "switchboardwindow.h"

/*! \brief templates for returning a new XLet */
template <class T>
XLet * newXLet(BaseEngine * engine, QWidget * parent)
{
    return new T(engine, parent);
}

/*! \brief list of XLets */
static const struct {
    const char * name;
    const newXLetProto construct;
} xlets[] = {
    { "history"           ,newXLet<LogWidget>              },
    { "identity"          ,newXLet<IdentityDisplay>        },
    { "agents"            ,newXLet<AgentsPanel>            },
    { "agentsnext"        ,newXLet<AgentsPanelNext>        },
    { "agentdetails"      ,newXLet<AgentdetailsPanel>      },
    { "conference"        ,newXLet<ConferencePanel>        },
    { "customerinfo"      ,newXLet<CustomerInfoPanel>      },
    { "queues"            ,newXLet<QueuesPanel>            },
    { "queuedetails"      ,newXLet<QueuedetailsPanel>      },
    { "queueentrydetails" ,newXLet<QueueentrydetailsPanel> },
    { "datetime"          ,newXLet<DatetimePanel>          },
    { "dial"              ,newXLet<DialPanel>              },
    { "operator"          ,newXLet<StatusPanel>            },
    { "messages"          ,newXLet<DisplayMessagesPanel>   },
    { "switchboard"       ,newXLet<SwitchBoardWindow>      },
    { "parking"           ,newXLet<ParkingPanel>           },
    { "fax"               ,newXLet<FaxPanel>               },
    { "search"            ,newXLet<SearchPanel>            },
    { "features"          ,newXLet<ServicePanel>           },
    { "directory"         ,newXLet<DirectoryPanel>         },
    { "callcampaign"      ,newXLet<CallCampaignPanel>      },
    { "mylocaldir"        ,newXLet<MyLocalDirPanel>        },
    { "calls"             ,newXLet<CallStackWidget>        },
};

/*! \brief Constructor 
 *
 * Find and initialize plugins directory,
 * populate the m_xlets hash table.
 */
XLetFactory::XLetFactory(BaseEngine * engine, QObject * parent)
    : QObject(parent), m_engine(engine),
      m_pluginsDir(qApp->applicationDirPath()),
      m_pluginsDirFound(false)
{
#ifndef Q_WS_MAC
    if(m_pluginsDir.cd("plugins")) {
        // if there is a plugins dir next to where, the application remain
        m_pluginsDirFound = true;
    } else {
        if (m_pluginsDir.cd("/usr/share/xivoclient/plugins")) {
            //  the xivo_client might be installed properly for all the user on an unix box
            m_pluginsDirFound = true;
        } else {
            qDebug() << "cannot find plugins directory";
        }
    }
#else
    if (m_pluginsDir.cd("../Resources/plugins"))
        m_pluginsDirFound = true;
    } else {
        qDebug() << "cannot find plugins directory";
    }
#endif

    uint i;

    // populate the m_xlets hash table
    for(i=0;i<(sizeof(xlets)/sizeof(xlets[0]));i++) {
        m_xlets.insert(QString(xlets[i].name),xlets[i].construct);
    }
}

/*! \brief build a new XLet depending on the type wanted
 *
 *  First try to find the XLet into the build in list, then
 *  search for a plugin.
 *  Plugin name is lib<id>plugin.so under Linux, <id>plugin.dll under
 *  Win32.
 *
 *  \return a pointer to the XLet or NULL if it was not found
 */
XLet * XLetFactory::newXLet(const QString & id, QWidget * topwindow) const
{
    XLet * xlet = 0;
    newXLetProto construct = m_xlets.value(id);
    if(construct) {
        xlet = construct(m_engine, topwindow);
    } else if(m_pluginsDirFound) {
#ifdef Q_WS_WIN
        QString fileName = id + "plugin.dll";
#endif
#ifdef Q_WS_X11
        QString fileName = "lib" + id + "plugin.so";
#endif
#ifdef Q_WS_MAC
        QString fileName = "lib" + id + "plugin.dylib"; // TODO : check for correctness
#endif
        qDebug() << "Trying to load plugin" << fileName << m_pluginsDir.absoluteFilePath(fileName);
        QPluginLoader pluginLoader(m_pluginsDir.absoluteFilePath(fileName));
        QObject * plugin = pluginLoader.instance();

        if (plugin) {
            XLetInterface *xleti = qobject_cast<XLetInterface *>(plugin);
            if(xleti) {
                xlet = xleti->newXLetInstance(m_engine, topwindow);
            } else {
                qDebug() << "failed to cast plugin loaded to XLetInterface";
            }
        } else {
            qDebug() << "failed to load plugin :"<< pluginLoader.errorString();
        }
    }

    return xlet;
}


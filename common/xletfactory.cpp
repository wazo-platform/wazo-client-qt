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
#include <QHash>

#include "xletfactory.h"
#include "xletinterface.h"

#include <agentspanel.h>
#include <agentspanel_next.h>
#include <agentdetailspanel.h>
#include <callcampaignpanel.h>
#include <callstackwidget.h>
#include <customerinfopanel.h>
#include <dialpanel.h>
#include <directorypanel.h>
#include <identitydisplay.h>
#include <faxpanel.h>
#include <mylocaldirpanel.h>
#include <parkingpanel.h>
#include <queuespanel.h>
#include <queuedetailspanel.h>
#include <queueentrydetailspanel.h>
#include <searchpanel.h>
#include <servicepanel.h>
#include <statuspanel.h>
#include <switchboardwindow.h>

/*! \brief template to return a new XLet */
template <class T>
XLet* newXLet(QWidget *parent)
{
    return new T(parent);
}

typedef XLet* (*newXLetProto)(QWidget *);

/*! \brief list of the built-in XLets */
static const struct {
    const char *name;
    const newXLetProto construct;
} xlets[] = {
    //{ "history"           ,newXLet<LogWidget>              },
    { "identity"          ,newXLet<IdentityDisplay>        },
    { "agents"            ,newXLet<AgentsPanel>            },
    { "agentsnext"        ,newXLet<AgentsPanelNext>        },
    { "agentdetails"      ,newXLet<AgentdetailsPanel>      },
    { "customerinfo"      ,newXLet<CustomerInfoPanel>      },
    { "queues"            ,newXLet<QueuesPanel>            },
    { "queuedetails"      ,newXLet<QueuedetailsPanel>      },
    { "queueentrydetails" ,newXLet<QueueentrydetailsPanel> },
    { "dial"              ,newXLet<DialPanel>              },
    { "operator"          ,newXLet<StatusPanel>            },
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

/*! xlet creator function prototype */
    
static QHash<QString, newXLetProto> xletList;  //!< built-in XLets constuctors
static QDir pluginDir;  //!< directory where to find plugins
static bool pluginDirFound = false;  //!< Is the plugins directory found ?
static bool initied = false;

static void init()
{
    int i;
    for (i=0;i<nelem(xlets);i++) {
        xletList.insert(xlets[i].name, xlets[i].construct);
    }
    initied = true;
}


/*! 
 * Find a plugins directory,
 * ./plugins
 * or 
 * /usr/share/xivoclient/plugins
 * and populate the xletList hash table.
 */
static bool findPluginDir()
{
    pluginDir = qApp->applicationDirPath();
    pluginDirFound = false;
#ifndef Q_WS_MAC
    if (pluginDir.cd("plugins")) {
        // if there is a plugins dir next to where, the application remain
        pluginDirFound = true;
    } else {
        if (pluginDir.cd("/usr/share/xivoclient/plugins")) {
            //  the xivo_client might be installed properly for all the user on an unix box
            pluginDirFound = true;
        } else {
            qDebug() << "cannot find plugins directory";
        }
    }
#else
    QString pluginDirPath = qApp->applicationDirPath() + "/../Resources/plugins";
    pluginDirFound = pluginDir.cd(pluginDirPath);
    if (!pluginDirFound) {
        qDebug() << "cannot find plugins directory (tryed: " << pluginDirPath << ")";
    }
#endif

    return pluginDirFound;
}

/*! \brief build a new XLet depending on the type wanted
 *
 *  First try to find the XLet into the built-in list, then
 *  search for a plugin.
 *  Plugin name is lib<id>plugin.so under Linux, <id>plugin.dll under
 *  Win32, and lib<id>plugin.dylib under Mac Os X.
 *
 *  \return a pointer to the XLet or NULL if it was not found
 */
namespace XLetFactory {
XLet* spawn(const QString &id, QWidget *parent)
{
    if (initied == false) {
        init();
        findPluginDir();
    }

    XLet *xlet = NULL;

    newXLetProto construct = xletList.value(id);

    if (construct) {
        xlet = construct(parent);
    } else {
        QString fileName =
#ifdef Q_WS_WIN
        id + "plugin.dll";
#endif
#ifdef Q_WS_X11
        "lib" + id + "plugin.so";
#endif
#ifdef Q_WS_MAC
        "lib" + id + "plugin.dylib";
#endif

        qDebug() << "Trying to load plugin" << fileName << pluginDir.absoluteFilePath(fileName);
        QPluginLoader loader(pluginDir.absoluteFilePath(fileName));
        QObject *plugin = loader.instance();

        if (plugin) {
            XLetInterface *xleti = qobject_cast<XLetInterface *>(plugin);
            if (xleti) {
                xlet = xleti->newXLetInstance(parent);
            } else {
                qDebug() << "failed to cast plugin loaded to XLetInterface";
            }
        } else {
            qDebug() << "failed to load plugin :"<< loader.errorString();
        }
    }

    return xlet;
}
}


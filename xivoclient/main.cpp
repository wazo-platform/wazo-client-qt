/* XIVO CTI Clients
 * Copyright (C) 2007, 2008  Proformatique
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

#include <QApplication>
#include <QFile>
#include <QLocale>
#include <QSettings>
#include <QSplashScreen>
#include <QTranslator>

#include "baseengine.h"
#include "mainwidget.h"

/*! \fn main
 *
 * \brief program entry point
 *
 * Set some static Qt parameters for using QSettings,
 * instantiate a MainWidget window and a BaseEngine object.
 *
 * \sa MainWidget, BaseEngine
 */
int main(int argc, char ** argv)
{
	QString locale = QLocale::system().name();
        QString applaunch = argv[0];
        QString appoptions = "";
        if(argc > 1)
                appoptions = argv[1];
        // QApplication::setStyle(new XIVOCTIStyle());
	QCoreApplication::setOrganizationName("XIVO");
	QCoreApplication::setOrganizationDomain("xivo.fr");
        if(applaunch.contains("xivoclient"))
                QCoreApplication::setApplicationName("XIVO_Client");
        else
                QCoreApplication::setApplicationName("XIVO_Switchboard");
	QApplication app(argc, argv);
        QSettings * settings = new QSettings(QSettings::IniFormat,
                                             QSettings::UserScope,
                                             QCoreApplication::organizationName(),
                                             QCoreApplication::applicationName());

        QString qsskind = settings->value("display/qss", "none").toString();
        QFile qssFile(":/common/" + qsskind + ".qss");
        QString qssStr;
        if(qssFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
                qssStr = qssFile.readAll();
                qssFile.close();
        }
        app.setStyleSheet(qssStr);
        app.setWindowIcon(QIcon(":/images/xivoicon.png"));

        QTranslator qtTranslator;
        QString forcelocale = settings->value("display/forcelocale", "").toString();
        if(forcelocale.size() > 0)
                locale = forcelocale;
        qtTranslator.load(QString(":/xivoclient_") + locale);
        if(applaunch.contains("xivoclient"))
                app.setQuitOnLastWindowClosed(false);
        app.installTranslator(&qtTranslator);

        BaseEngine * engine = new BaseEngine(settings);

        QString appname = "";
        if(applaunch.contains("xivoclient")) {
                engine->setIsASwitchboard(false);
                appname = "Client";
                // appoptions = "services:grid,dial:grid,peers:tab,history:tab,directory:tab,customerinfo:tab,fax:tab,features:tab"
        } else {
                engine->setIsASwitchboard(true);
                appname = "Switchboard";
                // appoptions = "services:dock:m,dial:dock:m,peers:dock:m,history,directory:dock:m,customerinfo:dock:m,po:dock:mfc,parking:tab,messages:tab,switchboard:dock:mc";
        }
        MainWidget main(engine, appname, appoptions);

	//main.dumpObjectTree();
        QObject::connect( &app, SIGNAL(lastWindowClosed()),
                          engine, SLOT(stop()) );
	//engine.startTimer(1000);
        return app.exec();
}

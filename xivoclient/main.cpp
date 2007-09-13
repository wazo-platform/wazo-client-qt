/*
XIVO customer information client : popup profile for incoming calls
Copyright (C) 2007  Proformatique

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
*/

/* $Id$
   $Date$
*/

#include <QApplication>
#include <QFile>
#include <QLocale>
#include <QSettings>
#include <QSplashScreen>
#include <QTranslator>

#include "mainwidget.h"
#include "baseengine.h"

/*! \mainpage Profile Popup
 *
 * \section intro_sec Introduction
 *
 * This Qt program is designed to run quietly in the background
 * waiting for incoming connections from the server.
 * When a profile is received, it is displayed.
 *
 * \sa main
 */

/*! \fn main
 *
 * \brief program entry point
 *
 * Set some static Qt parametters for using QSettings,
 * instanciate a MainWidget window and a BaseEngine object.
 *
 * \sa MainWidget, BaseEngine
 */
int main(int argc, char * * argv)
{
	QString locale = QLocale::system().name();
//	QApplication::setStyle(new XivoClientStyle());
	QCoreApplication::setOrganizationName("Xivo");
	QCoreApplication::setOrganizationDomain("xivo.fr");
	QCoreApplication::setApplicationName("XivoClient");
	QApplication app(argc, argv);
        QSettings settings;

        QString qsskind = settings.value("display/qss", "none").toString();
        QFile qssFile(":/common/" + qsskind + ".qss");
        QString qssStr;
        if(qssFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
                qssStr = qssFile.readAll();
                qssFile.close();
        }
        app.setStyleSheet(qssStr);

	QTranslator qtTranslator;
	qtTranslator.load(QString(":/xivoclient_") + locale);
	app.installTranslator(&qtTranslator);
        app.setQuitOnLastWindowClosed(false);

	BaseEngine engine;
	engine.setIsASwitchboard(false);

	MainWidget main(&engine);
	main.show();
	//main.dumpObjectTree();
 	QObject::connect( &app, SIGNAL(lastWindowClosed()),
                          &engine, SLOT(stop()) );
	//engine.startTimer(1000);
        return app.exec();
}

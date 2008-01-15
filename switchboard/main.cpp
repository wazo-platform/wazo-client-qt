/* XIVO CTI switchboard
Copyright (C) 2007, 2008  Proformatique

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
version 2 as published by the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
*/

/* $Revision$
 * $Date$
 */

#include <QApplication>
#include <QFile>
#include <QLocale>
#include <QSettings>
#include <QTranslator>

#include "baseengine.h"
#include "mainwidget.h"

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
//	QApplication::setStyle(new SwitchboardStyle());
	QCoreApplication::setOrganizationName("XIVO");
	QCoreApplication::setOrganizationDomain("xivo.fr");
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
	qtTranslator.load(QString(":/switchboard_") + locale);
	app.installTranslator(&qtTranslator);

        BaseEngine * engine = new BaseEngine(settings);
	engine->setIsASwitchboard(true);

	MainWidget main(engine);
	main.show();
	//main.dumpObjectTree();
        QObject::connect( &app, SIGNAL(lastWindowClosed()),
                          engine, SLOT(stop()) );
	//engine.startTimer(1000);
        return app.exec();
}

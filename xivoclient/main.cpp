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

//#include <QApplication>
//#include <QtSingleApplication>
#include <QFile>
#include <QLocale>
#include <QSettings>
#include <QSplashScreen>
#include <QStyle>
#include <QSysInfo>
#include <QTranslator>

#include "baseengine.h"
#include "mainwidget.h"
#include "powerawareapplication.h"

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
    // QApplication::setStyle(new XIVOCTIStyle());
    QCoreApplication::setOrganizationName("XIVO");
    QCoreApplication::setOrganizationDomain("xivo.fr");
    QCoreApplication::setApplicationName("XIVO_Client");
    PowerAwareApplication app(argc, argv);
    if(app.isRunning()) {
        qDebug() << "application is already running";
        // do not create a new application, just activate the currently running one
        QString msg;
        if(argc > 1) {
            // send message if there is an argument.
            // see http://people.w3.org/~dom/archives/2005/09/integrating-a-new-uris-scheme-handler-to-gnome-and-firefox/
            // to learn how to handle "tel:0123456" uri scheme
            msg.append(argv[1]);
            qDebug() << "sending" << msg;
            qDebug() << app.sendMessage(msg);
        }
        return 0;
    }
    QSettings * settings = new QSettings(QSettings::IniFormat,
                                         QSettings::UserScope,
                                         QCoreApplication::organizationName(),
                                         QCoreApplication::applicationName());
    qDebug() << "style" << app.style() << settings->fileName();
    if(argc > 1) {
        QString argv1(argv[1]);
        if(!argv1.startsWith("tel:", Qt::CaseInsensitive) && !argv1.startsWith("callto:", Qt::CaseInsensitive))
            settings->setValue("profile/default", argv1);
    }
    QString qsskind = settings->value("display/qss", "none").toString();
    QFile qssFile(QString(":/common/%1.qss").arg(qsskind));
    QString qssStr;
    if(qssFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qssStr = qssFile.readAll();
        qssFile.close();
    }
    app.setStyleSheet(qssStr);
    app.setWindowIcon(QIcon(":/images/xivo-login.png"));
    
    QTranslator qtTranslator_xivo, qtTranslator_qt;
    QString forcelocale = settings->value("display/forcelocale", "").toString();
    if(forcelocale.size() > 0)
        locale = forcelocale;
    qtTranslator_xivo.load(QString(":/xivoclient_%1").arg(locale));
    qtTranslator_qt.load(QString(":/qt_%1").arg(locale));
    // QLibraryInfo::location(QLibraryInfo::TranslationsPath)
    app.installTranslator(&qtTranslator_xivo);
    app.installTranslator(&qtTranslator_qt);
    app.setQuitOnLastWindowClosed(false);
    
    BaseEngine * engine = new BaseEngine(settings);
    
    QString info_osname;
    QString info_endianness;
    if(QSysInfo::ByteOrder == 0)
        info_endianness = "BE";
    else
        info_endianness = "LE";
#if defined(Q_WS_X11)
    info_osname = QString("X11-%1-%2").arg(info_endianness).arg(app.applicationPid());
#elif defined(Q_WS_WIN)
    info_osname = QString("WIN-%1-0x%2-%3").arg(info_endianness).arg(QSysInfo::WindowsVersion, 2, 16, QChar('0')).arg(app.applicationPid());
#elif defined(Q_WS_MAC)
    info_osname = QString("MAC-%1-0x%2-%3").arg(info_endianness).arg(QSysInfo::MacintoshVersion, 2, 16, QChar('0')).arg(app.applicationPid());
#else
    info_osname = QString("unknown-%1-%2").arg(info_endianness).arg(app.applicationPid());
#endif
    qDebug() << "main() osname=" << info_osname << "locale=" << locale;
    
    engine->setOSInfos(info_osname);
    MainWidget main(engine);
    app.setActivationWindow( &main );
    
    //main.dumpObjectTree();
    app.setProperty("stopper", "lastwindow");
    
    // setting this connection breeds the following behaviour :
    //  * exit of config window when systray-only => disconnects from server
    // there seemed to be a case when this was useful however ...
    //    we let this commented until a relevant use case is met again
    // QObject::connect( &app, SIGNAL(lastWindowClosed()),
    // engine, SLOT(stop()) );
    
    QObject::connect( &app, SIGNAL(standBy()),
                      engine, SLOT(stop()) );
    QObject::connect( &app, SIGNAL(resume()),
                      engine, SLOT(start()) );
    QObject::connect( &app, SIGNAL(powerEvent(const QString &)),
                      engine, SLOT(powerEvent(const QString &)) );
    QObject::connect( &app, SIGNAL(messageReceived(const QString &)),
                      engine, SLOT(handleOtherInstanceMessage(const QString &)) );
    
    //engine.startTimer(1000);
    return app.exec();
}

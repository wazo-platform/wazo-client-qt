/* XiVO Client
 * Copyright (C) 2007-2011, Proformatique
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
#include <QLibraryInfo>

#include "baseengine.h"
#include "mainwidget.h"
#include "powerawareapplication.h"
#include "phonenumber.h"

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
    // QApplication::setStyle(new XiVOCTIStyle());
    QCoreApplication::setOrganizationName("XIVO");
    QCoreApplication::setOrganizationDomain("xivo.fr");
    QCoreApplication::setApplicationName("XIVO_Client");
    PowerAwareApplication app(argc, argv);

    QSettings * settings = new QSettings(QSettings::IniFormat,
                                         QSettings::UserScope,
                                         QCoreApplication::organizationName(),
                                         QCoreApplication::applicationName());
    qDebug() << Q_FUNC_INFO << "style" << app.style() << settings->fileName();

    QString profile = "default-user";
    QString msg = "";
    for (int argi = 0; argi < argc - 1 ; argi ++) {
        QString argn(argv[argi + 1]);
        if(! PhoneNumber::isURI(argn))
            profile = argn;
        else
            msg = argn;
    }

    if (! msg.isEmpty()) {
        // send message if there is an argument.
        // see http://people.w3.org/~dom/archives/2005/09/integrating-a-new-uris-scheme-handler-to-gnome-and-firefox/
        // to learn how to handle "tel:0123456" uri scheme
        bool sentmsg = app.sendMessage(msg);
        // warning : this sends the message only to the first instance, if ever there are >1 instances running
        qDebug() << Q_FUNC_INFO << "sent message" << msg << sentmsg;
    }

    app.setWindowIcon(QIcon(":/images/xivo-login.png"));

    QString info_osname;
    QString info_endianness = QSysInfo::ByteOrder ? "LE" : "BE";

#if defined(Q_WS_X11)
    info_osname = QString("X11-%1-%2")
        .arg(info_endianness)
        .arg(app.applicationPid());
#elif defined(Q_WS_WIN)
    info_osname = QString("WIN-%1-0x%2-%3")
        .arg(info_endianness)
        .arg(QSysInfo::WindowsVersion, 2, 16, QChar('0'))
        .arg(app.applicationPid());
#elif defined(Q_WS_MAC)
    info_osname = QString("MAC-%1-0x%2-%3")
        .arg(info_endianness)
        .arg(QSysInfo::MacintoshVersion, 2, 16, QChar('0'))
        .arg(app.applicationPid());
#else
    info_osname = QString("unknown-%1-%2")
        .arg(info_endianness)
        .arg(app.applicationPid());
#endif
    qDebug() << Q_FUNC_INFO << "osname=" << info_osname;
    
    bool shallbeunique = settings->value("display/unique").toBool();
    if (shallbeunique && app.isRunning()) {
        qDebug() << Q_FUNC_INFO << "unique mode : application is already running : exiting";
        // do not create a new application, just activate the currently running one
        return 0;
    }
    
    settings->setValue("profile/lastused", profile);
    
    BaseEngine *engine = new BaseEngine(settings, info_osname);

    QString qsskind = engine->getConfig("qss").toString();

    QFile qssFile(QString(":/common/%1.qss").arg(qsskind));
    if(qssFile.open(QIODevice::ReadOnly)) {
        app.setStyleSheet(qssFile.readAll());
    }

    MainWidget window;
    bool activate_on_tel = engine->getConfig("activate_on_tel").toBool();
    app.setActivationWindow(&window, activate_on_tel);

    app.setQuitOnLastWindowClosed(false);
    app.setProperty("stopper", "lastwindow");

    QObject::connect(&app, SIGNAL(standBy()),
                     engine, SLOT(stop()));
    QObject::connect(&app, SIGNAL(resume()),
                     engine, SLOT(start()));
    QObject::connect(&app, SIGNAL(powerEvent(const QString &)),
                     engine, SLOT(powerEvent(const QString &)));
    QObject::connect(&app, SIGNAL(messageReceived(const QString &)),
                     engine, SLOT(handleOtherInstanceMessage(const QString &)));

    return app.exec();
}

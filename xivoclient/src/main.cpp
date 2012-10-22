/* XiVO Client
 * Copyright (C) 2007-2011, Avencall
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

#include <QFile>
#include <QLocale>
#include <QSettings>
#include <QSplashScreen>
#include <QStyle>
#include <QSysInfo>
#include <QTranslator>
#include <QLibraryInfo>

#include <baseengine.h>
#include <phonenumber.h>

#include "mainwidget.h"
#include "powerawareapplication.h"
#include "fileopeneventhandler.h"

#ifdef FUNCTESTS
#include "remote_control/remote_control.h"
#endif

#include "main.h"

/*
 * Set some static Qt parameters for using QSettings,
 * instantiate a MainWidget window and a BaseEngine object.
 *
 * argc has to be a reference, or QCoreApplication will segfault
 *
 * \sa MainWidget, BaseEngine
 */
ExecObjects init_xivoclient(int & argc, char **argv)
{
    ExecObjects ret;
    QCoreApplication::setOrganizationName("XIVO");
    QCoreApplication::setOrganizationDomain("xivo.fr");
    QCoreApplication::setApplicationName("XIVO_Client");
    PowerAwareApplication *app = new PowerAwareApplication(argc, argv);

    FileOpenEventHandler* fileOpenHandler = new FileOpenEventHandler(app);
    app->installEventFilter(fileOpenHandler);

    QSettings * settings = new QSettings(QSettings::IniFormat,
                                         QSettings::UserScope,
                                         QCoreApplication::organizationName(),
                                         QCoreApplication::applicationName());
    qDebug() << Q_FUNC_INFO << "style" << app->style() << settings->fileName();

    QString profile = "default-user";
    QString msg = "";
    for (int i = 1; i < argc; i ++) {
        QString arg_str(argv[i]);
        if(! PhoneNumber::isURI(arg_str))
            profile = arg_str;
        else
            msg = PhoneNumber::extract(arg_str);
    }

    if (! msg.isEmpty()) {
        // send message if there is an argument.
        // see http://people.w3.org/~dom/archives/2005/09/integrating-a-new-uris-scheme-handler-to-gnome-and-firefox/
        // to learn how to handle "tel:0123456" uri scheme
        bool sentmsg = app->sendMessage(msg);
        // warning : this sends the message only to the first instance, if ever there are >1 instances running
        qDebug() << Q_FUNC_INFO << "sent message" << msg << sentmsg;
    }

    app->setWindowIcon(QIcon(":/images/xivo-login.png"));

    QString info_osname;
    QString info_endianness = QSysInfo::ByteOrder ? "LE" : "BE";

#if defined(Q_WS_X11)
    info_osname = QString("X11-%1-%2")
        .arg(info_endianness)
        .arg(app->applicationPid());
#elif defined(Q_WS_WIN)
    info_osname = QString("WIN-%1-0x%2-%3")
        .arg(info_endianness)
        .arg(QSysInfo::WindowsVersion, 2, 16, QChar('0'))
        .arg(app->applicationPid());
#elif defined(Q_WS_MAC)
    info_osname = QString("MAC-%1-0x%2-%3")
        .arg(info_endianness)
        .arg(QSysInfo::MacintoshVersion, 2, 16, QChar('0'))
        .arg(app->applicationPid());
#else
    info_osname = QString("unknown-%1-%2")
        .arg(info_endianness)
        .arg(app->applicationPid());
#endif
    qDebug() << Q_FUNC_INFO << "osname=" << info_osname;

    bool shallbeunique = settings->value("display/unique").toBool();
    if (shallbeunique && app->isRunning()) {
        qDebug() << Q_FUNC_INFO << "unique mode : application is already running : exiting";
        // do not create a new application, just activate the currently running one
        ret.initOK = false;
        return ret;
    }

    settings->setValue("profile/lastused", profile);

    b_engine = new BaseEngine(settings, info_osname);

    QString qsskind = b_engine->getConfig("qss").toString();

    QFile qssFile(QString(":/%1.qss").arg(qsskind));
    if(qssFile.open(QIODevice::ReadOnly)) {
        app->setStyleSheet(qssFile.readAll());
    }

    MainWidget *window = new MainWidget();
    bool activate_on_tel = b_engine->getConfig("activate_on_tel").toBool();
    app->setActivationWindow(window, activate_on_tel);

    app->setQuitOnLastWindowClosed(false);
    app->setProperty("stopper", "lastwindow");

    QObject::connect(app, SIGNAL(standBy()),
                     b_engine, SLOT(stop()));
    QObject::connect(app, SIGNAL(resume()),
                     b_engine, SLOT(start()));
    QObject::connect(app, SIGNAL(powerEvent(const QString &)),
                     b_engine, SLOT(powerEvent(const QString &)));
    QObject::connect(app, SIGNAL(messageReceived(const QString &)),
                     b_engine, SLOT(handleOtherInstanceMessage(const QString &)));
    QObject::connect(fileOpenHandler, SIGNAL(dialNumber(QString)),
                     b_engine, SLOT(handleOtherInstanceMessage(const QString &)));

    ret.app = app;
    ret.win = window;
    ret.baseengine = b_engine;
    ret.initOK = true;

#ifdef FUNCTESTS
    ret.rc = new RemoteControl(ret);
#endif

    return ret;
}

int run_xivoclient(ExecObjects exec_obj)
{
    if (exec_obj.initOK == true)
        return exec_obj.app->exec();
    else
        return 1;
}

void clean_xivoclient(ExecObjects exec_obj)
{

    if (exec_obj.initOK) {
        delete exec_obj.win;

        // BaseEngine is already deleted by MainWidget

#ifdef FUNCTESTS
        delete exec_obj.rc;
#endif
	delete exec_obj.app;
    }
}

/*! \brief program entry point
 *
 */
int main(int argc, char **argv)
{
    ExecObjects exec_obj= init_xivoclient(argc, argv);
    int ret = run_xivoclient(exec_obj);
    clean_xivoclient(exec_obj);
    return ret;
}

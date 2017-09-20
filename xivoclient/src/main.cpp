/*
 * Copyright 2007-2017 The Wazo Authors  (see the AUTHORS file)
 *
 * This program is free software: you can redistribute it and/or modify
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
 * This programm is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QFile>
#include <QSettings>
#include <QSplashScreen>
#include <QStyle>
#include <QSysInfo>
#include <QTranslator>
#include <QLibraryInfo>

#include <baseengine.h>
#include <phonenumber.h>

#include "assembler.h"
#include "main_window/main_window.h"
#include "power_event_handler.h"

#ifdef FUNCTESTS
#include "remote_control/remote_control.h"
#endif

#include "main.h"

const QString &str_socket_arg_prefix = "socket:";

bool renameConfigFile(const QString &old_org, const QString &old_app, const QString &new_org, const QString &new_app)
{
    QSettings old_settings(QSettings::IniFormat, QSettings::UserScope, old_org, old_app);
    QSettings new_settings(QSettings::IniFormat, QSettings::UserScope, new_org, new_app);
    const QString &old_filename = old_settings.fileName();
    const QString &new_filename = new_settings.fileName();

    if (QFile::exists(old_filename) == false) {
        return false;
    }

    const QString &config_dir_name = QFileInfo(new_filename).absolutePath();
    QDir config_dir(config_dir_name);
    config_dir.mkpath(config_dir_name);

    if (QFile::exists(new_filename)) {
        QFile::remove(new_filename);
    }

    return QFile::rename(old_filename, new_filename);
}

// argc has to be a reference, or QCoreApplication will segfault
ExecObjects init_xivoclient(int & argc, char **argv)
{
    ExecObjects ret;
    QCoreApplication::setOrganizationName("Wazo");
    QCoreApplication::setOrganizationDomain("wazo.community");
    QCoreApplication::setApplicationName("Wazo_Client");
    EventAwareApplication  *app = new EventAwareApplication(argc, argv);

    PowerEventHandler * power_event_handler = new PowerEventHandler();
    app->installNativeEventFilter(power_event_handler);

    bool renamed = renameConfigFile("XIVO", "XIVO_Client",
                                    QCoreApplication::organizationName(), QCoreApplication::applicationName());
    QSettings * settings = new QSettings(QSettings::IniFormat,
                                         QSettings::UserScope,
                                         QCoreApplication::organizationName(),
                                         QCoreApplication::applicationName());

    if (renamed) {
        qDebug() << "Old migration file migrated to" << settings->fileName();
    }

    qDebug() << "Reading configuration file" << settings->fileName();

    QString profile = "default-user";
    QString number = "";
    for (int i = 1; i < argc; i ++) {
        QString arg_str(argv[i]);

        if (arg_str.isEmpty() || arg_str.contains(str_socket_arg_prefix)) {
            continue;
        }

        if(PhoneNumber::isURI(arg_str)) {
            number = PhoneNumber::extract(arg_str);
        } else {
            profile = arg_str;
        }
    }

    qDebug() << "Selected profile: " << profile;

    bool shallbeunique = settings->value("display/unique").toBool();
    if (shallbeunique && app->isRunning()) {
        qDebug() << Q_FUNC_INFO << "unique mode : application is already running : exiting";

        if (! number.isEmpty()) {
            app->sendNumberToDial(number);
        } else {
            app->sendFocusRequest();
        }

        ret.initOK = false;
        return ret;
    }

    app->setWindowIcon(QIcon(":/images/xivoicon-orange.png"));

    QString info_osname;
    QString info_endianness = QSysInfo::ByteOrder ? "LE" : "BE";

#if defined(Q_OS_UNIX)
    info_osname = QString("X11-%1-%2")
        .arg(info_endianness)
        .arg(app->applicationPid());
#elif defined(Q_OS_WIN)
    info_osname = QString("WIN-%1-0x%2-%3")
        .arg(info_endianness)
        .arg(QSysInfo::WindowsVersion, 2, 16, QChar('0'))
        .arg(app->applicationPid());
#elif defined(Q_OS_MAC)
    info_osname = QString("MAC-%1-0x%2-%3")
        .arg(info_endianness)
        .arg(QSysInfo::MacintoshVersion, 2, 16, QChar('0'))
        .arg(app->applicationPid());
#else
    info_osname = QString("unknown-%1-%2")
        .arg(info_endianness)
        .arg(app->applicationPid());
#endif

    settings->setValue("profile/lastused", profile);

    b_engine = new BaseEngine(settings, info_osname);

    assembler = new Assembler();
    if (! assembler) {
        qDebug() << Q_FUNC_INFO << "Failed to instantiate the Assembler";
        return ret;
    }
    MainWindow *main_window = assembler->mainWindow();
    main_window->initialize();

    bool activate_on_tel = b_engine->getConfig("activate_on_tel").toBool();
    app->setActivationWindow(main_window, false);

    if (activate_on_tel) {
        QObject::connect(app, SIGNAL(numberToDialReceived(const QString &)),
                        app, SLOT(activateWindow()));
    }

    app->setQuitOnLastWindowClosed(false);
    app->setProperty("stopper", "lastwindow");

    QObject::connect(power_event_handler, SIGNAL(standBy()),
                     b_engine, SLOT(stop()));
    QObject::connect(power_event_handler, SIGNAL(resume()),
                     b_engine, SLOT(start()));
    QObject::connect(app, SIGNAL(numberToDialReceived(const QString &)),
                     b_engine, SLOT(actionDial(const QString &)));
    QObject::connect(app, SIGNAL(focusRequestReceived()),
                     app, SLOT(activateWindow()));

    ret.app = app;
    ret.event_handler = power_event_handler;
    ret.win = main_window;
    ret.baseengine = b_engine;
    ret.initOK = true;

#ifdef FUNCTESTS
    QString socket = "/tmp/xc-default.sock";
    for (int i = 1; i < argc; i ++) {
        QString arg_str(argv[i]);
        if (arg_str.contains(str_socket_arg_prefix)) {
            socket = arg_str.replace(str_socket_arg_prefix, "");
        }
    }
    qDebug() << "Selected RC socket: " << socket;

    ret.rc = new RemoteControl(ret, socket);
#endif

    return ret;
}

int run_xivoclient(ExecObjects exec_obj)
{
    bool should_start = exec_obj.initOK == true && exec_obj.app != NULL;
    if (should_start) {
        return exec_obj.app->exec();
    } else {
        return 1;
    }
}

void clean_xivoclient(ExecObjects exec_obj)
{
#ifdef FUNCTESTS
    delete exec_obj.rc;
    exec_obj.rc = NULL;
#endif
    delete assembler;
    assembler = NULL;
    delete exec_obj.app;
    exec_obj.app = NULL;
    delete exec_obj.event_handler;
    exec_obj.event_handler = NULL;
}

int main(int argc, char **argv)
{
    ExecObjects exec_obj= init_xivoclient(argc, argv);
    int ret = run_xivoclient(exec_obj);
    qDebug() << "Exiting";
    clean_xivoclient(exec_obj);
    return ret;
}

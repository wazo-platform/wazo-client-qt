/* XiVO Client
 * Copyright (C) 2007-2014 Avencall
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

#include <QObject>
#include <QEvent>
#include <QUrl>
#include <QFileOpenEvent>
#include <QDebug>
#include <QString>

#include <phonenumber.h>
#include "fileopeneventhandler.h"

FileOpenEventHandler::FileOpenEventHandler(PowerAwareApplication* app, QObject* parent)
    : QObject(parent)
{
    this->m_app = app;
    this->m_activate = false;
}

FileOpenEventHandler::~FileOpenEventHandler()
{
}

bool FileOpenEventHandler::eventFilter(QObject*, QEvent* event)
{
    if(event->type() == QEvent::FileOpen)
    {
        QFileOpenEvent* fileOpenEvent = static_cast<QFileOpenEvent*>(event);
        qDebug() << "received file open event " << fileOpenEvent->url().toString();

        if (!fileOpenEvent->url().isEmpty())
        {
            QString url = fileOpenEvent->url().toString();
            if(isXivoUrl(url))
            {
                qDebug() << "handling url event " << url;
                handleUrl(url);
                return true;
            }
        }
    }

    return false;

}

bool FileOpenEventHandler::isXivoUrl(const QString url)
{
    return PhoneNumber::isURI(url);
}

void FileOpenEventHandler::handleUrl(const QString url)
{
    if (this->m_activate) {
        qDebug() << "activating window";
        this->m_app->activateWindow();
    }

    QString number = PhoneNumber::extract(url);
    qDebug() << "emitting signal dialNumber " << number;
    emit dialNumber(number);
}

void FileOpenEventHandler::setActivationWindow(bool activate)
{
    this->m_activate = activate;
}

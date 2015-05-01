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

#ifndef __CTISERVER_H__
#define __CTISERVER_H__

#include <QObject>
#include <QTimer>
#include <QWebSocket>

#include "baseengine.h"
#include "connection_config.h"

class CTIServer : public QObject
{
    Q_OBJECT

    public:
        CTIServer(QWebSocket * socket);
        bool canReadLine();
        void connectToServer(ConnectionConfig config);
        void disconnectFromServer();
        QByteArray readLine();
        // bool connected();

    signals:
        void disconnected();
        void failedToConnect(const QString &, const QString &, const QString &);
        void readyRead();

    private slots:
        void ctiSocketError(QAbstractSocket::SocketError);
        void onSocketDisconnected();
        void receiveBinaryMessage(const QByteArray &message);

    private:
        void connectSocket(const QString & address,
                           unsigned port,
                           bool encrypt);
        void catchSocketError();
        void ignoreSocketError();
        void sendError(const QString & message);

    private:
        QWebSocket * m_socket;
        QByteArray m_buffer;
        QString m_last_address;
        unsigned m_last_port;
};
#endif

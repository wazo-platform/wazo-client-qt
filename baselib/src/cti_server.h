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

#ifndef __CTISERVER_H__
#define __CTISERVER_H__

#include <QSslSocket>
#include <QObject>
#include <QTimer>

#include "baseengine.h"
#include "connection_config.h"

class CTIServer : public QObject
{
    Q_OBJECT

    public:
        CTIServer(QSslSocket * socket);
        void connectToServer(const ConnectionConfig &config);
        void disconnectFromServer();
        bool connected();
        bool isConnectionEncrypted() const;
        void startTls();
        bool useStartTls() const;

    signals:
        void failedToConnect(const QString &, const QString &, const QString &);
        void disconnected();
        void disconnectedBeforeStartTls();

    private slots:
        void ctiSocketError(QAbstractSocket::SocketError);
        void onSocketDisconnected();

    private:
        bool hasSlaveConfig() const;
        void connectToMaster();
        void connectToSlave();
        void connectSocket(const QString & address, unsigned port);
        void sendError(const QString & message);
        void onConnectionError(QAbstractSocket::SocketError error);
        void onDisconnectError();

    private:
        ConnectionConfig m_config;
        QSslSocket *m_socket;
        QString m_last_address;
        unsigned m_last_port;
        bool m_use_start_tls;
        bool m_waiting_for_start_tls;
        bool m_connecting_to_master;
};
#endif

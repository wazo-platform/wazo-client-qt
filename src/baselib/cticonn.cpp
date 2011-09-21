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

#include <cticonn.h>

CtiConn::CtiConn(QTcpSocket * s)
    : QObject(NULL)
{
    connect(s, SIGNAL(disconnected()),
            this, SLOT(ctiSocketDisconnected()));
    connect(s, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(ctiSocketError(QAbstractSocket::SocketError)));
    connect(s, SIGNAL(stateChanged(QAbstractSocket::SocketState)),
            this, SLOT(ctiSocketStateChanged(QAbstractSocket::SocketState)));
}

void CtiConn::ctiSocketError(QAbstractSocket::SocketError socketError)
{
    qDebug() << Q_FUNC_INFO << socketError;
    switch (socketError) {
        // ~ once connected
        case QAbstractSocket::RemoteHostClosedError:
            ctiSocketClosedByRemote();
            break;

        // ~ when trying to connect
        case QAbstractSocket::ConnectionRefusedError:
            b_engine->popupError("socket_error_connectionrefused");
            break;
        case QAbstractSocket::HostNotFoundError:
            b_engine->popupError("socket_error_hostnotfound");
            break;
        case QAbstractSocket::NetworkError:
            b_engine->popupError("socket_error_network");
            break;
        case QAbstractSocket::SocketTimeoutError:
            b_engine->popupError("socket_error_timeout");
            break;
        case QAbstractSocket::SslHandshakeFailedError:
            b_engine->popupError("socket_error_sslhandshake");
            break;
        case QAbstractSocket::UnknownSocketError:
            b_engine->popupError("socket_error_unknown");
            break;

        default:
            // see http://doc.trolltech.com/4.6/qabstractsocket.html#SocketError-enum
            // for unmanaged error cases
            b_engine->popupError(QString("socket_error_unmanagedyet:%1").arg(socketError));
            break;
    }
}

void CtiConn::ctiSocketClosedByRemote()
{
    qDebug() << Q_FUNC_INFO;
    b_engine->emitMessage(tr("Connection lost with XiVO CTI server"));
    b_engine->startTryAgainTimer();
    b_engine->popupError("socket_error_remotehostclosed");

    QTimer * timer = new QTimer(this);
    timer->setProperty("stopper", "connection_lost");
    timer->setSingleShot(true);
    connect(timer, SIGNAL(timeout()),
            b_engine, SLOT(stop()));
    timer->start();
}

/*! \brief called when the socket is closed, whatever reason
 */
void CtiConn::ctiSocketDisconnected()
{
    qDebug() << Q_FUNC_INFO;
}

void CtiConn::ctiSocketStateChanged(QAbstractSocket::SocketState socketState)
{
    // qDebug() << Q_FUNC_INFO << socketState;
    // QAbstractSocket::HostLookupState
    // QAbstractSocket::ConnectingState
    // QAbstractSocket::ConnectedState
    // ...
    // QAbstractSocket::ClosingState
    // QAbstractSocket::UnconnectedState
}

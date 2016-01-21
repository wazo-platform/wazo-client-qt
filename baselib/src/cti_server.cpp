/* XiVO Client
 * Copyright (C) 2007-2016 Avencall
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

#include "cti_server.h"

CTIServer::CTIServer(QSslSocket *socket)
    : QObject(NULL),
      m_config(),
      m_socket(socket),
      m_last_port(0),
      m_use_start_tls(false),
      m_waiting_for_start_tls(false),
      m_connecting_to_master(false)
{
    connect(m_socket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(ctiSocketError(QAbstractSocket::SocketError)));
}

void CTIServer::ctiSocketError(QAbstractSocket::SocketError socketError)
{
    qDebug() << Q_FUNC_INFO << socketError;

    if (socketError == QAbstractSocket::RemoteHostClosedError) {
        this->onDisconnectError();
    } else {
        this->onConnectionError(socketError);
    }
}

void CTIServer::onDisconnectError()
{
    if (this->m_waiting_for_start_tls) {
        this->m_waiting_for_start_tls = false;
        emit disconnectedBeforeStartTls();
    } else {
        onSocketDisconnected();
    }
}


void CTIServer::onConnectionError(QAbstractSocket::SocketError socketError)
{
    if (m_connecting_to_master && this->hasSlaveConfig()) {
        this->connectToSlave();
    }

    switch (socketError) {
    case QAbstractSocket::ConnectionRefusedError:
        sendError("socket_error_connectionrefused");
        break;
    case QAbstractSocket::HostNotFoundError:
        sendError("socket_error_hostnotfound");
        break;
    case QAbstractSocket::NetworkError:
        sendError("socket_error_network");
        break;
    case QAbstractSocket::SocketTimeoutError:
        sendError("socket_error_timeout");
        break;
    case QAbstractSocket::SslHandshakeFailedError:
        sendError("socket_error_sslhandshake");
        break;
    case QAbstractSocket::UnknownSocketError:
        sendError("socket_error_unknown");
        break;
    default:
        sendError(QString("socket_error_unmanagedyet:%1").arg(socketError));
        break;
    }
}

void CTIServer::sendError(const QString & message)
{
    emit failedToConnect(message, m_last_address, QString::number(m_last_port));
}

/*! \brief called when the socket is closed, whatever reason
 */
void CTIServer::onSocketDisconnected()
{
    emit disconnected();
    this->sendError("socket_error_remotehostclosed");
}

void CTIServer::connectToServer(const ConnectionConfig &config)
{
    m_config = config;
    connectToMaster();
}

void CTIServer::connectToMaster()
{
    this->m_use_start_tls = m_config.main_encrypt;
    this->m_connecting_to_master = true;
    this->connectSocket(m_config.main_address,
                        m_config.main_port);
}

void CTIServer::connectToSlave()
{
    this->m_use_start_tls = m_config.backup_encrypt;
    this->m_connecting_to_master = false;
    this->connectSocket(m_config.backup_address,
                        m_config.backup_port);
}

bool CTIServer::hasSlaveConfig() const
{
    return m_config.backup_address.isEmpty() == false;
}

void CTIServer::disconnectFromServer() {
    m_socket->flush();
    disconnect(m_socket, SIGNAL(disconnected()),
               this, SLOT(onSocketDisconnected()));
    m_socket->disconnectFromHost();
    if (m_socket->isEncrypted() && m_socket->state() != QAbstractSocket::UnconnectedState)
      m_socket->waitForDisconnected();
    connect(m_socket, SIGNAL(disconnected()),
            this, SLOT(onSocketDisconnected()));
}

void CTIServer::connectSocket(const QString & address, unsigned port)
{
    m_last_address = address;
    m_last_port = port;
    m_socket->abort();
    m_socket->connectToHost(address, port);
    if (this->m_use_start_tls) {
        m_waiting_for_start_tls = true;
    }
}

bool CTIServer::connected()
{
    return m_socket->state() == QAbstractSocket::ConnectedState;
}


void CTIServer::startTls()
{
    qDebug() << "Received a STARTTLS. Starting encryption:" << m_use_start_tls;
    this->m_waiting_for_start_tls = false;
    if (this->m_use_start_tls) {
        m_socket->startClientEncryption();
    }
}

bool CTIServer::useStartTls() const
{
    return this->m_use_start_tls;
}


bool CTIServer::isConnectionEncrypted() const
{
    return m_socket->isEncrypted();
}

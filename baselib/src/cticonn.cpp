#include <cticonn.h>

CtiConn::CtiConn(QTcpSocket *s)
    : QObject(NULL)
{
    connect(s, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(ctiSocketError(QAbstractSocket::SocketError)));
    connect(s, SIGNAL(disconnected()),
            this, SLOT(ctiSocketDisconnected()));
}

void CtiConn::ctiSocketError(QAbstractSocket::SocketError socketError)
{
    qDebug() << Q_FUNC_INFO << socketError;
    switch(socketError) {
        case QAbstractSocket::ConnectionRefusedError:
            b_engine->emitTextMessage(tr("Connection refused"));
            break;
        case QAbstractSocket::RemoteHostClosedError:
            b_engine->stopKeepAliveTimer();
            b_engine->popupError("connection_closed");
            break;
        case QAbstractSocket::HostNotFoundError:
            b_engine->emitTextMessage(tr("Host not found"));
            break;
        case QAbstractSocket::NetworkError:
            b_engine->popupError("network_error");
            break;
        case QAbstractSocket::UnknownSocketError:
        default:
            b_engine->popupError("socket_error:" + QString::number(socketError));
            break;
    }
}

/*! \brief called when the socket is disconnected from the server
 */
void CtiConn::ctiSocketDisconnected()
{
    b_engine->setState(BaseEngine::ENotLogged);
    b_engine->emitTextMessage(tr("Connection lost with XiVO CTI server"));
    b_engine->startTryAgainTimer();
}

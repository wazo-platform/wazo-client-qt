#ifndef __CTICONN_H__
#define __CTICONN_H__

#include <QTcpSocket>
#include <QObject>
#include <QTimer>
#include <baseengine.h>

class CtiConn : public QObject
{
    Q_OBJECT

    public:
        CtiConn(QTcpSocket *s);
    private slots:
        void ctiSocketError(QAbstractSocket::SocketError socketError);
        void ctiSocketDisconnected();
};
#endif

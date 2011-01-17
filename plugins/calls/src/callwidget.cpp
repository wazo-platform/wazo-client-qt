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

#include <QHBoxLayout>
#include <QGridLayout>
#include <QApplication>
#include <QLabel>
#include <QMouseEvent>
#include <QDebug>
#include <QFont>
#include <QTextFormat>
#include <QMenu>

#include "calls.h"
#include "callwidget.h"
#include "userinfo.h"
#include "xivoconsts.h"
#include "taintedpixmap.h"

/*! \brief Constructor
 *
 * set up the widget, start timer.
 */
CallWidget::CallWidget(UserInfo *ui, const QString &channelme,
                       const QString &status, uint ts,
                       const QString &channelpeer, const QString & callerid,
                       const QString &calleridname, QWidget *parent,
                       const PhoneInfo *_pi)
    : QWidget(parent), m_square(16,16), m_parkedCall(false)
{
    // qDebug() << Q_FUNC_INFO << channelme;
    m_ui = ui;
    pi = _pi;
    QGridLayout * gridlayout = new QGridLayout(this);

    m_channelme = channelme;
    m_channelpeer = channelpeer;

    gridlayout->setColumnStretch(3, 1);
    m_lbl_status = new QLabel(this);
    gridlayout->addWidget(m_lbl_status, 0, 0);

    m_lbl_time = new QLabel(this);
    m_lbl_time->setFont(QFont("", 8, QFont::Bold));
    m_startTime = QDateTime::fromTime_t(ts);
    startTimer(1000);
    gridlayout->addWidget(m_lbl_time, 1, 0, 1, 3);

    m_lbl_direction = new QLabel(this);
    gridlayout->addWidget(m_lbl_direction, 0, 1);

    m_lbl_exten = new QLabel(this);
    m_lbl_exten->setFont(QFont("courier", 10, QFont::Light));
    gridlayout->addWidget(m_lbl_exten, 0, 2);

    //updateWidget(status, ts, "cpeer", callerid, calleridname);
    updateWidget(status, ts, channelpeer, callerid, calleridname, pi);

    m_hangUpAction = new QAction(tr("&Hangup"), this);
    m_hangUpAction->setStatusTip(tr("Hang up/Close the channel"));
    connect(m_hangUpAction, SIGNAL(triggered()),
             this, SLOT(hangUp()));

    m_transferToNumberAction = new QAction(tr("&Transfer to number"), this);
    m_transferToNumberAction->setStatusTip(tr("Transfer the channel to the dialed number") );
    connect(m_transferToNumberAction, SIGNAL(triggered()),
             this, SLOT(transferToNumber()));

    m_parkCall = new QAction(tr("&Park the call"), this);
    m_parkCall->setStatusTip(tr("Park this call") );
    connect(m_parkCall, SIGNAL(triggered()),
             this, SLOT(parkCall()));
}

/*! \brief update time displayed in m_lbl_time
 */
void CallWidget::updateCallTimeLabel()
{
    int time = m_startTime.secsTo(QDateTime::currentDateTime());
    m_lbl_time->setText("[" + QString::number(time/60) + " min "
                        + QString::number(time%60) + " s]" );
}

/*! \brief timer event
 *
 * update the time displayed.
 */
void CallWidget::timerEvent(QTimerEvent * /*event*/)
{
    updateCallTimeLabel();
}

/*! \brief update displayed stuff
 */
void CallWidget::updateWidget(const QString & status,
                              uint ts,
                              const QString & channelpeer,
                              const QString & callerid,
                              const QString & calleridname,
                              const PhoneInfo * _pi
                              )
{
    pi = _pi;
    qDebug() << Q_FUNC_INFO << status << ts << channelpeer << callerid << calleridname;
    m_parkedCall = (callerid == QString("<parked>")) || (calleridname == QString("<parked>"));
    setActionPixmap(status);
    m_channelpeer = channelpeer;
    //qDebug() << time << m_startTime << m_startTime.secsTo(QDateTime::currentDateTime());
    //m_startTime = QDateTime::currentDateTime().addSecs(-time);
    m_startTime = QDateTime::fromTime_t(ts);
    updateCallTimeLabel();
    if ((status == CHAN_STATUS_CALLING) || (status == CHAN_STATUS_LINKED_CALLER))
        m_lbl_direction->setPixmap(QPixmap(":/in_calls/rightarrow.png"));
    else if ((status == CHAN_STATUS_RINGING) || (status == CHAN_STATUS_LINKED_CALLED))
        m_lbl_direction->setPixmap(QPixmap(":/in_calls/leftarrow.png"));
    else
        qDebug() << Q_FUNC_INFO << "status unknown" << status;

    QString text = tr("Unknown");
    if(calleridname == "<meetme>")
        text = tr("Conference room number %1").arg(callerid);
    else if(calleridname == "<parked>")
        text = tr("Parked call in %1").arg(callerid);
    else if(callerid == "<parked>")
        text = tr("Parked call");
    else if(calleridname != "<unknown>" && !calleridname.isEmpty() && calleridname != callerid)
        text = tr("%1 : %2").arg(callerid).arg(calleridname);
    else if(!callerid.isEmpty())
        text = callerid;
    m_lbl_exten->setText(text);
}

/*! \brief set icon depending on status
 */
void CallWidget::setActionPixmap(const QString &)
{
    QColor color = QColor(pi->hintstatus("color"));
    m_lbl_status->setPixmap(TaintedPixmap(QString(":/images/phone-trans.png"),
                                          color).getPixmap());
}

/*! \brief mouse press event
 *
 * store mouse position for drag&drop.
 */
void CallWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_dragstartpos = event->pos();
    }
}

/*! \brief mouse move event
 *
 * start drag if left button pressed and if the
 * mouse has been moved enough.
 */
void CallWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (!(event->buttons() & Qt::LeftButton))
        return;
    if ((event->pos() - m_dragstartpos).manhattanLength()
        < QApplication::startDragDistance())
        return;

    qDebug() << Q_FUNC_INFO << "starting DRAG" << m_channelme ;

    QDrag *drag = new QDrag(this);
    QMimeData *mimeData = new QMimeData();
    mimeData->setText(m_channelpeer);
    mimeData->setData(USERID_MIMETYPE, m_ui->userid().toAscii());
    mimeData->setData(CHANNEL_MIMETYPE, m_channelpeer.toAscii());
    drag->setMimeData(mimeData);

    Qt::DropAction dropAction = drag->start(Qt::CopyAction | Qt::MoveAction);
    qDebug() << "dropAction =" << dropAction;
}

/*! \brief hang up the channel
 */
void CallWidget::hangUp()
{
    qDebug() << Q_FUNC_INFO << m_channelme;
    doHangUp( m_channelme );
}

/*! \brief transfers the channel to a number
 */
void CallWidget::transferToNumber()
{
    qDebug() << Q_FUNC_INFO << m_channelpeer;
    doTransferToNumber( m_channelpeer);
}

/*! \brief transfers the channel to a number
 */
void CallWidget::parkCall()
{
    qDebug() << Q_FUNC_INFO << m_channelme;
    doParkCall( m_channelme );
}

/*! \brief open the context menu
 */
void CallWidget::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu contextMenu;
    contextMenu.addAction(m_hangUpAction);
    // m_transferToNumberAction only if there is something written
    if(!m_parkedCall) {
        contextMenu.addAction(m_transferToNumberAction);
        contextMenu.addAction(m_parkCall);
    }
    contextMenu.exec(event->globalPos());
}

/*! \brief return m_channelme
 */
const QString& CallWidget::channel() const
{
    return m_channelme;
}

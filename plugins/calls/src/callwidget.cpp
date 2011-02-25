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
CallWidget::CallWidget(UserInfo * ui, const QString & channel,
                       uint ts, QWidget *parent)
    : QWidget(parent), m_square(16,16), m_parkedCall(false)
{
    qDebug() << Q_FUNC_INFO << channel;

    m_ui = ui;
    gridlayout = new QGridLayout(this);

    m_channel = channel;

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
    updateWidget(channel, ts);

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
void CallWidget::updateWidget(const QString & xchannel, uint ts)
{
    qDebug() << Q_FUNC_INFO << xchannel;
    const ChannelInfo * channelinfo = b_engine->channels().value(xchannel);
    if(channelinfo == NULL)
        return;
    QString status = channelinfo->status();
    m_parkedCall = channelinfo->isparked();
    setActionPixmap(status);
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
    if(channelinfo->talkingto_kind() == "<meetme>")
        text = tr("Conference room number %1").arg(channelinfo->talkingto_id());
    else if(channelinfo->isparked())
        text = tr("Parked call in %1").arg("767");
    else
        text = channelinfo->peerdisplay();
    m_lbl_exten->setText(text);
}

/*! \brief set icon depending on status
 */
void CallWidget::setActionPixmap(const QString &)
{
    QString scolor = "white"; // XXXX function of pi->hintstatus();
    QColor color = QColor(scolor);
    TaintedPixmap tp = TaintedPixmap(QString(":/images/phone-trans.png"), color);
    m_lbl_status->setPixmap(tp.getPixmap());
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

    qDebug() << Q_FUNC_INFO << "starting DRAG" << m_channel ;

    QDrag *drag = new QDrag(this);
    QMimeData *mimeData = new QMimeData();
    mimeData->setText(m_channel); // XXX was m_channelpeer --- why ??
    mimeData->setData(XUSERID_MIMETYPE, m_ui->xuserid().toAscii());
    mimeData->setData(CHANNEL_MIMETYPE, m_channel.toAscii()); // XXX was m_channelpeer --- what for ??
    drag->setMimeData(mimeData);

    Qt::DropAction dropAction = drag->start(Qt::CopyAction | Qt::MoveAction);
    qDebug() << "dropAction =" << dropAction;
}

/*! \brief hang up the channel
 */
void CallWidget::hangUp()
{
    qDebug() << Q_FUNC_INFO << m_channel;
    doHangUp( m_channel );
}

/*! \brief transfers the channel to a number
 */
void CallWidget::transferToNumber()
{
    doTransferToNumber(m_channel);
}

/*! \brief transfers the channel to a number
 */
void CallWidget::parkCall()
{
    qDebug() << Q_FUNC_INFO << m_channel;
    doParkCall( m_channel );
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

/*! \brief return m_channel
 */
const QString& CallWidget::channel() const
{
    return m_channel;
}

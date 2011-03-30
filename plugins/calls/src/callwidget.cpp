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
CallWidget::CallWidget(UserInfo * ui,
                       const QString & xchannel,
                       QWidget *parent)
    : QWidget(parent), m_parkedCall(false), m_square(16,16)
{
    qDebug() << Q_FUNC_INFO << xchannel;

    m_ui = ui;
    m_gridlayout = new QGridLayout(this);

    m_xchannel = xchannel;

    m_gridlayout->setColumnStretch(3, 1);
    m_lbl_status = new QLabel(this);
    m_gridlayout->addWidget(m_lbl_status, 0, 0);

    m_lbl_time = new QLabel(this);
    m_lbl_time->setFont(QFont("", 8, QFont::Bold));
    startTimer(1000);
    m_gridlayout->addWidget(m_lbl_time, 1, 0, 1, 3);

    m_lbl_direction = new QLabel(this);
    m_gridlayout->addWidget(m_lbl_direction, 0, 1);

    m_lbl_exten = new QLabel(this);
    m_lbl_exten->setFont(QFont("courier", 10, QFont::Light));
    m_gridlayout->addWidget(m_lbl_exten, 0, 2);

    //updateWidget(status, ts, "cpeer", callerid, calleridname);
    updateWidget(xchannel);

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
    const ChannelInfo * channelinfo = b_engine->channels().value(m_xchannel);
    if (channelinfo == NULL)
        return;
    m_lbl_time->setText(b_engine->timeElapsed(channelinfo->timestamp()));
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
void CallWidget::updateWidget(const QString & xchannel)
{
    const ChannelInfo * channelinfo = b_engine->channels().value(xchannel);
    if (channelinfo == NULL)
        return;

    QString status = channelinfo->commstatus();
    QString direction = channelinfo->direction();
    m_parkedCall = channelinfo->isparked();

    setActionPixmap();
    updateCallTimeLabel();

    if (direction == "out")
        m_lbl_direction->setPixmap(QPixmap(":/in_calls/rightarrow.png"));
    else if (direction == "in")
        m_lbl_direction->setPixmap(QPixmap(":/in_calls/leftarrow.png"));
    else
        qDebug() << Q_FUNC_INFO << "unknown direction" << direction;

    QString text = tr("Unknown");
    if (channelinfo->talkingto_kind() == "meetme")
        text = tr("Conference room number %1").arg(channelinfo->talkingto_id());
    else if (channelinfo->isparked())
        text = tr("Parked call in %1").arg("767");
    else
        text = channelinfo->peerdisplay();
    m_lbl_exten->setText(text);
}

/*! \brief set icon depending on status
 */
void CallWidget::setActionPixmap()
{
    const ChannelInfo * channelinfo = b_engine->channels().value(m_xchannel);
    if (channelinfo == NULL)
        return;
    QString status = channelinfo->commstatus();
    QString color;
    QString tooltip;
    qDebug() << status;
    if (b_engine->getOptionsChannelStatus().contains(status)) {
        color = b_engine->getOptionsChannelStatus().value(status).toMap().value("color").toString();
        tooltip = b_engine->getOptionsChannelStatus().value(status).toMap().value("longname").toString();
    } else {
        color = "white";
        tooltip = "unknown status";
    }
    TaintedPixmap tp = TaintedPixmap(QString(":/images/phone-trans.png"), QColor(color));
    m_lbl_status->setPixmap(tp.getPixmap());
    setToolTip(tooltip);
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

    qDebug() << Q_FUNC_INFO << "starting DRAG" << m_xchannel ;

    QDrag *drag = new QDrag(this);
    QMimeData *mimeData = new QMimeData();
    mimeData->setText(m_xchannel); // XXX was m_channelpeer --- why ??
    mimeData->setData(XUSERID_MIMETYPE, m_ui->xid().toAscii());
    mimeData->setData(CHANNEL_MIMETYPE, m_xchannel.toAscii()); // XXX was m_channelpeer --- what for ??
    drag->setMimeData(mimeData);

    Qt::DropAction dropAction = drag->start(Qt::CopyAction | Qt::MoveAction);
    qDebug() << "dropAction =" << dropAction;
}

/*! \brief hang up the channel
 */
void CallWidget::hangUp()
{
    qDebug() << Q_FUNC_INFO << m_xchannel;
    doHangUp( m_xchannel );
}

/*! \brief transfers the channel to a number
 */
void CallWidget::transferToNumber()
{
    doTransferToNumber(m_xchannel);
}

/*! \brief transfers the channel to a number
 */
void CallWidget::parkCall()
{
    qDebug() << Q_FUNC_INFO << m_xchannel;
    doParkCall( m_xchannel );
}

/*! \brief open the context menu
 */
void CallWidget::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu contextMenu;
    contextMenu.addAction(m_hangUpAction);
    // m_transferToNumberAction only if there is something written
    if (! m_parkedCall) {
        contextMenu.addAction(m_transferToNumberAction);
        contextMenu.addAction(m_parkCall);
    }
    contextMenu.exec(event->globalPos());
}

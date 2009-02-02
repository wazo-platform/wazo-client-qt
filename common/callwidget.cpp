/* XIVO CTI clients
 * Copyright (C) 2007-2009  Proformatique
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License version 2 for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Linking the Licensed Program statically or dynamically with other
 * modules is making a combined work based on the Licensed Program. Thus,
 * the terms and conditions of the GNU General Public License version 2
 * cover the whole combination.
 *
 * In addition, as a special exception, the copyright holders of the
 * Licensed Program give you permission to combine the Licensed Program
 * with free software programs or libraries that are released under the
 * GNU Library General Public License version 2.0 or GNU Lesser General
 * Public License version 2.1 or any later version of the GNU Lesser
 * General Public License, and with code included in the standard release
 * of OpenSSL under a version of the OpenSSL license (with original SSLeay
 * license) which is identical to the one that was published in year 2003,
 * or modified versions of such code, with unchanged license. You may copy
 * and distribute such a system following the terms of the GNU GPL
 * version 2 for the Licensed Program and the licenses of the other code
 * concerned, provided that you include the source code of that other code
 * when and as the GNU GPL version 2 requires distribution of source code.
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

#include "callstackwidget.h"
#include "callwidget.h"
#include "userinfo.h"
#include "xivoconsts.h"

/*! \brief Constructor
 *
 * set up the widget, start timer.
 */
CallWidget::CallWidget(UserInfo * ui,
                       const QString & uidref,
                       const QString & channelme,
		       const QString & status,
		       int time,
		       const QString &/* channelpeer*/,
		       const QString & exten,
                       QWidget * parent)
        : QWidget(parent), m_square(16,16),
          m_call_yellow(":/images/phone-yellow.png"),
          m_call_blue  (":/images/phone-blue.png"),
          m_call_red   (":/images/phone-red.png"),
          m_call_gray  (":/images/phone-grey.png")
{
        // qDebug() << "CallWidget::CallWidget()" << channelme;
        m_ui = ui;
        m_uidref = uidref;
	QGridLayout * gridlayout = new QGridLayout(this);
        
// 	m_callerid = callerid;
// 	m_calleridname = calleridname;
 	m_channelme = channelme;

//	qDebug() << "spacing" << gridlayout->spacing()
//	         << ", margin" << gridlayout->margin();
//	gridlayout->setSpacing(0);
	//gridlayout->setMargin(0);

	gridlayout->setColumnStretch(3, 1);
	m_lbl_status = new QLabel(this);
	gridlayout->addWidget(m_lbl_status, 0, 0);
        
	m_lbl_time = new QLabel(this);
	m_lbl_time->setFont(QFont("", 8, QFont::Bold));
	m_startTime = QDateTime::currentDateTime().addSecs(-time);
	startTimer(1000);
	gridlayout->addWidget(m_lbl_time, 1, 0);
        
	m_lbl_direction = new QLabel(this);
	gridlayout->addWidget(m_lbl_direction, 0, 1);
        
	// 	m_lbl_channelpeer = new QLabel(channelpeer, this);
	// 	gridlayout->addWidget(m_lbl_channelpeer, 0, Qt::AlignLeft );
        
	m_lbl_exten = new QLabel(this);
	m_lbl_exten->setFont(QFont("courier", 10, QFont::Light));
	gridlayout->addWidget(m_lbl_exten, 0, 2);
        
        updateWidget(status, time, "cpeer", exten);
        
	// for caller id information
	QLabel * dummy = new QLabel("", this);
	dummy->setFont(QFont("times", 10, QFont::Light, true));
	gridlayout->addWidget(dummy, 1, 2);
	//setAcceptDrops(true);
	
	m_hangUpAction = new QAction( tr("&Hangup"), this);
	m_hangUpAction->setStatusTip( tr("Hang up/Close the channel") );
	connect( m_hangUpAction, SIGNAL(triggered()),
	         this, SLOT(hangUp()) );
        
	m_transferToNumberAction = new QAction( tr("&Transfer to number"), this);
	m_transferToNumberAction->setStatusTip( tr("Transfer the channel to the dialed number") );
	connect( m_transferToNumberAction, SIGNAL(triggered()),
	         this, SLOT(transferToNumber()) );
        
	m_parkCall = new QAction( tr("&Park the call"), this);
	m_parkCall->setStatusTip( tr("Park this call") );
	connect( m_parkCall, SIGNAL(triggered()),
	         this, SLOT(parkCall()) );
}

/*! \brief Destructor
 */
CallWidget::~CallWidget()
{
        // qDebug() << "CallWidget::~CallWidget()";
}

/*! \brief update time displayed in m_lbl_time
 */
void CallWidget::updateCallTimeLabel()
{
	int time = m_startTime.secsTo(QDateTime::currentDateTime());
	m_lbl_time->setText( "[" + QString::number(time/60) + " min "
	                    + QString::number(time%60) + " s]" );
}

/*! \brief timer event
 *
 * update the time displayed.
 */
void CallWidget::timerEvent(QTimerEvent */* event*/)
{
	// event->timerId();
	updateCallTimeLabel();
}

/*! \brief update displayed stuff
 */
void CallWidget::updateWidget(const QString & status,
			      int time,
			      const QString &/* channelpeer*/,
			      const QString & exten)
{
        qDebug() << "CallWidget::updateWidget()" << status << time << exten;
	//m_lbl_status->setText(status);
	setActionPixmap(status);
	//qDebug() << time << m_startTime << m_startTime.secsTo(QDateTime::currentDateTime());
	m_startTime = QDateTime::currentDateTime().addSecs(-time);
	updateCallTimeLabel();
        if ((status == CHAN_STATUS_CALLING) || (status == CHAN_STATUS_LINKED_CALLER))
                m_lbl_direction->setPixmap(QPixmap(":/images/rightarrow.png"));
        else if ((status == CHAN_STATUS_RINGING) || (status == CHAN_STATUS_LINKED_CALLED))
                m_lbl_direction->setPixmap(QPixmap(":/images/leftarrow.png"));
        else
                qDebug() << "CallWidget::updateWidget() : status unknown" << status;
        
	//	m_lbl_channelpeer->setText(channelpeer);
	m_lbl_exten->setText(exten);
}

/*! \brief set icon depending on status
 */
void CallWidget::setActionPixmap(const QString & status)
{
	if (status == CHAN_STATUS_CALLING)
		m_lbl_status->setPixmap( m_call_yellow );
	else if (status == CHAN_STATUS_RINGING)
		m_lbl_status->setPixmap( m_call_blue );
	else if ((status == "On the phone") || (status == "Up"))
		m_lbl_status->setPixmap( m_call_red );
	else if (status == CHAN_STATUS_LINKED_CALLER)
		m_lbl_status->setPixmap( m_call_red );
	else if (status == CHAN_STATUS_LINKED_CALLED)
		m_lbl_status->setPixmap( m_call_red );
	else {
		m_lbl_status->setPixmap( m_call_gray );
		qDebug() << "CallWidget::setActionPixmap() : status unknown" << status;
	}
}

/*! \brief mouse press event
 *
 * store mouse position for drag&drop.
 */
void CallWidget::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton) {
		m_dragstartpos = event->pos();
/*
		if(m_lbl_channelpeer)
		{
			qDebug() << "I'm selecting this one for future use :" << m_channelme
			         << m_lbl_channelpeer->text();
		}
*/
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

	qDebug() << "CallWidget::mouseMoveEvent() starting DRAG" << m_channelme ;

	QDrag *drag = new QDrag(this);
	QMimeData *mimeData = new QMimeData();
	mimeData->setText(m_channelme); // XXX
        mimeData->setData("userid", m_ui->userid().toAscii());
	mimeData->setData("channel", m_channelme.toAscii());
	mimeData->setData(CHANNEL_MIMETYPE, m_channelme.toAscii());
	drag->setMimeData(mimeData);

	Qt::DropAction dropAction = drag->start(Qt::CopyAction | Qt::MoveAction);
	qDebug() << "dropAction =" << dropAction;
}

/*
void CallWidget::dragEnterEvent(QDragEnterEvent *event)
{
	qDebug() << "dragEnterEvent()";
// 	qDebug() << event->mimeData()->formats();
// 	if(event->mimeData()->hasText())
// 	{
// 		if(event->proposedAction() & (Qt::CopyAction|Qt::MoveAction))
// 			event->acceptProposedAction();
// 	}
}
*/

#if 0
void CallWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
	qDebug() << "mouseDoubleClickEvent" << event;
	if (event->button() == Qt::RightButton) {
// 		qDebug() << this << this->parentWidget();
// 		qDebug() << "I want to Hangup" << m_channelme << m_lbl_channelpeer->text();
		CallStackWidget * csw = (CallStackWidget *) this->parentWidget();
		csw->hupchan(m_channelme);
	}
}
#endif

/*! \brief hang up the channel
 */
void CallWidget::hangUp()
{
	qDebug() << "CallWidget::hangUp()" << m_channelme;
	doHangUp( m_channelme );
}

/*! \brief transfers the channel to a number
 */
void CallWidget::transferToNumber()
{
        qDebug() << "CallWidget::transferToNumber()" << m_channelme;
        doTransferToNumber( m_channelme );
}

/*! \brief transfers the channel to a number
 */
void CallWidget::parkCall()
{
        qDebug() << "CallWidget::parkCall()" << m_channelme;
        doParkCall( m_channelme );
}

/*! \brief open the context menu
 */
void CallWidget::contextMenuEvent(QContextMenuEvent *event)
{
        m_contextMenu = new QMenu(this);
	m_contextMenu->addAction(m_hangUpAction);
        // m_transferToNumberAction only if there is something written
	m_contextMenu->addAction(m_transferToNumberAction);
	m_contextMenu->addAction(m_parkCall);
	m_contextMenu->exec(event->globalPos());
}

/*
void CallWidget::setCallerId(const QString & callerid)
{
	m_callerid = callerid;
}

void CallWidget::setCallerIdName(const QString & calleridname)
{
	m_calleridname = calleridname;
}

void CallWidget::setChannel(const QString & channel)
{
	m_channel = channel;
}
*/

/*! \brief return m_channelme
 */
const QString & CallWidget::channel() const
{
	return m_channelme;
}

const QString & CallWidget::uidref() const
{
	return m_uidref;
}

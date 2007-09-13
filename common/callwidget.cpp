/* XIVO switchboard
Copyright (C) 2007  Proformatique

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
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
#include "xivoconsts.h"

/*! \brief Constructor
 *
 * set up the widget, start timer.
 */
CallWidget::CallWidget(const QString & channelme,
		       const QString & action,
		       int time,
		       const QString & direction,
		       const QString & channelpeer,
		       const QString & exten,
                       QWidget * parent)
        : QWidget(parent), m_square(16,16),
          m_call_yellow(":/images/phone-yellow.png"),
          m_call_blue  (":/images/phone-blue.png"),
          m_call_red   (":/images/phone-red.png"),
          m_call_gray  (":/images/phone-grey.png")
{
	QGridLayout * gridlayout = new QGridLayout(this);
        
// 	m_callerid = callerid;
// 	m_calleridname = calleridname;
 	m_channelme = channelme;

//	qDebug() << "spacing" << gridlayout->spacing()
//	         << ", margin" << gridlayout->margin();
//	gridlayout->setSpacing(0);
	//gridlayout->setMargin(0);

	gridlayout->setColumnStretch(3, 1);
	m_lbl_action = new QLabel(this);
	gridlayout->addWidget(m_lbl_action, 0, 0);
	setActionPixmap(action);

	m_lbl_time = new QLabel(this);
	m_lbl_time->setFont(QFont("", 8, QFont::Bold));
	m_startTime = QDateTime::currentDateTime().addSecs(-time);
	startTimer(1000);
	updateCallTimeLabel();
	gridlayout->addWidget(m_lbl_time, 1, 0);

	m_lbl_direction = new QLabel(this);
        if(direction == ">")
                m_lbl_direction->setPixmap(QPixmap(":/images/rightarrow.png"));
        else
                m_lbl_direction->setPixmap(QPixmap(":/images/leftarrow.png"));
	gridlayout->addWidget(m_lbl_direction, 0, 1);

	// 	m_lbl_channelpeer = new QLabel(channelpeer, this);
	// 	gridlayout->addWidget(m_lbl_channelpeer, 0, Qt::AlignLeft );

	m_lbl_exten = new QLabel(exten, this);
	m_lbl_exten->setFont(QFont("courier", 10, QFont::Light));
	gridlayout->addWidget(m_lbl_exten, 0, 2);

	// for caller id information
	QLabel * dummy = new QLabel("", this);
	dummy->setFont(QFont("times", 10, QFont::Light, TRUE));
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
}

/*! \brief Destructor
 */
CallWidget::~CallWidget()
{
        qDebug() << "CallWidget::~CallWidget()";
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
void CallWidget::timerEvent( QTimerEvent * event )
{
	// event->timerId();
	updateCallTimeLabel();
}

/*! \brief update displayed stuff
 */
void CallWidget::updateWidget(const QString & action,
			      int time,
			      const QString & direction,
			      const QString & channelpeer,
			      const QString & exten)
{
	//	qDebug() << this << "updateWidget";
	//m_lbl_action->setText(action);
	setActionPixmap(action);
	//qDebug() << time << m_startTime << m_startTime.secsTo(QDateTime::currentDateTime());
	m_startTime = QDateTime::currentDateTime().addSecs(-time);
	updateCallTimeLabel();
        if(direction == ">")
                m_lbl_direction->setPixmap(QPixmap(":rightarrow.png"));
        else
                m_lbl_direction->setPixmap(QPixmap(":leftarrow.png"));

	//	m_lbl_channelpeer->setText(channelpeer);
	m_lbl_exten->setText(exten);
}

/*! \brief set icon depending on status
 */
void CallWidget::setActionPixmap(const QString & action)
{
#if 0
	if(action == QString("Calling"))
		m_square.fill( Qt::yellow );
	else if(action == QString("Ringing"))
		m_square.fill( Qt::cyan );
	else if(action == QString("On the phone"))
		m_square.fill( Qt::red );
	else
		m_square.fill( Qt::gray );
	m_lbl_action->setPixmap( m_square );
#endif
	if(action == "Calling")
		m_lbl_action->setPixmap( m_call_yellow );
	else if(action == "Ringing")
		m_lbl_action->setPixmap( m_call_blue );
	else if(action == "On the phone")
		m_lbl_action->setPixmap( m_call_red );
	else {
		m_lbl_action->setPixmap( m_call_gray );
		qDebug() << " *** WARNING *** action unknown for call" << action;
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
	mimeData->setText(/*"test"*/ m_channelme);
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
	//qDebug() << "CallWidget::hangUp()";
	doHangUp( m_channelme );
}

/*! \brief transfers the channel to a number
 */
void CallWidget::transferToNumber()
{
        //qDebug() << "CallWidget::transferToNumber()" << m_channelme;
        doTransferToNumber( m_channelme );
}

/*! \brief open the context menu
 */
void CallWidget::contextMenuEvent(QContextMenuEvent *event)
{
        m_contextMenu = new QMenu(this);
	m_contextMenu->addAction(m_hangUpAction);
        // m_transferToNumberAction only if there is something written
	m_contextMenu->addAction(m_transferToNumberAction);
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


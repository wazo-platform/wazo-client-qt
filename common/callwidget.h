/* XIVO CTI clients
Copyright (C) 2007, 2008  Proformatique

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

#ifndef __CALLWIDGET_H__
#define __CALLWIDGET_H__

// QT includes.
#include <QDateTime>
#include <QMenu>
#include <QObject>
#include <QWidget>

class QLabel;

/*! \brief Widget displaying a call (channel)
 *
 * The Call is displayed with a colored square representing
 * its state, the id of the channel, the direction and
 * the source of the call
 */
class CallWidget : public QWidget
{
	Q_OBJECT
public:
	//! Default constructor
	CallWidget(QWidget * parent = 0);
        ~CallWidget();

	//CallWidget(const QString & tomonitor,
	//	   QWidget * parent = 0);
	CallWidget(const QString & channelme,
		   const QString & action,
		   int time,
		   const QString & direction,
		   const QString & channelpeer,
		   const QString & exten,
		   QWidget * parent = 0);
	void updateWidget(const QString & action,
			  int time,
			  const QString & direction,
			  const QString & channelpeer,
			  const QString & exten);
	//void setChannel(const QString &);
	const QString & channel() const;
	//void setCallerId(const QString &);
	//const QString & callerId() const;
	//void setCallerIdName(const QString &);
	//const QString & callerIdName() const;
protected:
	void mousePressEvent(QMouseEvent *);
	void mouseMoveEvent(QMouseEvent *);
	//void mouseDoubleClickEvent(QMouseEvent * event);
	//void dragEnterEvent(QDragEnterEvent * event);
	//	void dragMoveEvent(QDragMoveEvent * event);
	//	void dropEvent(QDropEvent * event);
	void timerEvent(QTimerEvent *);
	void contextMenuEvent(QContextMenuEvent *);
private:
	void setActionPixmap(const QString &);
	void updateCallTimeLabel();
signals:
	void doHangUp(const QString &);	//!< hang up the channel
	void doTransferToNumber(const QString &);	//!< transfers the channel
	void doParkCall(const QString &);	//!< parks the channel
public slots:
	void hangUp();
	void transferToNumber();
	void parkCall();
private:
	QPoint m_dragstartpos;	//!< used for drag
	QString m_channelme;	//!< channel identifier
	QString m_callerid;		//!< caller id
	QString m_calleridname;	//!< caller id name
	QLabel * m_lbl_action;	//!< sub widget
	QLabel * m_lbl_time;	//!< sub widget
	QLabel * m_lbl_direction;	//!< sub widget
	QLabel * m_lbl_channelpeer;	//!< sub widget
	QLabel * m_lbl_exten;	//!< sub widget
	QPixmap m_square;		//!< QPixmap used to display the action square
	QPixmap m_call_yellow;	//!< yellow phone icon
	QPixmap m_call_blue;	//!< blue phone icon
	QPixmap m_call_red;		//!< red phone icon
	QPixmap m_call_gray;	//!< gray phone icon
	QDateTime m_startTime;	//!< call start date/time
	QAction * m_hangUpAction;	//!< Hang Up Action
        QAction * m_transferToNumberAction;	//!< Transfer to Number Action
        QAction * m_parkCall;	//!< Park the Call Action
        QMenu * m_contextMenu;
};

#endif

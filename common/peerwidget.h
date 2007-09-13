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

/* $Id$ */
#ifndef __PEERWIDGET_H__
#define __PEERWIDGET_H__
#include <QWidget>
#include <QPixmap>
#include <QPoint>

class QLabel;

class BaseEngine;
class PeerChannel;

/*! \brief Widget to display a Peer status
 *
 * Display Icons for the state and the peer name.
 */
class PeerWidget : public QWidget
{
	Q_OBJECT
public:
	PeerWidget(const QString & id, const QString & name,
	           QWidget * parent = 0/*, int size = 16*/);
	~PeerWidget();
	void clearChanList();
	void addChannel(const QString &, const QString &, const QString &);
	void setName(const QString &);
	void setEngine(BaseEngine *);
protected:
	void mouseMoveEvent(QMouseEvent * event);
	void mousePressEvent(QMouseEvent * event);
//	void mouseDoubleClickEvent(QMouseEvent * event);
	void dragEnterEvent(QDragEnterEvent * event);
	void dragMoveEvent(QDragMoveEvent * event);
	void dropEvent(QDropEvent * event);
	void contextMenuEvent(QContextMenuEvent *);
signals:
	//! originate signal
	void originateCall(const QString &, const QString &);
	//! transfer signal
	void transferCall(const QString &, const QString &);
	//! intercept signal
	void interceptChan(const QString &);
	//! hang up signal
	void hangUpChan(const QString &);
	//! dial/call signal
	void emitDial(const QString &);
	//! hide the widget in the channel
	void doRemoveFromPanel(const QString &);
public slots:
	//! set phone or person icon in blue
	void setBlue(int n);
	//void setCyan(int n);
	//! set phone or person icon in gray
	void setGray(int n);
	//! set phone or person icon in green
	void setGreen(int n);
	//! set phone or person icon in orange
	void setOrange(int n);
	//! set phone or person icon in red
	void setRed(int n);
	//! set phone or person icon in yellow
	void setYellow(int n);
	//void setBlack(int n);
	//void setDarkGreen(int n);
	void updateMyCalls(const QStringList &, const QStringList &, const QStringList &);
private slots:
	void transferChan(const QString &);
	void removeFromPanel();
	void dial();
private:
	BaseEngine * m_engine;  //!< Base Engine reference
	QLabel * m_statelbl;	//!< Peer state display (ringing, online, ...)
	QLabel * m_availlbl;	//!< Peer state display from XIVO CTI Client
	QLabel * m_voicelbl;
	QLabel * m_fwdlbl;
	QLabel * m_textlbl;		//!< text label : to display peer name
	//QPixmap m_square;		//!< pixmap used to display the states
	QPoint m_dragstartpos;	//!< drag start position
	QString m_id;	//!< peer id : asterisk/protocol/extension
	QString m_name;	//!< caller id to display : usualy the NAME of the person

	QAction * m_removeAction;	//!< action to remove this peer from the window
	QAction * m_dialAction;		//!< action to dial this number
	
	QList<PeerChannel *> m_channels;	//!< channels associated with THIS peer
	QList<PeerChannel *> m_mychannels;	//!< channels assiciated with ME

	/* TODO : have the Pixmaps as static objects */
	QPixmap m_phone_green;	//!< green phone icon
	QPixmap m_phone_red;	//!< red phone icon
	QPixmap m_phone_orange;	//!< orange phone icon
	QPixmap m_phone_gray;	//!< gray phone icon
	QPixmap m_phone_yellow;	//!< yellow phone icon
	QPixmap m_phone_blue;	//!< blue phone icon

	QPixmap m_person_green;	//!< green person icon
	QPixmap m_person_red;	//!< red person icon
	QPixmap m_person_orange;	//!< orange person icon
	QPixmap m_person_gray;	//!< gray person icon
	QPixmap m_person_yellow;	//!< yellow person icon
	QPixmap m_person_blue;	//!< blue person icon
};

#endif


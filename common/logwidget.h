/* XIVO CTI clients
Copyright (C) 2007, 2008  Proformatique

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
version 2 as published by the Free Software Foundation.

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

#ifndef __LOGWIDGET_H__
#define __LOGWIDGET_H__

#include <QWidget>
#include "logeltwidget.h"

class QVBoxLayout;
class BaseEngine;
class QRadioButton;

/*! \brief Call Log display widget
 */
class LogWidget : public QWidget
{
	Q_OBJECT
public:
	LogWidget(BaseEngine * engine, QWidget * parent = 0);
	void addElement(const QString & peer, LogEltWidget::Direction d,
	                const QDateTime & dt, int duration);
protected:
	void timerEvent(QTimerEvent *);
private:
	int mode();
public slots:
	void clear();
	void addLogEntry(const QDateTime & dt, int, const QString &, int);
	void setPeerToDisplay(const QString &);
private slots:
	void modeChanged(bool);
signals:
	void askHistory(const QString &, int);	//!< need history to be updated !
private:
	BaseEngine * m_engine;	//!< BaseEngine object
	QVBoxLayout * m_layout;			//!< Widget layout
	QString m_peer;					//!< "monitored" peer
	int m_timer;					//!< timer id for refresh
	QRadioButton * m_radioNone;		//!< "None" radio button
	QRadioButton * m_radioOut;		//!< "Out" radio button
	QRadioButton * m_radioIn;		//!< "In" radio button
	QRadioButton * m_radioMissed;	//!< "Missed" radio button
};

#endif


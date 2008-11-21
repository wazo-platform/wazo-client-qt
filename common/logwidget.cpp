/* XIVO CTI clients
 * Copyright (C) 2007, 2008  Proformatique
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

#include <QDebug>
#include <QHBoxLayout>
#include <QLabel>
#include <QLayoutItem>
#include <QGroupBox>
#include <QRadioButton>
#include <QScrollArea>
#include <QTabWidget>
#include <QVBoxLayout>

#include "baseengine.h"
#include "logeltwidget.h"
#include "logwidget.h"
#include "userinfo.h"

/*! \brief Constructor
 *
 * build layout and child widgets.
 */
LogWidget::LogWidget(BaseEngine * engine, const QVariant &, QWidget * parent)
	: QWidget(parent), m_engine(engine), m_timer(-1)
{
	QVBoxLayout * layout = new QVBoxLayout(this);
	layout->setMargin(0);
	layout->setSpacing(0);

	QGroupBox * groupBox = new QGroupBox( this );
	groupBox->setAlignment( Qt::AlignHCenter );
	QHBoxLayout * vbox = new QHBoxLayout( groupBox );
	vbox->setMargin(0);
	vbox->setSpacing(0);

        vbox->addStretch( 1 );
	m_radioNone = new QRadioButton( " ", groupBox );
        m_radioNone->setIcon(QIcon(":/images/cancel.png"));
	m_radioNone->setToolTip( tr("No calls") );
	connect( m_radioNone, SIGNAL(toggled(bool)),
	         this, SLOT(modeChanged(bool)) );
	vbox->addWidget( m_radioNone );

	m_radioOut = new QRadioButton( " ", groupBox );
        m_radioOut->setIcon(QIcon(":/images/green_up.png"));
	m_radioOut->setToolTip( tr("Outgoing calls") );
	connect( m_radioOut, SIGNAL(toggled(bool)),
	         this, SLOT(modeChanged(bool)) );
	vbox->addWidget( m_radioOut );

	m_radioIn = new QRadioButton( " ", groupBox );
        m_radioIn->setIcon(QIcon(":/images/green_down.png"));
	m_radioIn->setToolTip( tr("Incoming calls") );
	connect( m_radioIn, SIGNAL(toggled(bool)),
	         this, SLOT(modeChanged(bool)) );
	vbox->addWidget( m_radioIn );

	m_radioMissed = new QRadioButton( " ", groupBox );
        m_radioMissed->setIcon(QIcon(":/images/red_down.png"));
	m_radioMissed->setToolTip( tr("Missed calls") );
	connect( m_radioMissed, SIGNAL(toggled(bool)),
	         this, SLOT(modeChanged(bool)) );
	vbox->addWidget( m_radioMissed );
        vbox->addStretch( 1 );

	m_radioNone->setChecked( true );

	layout->addWidget( groupBox );
	QScrollArea * scrollArea = new QScrollArea( this );
	scrollArea->setWidgetResizable( true );

	QWidget * widget = new QWidget( this );
	scrollArea->setWidget( widget );
        widget->setObjectName("scroller"); // in order for the style settings to be set accordingly

	m_layout = new QVBoxLayout( widget );
	m_layout->setMargin(0);
	m_layout->setSpacing(0);
	m_layout->addStretch(0);
	layout->addWidget( scrollArea );
}

void LogWidget::setGuiOptions(const QVariant &)
{
}

/*! \brief add an entry
 *
 * \sa addLogEntry
 */
void LogWidget::addElement(const QString & peer, LogEltWidget::Direction d,
                           const QDateTime & dt, int duration, const QString & termin)
{
	//qDebug() << "LogWidget::addElement()" << peer << d << dt << duration;
	int i, index = 0;
	for(i = 0; i < m_layout->count(); i++) {
		QWidget * widget = m_layout->itemAt( i )->widget();
		if(widget) {
			LogEltWidget * logelt = qobject_cast<LogEltWidget *>(widget);
			if(logelt) {
				if((dt == logelt->dateTime())
				   && (peer == logelt->peer())
				   && (d == logelt->direction()))
					return;
				else if(dt > logelt->dateTime())
					break;
				index = i + 1;
			}
		}
	}
	LogEltWidget * logelt = new LogEltWidget(peer, d, dt, duration, termin, this);
        connect( logelt, SIGNAL(actionCall(const QString &, const QString &)),
                 this, SLOT(proxyCallRequests(const QString &, const QString &)) );
        connect( logelt, SIGNAL(copyNumber(const QString &)),
                 m_engine, SLOT(copyNumber(const QString &)) );
	m_layout->insertWidget(index, logelt);
}

/*! \brief remove all child widgets
 */
void LogWidget::clear()
{
	QLayoutItem * child;
	while ((child = m_layout->itemAt(0)) != 0)
	{
		if(child->widget())
		{
			m_layout->removeItem(child);
			delete child->widget();
			delete child;
		}
		else
			break;
	}
	//m_layout->addStretch(1);
}

void LogWidget::setUserInfo(const UserInfo * ui)
{
        m_userinfo_owner = ui;
}

void LogWidget::proxyCallRequests(const QString & src, const QString & dst)
{
        actionCall(sender()->property("action").toString(), src, dst); // Call
}

/*! \brief add an entry
 */
void LogWidget::addLogEntry(const QDateTime & dt, int duration,
                            const QString & peer, const QString & direction, const QString & termin)
{
        LogEltWidget::Direction d;
        d = (direction == "IN") ? LogEltWidget::InCall : LogEltWidget::OutCall;
	// TODO: manage the list !
	addElement(peer, d, dt, duration, termin);
}

/*! \brief change the monitored peer
 */
void LogWidget::monitorPeer(UserInfo * ui)
{
        // qDebug() << "LogWidget::monitorPeer()" << ui->userid();
	clear();
	m_peer = ui->userid();
	if(m_peer.size() > 0) {
		askHistory(m_peer, mode());
		if(m_timer < 0)
			m_timer = startTimer(10000);
	}
}

/*! \brief timer event : ask for update
 */
void LogWidget::timerEvent(QTimerEvent *)
{
        // qDebug() << "LogWidget::timerEvent() id=" << event->timerId();
	if(m_peer.size() > 0)
		askHistory(m_peer, mode());
}

/*! \brief return the mode (out/in or missed)
 */
int LogWidget::mode()
{
	int r = -1;
	if(m_radioOut->isChecked())
		r = 0;
	else if(m_radioIn->isChecked())
		r = 1;
	else if(m_radioMissed->isChecked())
		r = 2;
	return r;
}

/*! \brief triggered when mode is changed.
 *
 * clear the list and ask an update.
 */
void LogWidget::modeChanged(bool b)
{
        // qDebug() << "LogWidget::modeChanged()" << b << mode();
	if(b && m_peer.size() > 0) {
		clear();
		askHistory(m_peer, mode());
	}
}

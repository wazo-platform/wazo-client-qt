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

#ifndef __CALLSTACKWIDGET_H__
#define __CALLSTACKWIDGET_H__

#include <QObject>
#include <QList>
#include <QWidget>
#include <QString>
#include <QDateTime>

#include "callwidget.h"

class QVBoxLayout;

class UserInfo;

/*! \brief Object storing call parametters
 */
class Call// : public QObject
{
public:
	//Peer( const QString & ext, QObject * parent = 0 );
	Call(const QString &);
	Call(UserInfo *,
             const QString &, const QString &, int,
             const QString &, const QString &,
             const QString &);
	Call(const Call &);
	const QString & getUserId() const;	//! get m_phonen
	const QString & getChannelMe() const;	//! get m_channelme
	const QString & getAction() const;	//! get m_action
        
	//! get duration of the channel
	int getTime() const {
		return m_startTime.secsTo(QDateTime::currentDateTime());
	};
	//! get m_direction
	const QString & getDirection() const {return m_direction;};
	//! get m_channelpeer
	const QString & getChannelPeer() const {return m_channelpeer;};
	//! get m_exten
	const QString & getExten() const {return m_exten;};
	void updateCall(const QString &,
			int,
			const QString &,
			const QString &,
                        const QString &);
private:
        UserInfo * m_ui;
	QString m_channelme;	//!< "my" channel 
	QString m_action;		//!< action
	QDateTime m_startTime;	//!< channel start time
	QString m_direction;	//!< chan direction
	QString m_channelpeer;	//!< linked channel
	QString m_exten;		//!< extension
};

/*! \brief Widget displaying the current open channels for a phone line.
 */
class CallStackWidget : public QWidget
{
	Q_OBJECT
public:
	CallStackWidget(QWidget *);	//!< Constructor
public slots:
	//! Add a call to the list to be displayed
        void updatePeer(UserInfo *,
                        const QString &,
                        const QVariant &);
        void addCall(UserInfo *, const QVariant &);
        //	void showCalls(const QString & tomonitor, const QString & callerid);
	void updateDisplay();
	void hupchan(const QString &);
	void transftonumberchan(const QString &);
	void parkcall(const QString &);
	void reset();
	void monitorPeer(UserInfo *);
protected:
	void dragEnterEvent(QDragEnterEvent *event);
	void dropEvent(QDropEvent *event);
private:
	void emptyList();	//!< remove all calls from the list
signals:
	void hangUp(const QString &);			//!< hang up a channel
        void transferToNumber(const QString &);	//!< transfers a channel to a number
        void parkCall(const QString &);	//!< parks a channel
	void changeTitle(const QString &);		//!< change Title
	void monitorPeerRequest(const QString &);	//!< send the userid of the new monitored peer
private:
	QVBoxLayout * m_layout;	//!< Vertical Layout used
	QList<Call> m_calllist;	//!< list of Call Objects
	QList<CallWidget *> m_afflist;	//!< List of CallWidget Widgets
	QString m_monitored_userid;	//!< Peer monitored
};

#endif

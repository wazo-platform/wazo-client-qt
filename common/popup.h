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

#ifndef __POPUP_H__
#define __POPUP_H__

#include <QHash>
#include <QIODevice>
#include <QVBoxLayout>
#include <QTcpSocket>
#include <QVariant>
#include <QWidget>
#include "xmlhandler.h"
#ifdef USE_OUTLOOK
#include "outlook_contact.h"
#endif

class QLabel;
class QLineEdit;
class QPushButton;
class QUrl;
class QXmlInputSource;
class QXmlSimpleReader;

class UserInfo;

/*! \brief Profile popup widget
 *
 * Constructed from the message received */
class Popup: public QWidget
{
	Q_OBJECT
public:
	//! Construct from a QIODevice used to read XML input
	Popup(const bool &,
              const UserInfo *,
              QWidget * parent = 0);
        void feed(QIODevice *, const bool &);
        void addInfoInternal(const QString &, const QString &);
	//! Add a Text field (name, value)
	void addInfoText(int, const QString &, const QString &);
	//! Add a url field
	void addInfoLink(int, const QString &, const QString &);
	void addInfoLinkX(int, const QString &, const QString &, const QString &);
        void addInfoLinkAuto(const QString &, const QString &);
	//! Add a Picture
	void addInfoPicture(int, const QString &, const QString &);
	//! Add a Phone number
	void addInfoPhone(int, const QString &, const QString &);
	//! Add a Phone number
	void addInfoPhoneURL(int, const QString &, const QString &);
	//! getter for the message
	void setMessage(const QString &, const QString &);
	//! access to the message
	const QHash<QString, QString> & message() const;
	//! getter for the message
	void setMessageTitle(const QString &);
	//! access to the message
	const QString & messagetitle() const;
	//! finalize the Construction of the window and show it
	void finishAndShow();
        void setSheetPopup(const bool &);
	bool sheetpopup();
	const QString & sessionid() const;
	const QString & channel() const;
	bool systraypopup();
	bool focus();
        void setTitle(const QString &);
        
        void addAnyInfo(const QString &,
                        const QString &,
                        const QString &,
                        const QString &,
                        const QString &);
        void addDefForm(const QString &, const QString &);
        void update(QList<QStringList> &);
        QList<QStringList> & sheetlines();
signals:
	void wantsToBeShown(Popup *);	//!< sent when the widget want to show itself
	void actionCall(const QString &,
                        const QString &,
                        const QString &);	//!< sent when the widget wants to dial
        void actionFromPopup(const QString &, const QVariant &);
        void save(const QString &);
public slots:
	void streamNewData();		//!< new input data is available
	void streamAboutToClose();	//!< catch aboutToClose() signal from the socket
	void socketDisconnected();	//!< connected to disconnected() signal
	void socketError(QAbstractSocket::SocketError err);	//!< socket error handling
        void dialThisNumber();
        void dispurl(const QUrl &);
        void httpGetNoreply();
        void actionFromForm();
protected:
	void closeEvent(QCloseEvent *);	//!< catch close event
private:
        void addInfoForm(int, const QString &);
        void saveandclose();
        
	QIODevice * m_inputstream;	//!< input stream where the XML is read from
	/* the following properties are for XML parsing */
	//! QXmlInputSource constructed from m_inputstream
	QXmlInputSource * m_xmlInputSource;
	//! XML parser object.
	QXmlSimpleReader m_reader;
        const UserInfo * m_ui;
	//! Handler for event generated by the XML parser
	XmlHandler * m_handler;
	bool m_parsingStarted;		//! Is the XML already started or not ?
	//! layout for the widget : vertical box
	QVBoxLayout * m_vlayout;
	QHash<QString, QString> m_message;	//! Message property
	QString m_messagetitle;	//! Message title
	QLabel * m_title;	//! Sheet Title
        QString m_sessionid;
        QString m_channel;
        QString m_called;
        bool m_sheetpopup;
        bool m_systraypopup;
        bool m_focus;
        bool m_urlautoallow;
        bool m_toupdate;
        bool m_sheetui;
        QWidget * m_sheetui_widget;
        QStringList m_orders;
        QList<QStringList> m_sheetlines;
        QHash<QString, QPushButton *> m_form_buttons;
        QHash<QString, QString> m_remoteforms;
        QVariantMap m_timestamps;
#ifdef USE_OUTLOOK
	COLContact m_OLContact;
	bool	m_bOLFound;
#endif
};

#endif

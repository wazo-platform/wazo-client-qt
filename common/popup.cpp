/* XIVO CTI clients
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

#include <QDebug>
#include <QDesktopServices>
#include <QIcon>
#include <QLabel>
#include <QPixmap>
#include <QPushButton>
#include <QUrl>
#include <QVariant>

#include "popup.h"
#include "xmlhandler.h"
#include "remotepicwidget.h"
#include "urllabel.h"

/*!
 * This constructor init all XML objects and connect signals
 * to slots.
 * \param inputstream	inputstream to read the XML
 * \param sessionid		sessionid to check incoming connection to
 * \param parent		parent widget
 */
Popup::Popup(QIODevice *inputstream, const QString & sessionid, QWidget *parent)
        : QWidget(parent), m_inputstream(inputstream),
          m_xmlInputSource(inputstream), m_handler(this),
          m_sessionid(sessionid)
{
	qDebug() << "Popup(" << inputstream << ")";
	setAttribute(Qt::WA_DeleteOnClose);
	m_reader.setContentHandler(&m_handler);
	m_reader.setErrorHandler(&m_handler);
	connect( inputstream, SIGNAL(readyRead()),
	         this, SLOT(streamNewData()) );
	connect( inputstream, SIGNAL(aboutToClose()),
	         this, SLOT(streamAboutToClose()) );
	connect( inputstream, SIGNAL(disconnected()),
	         this, SLOT(socketDisconnected()) );
	connect( inputstream, SIGNAL(error(QAbstractSocket::SocketError)),
	         this, SLOT(socketError(QAbstractSocket::SocketError)));
	m_parsingStarted = false;
	m_vlayout = new QVBoxLayout(this);
	QLabel * title = new QLabel(tr("Incoming call"), this);
	title->setAlignment(Qt::AlignHCenter);
	m_vlayout->addWidget(title);
	bool display_buttons = true;
	if(display_buttons)
	{
		QHBoxLayout * hlayout = new QHBoxLayout();
		QPushButton * btn1 = new QPushButton(tr("&Answer"), this);
		btn1->setEnabled(false);
		hlayout->addWidget(btn1);
		QPushButton * btn2 = new QPushButton(tr("Di&smiss"), this);
		btn2->setEnabled(false);
		hlayout->addWidget(btn2);
		QPushButton * btn3 = new QPushButton(tr("&Close"), this);
		connect( btn3, SIGNAL(clicked()), this, SLOT(close()) );
		hlayout->addWidget(btn3);

		m_vlayout->addLayout(hlayout);
	}
	setWindowIcon(QIcon(":/images/xivoicon.png"));
        QDesktopServices::setUrlHandler(QString("dial"), this, "dispurl");
}

void Popup::dispurl(const QUrl &url)
{
        // qDebug() << "Popup::dispurl()" << url;
        QString numbertodial = url.toString().mid(5);
        emitDial(numbertodial);
}

void Popup::addInfoText(const QString & name, const QString & value)
{
	QLabel * lblname = new QLabel(name, this);
	QLabel * lblvalue = new QLabel(value, this);
	lblvalue->setTextInteractionFlags( Qt::TextSelectableByMouse
	                                  | Qt::TextSelectableByKeyboard );
	QHBoxLayout * hlayout = new QHBoxLayout();
	hlayout->addWidget(lblname);
	hlayout->addWidget(lblvalue);
	m_vlayout->addLayout(hlayout);
}

void Popup::addInfoPhone(const QString & name, const QString & value)
{
        qDebug() << "Popup::addInfoPhone()" << value;
	QLabel * lblname = new QLabel(name, this);
        QPushButton * lblvalue = new QPushButton(value, this);
        lblvalue->setObjectName("phonenumber");
        lblvalue->setProperty("number", value);
        connect( lblvalue, SIGNAL(clicked()),
                 this, SLOT(dialThisNumber()) );
        QHBoxLayout * hlayout = new QHBoxLayout();
        hlayout->addWidget(lblname);
        hlayout->addWidget(lblvalue);
        m_vlayout->addLayout(hlayout);
}

void Popup::addInfoPhoneURL(const QString & name, const QString & value)
{
        qDebug() << "Popup::addInfoPhoneURL()" << value;
	QLabel * lblname = new QLabel(name, this);
	UrlLabel * lblvalue = new UrlLabel("dial:" + value, this);
	QHBoxLayout * hlayout = new QHBoxLayout();
	hlayout->addWidget(lblname);
	hlayout->addWidget(lblvalue);
	m_vlayout->addLayout(hlayout);
}

void Popup::addInfoLink(const QString & name, const QString & value)
{
	QLabel * lblname = new QLabel(name, this);
	UrlLabel * lblvalue = new UrlLabel(value, this);
	QHBoxLayout * hlayout = new QHBoxLayout();
	hlayout->addWidget(lblname);
	hlayout->addWidget(lblvalue);
	m_vlayout->addLayout(hlayout);
}

void Popup::addInfoPicture(const QString & name, const QString & value)
{
	QUrl url(value);
        qDebug() << "Popup::addInfoPicture()" << value << url.scheme();
	//QUrl url = QUrl::fromEncoded(value);
	// TODO: faire un widget special qui bouffe des Images HTTP ?
	if((url.scheme() != QString("http")) && (url.scheme() != QString("https")))
	{
		QLabel *lbl = new QLabel( name, this );
		QPixmap *face = new QPixmap( value );
		// TODO: connect a signal to close() ?
		lbl->setPixmap( *face );
		m_vlayout->addWidget( lbl, 0, Qt::AlignCenter );
	}
	else
	{
		RemotePicWidget * pic = new RemotePicWidget( name, value, this );
		m_vlayout->addWidget( pic, 0, Qt::AlignCenter );
	}
}

// === SLOTS ===
void Popup::streamNewData()
{
	bool b = false;
	qDebug() << "Popup::streamNewData()";
	qDebug() << m_inputstream->bytesAvailable() << "bytes available";
	if(m_parsingStarted)
	{
		b = m_reader.parseContinue();
	}
	else
	{
		b = m_reader.parse(&m_xmlInputSource, true);
		m_parsingStarted = b;
	}
	qDebug() << "parse returned" << b;
}

void Popup::dialThisNumber()
{
        QString numbertodial = this->sender()->property("number").toString();
        qDebug() << "Popup::dialThisNumber()" << numbertodial;
        emitDial(numbertodial);
}

void Popup::streamAboutToClose()
{
	qDebug() << "Popup::streamAboutToClose()";
	finishAndShow();
}

void Popup::socketDisconnected()
{
	qDebug() << "Popup::socketDisconnected()";
	/* finish the parsing */
	m_reader.parseContinue();
}

void Popup::socketError(QAbstractSocket::SocketError err)
{
	qDebug() << "Popup::socketError()" << err;
}

/*!
 * Send signal to be shown !
 */
void Popup::finishAndShow()
{
	qDebug() << "Popup::finishAndShow()";
	//dumpObjectInfo();
	//dumpObjectTree();
	// ...
	//show();
	wantsToBeShown( this );
}

void Popup::closeEvent(QCloseEvent * event)
{
	qDebug() << "Popup::closeEvent(" << event << ")";
}

void Popup::setMessage(const QString & message)
{
	m_message = message;
}

const QString & Popup::message() const
{
	return m_message;
}

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

#include <QComboBox>
#include <QDateTime>
#include <QDebug>
#include <QDesktopServices>
#include <QHttp>
#include <QIcon>
#include <QLabel>
#include <QLineEdit>
#include <QPixmap>
#include <QPushButton>
#include <QUiLoader>
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
Popup::Popup(QIODevice * inputstream,
             const bool & sheetui,
             QWidget * parent)
        : QWidget(parent),
          m_inputstream(inputstream),
          m_xmlInputSource(inputstream),
          m_handler(this),
          m_sheetui(sheetui)
{
	QDateTime currentDateTime = QDateTime::currentDateTime();
	QString currentDateTimeStr = currentDateTime.toString(Qt::LocalDate);
	qDebug() << "Popup::Popup()" << inputstream;

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

        QUiLoader loader;
        if(sheetui) {
                m_sheetui_widget = loader.load(inputstream, this);
        } else {
                QFile file(":/common/defaultform.ui");
                file.open(QFile::ReadOnly);
                m_sheetui_widget = loader.load(&file, this);
                file.close();
        }

        m_vlayout->addWidget(m_sheetui_widget, 0, 0);

        QPushButton * closebutton = m_sheetui_widget->findChild<QPushButton *>("close");
        QPushButton * savebutton  = m_sheetui_widget->findChild<QPushButton *>("save");
        if(closebutton)
                connect( closebutton, SIGNAL(clicked()), this, SLOT(close()) );
        if(savebutton)
                connect( savebutton, SIGNAL(clicked()), this, SLOT(saveandclose()) );

        QLineEdit   * datetime    = m_sheetui_widget->findChild<QLineEdit *>("datetime");
        QLineEdit   * year        = m_sheetui_widget->findChild<QLineEdit *>("year");
        if(datetime)
                datetime->setText(currentDateTimeStr);
        if(year)
                year->setText(currentDateTime.toString("yyyy"));

	setWindowIcon(QIcon(":/images/xivoicon.png"));
        QDesktopServices::setUrlHandler(QString("dial"), this, "dispurl");
}

void Popup::dispurl(const QUrl &url)
{
        // qDebug() << "Popup::dispurl()" << url;
        QString numbertodial = url.toString().mid(5);
        emitDial(numbertodial);
}

void Popup::saveandclose()
{
        qDebug() << "Popup::saveandclose()";
        QStringList qsl;

        QList<QLineEdit *> lineedits = m_sheetui_widget->findChildren<QLineEdit *>(QRegExp("XIVOFORM-"));
        for(int i = 0; i < lineedits.count(); i++) {
                qsl.append(lineedits[i]->objectName() + ":" + lineedits[i]->text());
        }

        save(qsl.join(";"));

        close();
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

void Popup::addInfoLinkX(const QString & name, const QString & value, const QString & dispvalue)
{
        qDebug() << "Popup::addInfoLinkX()" << name << value << dispvalue;
	QLabel * lblname = new QLabel(name, this);
        QPushButton * lblvalue = new QPushButton(dispvalue, this);
        // lblvalue->setObjectName("phonenumber");
        lblvalue->setProperty("urlx", value);
        connect( lblvalue, SIGNAL(clicked()),
                 this, SLOT(httpGetNoreply()) );
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
	if((url.scheme() != QString("http")) &&
           (url.scheme() != QString("https"))) {
		QLabel *lbl = new QLabel( name, this );
		QPixmap *face = new QPixmap( value );
		// TODO: connect a signal to close() ?
		lbl->setPixmap( *face );
		m_vlayout->addWidget( lbl, 0, Qt::AlignCenter );
	} else {
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
        if(m_sheetui == false)
                if(m_parsingStarted)
                        b = m_reader.parseContinue();
                else {
                        b = m_reader.parse(&m_xmlInputSource, false);
                        m_parsingStarted = b;
                }
        else
                finishAndShow();
                
	qDebug() << "parse returned" << b;
}

void Popup::dialThisNumber()
{
        QString numbertodial = this->sender()->property("number").toString();
        qDebug() << "Popup::dialThisNumber()" << numbertodial;
        emitDial(numbertodial);
}

void Popup::httpGetNoreply()
{
        QString urlx = this->sender()->property("urlx").toString();
        qDebug() << "Popup::httpGetNoreply()" << urlx;
        QUrl url = QUrl(urlx);
        QHttp * http = new QHttp();
        http->setHost(url.host(), url.port());
        http->get(url.path());
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
        m_vlayout->addStretch();
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

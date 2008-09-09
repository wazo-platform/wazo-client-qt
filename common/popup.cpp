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
#include <QCloseEvent>
#include <QDateTime>
#include <QDebug>
#include <QDesktopServices>
#include <QFileInfo>
#include <QHttp>
#include <QIcon>
#include <QLabel>
#include <QLineEdit>
#include <QPixmap>
#include <QProcess>
#include <QPushButton>
#include <QSettings>
#include <QUiLoader>
#include <QUrl>
#include <QVariant>

#include "popup.h"
#include "xmlhandler.h"
#include "remotepicwidget.h"
#include "urllabel.h"
#include "userinfo.h"

/*!
 * This constructor inits all XML objects and connect signals
 * to slots.
 * \param inputstream	inputstream to read the XML
 * \param sessionid		sessionid to check incoming connection to
 * \param parent		parent widget
 */
Popup::Popup(const bool & urlautoallow,
             const UserInfo * ui,
             QWidget * parent)
        : QWidget(parent),
          m_ui(ui),
          m_sheetpopup(false),
          m_systraypopup(true),
          m_focus(true),
          m_urlautoallow(urlautoallow),
          m_toupdate(false)
{
        // qDebug() << "Popup::Popup()";
}

void Popup::feed(QIODevice * inputstream,
                 const bool & sheetui)
{
        m_inputstream = inputstream;
        m_xmlInputSource = new QXmlInputSource(m_inputstream);
        m_handler = new XmlHandler(this);
        m_sheetui = sheetui;
        
        // qDebug() << "Popup::feed()" << inputstream;
	QDateTime currentDateTime = QDateTime::currentDateTime();
	QString currentDateTimeStr = currentDateTime.toString(Qt::LocalDate);
	qDebug() << "Popup::feed()" << currentDateTime;
        
	setAttribute(Qt::WA_DeleteOnClose);
	m_reader.setContentHandler(m_handler);
	m_reader.setErrorHandler(m_handler);
	connect( m_inputstream, SIGNAL(readyRead()),
	         this, SLOT(streamNewData()) );
	connect( m_inputstream, SIGNAL(aboutToClose()),
	         this, SLOT(streamAboutToClose()) );
        // 	connect( inputstream, SIGNAL(disconnected()),
        // 	         this, SLOT(socketDisconnected()) );
        // 	connect( inputstream, SIGNAL(error(QAbstractSocket::SocketError)),
        // 	         this, SLOT(socketError(QAbstractSocket::SocketError)));
	m_parsingStarted = false;
	m_vlayout = new QVBoxLayout(this);
        m_vlayout->addStretch();
        m_title = new QLabel(this);
	m_title->setAlignment(Qt::AlignHCenter);
	m_vlayout->insertWidget(m_vlayout->count() - 1, m_title);

        QUiLoader loader;
        if(sheetui) {
                m_sheetui_widget = loader.load(m_inputstream, this);
        } else {
                QFile file(":/common/form_default.ui");
                file.open(QFile::ReadOnly);
                m_sheetui_widget = loader.load(&file, this);
                file.close();
        }

        m_vlayout->insertWidget(m_vlayout->count() - 1, m_sheetui_widget, 0, 0);

        m_refbutton    = m_sheetui_widget->findChild<QPushButton *>("refuse");
        m_hupbutton    = m_sheetui_widget->findChild<QPushButton *>("hangup");
        m_closebutton  = m_sheetui_widget->findChild<QPushButton *>("close");
        m_savebutton   = m_sheetui_widget->findChild<QPushButton *>("save");
        m_answerbutton = m_sheetui_widget->findChild<QPushButton *>("answer");
        QLineEdit   * datetime    = m_sheetui_widget->findChild<QLineEdit *>("datetime");
        QLineEdit   * year        = m_sheetui_widget->findChild<QLineEdit *>("year");
        
        if(m_refbutton)
                connect( m_refbutton, SIGNAL(clicked()), this, SLOT(refuse()) );
        if(m_hupbutton)
                connect( m_hupbutton, SIGNAL(clicked()), this, SLOT(hangup()) );
        if(m_closebutton)
                connect( m_closebutton, SIGNAL(clicked()), this, SLOT(close()) );
        if(m_savebutton)
                connect( m_savebutton, SIGNAL(clicked()), this, SLOT(saveandclose()) );
        if(m_answerbutton)
                connect( m_answerbutton, SIGNAL(clicked()), this, SLOT(answer()) );
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
        originateCall("user:special:me", "ext:" + numbertodial);
}

void Popup::refuse()
{
        // qDebug() << "Popup::refuse()" << m_channel;
}

void Popup::hangup()
{
        // qDebug() << "Popup::hangup()" << m_channel;
        hangUp(m_ui, m_channel);
}

void Popup::answer()
{
        // qDebug() << "Popup::answer()" << m_called;
        pickUp(m_ui, m_called);
}

void Popup::saveandclose()
{
        // qDebug() << "Popup::saveandclose()";
        QStringList qsl;

        QList<QLineEdit *> lineedits = m_sheetui_widget->findChildren<QLineEdit *>(QRegExp("XIVOFORM-"));
        for(int i = 0; i < lineedits.count(); i++) {
                qsl.append(lineedits[i]->objectName() + ":" + lineedits[i]->text());
        }

        save(qsl.join(";"));

        close();
}

void Popup::addAnyInfo(const QString & localName,
                       const QString & infoOrder,
                       const QString & infoType,
                       const QString & infoName,
                       const QString & infoValue)
{
        QStringList z  = (QStringList() << localName << infoOrder << infoType << infoName << infoValue);
        m_sheetlines.append(z);
	if( localName == "sheet_info" ) {
                setSheetPopup( true );
		if( infoType == "text" ) {
                        if(infoName == "")
                                setTitle( infoValue );
                        else
                                addInfoText( infoName, infoValue );
                } else if( infoType == "url" ) {
                        addInfoLink( infoName, infoValue );
                } else if( infoType == "picture" ) {
                        addInfoPicture( infoName, infoValue );
                } else if( infoType == "urlx" ) {
                        QStringList qsl = infoValue.split("@");
                        if(qsl.size() == 2)
                                addInfoLinkX( infoName, qsl[0], qsl[1] );
                        else
                                addInfoLinkX( infoName, infoValue, infoValue );
                } else if( infoType == "phone" ) {
                        QRegExp re_number("\\+?[0-9\\s\\.]+");
                        if(re_number.exactMatch(infoValue))
                                addInfoPhoneURL( infoName, infoValue );
                        else
                                addInfoText( infoName, infoValue );
                } else if( infoType == "form" ) {
                        addInfoForm( infoName, infoValue );
                }
                
	} else if( localName == "systray_info" ) {
                if ( infoType == "title" )
                        setMessageTitle( infoValue );
                else if ( infoType == "body" )
                        setMessage( infoOrder, infoValue );
                
	} else if( localName == "action_info" ) {
                if ( infoType == "urlauto" )
                        addInfoLinkAuto( infoName, infoValue );
                
	} else if( localName == "internal" ) {
                addInfoInternal( infoName, infoValue );
        }
}

void Popup::setTitle(const QString & title)
{
        // qDebug() << "Popup::setTitle()" << title;
        m_title->setText(title);
}

void Popup::addInfoForm(const QString & name, const QString & value)
{
        qDebug() << "Popup::addInfoForm()" << name << value;
	QLabel * lblname = new QLabel(name, this);
        QUiLoader loader;
        QFile file(value);
        file.open(QFile::ReadOnly);
        QWidget * form = loader.load(&file, this);
        file.close();
	QHBoxLayout * hlayout = new QHBoxLayout();
	hlayout->addWidget(form);
	m_vlayout->insertLayout(m_vlayout->count() - 1, hlayout);
}

void Popup::addInfoText(const QString & name, const QString & value)
{
        // qDebug() << "Popup::addInfoText()" << value;
	QLabel * lblname = new QLabel(name, this);
	QLabel * lblvalue = new QLabel(value, this);
	lblvalue->setTextInteractionFlags( Qt::TextSelectableByMouse
	                                  | Qt::TextSelectableByKeyboard );
	QHBoxLayout * hlayout = new QHBoxLayout();
	hlayout->addWidget(lblname);
	hlayout->addWidget(lblvalue);
	m_vlayout->insertLayout(m_vlayout->count() - 1, hlayout);
}

void Popup::addInfoInternal(const QString & name, const QString & value)
{
        // qDebug() << "Popup::addInfoInternal()" << value;
        if(name == "channel")
                m_channel = value;
        else if(name == "nosystraypopup")
                m_systraypopup = false;
        else if(name == "nofocus")
                m_focus = false;
        else if(name == "called")
                m_called = value;
        else if(name == "sessionid") {
                m_sessionid = value;
                setProperty("sessionid", m_sessionid);
        } else if(name == "kind") {
                if(value == "agi") {
                        if(m_hupbutton)
                                m_hupbutton->setEnabled(false);
                } else if(value == "link") {
                        if(m_refbutton)
                                m_refbutton->setEnabled(false);
                        if(m_hupbutton)
                                m_hupbutton->setEnabled(true);
                        if(m_answerbutton)
                                m_answerbutton->setEnabled(false);
                } else if(value == "unlink") {
                        if(m_hupbutton)
                                m_hupbutton->setEnabled(false);
                }
        } else
                qDebug() << "Popup::addInfoInternal() internal" << name << value;
}

void Popup::addInfoPhone(const QString & name, const QString & value)
{
        // qDebug() << "Popup::addInfoPhone()" << value;
	QLabel * lblname = new QLabel(name, this);
        QPushButton * lblvalue = new QPushButton(value, this);
        lblvalue->setObjectName("phonenumber");
        lblvalue->setProperty("number", value);
        connect( lblvalue, SIGNAL(clicked()),
                 this, SLOT(dialThisNumber()) );
        QHBoxLayout * hlayout = new QHBoxLayout();
        hlayout->addWidget(lblname);
        hlayout->addWidget(lblvalue);
        m_vlayout->insertLayout(m_vlayout->count() - 1, hlayout);
}

QList<QStringList> & Popup::sheetlines()
{
        return m_sheetlines;
}

void Popup::update(QList<QStringList> & newsheetlines)
{
        m_toupdate = true;
        QListIterator<QStringList> i(newsheetlines);
        while(i.hasNext()) {
                QStringList qsl = i.next();
                addAnyInfo(qsl[0], qsl[1], qsl[2], qsl[3], qsl[4]);
        }
}

void Popup::addInfoPhoneURL(const QString & name, const QString & value)
{
        // qDebug() << "Popup::addInfoPhoneURL()" << value;
	QLabel * lblname = new QLabel(name, this);
	UrlLabel * lblvalue = new UrlLabel("dial:" + value, this);
	QHBoxLayout * hlayout = new QHBoxLayout();
	hlayout->addWidget(lblname);
	hlayout->addWidget(lblvalue);
	m_vlayout->insertLayout(m_vlayout->count() - 1, hlayout);
}

void Popup::addInfoLink(const QString & name, const QString & value)
{
        // qDebug() << "Popup::addInfoLink()" << value;
	QLabel * lblname = new QLabel(name, this);
	UrlLabel * lblvalue = new UrlLabel(value, this);
	QHBoxLayout * hlayout = new QHBoxLayout();
	hlayout->addWidget(lblname);
	hlayout->addWidget(lblvalue);
	m_vlayout->insertLayout(m_vlayout->count() - 1, hlayout);
}

void Popup::addInfoLinkAuto(const QString &, const QString & value)
{
        // qDebug() << "Popup::addInfoLinkAuto()" << value << m_urlautoallow;
        if(m_urlautoallow) {
#ifdef Q_WS_WIN
                QSettings settings("HKEY_CLASSES_ROOT\\HTTP\\shell\\open\\command", QSettings::NativeFormat);
                QString command = settings.value(".").toString();
		QRegExp rx("\"(.+)\"");
		if (rx.indexIn(command) != -1)
			command = rx.capturedTexts()[1];
                QUrl url(value);
                QFileInfo browserFileInfo(command);
		if (browserFileInfo.fileName() == "iexplore.exe")
	                QProcess::startDetached(browserFileInfo.absoluteFilePath(), QStringList() << "-new" << url.toEncoded());
		else
			QDesktopServices::openUrl(url);
#else
                QDesktopServices::openUrl(QUrl(value));
#endif
        }
}

void Popup::addInfoLinkX(const QString & name, const QString & value, const QString & dispvalue)
{
        // qDebug() << "Popup::addInfoLinkX()" << name << value << dispvalue;
	QLabel * lblname = new QLabel(name, this);
        QPushButton * lblvalue = new QPushButton(dispvalue, this);
        // lblvalue->setObjectName("phonenumber");
        lblvalue->setProperty("urlx", value);
        connect( lblvalue, SIGNAL(clicked()),
                 this, SLOT(httpGetNoreply()) );
        QHBoxLayout * hlayout = new QHBoxLayout();
        hlayout->addWidget(lblname);
        hlayout->addWidget(lblvalue);
        m_vlayout->insertLayout(m_vlayout->count() - 1, hlayout);
}

void Popup::addInfoPicture(const QString & name, const QString & value)
{
	QUrl url(value);
        // qDebug() << "Popup::addInfoPicture()" << value << url.scheme();
	//QUrl url = QUrl::fromEncoded(value);
	// TODO: faire un widget special qui bouffe des Images HTTP ?
	if((url.scheme() != QString("http")) &&
           (url.scheme() != QString("https"))) {
		QLabel *lbl = new QLabel( name, this );
		QPixmap *face = new QPixmap( value );
		// TODO: connect a signal to close() ?
		lbl->setPixmap( *face );
		m_vlayout->insertWidget( m_vlayout->count() - 1, lbl, 0, Qt::AlignCenter );
	} else {
		RemotePicWidget * pic = new RemotePicWidget( name, value, this );
		m_vlayout->insertWidget( m_vlayout->count() - 1, pic, 0, Qt::AlignCenter );
	}
}

// === SLOTS ===
void Popup::streamNewData()
{
	bool b = false;
	// qDebug() << "Popup::streamNewData()" << m_inputstream->bytesAvailable() << "bytes available";
        if(m_sheetui == false)
                if(m_parsingStarted)
                        b = m_reader.parseContinue();
                else {
                        b = m_reader.parse(m_xmlInputSource, false);
                        m_parsingStarted = b;
                }
        else
                finishAndShow();
                
	// qDebug() << "Popup::streamNewData() parse returned" << b;
}

void Popup::dialThisNumber()
{
        QString numbertodial = this->sender()->property("number").toString();
        // qDebug() << "Popup::dialThisNumber()" << numbertodial;
        originateCall("user:special:me", "ext:" + numbertodial);
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
	//dumpObjectInfo();
	//dumpObjectTree();
	// ...
	//show();
	wantsToBeShown( this );
}


void Popup::closeEvent(QCloseEvent * event)
{
	qDebug() << "Popup::closeEvent(" << event << ")" << this->sender();
}

void Popup::setMessage(const QString & order, const QString & message)
{
	m_message[order] = message;
}

const QHash<QString, QString> & Popup::message() const
{
	return m_message;
}

void Popup::setMessageTitle(const QString & messagetitle)
{
	m_messagetitle = messagetitle;
}

const QString & Popup::messagetitle() const
{
	return m_messagetitle;
}

bool Popup::systraypopup()
{
        return m_systraypopup;
}

void Popup::setSheetPopup(const bool & sheetpopup)
{
	m_sheetpopup = sheetpopup;
}

const QString & Popup::sessionid() const
{
        return m_sessionid;
}

bool Popup::sheetpopup()
{
        return m_sheetpopup;
}

bool Popup::focus()
{
        return m_focus;
}

/* XiVO Client
 * Copyright (C) 2007-2009, Proformatique
 *
 * This file is part of XiVO Client.
 *
 * XiVO Client is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version, with a Section 7 Additional
 * Permission as follows:
 *   This notice constitutes a grant of such permission as is necessary
 *   to combine or link this software, or a modified version of it, with
 *   the OpenSSL project's "OpenSSL" library, or a derivative work of it,
 *   and to copy, modify, and distribute the resulting work. This is an
 *   extension of the special permission given by Trolltech to link the
 *   Qt code with the OpenSSL library (see
 *   <http://doc.trolltech.com/4.4/gpl.html>). The OpenSSL library is
 *   licensed under a dual license: the OpenSSL License and the original
 *   SSLeay license.
 *
 * XiVO Client is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with XiVO Client.  If not, see <http://www.gnu.org/licenses/>.
 */

/* $Revision$
 * $Date$
 */

#include <QBuffer>
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
#include <QHBoxLayout>

#include "popup.h"
#include "baseengine.h"
#include "xmlhandler.h"
#include "remotepicwidget.h"
#include "urllabel.h"
#include "userinfo.h"
#include "remarkarea.h"
#ifdef USE_OUTLOOK
#include "outlook_engine.h"
#endif

QStringList g_formbuttonnames = (QStringList()
                                 << "refuse" << "hangup" << "answer"
                                 << "close" << "save");

/*!
 * This constructor inits all XML objects and connect signals
 * to slots.
 * \param parent                parent widget
 */
Popup::Popup(const bool & urlautoallow,
             BaseEngine * engine,
             QWidget * parent)
    : QWidget(parent),
      m_xmlInputSource(0),
      m_engine(engine),
      m_handler(0),
      m_sheetpopup(false),
      m_systraypopup(true),
      m_focus(true),
      m_urlautoallow(urlautoallow),
      m_toupdate(false),
      m_firstline(3)
{
#ifdef USE_OUTLOOK
    m_bOLFound=FALSE;
#endif
    // qDebug() << "Popup::Popup()";
    m_remarkarea = 0;
}

Popup::~Popup()
{
    delete m_xmlInputSource;
    delete m_handler;
}


/*!
 * \param inputstream        inputstream to read the XML
 */
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
    //         connect( inputstream, SIGNAL(disconnected()),
    //                  this, SLOT(socketDisconnected()) );
    //         connect( inputstream, SIGNAL(error(QAbstractSocket::SocketError)),
    //                  this, SLOT(socketError(QAbstractSocket::SocketError)));
    m_parsingStarted = false;
    m_vlayout = new QVBoxLayout(this);
    m_title = new QLabel(this);
    m_title->setAlignment(Qt::AlignHCenter);
    QHBoxLayout * hlayout = new QHBoxLayout();
    m_closesheet = new QPushButton(this);
    m_closesheet->setIcon(QIcon(":/images/cancel.png"));
    m_closesheet->setIconSize(QSize(10, 10));
    connect( m_closesheet, SIGNAL(clicked()),
             this, SLOT(close()) );
    hlayout->addStretch();
    hlayout->addWidget(m_closesheet);
    m_vlayout->addLayout(hlayout);
    m_vlayout->addWidget(m_title);
    QFrame * qf = new QFrame(this);
    qf->setFrameStyle(QFrame::HLine | QFrame::Plain);
    qf->setLineWidth(1);
    m_vlayout->addWidget(qf);
    m_vlayout->addStretch();
    
    QUiLoader loader;
    if(sheetui) {
        m_sheetui_widget = loader.load(m_inputstream, this);
        // qDebug() << "m_vlayout->count()" << m_vlayout->count();
        m_vlayout->insertWidget(m_vlayout->count() - 1, m_sheetui_widget, 0, 0);
        foreach(QString formbuttonname, g_formbuttonnames) {
            m_form_buttons[formbuttonname] = m_sheetui_widget->findChild<QPushButton *>(formbuttonname);
            if(m_form_buttons[formbuttonname]) {
                m_form_buttons[formbuttonname]->setProperty("buttonname", formbuttonname);
                connect( m_form_buttons[formbuttonname], SIGNAL(clicked()),
                         this, SLOT(actionFromForm()) );
            }
        }
        setEnablesOnForms();
        QRegExp re_status("^XIVO_CALL_STATUS-");
        qDebug() << "Popup::feed() found" << m_sheetui_widget->findChildren<QPushButton *>(re_status);
        
        QLineEdit   * datetime    = m_sheetui_widget->findChild<QLineEdit *>("datetime");
        QLineEdit   * year        = m_sheetui_widget->findChild<QLineEdit *>("year");
        if(datetime)
            datetime->setText(currentDateTimeStr);
        if(year)
            year->setText(currentDateTime.toString("yyyy"));
    }
    
    setWindowIcon(QIcon(":/images/xivoicon.png"));
    QDesktopServices::setUrlHandler(QString("dial"), this, "dispurl");
    
    qDebug() << "Popup::feed()" << m_inputstream->bytesAvailable() << "bytes available";
    if(m_inputstream->bytesAvailable() > 0) {
        streamNewData();
    }
}

void Popup::dispurl(const QUrl &url)
{
    // qDebug() << "Popup::dispurl()" << url;
    // TODO : check if the string starts with "dial:"
    QString numbertodial = url.toString().mid(5);
    actionCall("originate", "user:special:me", "ext:" + numbertodial); // Call
}

void Popup::actionFromForm()
{
    QString buttonname = sender()->property("buttonname").toString();
    qDebug() << "Popup::actionFromForm()" << buttonname << m_astid << m_context << m_uniqueid << m_channel;
    if(buttonname == "close")
        close();
    else if(buttonname == "save")
        saveandclose();
    else if((buttonname == "hangup") || (buttonname == "answer") || (buttonname == "refuse"))
        actionFromPopup(buttonname, QVariant(m_timestamps));
    else if(buttonname.startsWith("XIVO_CALL_STATUS-")) {
        actionFromPopup(buttonname, QVariant(m_timestamps));
        close();
    }
}

/*! \brief saves the filled-in forms and closes the sheet
 */
void Popup::saveandclose()
{
    // qDebug() << "Popup::saveandclose()";
    QStringList qsl;

    QList<QLineEdit *> lineedits = m_sheetui_widget->findChildren<QLineEdit *>(QRegExp("^XIVOFORM-"));
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
        int where = 0;
        if(m_orders.contains(infoOrder)) {
            m_form_buttons.clear();
            // removes the layout and widgets there
            where = m_orders.indexOf(infoOrder) + m_firstline;
            QLayoutItem * qli = m_vlayout->itemAt(where);
            if(qli->layout()) {
                QLayoutItem * child;
                while ((child = qli->layout()->takeAt(0)) != 0) {
                    if(child->widget())
                        delete child->widget();
                    delete child;
                }
                delete qli->layout();
            }
            if(qli->widget())
                delete qli->widget();
            m_vlayout->removeItem(qli);
        } else {
            m_orders.append(infoOrder);
            m_orders.sort();
            where = m_orders.indexOf(infoOrder) + m_firstline;
        }
        setSheetPopup( true );
        if( infoType == "text" ) {
            if(infoName == "") {
                setTitle( infoValue );
                m_orders.removeAll(infoOrder);
            } else
                addInfoText( where, infoName, infoValue );
        } else if( infoType == "url" ) {
            addInfoLink( where, infoName, infoValue );
        } else if( infoType == "picture" ) {
            addInfoPicture( where, infoName, infoValue );
        } else if( infoType == "urlx" ) {
            QStringList qsl = infoValue.split("@");
            if(qsl.size() == 2)
                addInfoLinkX( where, infoName, qsl[0], qsl[1] );
            else
                addInfoLinkX( where, infoName, infoValue, infoValue );
        } else if( infoType == "phone" ) {
            QRegExp re_number("\\+?[0-9\\s\\.]+");
            if(re_number.exactMatch(infoValue))
                addInfoPhoneURL( where, infoName, infoValue );
            else
                addInfoText( where, infoName, infoValue );
        } else if( infoType == "form" ) {
            addInfoForm( where, infoValue );
        } else
            m_orders.removeAll(infoOrder);
                
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

/*! \brief sets the sheet's title
 */
void Popup::setTitle(const QString & title)
{
    // qDebug() << "Popup::setTitle()" << title;
    m_title->setText(title);
}

void Popup::addDefForm(const QString & name, const QString & value)
{
    // qDebug() << "Popup::addDefForm()" << name << value;
    m_remoteforms[name] = value;
}

void Popup::addInfoForm(int where, const QString & value)
{
    // qDebug() << "Popup::addInfoForm()" << where << value << m_kind;
    QUiLoader loader;
    QWidget * form;
    if(m_remoteforms.contains(value)) {
        QBuffer * inputstream = new QBuffer(this);
        inputstream->open(QIODevice::ReadWrite);
        inputstream->write(m_remoteforms[value].toUtf8());
        inputstream->close();
        form = loader.load(inputstream, this);
    } else {
        QFile file(value);
        file.open(QFile::ReadOnly);
        form = loader.load(&file, this);
        file.close();
    }
    
    foreach(QString formbuttonname, g_formbuttonnames) {
        if(! m_form_buttons[formbuttonname]) {
            m_form_buttons[formbuttonname] = form->findChild<QPushButton *>(formbuttonname);
            if(m_form_buttons[formbuttonname]) {
                m_form_buttons[formbuttonname]->setProperty("buttonname", formbuttonname);
                connect( m_form_buttons[formbuttonname], SIGNAL(clicked()),
                         this, SLOT(actionFromForm()) );
            }
        } else {
            qDebug() << "Popup::addInfoForm()" << "already ?" << formbuttonname << form->findChild<QPushButton *>(formbuttonname);
        }
    }
    QRegExp re_callstatus("^XIVO_CALL_STATUS-");
    foreach(QPushButton * callstatusbutton, form->findChildren<QPushButton *>(re_callstatus)) {
        QString formbuttonname = callstatusbutton->objectName();
        m_form_buttons[formbuttonname] = callstatusbutton;
        m_form_buttons[formbuttonname]->setProperty("buttonname", formbuttonname);
        connect( m_form_buttons[formbuttonname], SIGNAL(clicked()),
                 this, SLOT(actionFromForm()) );
    }
    setEnablesOnForms();
    m_vlayout->insertWidget(where, form);
}

void Popup::addInfoText(int where, const QString & name, const QString & value)
{
    QString strValue(value);
#ifdef USE_OUTLOOK
    // hack
    if ( m_bOLFound && (value == QString("Inconnu") || value == QString("<b>Inconnu</b>")) ) {
        if ( name == QString("Nom") ) {
            QString strLastName=m_OLContact.m_properties.value("LastName");
            if ( ! strLastName.isEmpty() )
                strValue=strLastName;
        }
        else if ( name == QString("Prénom") ) {
            QString strFirstName=m_OLContact.m_properties.value("FirstName");
            if ( ! strFirstName.isEmpty() )
                strValue=strFirstName;
        }
    }
#endif
    // qDebug() << "Popup::addInfoText()" << value;
    QLabel * lblname = new QLabel(name, this);
    QLabel * lblvalue = new QLabel(strValue, this);
    lblvalue->setTextInteractionFlags( Qt::TextSelectableByMouse
                                       | Qt::TextSelectableByKeyboard );
    QHBoxLayout * hlayout = new QHBoxLayout();
    hlayout->addWidget(lblname);
    hlayout->addWidget(lblvalue);
    m_vlayout->insertLayout(where, hlayout);
}

void Popup::addInfoInternal(const QString & name, const QString & value)
{
    // qDebug() << "Popup::addInfoInternal()" << name << value;
    if(name == "channel") {
        m_channel = value;
        setProperty("channel", m_channel);
    } else if(name == "astid") {
        m_astid = value;
        setProperty("astid", m_astid);
    } else if(name == "context") {
        m_context = value;
        setProperty("context", m_context);
    } else if(name == "uniqueid") {
        m_uniqueid = value;
        setProperty("uniqueid", m_uniqueid);
        
    } else if(name == "nosystraypopup")
        m_systraypopup = false;
    else if(name == "nofocus")
        m_focus = false;
    else if(name == "kind") {
        // the form buttons should have been defined when arriving here
        // ('kind' definition at the end of the sheet on server-side)
        m_kind = value;
        m_timestamps[value] = QDateTime::currentDateTime().toTime_t();
        setEnablesOnForms();
    } else
        qDebug() << "Popup::addInfoInternal() : undefined internal" << name << value;
}

/*! \brief disables the call-related actions for most of the sheets
 */
void Popup::setEnablesOnForms()
{
    if(m_kind == "dial") {
        if(m_form_buttons["hangup"])
            m_form_buttons["hangup"]->setEnabled(false);
    } else if(m_kind == "link") {
        if(m_form_buttons["refuse"])
            m_form_buttons["refuse"]->setEnabled(false);
        if(m_form_buttons["hangup"])
            m_form_buttons["hangup"]->setEnabled(true);
        if(m_form_buttons["answer"])
            m_form_buttons["answer"]->setEnabled(false);
    } else if(m_kind == "unlink") {
        if(m_form_buttons["hangup"])
            m_form_buttons["hangup"]->setEnabled(false);
    } else {
        if(m_form_buttons["refuse"])
            m_form_buttons["refuse"]->setEnabled(false);
        if(m_form_buttons["hangup"])
            m_form_buttons["hangup"]->setEnabled(false);
        if(m_form_buttons["answer"])
            m_form_buttons["answer"]->setEnabled(false);
    }
}

/*! \brief adds phone-like data to the sheet
 */
void Popup::addInfoPhone(int where, const QString & name, const QString & value)
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
    m_vlayout->insertLayout(where, hlayout);
}

QList<QStringList> & Popup::sheetlines()
{
    return m_sheetlines;
}

void Popup::update(QList<QStringList> & newsheetlines)
{
    qDebug() << "Popup::update()";
    m_toupdate = true;
    foreach(QStringList qsl, newsheetlines)
        addAnyInfo(qsl[0], qsl[1], qsl[2], qsl[3], qsl[4]);
}

/*! \brief adds phone-like data (as clickable URL) to the sheet
 */
void Popup::addInfoPhoneURL(int where, const QString & name, const QString & value)
{
    // qDebug() << "Popup::addInfoPhoneURL()" << value;
    QLabel * lblname = new QLabel(name, this);
    UrlLabel * lblvalue = new UrlLabel("dial:" + value, this);
    QHBoxLayout * hlayout = new QHBoxLayout();
    hlayout->addWidget(lblname);
    hlayout->addWidget(lblvalue);
    m_vlayout->insertLayout(where, hlayout);
}

void Popup::addInfoLink(int where, const QString & name, const QString & value)
{
    // qDebug() << "Popup::addInfoLink()" << value;
    QLabel * lblname = new QLabel(name, this);
    UrlLabel * lblvalue = new UrlLabel(value, this);
    QHBoxLayout * hlayout = new QHBoxLayout();
    hlayout->addWidget(lblname);
    hlayout->addWidget(lblvalue);
    m_vlayout->insertLayout(where, hlayout);
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

void Popup::addInfoLinkX(int where, const QString & name, const QString & value, const QString & dispvalue)
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
    m_vlayout->insertLayout(where, hlayout);
}

void Popup::addInfoPicture(int where, const QString & name, const QString & value)
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
        m_vlayout->insertWidget(where, lbl, 0, Qt::AlignCenter);
    } else {
        RemotePicWidget * pic = new RemotePicWidget( name, value, this );
        m_vlayout->insertWidget(where, pic, 0, Qt::AlignCenter);
    }
}

// === SLOTS ===
void Popup::streamNewData()
{
    bool b = false;
    //qDebug() << "Popup::streamNewData()" << m_inputstream->bytesAvailable() << "bytes available";
    if(!m_sheetui)
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
    QString numbertodial = sender()->property("number").toString();
    actionCall("originate", "user:special:me", "ext:" + numbertodial); // Call
}

void Popup::httpGetNoreply()
{
    QString urlx = sender()->property("urlx").toString();
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

/*
void Popup::socketError(QAbstractSocket::SocketError err)
{
    qDebug() << "Popup::socketError()" << err;
}
*/

/*!
 * Send signal to be shown !
 */
void Popup::finishAndShow()
{
    qDebug() << "Popup::finishAndShow()";
    addRemarkArea();
    //dumpObjectInfo();
    //dumpObjectTree();
    // ...
    //show();
    wantsToBeShown( this );
}


void Popup::closeEvent(QCloseEvent * event)
{
    qDebug() << "Popup::closeEvent(" << event << ")" << sender();
}

void Popup::setMessage(const QString & order, const QString & message)
{
#ifdef USE_OUTLOOK
    // hack
    QStringList message_parts = message.split(" ");
    if ( message_parts.count() > 1 ) {
        if ( (m_bOLFound=OLEngine()->find_contact_by_num(message_parts[1], m_OLContact)) ) {
            m_message[order]=m_OLContact.m_properties.value("FullName") + ' ' + message_parts[1];
            return ;
        }
    }
    m_bOLFound=FALSE;
#endif
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


const QString & Popup::callAstid() const
{
    return m_astid;
}

const QString & Popup::callContext() const
{
    return m_context;
}

const QString & Popup::callUniqueid() const
{
    return m_uniqueid;
}

const QString & Popup::callChannel() const
{
    return m_channel;
}

const QString & Popup::callKind() const
{
    return m_kind;
}


bool Popup::sheetpopup()
{
    return m_sheetpopup;
}

bool Popup::focus()
{
    return m_focus;
}

void Popup::addRemarkArea()
{
    qDebug() << "Popup::addRemarkArea()";
    m_remarkarea = new RemarkArea(this);
    m_vlayout->addWidget( m_remarkarea );
    connect( m_remarkarea, SIGNAL(textSubmitted(const QString &)),
             this, SLOT(newRemark(const QString &)) );
}

void Popup::activateRemarkArea()
{
    if(m_remarkarea)
        m_remarkarea->displayInputForm();
}

void Popup::desactivateRemarkArea()
{
    if(m_remarkarea)
        m_remarkarea->hideInputForm();
}

void Popup::newRemark(const QString & text)
{
    emit newRemarkSubmitted(id(), text);
}

void Popup::addRemark(const QVariantMap & entry)
{
    if(m_remarkarea) {
        QString type = QString("text");
        QString text = entry["text"].toString();
        if(entry.contains("type") && entry.contains("data")) {
            type = entry["type"].toString();
            if(type == QString("text"))
                text = entry["data"].toString();
            // TODO handle other types of data
            else
                text = type + " : " + entry["data"].toString();
        }
        QDateTime datetime;
        datetime.setTime_t( entry["time"].toUInt() );
        QString user = entry["user"].toString();
        if(m_engine) {
            UserInfo * ui = m_engine->users()[user];
            if(ui)
                user = ui->fullname();
        }
        QString header = datetime.toString(Qt::DefaultLocaleShortDate) + " - " + user + " :";
        m_remarkarea->addRemark(header, text);
    }
}

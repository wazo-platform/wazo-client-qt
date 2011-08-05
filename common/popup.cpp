/* XiVO Client
 * Copyright (C) 2007-2011, Proformatique
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
#include <QIcon>
#include <QLabel>
#include <QLineEdit>
#include <QPixmap>
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

QStringList g_formbuttonnames = (QStringList()
                                 << "refuse" << "hangup" << "answer"
                                 << "close" << "save");

/*!
 * This constructor inits all XML objects and connect signals
 * to slots.
 * \param parent                parent widget
 */
Popup::Popup(const bool urlautoallow, QWidget *parent)
    : QWidget(parent),
      m_xmlInputSource(0),
      m_handler(0),
      m_sheetpopup(false),
      m_systraypopup(true),
      m_focus(false),
      m_urlautoallow(urlautoallow),
      m_toupdate(false),
      m_firstline(3),
      m_sheetui_widget(NULL)
{
    // qDebug() << Q_FUNC_INFO;
    m_remarkarea = 0;
    m_nfeeds = 0;
}

Popup::~Popup()
{
    // qDebug() << Q_FUNC_INFO;
    delete m_xmlInputSource;
    delete m_handler;
    delete m_reader;
    delete m_sheetui_widget;
    delete m_uiloader;
}

/*!
 * \param inputstream        inputstream to read the XML
 */
void Popup::feed(QIODevice * inputstream,
                 const bool & sheetui)
{
    m_nfeeds ++;
    m_inputstream = inputstream;

    m_xmlInputSource = new QXmlInputSource(m_inputstream);
    m_handler = new XmlHandler(this);
    m_reader = new QXmlSimpleReader();

    m_sheetui = sheetui;

    QDateTime currentDateTime = QDateTime::currentDateTime();
    QString currentDateTimeStr = currentDateTime.toString(Qt::LocalDate);
    qDebug() << Q_FUNC_INFO << this << m_nfeeds << inputstream << currentDateTime << sheetui << m_handler;

    m_reader->setContentHandler(m_handler);
    m_reader->setErrorHandler(m_handler);
    m_parsingStarted = false;

    if(m_nfeeds == 1) {
        setAttribute(Qt::WA_DeleteOnClose);
        connect( m_inputstream, SIGNAL(readyRead()),
                 this, SLOT(streamNewData()) );
        connect( m_inputstream, SIGNAL(aboutToClose()),
                 this, SLOT(streamAboutToClose()) );
        //         connect( inputstream, SIGNAL(disconnected()),
        //                  this, SLOT(socketDisconnected()) );
        //         connect( inputstream, SIGNAL(error(QAbstractSocket::SocketError)),
        //                  this, SLOT(socketError(QAbstractSocket::SocketError)));
        m_vlayout = new QVBoxLayout(this);
        m_title = new QLabel(this);
        m_title->setAlignment(Qt::AlignHCenter);
        m_hlayout = new QHBoxLayout();
        m_closesheet = new QPushButton(this);
        m_closesheet->setIcon(QIcon(":/images/cancel.png"));
        m_closesheet->setIconSize(QSize(10, 10));
        connect( m_closesheet, SIGNAL(clicked()),
                 this, SLOT(close()) );
        m_hlayout->addStretch();
        m_hlayout->addWidget(m_closesheet);
        m_vlayout->addLayout(m_hlayout);
        m_vlayout->addWidget(m_title);
        m_qf = new QFrame(this);
        m_qf->setFrameStyle(QFrame::HLine | QFrame::Plain);
        m_qf->setLineWidth(0);
        m_vlayout->addWidget(m_qf);
        m_vlayout->addStretch();
    }

    m_uiloader = new QUiLoader();
    if(sheetui) {
        m_sheetui_widget = m_uiloader->load(m_inputstream, this);
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

        QLineEdit   * datetime = m_sheetui_widget->findChild<QLineEdit *>("datetime");
        QLineEdit   * year     = m_sheetui_widget->findChild<QLineEdit *>("year");
        if(datetime)
            datetime->setText(currentDateTimeStr);
        if(year)
            year->setText(currentDateTime.toString("yyyy"));
    }

    setWindowIcon(QIcon(":/images/xivoicon.png"));
    QDesktopServices::setUrlHandler(QString("tel"), this, "dispurl");

    qDebug() << Q_FUNC_INFO << this << m_inputstream->bytesAvailable() << "bytes available";
    if(m_inputstream->bytesAvailable() > 0) {
        streamNewData();
    }
}

void Popup::dispurl(const QUrl &url)
{
    // qDebug() << Q_FUNC_INFO << url;
    // TODO : check if the string starts with "tel:"
    QString numbertodial = url.toString().mid(4);
    b_engine->actionDialNumber(numbertodial);
}

void Popup::actionFromForm()
{
    QString buttonname = sender()->property("buttonname").toString();
    // qDebug() << Q_FUNC_INFO << buttonname << m_ipbxid << m_channel;
    if(buttonname == "close")
        close();
    else if(buttonname == "save")
        saveandclose();
    else if((buttonname == "hangup") || (buttonname == "answer") || (buttonname == "refuse"))
        emit actionFromPopup(buttonname, QVariant(m_timestamps));
    else if(buttonname.startsWith("XIVO_CALL_STATUS-")) {
        emit actionFromPopup(buttonname, QVariant(m_timestamps));
        close();
    }
}

/*! \brief saves the filled-in forms and closes the sheet
 */
void Popup::saveandclose()
{
    // qDebug() << Q_FUNC_INFO;
    QStringList qsl;
    QRegExp re_formentry("^XIVOFORM-");

    QVariantMap qv;
    QList<QLineEdit *> lineedits = m_sheetui_widget->findChildren<QLineEdit *>(re_formentry);
    for(int i = 0; i < lineedits.count(); i++) {
        qv[lineedits[i]->objectName()] = lineedits[i]->text();
    }

    if (qv.size() > 0) {
        QVariantMap data;
        data["buttonname"] = "saveandclose";
        data["variables"] = qv;
        b_engine->actionFromFiche(QVariant(data));
    }
    close();
}

void Popup::addAnyInfo(const QString & localName,
                       const QString & infoOrder,
                       const QString & infoType,
                       const QString & infoName,
                       const QString & infoValue)
{
    // qDebug() << Q_FUNC_INFO << this << m_vlayout->count();
    QStringList z  = (QStringList() << localName << infoOrder << infoType << infoName << infoValue);
    m_sheetlines.append(z);
    if( localName == "sheet_info" ) {
        int where = 0;
        if(m_orders.contains(infoOrder)) {
            m_form_buttons.clear();
            // removes the layout and widgets there
            where = m_orders.indexOf(infoOrder) + m_firstline;
            QLayoutItem * qli = m_vlayout->itemAt(where);
            if(qli != NULL) {
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
            }
        } else {
            m_orders.append(infoOrder);
            m_orders.sort();
            where = m_orders.indexOf(infoOrder) + m_firstline;
        }
        setSheetPopup( true );
        if (infoType == "title") {
            setTitle(infoValue);
            m_orders.removeAll(infoOrder);
        } else if (infoType == "text") {
            addInfoText( where, infoName, infoValue );
        } else if( infoType == "url" ) {
            addInfoLink( where, infoName, infoValue );
        } else if( infoType == "urlx" ) {
            addInfoLinkX( where, infoName, infoValue );
        } else if( infoType == "picture" ) {
            addInfoPicture( where, infoName, infoValue );
        } else if( infoType == "phone" ) {
            QRegExp re_number("\\+?[0-9\\s\\.]+");
            if(re_number.exactMatch(infoValue))
                addInfoLink( where, infoName, "tel:" + infoValue );
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
        if ( m_urlautoallow && (infoType == "urlauto") )
            b_engine->urlAuto(infoValue);

    } else if( localName == "internal" ) {
        addInfoInternal( infoName, infoValue );
    }
}

/*! \brief sets the sheet's title
 */
void Popup::setTitle(const QString & title)
{
    // qDebug() << Q_FUNC_INFO << title;
    m_title->setText(title);
}

void Popup::addDefForm(const QString & name, const QString & value)
{
    // qDebug() << Q_FUNC_INFO << this << name << value.size();
    m_remoteforms[name] = value;
}

void Popup::addInfoForm(int where, const QString & value)
{
    // qDebug() << Q_FUNC_INFO << this << where << value << m_where << m_remoteforms.keys();
    QUiLoader loader;
    if(m_remoteforms.contains(value)) {
        m_buffer = new QBuffer(this);
        m_buffer->open(QIODevice::ReadWrite);
        m_buffer->write(m_remoteforms[value].toUtf8());
        m_buffer->close();
        m_sheetui_widget = loader.load(m_buffer, this);
    } else {
        QFile file(value);
        if (file.exists()) {
            file.open(QFile::ReadOnly);
            m_sheetui_widget = loader.load(&file, this);
            file.close();
        } else {
            qDebug() << Q_FUNC_INFO << "file does not exist" << value;
            return;
        }
    }

    foreach(QString formbuttonname, g_formbuttonnames) {
        if(! m_form_buttons[formbuttonname]) {
            m_form_buttons[formbuttonname] = m_sheetui_widget->findChild<QPushButton *>(formbuttonname);
            if(m_form_buttons[formbuttonname]) {
                m_form_buttons[formbuttonname]->setProperty("buttonname", formbuttonname);
                connect( m_form_buttons[formbuttonname], SIGNAL(clicked()),
                         this, SLOT(actionFromForm()) );
            }
        } else {
            qDebug() << Q_FUNC_INFO << "already ?" << formbuttonname << m_sheetui_widget->findChild<QPushButton *>(formbuttonname);
        }
    }

    QRegExp re_callstatus("^XIVO_CALL_STATUS-");
    foreach(QPushButton * callstatusbutton, m_sheetui_widget->findChildren<QPushButton *>(re_callstatus)) {
        QString formbuttonname = callstatusbutton->objectName();
        m_form_buttons[formbuttonname] = callstatusbutton;
        m_form_buttons[formbuttonname]->setProperty("buttonname", formbuttonname);
        connect( m_form_buttons[formbuttonname], SIGNAL(clicked()),
                 this, SLOT(actionFromForm()) );
    }
    setEnablesOnForms();
    if(m_sheetui_widget != NULL)
        m_vlayout->insertWidget(where, m_sheetui_widget);
}

void Popup::addInfoText(int where, const QString & name, const QString & value)
{
    QString strValue(value);
    // qDebug() << Q_FUNC_INFO << value;
    QLabel * lblname = new QLabel(name, this);
    QLabel * lblvalue = new QLabel(strValue, this);
    lblvalue->setTextInteractionFlags( Qt::TextSelectableByMouse
                                       | Qt::TextSelectableByKeyboard );
    QHBoxLayout * hlayout = new QHBoxLayout();
    hlayout->addWidget(lblname, 1, Qt::AlignTop);
    hlayout->addWidget(lblvalue, 1, Qt::AlignTop);
    m_vlayout->insertLayout(where, hlayout);
}

void Popup::addInfoInternal(const QString & name, const QString & value)
{
    // qDebug() << Q_FUNC_INFO << name << value;
    if(name == "channel") {
        m_channel = value;
        setProperty("channel", m_channel);
    } else if(name == "ipbxid") {
        m_ipbxid = value;
        setProperty("ipbxid", m_ipbxid);
    } else if(name == "nosystraypopup")
        m_systraypopup = false;
    else if(name == "focus") {
        if (value == "yes")
            m_focus = true;
    } else if(name == "where") {
        // the form buttons should have been defined when arriving here
        // ('where' definition at the end of the sheet on server-side)
        m_where = value;
        m_timestamps[value] = QDateTime::currentDateTime().toTime_t();
        setEnablesOnForms();
    } else
        qDebug() << Q_FUNC_INFO << "undefined internal" << name << value;
}

/*! \brief disables the call-related actions for most of the sheets
 */
void Popup::setEnablesOnForms()
{
    if(m_where == "dial") {
        if(m_form_buttons["hangup"])
            m_form_buttons["hangup"]->setEnabled(false);
    } else if(m_where == "link") {
        if(m_form_buttons["refuse"])
            m_form_buttons["refuse"]->setEnabled(false);
        if(m_form_buttons["hangup"])
            m_form_buttons["hangup"]->setEnabled(true);
        if(m_form_buttons["answer"])
            m_form_buttons["answer"]->setEnabled(false);
    } else if(m_where == "unlink") {
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
    QLabel * lblname = new QLabel(name, this);
    QPushButton * lblvalue = new QPushButton(value, this);
    QHBoxLayout * hlayout = new QHBoxLayout();

    lblvalue->setObjectName("phonenumber");
    lblvalue->setProperty("number", value);
    connect( lblvalue, SIGNAL(clicked()),
             this, SLOT(dialThisNumber()) );

    hlayout->addWidget(lblname, 1, Qt::AlignTop);
    hlayout->addWidget(lblvalue, 1, Qt::AlignTop);
    m_vlayout->insertLayout(where, hlayout);
}

QList<QStringList> & Popup::sheetlines()
{
    return m_sheetlines;
}

void Popup::update(QList<QStringList> & newsheetlines)
{
    // qDebug() << Q_FUNC_INFO << this << newsheetlines;
    m_toupdate = true;
    foreach(QStringList qsl, newsheetlines)
        addAnyInfo(qsl[0], qsl[1], qsl[2], qsl[3], qsl[4]);
}

void Popup::addInfoLink(int where, const QString & name, const QString & value)
{
    // qDebug() << Q_FUNC_INFO << value;
    QLabel * lblname = new QLabel(name, this);
    UrlLabel * lblvalue = new UrlLabel(value, this);
    QHBoxLayout * hlayout = new QHBoxLayout();
    lblvalue->setToolTip(value);

    hlayout->addWidget(lblname, 1, Qt::AlignTop);
    hlayout->addWidget(lblvalue, 1, Qt::AlignTop);
    m_vlayout->insertLayout(where, hlayout);
}

void Popup::addInfoLinkX(int where, const QString & name, const QString & value)
{
    // qDebug() << Q_FUNC_INFO << name << value << dispvalue;
    QString linkvalue = value;
    QString dispvalue = value;
    QStringList qsl = value.split("@");
    if(qsl.size() == 2) {
        linkvalue = qsl[0];
        dispvalue = qsl[1];
    }
    QLabel * lblname = new QLabel(name, this);
    QPushButton * lblvalue = new QPushButton(dispvalue, this);
    // lblvalue->setObjectName("phonenumber");
    lblvalue->setProperty("urlx", linkvalue);
    connect( lblvalue, SIGNAL(clicked()),
             this, SLOT(httpGetNoreply()) );
    QHBoxLayout * hlayout = new QHBoxLayout();
    hlayout->addWidget(lblname, 1, Qt::AlignTop);
    hlayout->addWidget(lblvalue, 1, Qt::AlignTop);
    lblvalue->setToolTip(linkvalue);
    m_vlayout->insertLayout(where, hlayout);
}

void Popup::addInfoPicture(int where, const QString & name, const QString & value)
{
    QLabel * lblname = new QLabel(this);
    QLabel * lblvalue = new QLabel(this);
    QHBoxLayout * hlayout = new QHBoxLayout();

    QByteArray imagedata = QByteArray::fromBase64(value.toAscii());
    QPixmap * picture = new QPixmap();
    lblname->setText(name);
    picture->loadFromData(imagedata);
    lblvalue->setPixmap(* picture);
    // TODO: connect a signal to close() ?

    hlayout->addWidget(lblname, 1, Qt::AlignTop);
    hlayout->addWidget(lblvalue, 1, Qt::AlignTop);
    m_vlayout->insertLayout(where, hlayout);
}

// void Popup::addInfoPictureHttp(int where, const QString & name, const QString & value)
// {
//     QUrl url(value);
//     // qDebug() << Q_FUNC_INFO << value << url.scheme();
//     //QUrl url = QUrl::fromEncoded(value);
//     // TODO: faire un widget special qui bouffe des Images HTTP ?
//     if((url.scheme() != QString("http")) &&
//        (url.scheme() != QString("https"))) {
//         QLabel *lbl = new QLabel( name, this );
//         QPixmap *face = new QPixmap( value );
//         // TODO: connect a signal to close() ?
//         lbl->setPixmap( *face );
//         m_vlayout->insertWidget(where, lbl, 0, Qt::AlignCenter);
//     } else {
//         RemotePicWidget * pic = new RemotePicWidget( name, value, this );
//         m_vlayout->insertWidget(where, pic, 0, Qt::AlignCenter);
//     }
// }

// === SLOTS ===
void Popup::streamNewData()
{
    bool b = false;
    // qDebug() << Q_FUNC_INFO << this << m_sheetui << m_inputstream->bytesAvailable() << "bytes available";
    if(!m_sheetui)
        if(m_parsingStarted)
            b = m_reader->parseContinue();
        else {
            b = m_reader->parse(m_xmlInputSource, false);
            m_parsingStarted = b;
        }
    else
        finishAndShow();

    // qDebug() << Q_FUNC_INFO << "parse returned" << b;
}

void Popup::dialThisNumber()
{
    QString numbertodial = sender()->property("number").toString();
    b_engine->actionDialNumber(numbertodial);
}

void Popup::httpGetNoreply()
{
    QString urlx = sender()->property("urlx").toString();
    b_engine->urlAuto(urlx);
    // qDebug() << Q_FUNC_INFO << urlx;
}

void Popup::streamAboutToClose()
{
    // qDebug() << Q_FUNC_INFO;
    finishAndShow();
}

void Popup::socketDisconnected()
{
    // qDebug() << Q_FUNC_INFO;
    /* finish the parsing */
    m_reader->parseContinue();
}

/*
void Popup::socketError(QAbstractSocket::SocketError err)
{
  qDebug() << Q_FUNC_INFO << err;
}
*/

/*!
 * Send signal to be shown !
 */
void Popup::finishAndShow()
{
    // qDebug() << Q_FUNC_INFO << this;
    if(m_nfeeds == 1)
        addRemarkArea();
    //dumpObjectInfo();
    //dumpObjectTree();
    // ...
    //show();
    emit wantsToBeShown(this);
}


void Popup::closeEvent(QCloseEvent * event)
{
    qDebug() << Q_FUNC_INFO << event << sender();
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


const QString & Popup::callIpbxId() const
{
    return m_ipbxid;
}

const QString & Popup::callChannel() const
{
    return m_channel;
}

const QString & Popup::callWhere() const
{
    return m_where;
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
    qDebug() << Q_FUNC_INFO;
    m_remarkarea = new RemarkArea(this);
    m_vlayout->addWidget( m_remarkarea );
    m_vlayout->setStretchFactor( m_remarkarea, 1 ); // take available space
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
        if(b_engine) {
            const UserInfo * ui = b_engine->user(user);
            if(ui)
                user = ui->fullname();
        }
        QString header = datetime.toString(Qt::DefaultLocaleShortDate) + " - " + user + " :";
        m_remarkarea->addRemark(header, text);
    }
}

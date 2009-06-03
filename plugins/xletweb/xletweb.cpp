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

#include <QDebug>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QtWebKit>
#include <QLineEdit>
#include <QProgressBar>
#include <QPushButton>

#include "xletweb.h"

/*! \brief Constructor
 */
XletWeb::XletWeb(BaseEngine * engine, QWidget * parent)
    : XLet(engine, parent)
{
    qDebug() << "XletWeb::XletWeb()";
    setTitle( tr("Web Browser") );
        
    QVBoxLayout * layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    QLabel * title = new QLabel(this);
    title->setAlignment(Qt::AlignCenter);
    layout->addWidget(title);

    QHBoxLayout * hlayout = new QHBoxLayout();

    icon = new QLabel(this);
    hlayout->addWidget(icon);
    
    urlLine = new QLineEdit(this);
    hlayout->addWidget(urlLine, 3);

    QProgressBar * progress = new QProgressBar(this);
    hlayout->addWidget(progress, 1);

    QPushButton * back = new QPushButton(tr("Back"), this);
    hlayout->addWidget(back);

    QPushButton * forward = new QPushButton(tr("Forward"), this);
    hlayout->addWidget(forward);
    
    layout->addLayout(hlayout);

    web = new QWebView(this);
    layout->addWidget(web);

    QLabel * statusbar = new QLabel(this);
    layout->addWidget(statusbar);

    connect( web, SIGNAL(titleChanged(const QString &)),
             title, SLOT(setText(const QString &)) );
    connect( web, SIGNAL(statusBarMessage(const QString &)),
             statusbar, SLOT(setText(const QString &)) );
    connect( urlLine, SIGNAL(editingFinished()),
             this, SLOT(loadAddress()) );
    connect( web, SIGNAL(urlChanged(const QUrl &)),
             this, SLOT(displayUrl(const QUrl &)) );
    connect( web, SIGNAL(iconChanged()),
             this, SLOT(displayIcon()) );
    connect( web, SIGNAL(loadProgress(int)),
             progress, SLOT(setValue(int)) );
    connect( back, SIGNAL(clicked()),
             web, SLOT(back()) );
    connect( forward, SIGNAL(clicked()),
             web, SLOT(forward()) );
}

XletWeb::~XletWeb()
{
    qDebug() << "XletWeb::~XletWeb()";
}

void XletWeb::loadAddress()
{
    QString address(urlLine->text());
    QUrl url(address);
    if(url.scheme().isEmpty())
        url.setUrl("http://" + address);
    qDebug() << "XletWeb::loadAddress" << address << url;
    web->load(url);
}

void XletWeb::displayUrl(const QUrl & url)
{
    urlLine->setText(url.toString());
}

void XletWeb::displayIcon()
{
    const QIcon & _icon = web->icon();
#if QT_VERSION >= 0x040500
    qDebug() << "XletWeb::displayIcon() available sizes"
             << _icon.availableSizes(); //Qt 4.5
#endif
    icon->setPixmap(_icon.pixmap(32));
}


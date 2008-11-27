/* XIVO CTI Clients
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

#include <QBuffer>
#include <QDateTime>
#include <QDebug>
#include <QLabel>
#include <QGridLayout>
#include <QTabWidget>
#include <QVariant>

#include "customerinfopanel.h"
#include "popup.h"
#include "userinfo.h"

CustomerInfoPanel::CustomerInfoPanel(const QVariant & options,
                                     QWidget * parent)
        : QWidget(parent)
{
        // qDebug() << "CustomerInfoPanel::CustomerInfoPanel()";
        QGridLayout * glayout = new QGridLayout(this);
        m_tabs = new QTabWidget(this);
        glayout->addWidget( m_tabs, 0, 0 );
        glayout->setRowStretch(0, 1);
        glayout->setColumnStretch(0, 1);
        m_tablimit = options.toMap()["sheet-tablimit"].toUInt();
        m_autourl_allowed = options.toMap()["autourl_allowed"].toBool();
}

CustomerInfoPanel::~CustomerInfoPanel()
{
}

void CustomerInfoPanel::setGuiOptions(const QVariant &)
{
}

void CustomerInfoPanel::setUserInfo(const UserInfo * ui)
{
        m_ui = ui;
}

/*!
 * Display the new profile in the tabbed area
 * and show a message with the systray icon
 */
void CustomerInfoPanel::showNewProfile(Popup * popup)
{
        QString opt = "";
        qDebug() << "CustomerInfoPanel::showNewProfile()" << popup->sessionid();
        if(popup->sheetpopup()) {
                Popup * already_popup = NULL;
                foreach(Popup * mpopup, m_popups)
                        if(mpopup->sessionid() == popup->sessionid()) {
                                already_popup = mpopup;
                                break;
                        }
                if(already_popup) {
                        qDebug() << "CustomerInfoPanel::showNewProfile()" << "found a match for" << popup->sessionid();
                        already_popup->update(popup->sheetlines());
                } else {
                        QString currentTimeStr = QDateTime::currentDateTime().toString("hh:mm:ss");
                        quint32 index = m_tabs->addTab(popup, currentTimeStr);
                        qDebug() << "CustomerInfoPanel::showNewProfile() : added tab" << index;
                        m_popups.append(popup);
                        m_tabs->setCurrentIndex(index);
                        if (index >= m_tablimit)
                                // close the first widget
                                m_tabs->removeTab(0);
                }
                
                // no need to focus if there is no sheet popup
                if(popup->focus())
                        opt += "fp";
        }
        
        // tells the main widget that a new popup has arrived here
        if(popup->systraypopup())
                opt += "s";
        newPopup(popup->messagetitle(), popup->message(), opt);
        
        //         connectDials(popup);
}

void CustomerInfoPanel::popupDestroyed(QObject * obj)
{
	qDebug() << "CustomerInfoPanel::popupDestroyed()" << obj->property("sessionid");
        foreach(Popup * mpopup, m_popups)
                if(mpopup->sessionid() == obj->property("sessionid"))
                        m_popups.removeAll(mpopup);
}

void CustomerInfoPanel::addToDataBase(const QString & dbdetails)
{
        qDebug() << "CustomerInfoPanel::addToDataBase()" << dbdetails;
        // if (dbdetails.size() > 0)
        // sendCommand("database " + dbdetails);
}

void CustomerInfoPanel::displayFiche(const QString & fichecontent, bool qtui)
{
        QBuffer * inputstream = new QBuffer(this);
        inputstream->open(QIODevice::ReadWrite);
        inputstream->write(fichecontent.toUtf8());
        inputstream->close();
        // Get Data and Popup the profile if ok
        Popup * popup = new Popup(m_autourl_allowed, m_ui);
        popup->feed(inputstream, qtui);
        connect( popup, SIGNAL(destroyed(QObject *)),
                 this, SLOT(popupDestroyed(QObject *)) );
        connect( popup, SIGNAL(wantsToBeShown(Popup *)),
                 this, SLOT(showNewProfile(Popup *)) );
        connect( popup, SIGNAL(save(const QString &)),
                 this, SLOT(addToDataBase(const QString &)) );
        connect( popup, SIGNAL(actionFromPopup(const QString &, const QVariant &)),
                 this, SLOT(actionFromPopup(const QString &, const QVariant &)) );
        connect( popup, SIGNAL(actionCall(const QString &, const QString &, const QString &)),
                 this, SLOT(localActionCall(const QString &, const QString &, const QString &)) );
}

void CustomerInfoPanel::localActionCall(const QString & a, const QString & b, const QString & c)
{
        actionCall(a, b, c);
}

void CustomerInfoPanel::actionFromPopup(const QString & buttonname, const QVariant & timestamps)
{
        QString sessionid = sender()->property("sessionid").toString();
        Popup * thispopup = NULL;
        foreach(Popup * mpopup, m_popups)
                if(mpopup->sessionid() == sessionid) {
                        thispopup = mpopup;
                        break;
                }
        if(thispopup) {
                QVariantMap data;
                data["buttonname"] = buttonname;
                data["sessionid"] = thispopup->sessionid();
                data["channel"] = thispopup->channel();
                data["timestamps"] = timestamps;
                actionFromFiche(QVariant(data));
        }
}

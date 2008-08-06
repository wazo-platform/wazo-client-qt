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
#include <QDebug>
#include <QLabel>
#include <QGridLayout>
#include <QTabWidget>
#include <QTime>

#include "customerinfopanel.h"
#include "popup.h"

CustomerInfoPanel::CustomerInfoPanel(QWidget * parent)
        : QWidget(parent)
{
        qDebug() << "CustomerInfoPanel::CustomerInfoPanel()";
        QGridLayout * glayout = new QGridLayout(this);
        // QLabel * ql = new QLabel("help");
        m_tabs = new QTabWidget(this);
        glayout->addWidget( m_tabs, 0, 0 );
//         glayout->setRowStretch(0, 1);
//         glayout->setColumnStretch(0, 1);
}

/*!
 * Display the new profile in the tabbed area
 * and show a message with the systray icon
 */
void CustomerInfoPanel::showNewProfile(Popup * popup)
{
	QString currentTimeStr = QTime::currentTime().toString("hh:mm:ss");

        int index = m_tabs->addTab(popup, currentTimeStr);
        qDebug() << "added tab" << index;
        m_tabs->setCurrentIndex(index);

        // tells the main widget that a new popup has arrived here
        // args : message title, message data
        newPopup();
        
        // 		if (index >= m_tablimit) {
        // 			// close the first widget
        // 			m_cinfo_tabwidget->widget(0)->close();
        // 		}
        //         connectDials(popup);
}

void CustomerInfoPanel::popupDestroyed(QObject * obj)
{
	qDebug() << "CustomerInfoPanel::popupDestroyed()" << obj;
}

void CustomerInfoPanel::addToDataBase(const QString & dbdetails)
{
        qDebug() << "CustomerInfoPanel::addToDataBase()" << dbdetails;
        // if (dbdetails.size() > 0)
        // sendCommand("database " + dbdetails);
}

void CustomerInfoPanel::displayFiche(const QString & fichecontent,
                                     bool qtui, bool autourl,
                                     const UserInfo * ui)
{
        QBuffer * inputstream = new QBuffer(this);
        inputstream->open(QIODevice::ReadWrite);
        inputstream->write(fichecontent.toUtf8());
        inputstream->close();
        // Get Data and Popup the profile if ok
        Popup * popup = new Popup(inputstream, qtui, autourl, ui);
        connect( popup, SIGNAL(destroyed(QObject *)),
                 this, SLOT(popupDestroyed(QObject *)) );
        connect( popup, SIGNAL(save(const QString &)),
                 this, SLOT(addToDataBase(const QString &)) );
        connect( popup, SIGNAL(wantsToBeShown(Popup *)),
                 this, SLOT(showNewProfile(Popup *)) );
}

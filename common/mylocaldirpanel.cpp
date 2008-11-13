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

#include <QDebug>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>

#include "baseengine.h"
#include "mylocaldirpanel.h"
#include "userinfo.h"

/*! \brief Constructor
 */
MyLocalDirPanel::MyLocalDirPanel(BaseEngine * engine, QWidget * parent)
        : QWidget(parent), m_engine(engine), m_ui(NULL)
{
        qDebug() << "MyLocalDirPanel::MyLocalDirPanel()";
	
        // replace by whatever you need
        QVBoxLayout * vlayout = new QVBoxLayout(this);
        QHBoxLayout * hlayout = new QHBoxLayout();
        QLabel * title = new QLabel(tr("New Entry"));
        QLabel * dummy = new QLabel();
        m_who = new QLineEdit();
        m_number = new QLineEdit();
        hlayout->addWidget(title);
        hlayout->addWidget(m_who);
        hlayout->addWidget(m_number);
        vlayout->addLayout(hlayout);
        vlayout->addWidget(dummy);

        connect( m_who, SIGNAL(returnPressed()),
                 this, SLOT(returnPressed()) );
        connect( m_number, SIGNAL(returnPressed()),
                 this, SLOT(returnPressed()) );
        //
}

void MyLocalDirPanel::setGuiOptions(const QVariant &)
{
}

void MyLocalDirPanel::setUserInfo(const UserInfo * ui)
{
        m_ui = ui;
        // qDebug() << "MyLocalDirPanel::setUserInfo()" << m_ui->fullname();
}

void MyLocalDirPanel::returnPressed()
{
        qDebug() << "MyLocalDirPanel::returnPressed()" << m_who->text() << m_number->text();
        QString entry = m_who->text();
        bool already = false;
        foreach(QStringList qsl, m_localdir)
                if(qsl[0] == entry) {
                        already = true;
                        break;
                }
        if(! already) {
                QStringList qsl = (QStringList() << m_who->text() << m_number->text());
                m_localdir.append(qsl);
                saveNew(qsl);
                qDebug() << m_localdir;
        }
}

void MyLocalDirPanel::getDir()
{
        qDebug() << "MyLocalDirPanel::getDir()";
}

void MyLocalDirPanel::saveNew(const QStringList & qsl)
{
        qDebug() << "MyLocalDirPanel::saveNew()" << qsl;
        // QSettings * settings = m_engine->getSettings();
        // settings->setValue("localdirectory/" + qsl[0], qsl[1]);
}

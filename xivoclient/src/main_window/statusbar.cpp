/* XiVO Client
 * Copyright (C) 2013-2015 Avencall
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

#include <baseengine.h>

#include "statusbar.h"

#include "main_window.h"


Statusbar::Statusbar(MainWindow *parent)
    : QObject(parent),
      m_statusbar(parent->ui->statusbar),
      m_padlock(new QLabel),
      m_config_profile(new QLabel)
{
    this->connect(b_engine, SIGNAL(logged()), SLOT(setStatusLogged()));
    this->connect(b_engine, SIGNAL(delogged()), SLOT(setStatusNotLogged()));
    this->connect(b_engine, SIGNAL(settingsChanged()), SLOT(confUpdated()));
    this->connect(b_engine, SIGNAL(emitTextMessage(const QString &)), this->m_statusbar, SLOT(showMessage(const QString &)));
    this->connect(parent, SIGNAL(initialized()), SLOT(initialize()));
}

Statusbar::~Statusbar()
{
}

void Statusbar::initialize()
{
    this->m_config_profile->setText(b_engine->getConfig("profilename").toString());
    this->m_config_profile->setVisible(this->shouldDisplayProfile());
    this->m_statusbar->addPermanentWidget(this->m_config_profile);

    QPixmap padlock_pixmap = QPixmap(":/images/padlock.png").scaledToHeight(18, Qt::SmoothTransformation);
    this->m_padlock->setPixmap(padlock_pixmap);
    this->m_padlock->hide();
    this->m_statusbar->addPermanentWidget(this->m_padlock);
}

void Statusbar::confUpdated()
{
    this->m_config_profile->setVisible(this->shouldDisplayProfile());
}

bool Statusbar::shouldDisplayProfile() const
{
    return b_engine->getConfig("displayprofile").toBool();
}

void Statusbar::setStatusLogged()
{
    this->m_statusbar->showMessage(tr("Connected"));
    this->m_padlock->setVisible(b_engine->getConfig("cti_encrypt").toBool());
}

void Statusbar::setStatusNotLogged()
{
    this->m_statusbar->showMessage(tr("Disconnected"));
    this->m_padlock->hide();
}

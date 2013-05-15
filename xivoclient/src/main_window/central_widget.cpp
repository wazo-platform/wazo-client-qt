/* XiVO Client
 * Copyright (C) 2013, Avencall
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

#include <QDebug>

#include <baseengine.h>
#include <assembler.h>
#include <login_widget/login_widget.h>

#include "central_widget.h"


CentralWidget::CentralWidget(MainWindow *parent)
    : QStackedWidget(parent),
      m_main_window(parent),
      m_login_widget(NULL),
      m_main_widget(NULL)
{
    this->connect(b_engine, SIGNAL(logged()), SLOT(setStatusLogged()));
    this->connect(b_engine, SIGNAL(delogged()), SLOT(setStatusNotLogged()));
    this->connect(parent, SIGNAL(initialized()), SLOT(initialize()));
}

CentralWidget::~CentralWidget()
{
}

void CentralWidget::initialize()
{
    this->m_login_widget = assembler->loginWidget();
    this->m_main_widget = assembler->mainWidget();
    this->m_main_window->setCentralWidget(this);
    this->addWidget(this->m_login_widget);
    this->addWidget(this->m_main_widget);

    this->m_login_widget->setConfig();
    this->setDefaultWidget();
}

void CentralWidget::setDefaultWidget()
{
    this->setCurrentWidget(this->m_login_widget);
}

void CentralWidget::setStatusLogged()
{
    this->m_login_widget->saveConfig();
    this->setCurrentWidget(this->m_main_widget);
}

void CentralWidget::setStatusNotLogged()
{
    this->setCurrentWidget(this->m_login_widget);
}

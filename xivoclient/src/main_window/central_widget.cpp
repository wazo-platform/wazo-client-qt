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
#include <assembler.h>
#include <login_widget/login_widget.h>
#include <stdlib.h>

#include <xletlib/chitchat.h>
#include "central_widget.h"
#include "xlet_dispatcher.h"


CentralWidget::CentralWidget(MainWindow *parent)
    : QStackedWidget(parent),
      m_main_window(parent),
      m_login_widget(NULL),
      m_main_widget(NULL),
      ui_loading_dialog(new Ui::loading_dialog),
      m_loading_dialog(NULL)
{
    ChitChatWindow::chitchat_instance = new ChitChatWindow();
    this->connect(b_engine, SIGNAL(logged()), SLOT(setStatusLogged()));
    this->connect(b_engine, SIGNAL(delogged()), SLOT(setStatusNotLogged()));
    this->connect(b_engine, SIGNAL(initializing()), SLOT(initializing()));
    this->connect(b_engine, SIGNAL(initialized()), SLOT(initialized()));
    this->connect(parent, SIGNAL(initialized()), SLOT(initialize()));
}

CentralWidget::~CentralWidget()
{
    delete ChitChatWindow::chitchat_instance;
    delete this->ui_loading_dialog;
    this->ui_loading_dialog = NULL;
}

void CentralWidget::initializing()
{
    this->showLoading();
}

void CentralWidget::initialize()
{
    this->m_login_widget = assembler->loginWidget();
    this->m_main_widget = assembler->mainWidget();
    this->m_main_window->setCentralWidget(this);
    this->addWidget(this->m_login_widget);
    this->setCurrentWidget(this->m_login_widget);
}

void CentralWidget::initialized()
{
    XletDispatcher *xlet_dispatcher = assembler->xletDispatcher();
    if (xlet_dispatcher->hasWidget()) {
        this->show();
    }
    xlet_dispatcher->showXletsDock();
    this->hideLoading();
    this->m_main_window->prepareState();
}

void CentralWidget::setStatusLogged()
{
    this->m_login_widget->saveConfig();
    this->showMainWidget();
    this->hide();
}

void CentralWidget::setStatusNotLogged()
{
    this->showLoginWidget();
    this->show();
}

void CentralWidget::showMainWidget()
{
    this->removeWidget(this->m_login_widget);
    this->addWidget(this->m_main_widget);
    this->setCurrentWidget(this->m_main_widget);
}

void CentralWidget::showLoginWidget()
{
    this->removeWidget(this->m_main_widget);
    this->addWidget(this->m_login_widget);
    this->setCurrentWidget(this->m_login_widget);
}

void CentralWidget::showLoading()
{
    this->m_loading_dialog = new QDialog(this->m_main_window);
    if (! m_loading_dialog) {
        qDebug() << Q_FUNC_INFO << "Failed to instanciate the main window UI";
        exit(EXIT_FAILURE);
    }
    this->ui_loading_dialog->setupUi(this->m_loading_dialog);
    this->m_loading_dialog->adjustSize();
    this->m_loading_dialog->setFixedSize(this->m_loading_dialog->width(),this->m_loading_dialog->height());
    this->m_loading_dialog->show();
}

void CentralWidget::hideLoading()
{
    this->m_loading_dialog->hide();
    this->m_loading_dialog->deleteLater();
}

/* XiVO Client
 * Copyright (C) 2013-2016 Avencall
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

#include <QMovie>

#include <baseengine.h>
#include "login_widget.h"

#include "main_window/main_window.h"

LoginWidget::LoginWidget(MainWindow *main_window, QWidget *parent)
    : QWidget(parent),
      m_waiting_status(new QMovie(":/images/waiting-status.gif", QByteArray(), this))
{
    QVBoxLayout * login_layout = new QVBoxLayout(this);
    login_layout->setContentsMargins(0, 0, 0, 0);
    QWidget * login_widget = new QWidget(this);
    login_layout->addWidget(login_widget);
    this->ui.setupUi(login_widget);

    QFile qssFile(QString(":/default.qss"));
    if(qssFile.open(QIODevice::ReadOnly)) {
        this->setStyleSheet(qssFile.readAll());
    }

    this->connect(this->ui.user_login, SIGNAL(returnPressed()), SLOT(saveConfigAndStart()));
    this->connect(this->ui.user_password, SIGNAL(returnPressed()), SLOT(saveConfigAndStart()));
    this->connect(this->ui.agent_phone_number, SIGNAL(returnPressed()), SLOT(saveConfigAndStart()));
    this->connect(this->ui.connect_button, SIGNAL(pressed()), SLOT(saveConfigAndStart()));
    this->connect(this->ui.agent_options, SIGNAL(currentIndexChanged(int)), SLOT(syncAgentLoginWidgets()));
    this->connect(b_engine, SIGNAL(settingsChanged()), SLOT(confUpdated()));
    this->connect(b_engine, SIGNAL(connectionFailed()), SLOT(onConnectionFailed()));
    this->connect(b_engine, SIGNAL(logged()), SLOT(onLogged()));
    this->connect(main_window, SIGNAL(initialized()), SLOT(initialize()));
}

LoginWidget::~LoginWidget()
{
}

void LoginWidget::initialize()
{
    this->setConfig();
    this->setAgentLoginWidgetsVisible();
    this->ui.user_login->setFocus();
}

void LoginWidget::setAgentLoginWidgetsVisible()
{
    bool showagselect = b_engine->getConfig("showagselect").toBool();
    if (showagselect) {
        this->ui.agent_options->show();
        this->syncAgentLoginWidgets();
    } else {
        this->ui.agent_phone_number->hide();
        this->ui.agent_options->hide();
    }
}

void LoginWidget::syncAgentLoginWidgets()
{
    int index = this->ui.agent_options->currentIndex();
    if (index == 0) {
        this->ui.agent_phone_number->hide();
    } else if (index > 0) {
        this->ui.agent_phone_number->show();
    }
    this->ui.agent_options->show();
}

QVariantMap LoginWidget::getConfig()
{
    QStringList required = QStringList()
        << "userlogin"
        << "password"
        << "agentphonenumber"
        << "keeppass"
        << "guioptions.loginkind";
    QVariantMap config;

    foreach (const QString &key, required) {
        config[key] = b_engine->getConfig(key);
    }
    return config;
}

void LoginWidget::setConfig()
{
    QVariantMap config = this->getConfig();
    this->ui.user_login->setText(config["userlogin"].toString());
    this->ui.user_password->setText(config["password"].toString());
    this->ui.agent_phone_number->setText(config["agentphonenumber"].toString());
    this->ui.keep_password->setChecked(config["keeppass"].toBool());
    this->ui.agent_options->setCurrentIndex(config["guioptions.loginkind"].toInt());
}

void LoginWidget::saveConfig()
{
    QVariantMap config;
    config["userlogin"] = this->ui.user_login->text();
    config["password"] = this->ui.user_password->text();
    config["agentphonenumber"] = this->ui.agent_phone_number->text();
    config["keeppass"] = this->ui.keep_password->isChecked();
    config["guioptions.loginkind"] = this->ui.agent_options->currentIndex();
    b_engine->setConfig(config);
}

void LoginWidget::saveConfigAndStart()
{
    this->saveConfig();
    b_engine->start();
    this->ui.status_icon->clear();
    this->ui.status_icon->setMovie(m_waiting_status);
    this->ui.status_icon->movie()->start();
    this->ui.status_icon->setToolTip(tr("Connecting"));
}

void LoginWidget::onLogged()
{
    this->ui.status_icon->clear();
}

void LoginWidget::onConnectionFailed()
{
    this->ui.status_icon->setPixmap(QPixmap(":/images/dot-red.svg"));
    this->ui.status_icon->setToolTip(tr("Failed"));
}

void LoginWidget::confUpdated()
{
    this->setAgentLoginWidgetsVisible();
}

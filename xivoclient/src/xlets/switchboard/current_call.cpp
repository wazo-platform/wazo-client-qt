/* XiVO Client
 * Copyright (C) 2012, Avencall
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

#include <QWidget>
#include <QTimer>
#include <baseengine.h>
#include <message_factory.h>
#include "ui_current_call.h"
#include "current_call.h"


CurrentCall::CurrentCall(QObject *parent)
    : QObject(parent),
      m_current_call_widget(NULL),
      m_call_start(0)
{
    this->registerListener("current_calls");

    QTimer * timer_display = new QTimer(this);
    connect(timer_display, SIGNAL(timeout()),
            this, SLOT(updateCallInfo()));
    timer_display->start(1000);
}

CurrentCall::~CurrentCall()
{
}

void CurrentCall::setParentWidget(QWidget *parent)
{
    m_current_call_widget = new Ui::CurrentCallWidget();
    m_current_call_widget->setupUi(parent);

    this->connectButtons();

    this->disableButtons();
}

void CurrentCall::connectButtons()
{
    connect(m_current_call_widget->btn_hangup, SIGNAL(clicked()),
            this, SLOT(hangup()));
    connect(m_current_call_widget->btn_hold, SIGNAL(clicked()),
            this, SLOT(hold()));
    connect(m_current_call_widget->btn_attended_transfer, SIGNAL(clicked()),
            this, SLOT(attendedTransfer()));
}

void CurrentCall::updateCallerID(const QString &name,
                                 const QString &number)
{
    m_caller_id = QString("%1 <%2>").arg(name).arg(number);
}

void CurrentCall::updateCallInfo()
{
    QString time;
    if (m_call_start != 0) {
        time = b_engine->timeElapsed(m_call_start);
    }
    QString info = QString("%1 %2").arg(this->m_caller_id).arg(time);
    this->m_current_call_widget->lbl_call_info->setText(info);
}

void CurrentCall::parseCommand(const QVariantMap &current_calls)
{
    const QVariantList &calls = current_calls["current_calls"].toList();
    if (calls.isEmpty()) {
        this->clear();
    } else {
        this->updateCall(calls);
    }
}

void CurrentCall::updateCall(const QVariantList &calls)
{
    foreach (const QVariant &call, calls) {
        const QVariantMap &call_map = call.toMap();
        if (call_map["call_status"].toString() != "up") {
            continue;
        }
        this->updateCallerID(call_map["cid_name"].toString(),
                             call_map["cid_number"].toString());
        this->m_call_start = call_map["call_start"].toDouble();
        this->updateCallInfo();
        this->enableButtons();
    }
}

void CurrentCall::clear()
{
    this->m_caller_id.clear();
    this->m_call_start = 0;
    this->updateCallInfo();
    this->disableButtons();
}

void CurrentCall::hangup()
{
    b_engine->sendJsonCommand(MessageFactory::hangup());
}

void CurrentCall::hold()
{
    b_engine->sendJsonCommand(MessageFactory::holdSwitchboard());
}

void CurrentCall::attendedTransfer()
{
    qDebug() << "ATXFer";
}

void CurrentCall::disableButtons()
{
    this->setButtonsAvailability(false);
}

void CurrentCall::enableButtons()
{
    this->setButtonsAvailability(true);
}

void CurrentCall::setButtonsAvailability(bool enabled)
{
    this->m_current_call_widget->btn_attended_transfer->setEnabled(enabled);
    this->m_current_call_widget->btn_hangup->setEnabled(enabled);
    this->m_current_call_widget->btn_hold->setEnabled(enabled);
}

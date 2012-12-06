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
            this, SLOT(updateTime()));
    timer_display->start(1000);
}

CurrentCall::~CurrentCall()
{
}

void CurrentCall::setParentWidget(QWidget *parent)
{
    m_current_call_widget = new Ui::CurrentCallWidget();
    m_current_call_widget->setupUi(parent);
}

void CurrentCall::updateCallerID(const QString &name,
                                 const QString &number)
{
    QString caller_id = QString("%1 <%2>").arg(name).arg(number);
    this->m_current_call_widget->lbl_callerid->setText(caller_id);
}

void CurrentCall::updateTime()
{
    if (m_call_start == 0) {
        this->m_current_call_widget->lbl_time->clear();
    } else {
        const QString &time = b_engine->timeElapsed(m_call_start);
        this->m_current_call_widget->lbl_time->setText(time);
    }
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
        this->updateTime();
    }
}

void CurrentCall::clear()
{
    this->m_current_call_widget->lbl_callerid->clear();
    this->m_call_start = 0;
    this->updateTime();
}

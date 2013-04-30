/* XiVO Client
 * Copyright (C) 2012-2013, Avencall
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
#include <xletlib/signal_relayer.h>
#include "ui_current_call.h"
#include "switchboard.h"

#include "current_call.h"

QKeySequence CurrentCall::call_key = QKeySequence("F3");
QKeySequence CurrentCall::direct_transfer_key = QKeySequence("F4");
QKeySequence CurrentCall::attended_transfer_key = QKeySequence("F5");
QKeySequence CurrentCall::hangup_key = QKeySequence("F8");

CurrentCall::CurrentCall(QObject *parent)
    : QObject(parent),
      m_current_call_widget(NULL),
      m_call_start(0),
      m_attended_transfer_label(tr("Attended T")),
      m_call_label(tr("Call")),
      m_complete_transfer_label(tr("Complete T")),
      m_hangup_label(tr("Hangup")),
      m_cancel_transfer_label(tr("Cancel T"))
{
    this->registerListener("current_calls");
    this->registerListener("current_call_attended_transfer_answered");

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

    this->noCallsMode();
    connect(signal_relayer, SIGNAL(numberSelected(const QString &)),
            this, SLOT(numberSelected(const QString &)));
    connect(signal_relayer, SIGNAL(noNumberSelected()),
            this, SLOT(noNumberSelected()));
}

bool CurrentCall::hasCurrentCall() const
{
    return this->m_call_start != 0;
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

void CurrentCall::parseCommand(const QVariantMap &message)
{
    QString message_class = message["class"].toString();
    if (message_class == "current_calls") {
        this->parseCurrentCalls(message);
    } else if (message_class == "current_call_attended_transfer_answered") {
        this->parseAttendedTransferAnswered(message);
    }
}

void CurrentCall::parseCurrentCalls(const QVariantMap &message)
{
    const QVariantList &calls = message["current_calls"].toList();
    if (calls.isEmpty()) {
        this->clear();
    } else {
        this->updateCall(calls);
    }
}

void CurrentCall::parseAttendedTransferAnswered(const QVariantMap & /*message*/)
{
    this->transferAnsweredMode();
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
        this->answeringMode();
    }
}

void CurrentCall::clear()
{
    this->m_caller_id.clear();
    this->m_call_start = 0;
    this->updateCallInfo();
    this->noCallsMode();
}

void CurrentCall::numberSelected(const QString &number)
{
    switch(m_requested_action) {
    case ATTENDED_TRANSFER:
        b_engine->sendJsonCommand(MessageFactory::attendedTransfer(number));
        this->transferRingingMode();
        break;
    case CALL:
        b_engine->sendJsonCommand(MessageFactory::dial(number));
        break;
    case DIRECT_TRANSFER:
        b_engine->sendJsonCommand(MessageFactory::directTransfer(number));
        break;
    default:
        break;
    }
}

void CurrentCall::noNumberSelected()
{
    this->answeringMode();
}

void CurrentCall::answer()
{
    b_engine->sendJsonCommand(MessageFactory::answer());
}

void CurrentCall::call()
{
    m_requested_action = CALL;
    signal_relayer->relayNumberSelectionRequested();
    this->m_current_call_widget->btn_call->setShortcut(QString());
}

void CurrentCall::hangup()
{
    b_engine->sendJsonCommand(MessageFactory::hangup());
}

void CurrentCall::hold()
{
    const QString &hold_queue_name = b_engine->getConfig("switchboard_hold_queue_name").toString();
    b_engine->sendJsonCommand(MessageFactory::holdSwitchboard(hold_queue_name));
}

void CurrentCall::attendedTransfer()
{
    m_requested_action = ATTENDED_TRANSFER;
    signal_relayer->relayNumberSelectionRequested();
    this->m_current_call_widget->btn_attended_transfer->setShortcut(QString());
}

void CurrentCall::directTransfer()
{
    m_requested_action = DIRECT_TRANSFER;
    signal_relayer->relayNumberSelectionRequested();
    this->m_current_call_widget->btn_direct_transfer->setShortcut(QString());
}

void CurrentCall::completeTransfer()
{
    b_engine->sendJsonCommand(MessageFactory::completeTransfer());
}

void CurrentCall::cancelTransfer()
{
    b_engine->sendJsonCommand(MessageFactory::cancelTransfer());
    this->answeringMode();
}

void CurrentCall::noCallsMode()
{
    this->disconnectButtons();
    this->setCallButton();
    this->m_current_call_widget->btn_attended_transfer->setEnabled(false);
    m_current_call_widget->btn_attended_transfer->setText(m_attended_transfer_label);
    this->m_current_call_widget->btn_direct_transfer->setEnabled(false);

    this->m_current_call_widget->btn_hold->setEnabled(false);

    this->m_current_call_widget->btn_hangup->setEnabled(false);
    m_current_call_widget->btn_hangup->setText(m_hangup_label);

    this->m_current_call_widget->btn_answer->setEnabled(false);
}

void CurrentCall::ringingMode()
{
    this->disconnectButtons();
    this->setAnswerButton();

    this->m_current_call_widget->btn_call->setEnabled(false);
}

void CurrentCall::answeringMode()
{
    this->disconnectButtons();
    this->setAttendedTransferButton();
    this->setDirectTransferButton();

    this->setHoldButton();

    this->setHangupButton();

    m_current_call_widget->btn_answer->setEnabled(false);
}

void CurrentCall::transferRingingMode()
{
    this->disconnectButtons();

    this->m_current_call_widget->btn_hold->setEnabled(false);
    this->m_current_call_widget->btn_direct_transfer->setEnabled(false);
    this->m_current_call_widget->btn_attended_transfer->setEnabled(false);

    this->setCancelTransferButton();
}

void CurrentCall::transferAnsweredMode()
{
    this->disconnectButtons();
    this->setCompleteTransferButton();

    this->m_current_call_widget->btn_hold->setEnabled(false);
    this->m_current_call_widget->btn_direct_transfer->setEnabled(false);

    this->setCancelTransferButton();
}

void CurrentCall::disconnectButtons()
{
    disconnect(m_current_call_widget->btn_answer, SIGNAL(clicked()),
               this, SLOT(answer()));
    disconnect(m_current_call_widget->btn_attended_transfer, SIGNAL(clicked()),
               this, SLOT(attendedTransfer()));
    disconnect(m_current_call_widget->btn_attended_transfer, SIGNAL(clicked()),
               this, SLOT(completeTransfer()));
    disconnect(m_current_call_widget->btn_direct_transfer, SIGNAL(clicked()),
               this, SLOT(directTransfer()));
    disconnect(m_current_call_widget->btn_hold, SIGNAL(clicked()),
               this, SLOT(hold()));
    disconnect(m_current_call_widget->btn_hangup, SIGNAL(clicked()),
               this, SLOT(hangup()));
    disconnect(m_current_call_widget->btn_hangup, SIGNAL(clicked()),
               this, SLOT(cancelTransfer()));
    disconnect(m_current_call_widget->btn_call, SIGNAL(clicked()),
               this, SLOT(call()));
}

void CurrentCall::setButton(QPushButton *b, const char *slot)
{
    if (! b) {
        qDebug() << Q_FUNC_INFO << "Tried to enable a NULL button";
        return;
    }
    b->setEnabled(true);
    connect(b, SIGNAL(clicked()), this, slot);
}

void CurrentCall::setButton(QPushButton *b, const QKeySequence &k, const char *slot)
{
    this->setButton(b, slot);
    if (! b) {
        return;
    }
    b->setShortcut(k);
}

void CurrentCall::setButton(QPushButton *b, const QString &l, const QKeySequence &k, const char *slot)
{
    this->setButton(b, k, slot);
    if (! b) {
        return;
    }
    b->setText(l);
}

void CurrentCall::setAnswerButton()
{
    this->setButton(
        this->m_current_call_widget->btn_answer,
        SLOT(answer())
    );
}

void CurrentCall::setAttendedTransferButton()
{
    this->m_current_call_widget->btn_attended_transfer->setEnabled(true);
    m_current_call_widget->btn_attended_transfer->setText(m_attended_transfer_label);
    m_current_call_widget->btn_attended_transfer->setShortcut(attended_transfer_key);
    connect(m_current_call_widget->btn_attended_transfer, SIGNAL(clicked()),
            this, SLOT(attendedTransfer()));

void CurrentCall::setCallButton()
{
    this->setButton(
        this->m_current_call_widget->btn_call,
        m_call_label,
        call_key,
        SLOT(call())
    );
}

void CurrentCall::setDirectTransferButton()
{
    this->m_current_call_widget->btn_direct_transfer->setEnabled(true);
    m_current_call_widget->btn_direct_transfer->setShortcut(direct_transfer_key);
    connect(m_current_call_widget->btn_direct_transfer, SIGNAL(clicked()),
            this, SLOT(directTransfer()));
}

void CurrentCall::setCompleteTransferButton()
{
    this->m_current_call_widget->btn_attended_transfer->setEnabled(true);
    m_current_call_widget->btn_attended_transfer->setText(m_complete_transfer_label);
    m_current_call_widget->btn_attended_transfer->setShortcut(attended_transfer_key);
    connect(m_current_call_widget->btn_attended_transfer, SIGNAL(clicked()),
            this, SLOT(completeTransfer()));
}

void CurrentCall::setHoldButton()
{
    this->m_current_call_widget->btn_hold->setEnabled(true);
    connect(m_current_call_widget->btn_hold, SIGNAL(clicked()),
            this, SLOT(hold()));
}

void CurrentCall::setHangupButton()
{
    this->m_current_call_widget->btn_hangup->setEnabled(true);
    m_current_call_widget->btn_hangup->setText(m_hangup_label);
    m_current_call_widget->btn_hangup->setShortcut(hangup_key);
    connect(m_current_call_widget->btn_hangup, SIGNAL(clicked()),
            this, SLOT(hangup()));
}

void CurrentCall::setCancelTransferButton()
{
    this->m_current_call_widget->btn_hangup->setEnabled(true);
    m_current_call_widget->btn_hangup->setText(m_cancel_transfer_label);
    m_current_call_widget->btn_hangup->setShortcut(hangup_key);
    connect(m_current_call_widget->btn_hangup, SIGNAL(clicked()),
            this, SLOT(cancelTransfer()));
}

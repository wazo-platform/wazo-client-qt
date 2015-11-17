/* XiVO Client
 * Copyright (C) 2007-2015 Avencall
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

#include <storage/userinfo.h>

#include "chitchat.h"
#include <QScrollBar>
#include <QVBoxLayout>
#include <QPushButton>


QHash<QString, ChitChatWindow*> ChitChatWindow::m_chat_window_opened = QHash <QString, ChitChatWindow*>();
ChitChatWindow * ChitChatWindow::chitchat_instance = NULL;

void ChitChatWindow::addMessage(
        const QString &mcolor,
        const QString &message,
        const QString &ucolor,
        const QString &username)
{
    QString time = QTime::currentTime().toString("[ HH:mm:ss ]  ");
    QTextCursor recentCursor = m_message_history->textCursor();
    m_message_history->setTextCursor(lastCursor);
    m_message_history->insertHtml("<span style=\"color:black\">" + time + "</span>" +
                                  "<span style=\"color:" + ucolor + "\">" + username + "</span>" +
                                  "<pre style=\"padding:0;margin:0;color:" + mcolor + "\">" +
                                  message + "\n\n</pre>");

    QScrollBar *sb = m_message_history->verticalScrollBar();
    lastCursor = m_message_history->textCursor();
    m_message_history->setTextCursor(recentCursor);
    sb->setValue(sb->maximum());
    if (isVisible() == false) {
        show();
    }
    raise();
}

void ChitChatWindow::addMessage(
        const QString &mcolor,
        const QString &message,
        const QString &ucolor)
{
    QString full_desc = QString("%1: ").arg(m_name);
    this->addMessage(mcolor, message, ucolor, full_desc);
}


ChitChatWindow::ChitChatWindow(const QString &name, const QString &xivo_uuid, int user_id)
    : QWidget(NULL),
      m_name(name),
      m_xivo_uuid(xivo_uuid),
      m_user_id(user_id),
      m_main_instance(false)
{
    qDebug() << Q_FUNC_INFO << m_name << m_xivo_uuid << user_id;

    QVBoxLayout * v_layout = new QVBoxLayout;
    QHBoxLayout * h_layout = new QHBoxLayout;
    QVBoxLayout * v_layout2 = new QVBoxLayout;

    const int message_height = 60;
    v_layout2->setSpacing(2);
    v_layout2->setSizeConstraint(QLayout::SetFixedSize);

    setLayout(v_layout);
    m_message = new MessageEdit(this);
    m_message->setMaximumHeight(message_height);
    m_message_history = new QTextEdit(this);
    m_message_history->setReadOnly(true);
    m_message_history->setTextInteractionFlags(Qt::TextSelectableByMouse);
    lastCursor = m_message_history->textCursor();

    QPushButton *clear_btn = new QPushButton(tr("&Clear history"), this);
    QPushButton *send_btn = new QPushButton(tr("&Send"), this);

    connect(send_btn, SIGNAL(pressed()), m_message, SLOT(sendMessage()));
    connect(clear_btn, SIGNAL(pressed()), this, SLOT(clearMessageHistory()));

    v_layout2->addStretch(1);

    clear_btn->setMaximumHeight(message_height / 3);
    v_layout2->addWidget(clear_btn);
    send_btn->setMaximumHeight(message_height / 3);
    v_layout2->addWidget(send_btn);

    h_layout->addWidget(m_message, 1);
    h_layout->addLayout(v_layout2);

    v_layout->addWidget(m_message_history, 3);
    v_layout->addLayout(h_layout);

    setWindowTitle(tr("chitchat - %1").arg(m_name));
    addMessage("purple", tr("chat window opened with \"%1\"").arg(m_name), "gray", "system: ");

    show();
}


ChitChatWindow::ChitChatWindow()
    : m_main_instance(true)
{
    // qDebug() << Q_FUNC_INFO << "registered";
    registerListener("chitchat");
}

ChitChatWindow::~ChitChatWindow()
{
    if (! m_main_instance) {
        return;
    }

    foreach (const QString &key, m_chat_window_opened.keys()) {
        ChitChatWindow *to_delete = m_chat_window_opened[key];
        m_chat_window_opened.remove(key);
        delete to_delete;
    }
}

ChitChatWindow *ChitChatWindow::findOrNew(const QString &name, const QString &xivo_uuid, int user_id) const
{
    const QString &chat_key = QString("%1/%2").arg(xivo_uuid).arg(user_id);
    if (!m_chat_window_opened.contains(chat_key)) {
        m_chat_window_opened[chat_key] = new ChitChatWindow(name, xivo_uuid, user_id);
    }
    return m_chat_window_opened[chat_key];
}

void ChitChatWindow::parseCommand(const QVariantMap &map)
{
    QVariantList from = map.value("from").toList();
    if (from.size() < 2) {
        qDebug() << "Received an invalid chat message";
        return;
    }
    const QString &xivo_uuid = from[0].toString();
    int user_id = from[1].toInt();
    const QString &msg = map["text"].toString();

    receiveMessage(xivo_uuid, user_id, msg);
}

void ChitChatWindow::receiveMessage(const QString &xivo_uuid, int user_id, const QString &msg)
{
    const UserInfo *user = b_engine->user(QString("xivo/%1").arg(user_id));
    if (!user) {
        qDebug() << Q_FUNC_INFO << "received a message from an unknown user";
        return;
    }

    ChitChatWindow *window = findOrNew(user->fullname(), xivo_uuid, user_id);
    window->addMessage("black", msg, "red");
}


void ChitChatWindow::clearMessageHistory()
{
    m_message_history->setPlainText("");
}


void ChitChatWindow::sendMessage(const QString &message)
{
    addMessage("blue", message, "green", tr("you said: "));

    QVariantMap command;

    command["class"] = "chitchat";
    command["to"] = QVariantList() << m_xivo_uuid << m_user_id;
    command["text"] = message;

    b_engine->sendJsonCommand(command);
}


void ChitChatWindow::writeMessageTo(const QString &name, const QString &xivo_uuid, int user_id)
{
    ChitChatWindow *window = findOrNew(name, xivo_uuid, user_id);
    window->raise();
}

// to remove when the contact xlet is removed
void ChitChatWindow::writeMessageTo()
{
    const QString &xuserid = sender()->property("xuserid").toString();
    const UserInfo *user = b_engine->user(xuserid);
    const QString &name = user->fullname();
    const QString &xivo_uuid = user->xivoUuid();
    const QStringList &ipbxid_id = xuserid.split("/");
    int user_id = ipbxid_id[1].toInt();
    this->writeMessageTo(name, xivo_uuid, user_id);
}

void MessageEdit::sendMessage()
{
    if (toPlainText().trimmed() == "") {
        return ;
    }

    m_dad->sendMessage(toPlainText());
    setPlainText("");
    setFocus(Qt::OtherFocusReason);
}


void MessageEdit::keyPressEvent(QKeyEvent * event)
{
    if (event->text() == "\r") {
        if (event->modifiers() == Qt::ControlModifier) {
            event = new QKeyEvent(event->type(), event->key(), Qt::NoModifier, "\r");
        } else {
            sendMessage();
            return ;
        }
    }
    QTextEdit::keyPressEvent(event);
}

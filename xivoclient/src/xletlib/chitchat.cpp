/* XiVO Client
 * Copyright (C) 2007-2016 Avencall
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

#include <QScrollBar>
#include <QVBoxLayout>
#include <QPushButton>
#include <QTextEdit>

#include <storage/userinfo.h>

#include "chitchat.h"


XLETLIB_EXPORT ChitChatDispatcher* chit_chat;

ChitChatDispatcher::ChitChatDispatcher(QObject *parent)
    : QObject(parent)
{
    registerListener("chitchat");
}

ChitChatDispatcher::~ChitChatDispatcher()
{
    foreach (const QString &key, m_chat_window_opened.keys()) {
        delete m_chat_window_opened.take(key);
    }
}

ChitChatWindow *ChitChatDispatcher::findOrNew(const QString &alias, const QString &xivo_uuid, const QString &user_uuid)
{
    ChitChatWindow *w = NULL;
    if (!m_chat_window_opened.contains(user_uuid)) {
        m_chat_window_opened[user_uuid] = new ChitChatWindow(alias, xivo_uuid, user_uuid);
    }
    w = m_chat_window_opened[user_uuid];
    w->setAlias(alias);
    return w;
}

void ChitChatDispatcher::parseCommand(const QVariantMap &map)
{
    QVariantList from = map.value("from").toList();
    if (from.size() < 2) {
        return;
    }

    const QString &xivo_uuid = from[0].toString();
    const QString &user_uuid = from[1].toString();
    const QString &alias = map["alias"].toString();
    const QString &msg = map["text"].toString();

    receiveMessage(xivo_uuid, user_uuid, alias, msg);
}

void ChitChatDispatcher::receiveMessage(const QString &xivo_uuid, const QString &user_uuid,
                                        const QString &alias, const QString &msg)
{
    ChitChatWindow *window = findOrNew(alias, xivo_uuid, user_uuid);
    window->addMessage("black", msg, "red");
}

void ChitChatDispatcher::showChatWindow(const QString &alias, const QString &xivo_uuid, const QString &user_uuid)
{
    ChitChatWindow *window = findOrNew(alias, xivo_uuid, user_uuid);
    window->popup();
}

ChitChatWindow::ChitChatWindow(const QString &alias, const QString &xivo_uuid, const QString &user_uuid)
    : QWidget(NULL),
      m_remote_alias(alias),
      m_xivo_uuid(xivo_uuid),
      m_user_uuid(user_uuid),
      m_msg_edit(new ChatEditBox(this)),
      m_message_history(new QTextEdit(this))
{
    QVBoxLayout * v_layout = new QVBoxLayout;
    QHBoxLayout * h_layout = new QHBoxLayout;
    QVBoxLayout * v_layout2 = new QVBoxLayout;

    const int message_height = 60;
    v_layout2->setSpacing(2);
    v_layout2->setSizeConstraint(QLayout::SetFixedSize);

    setLayout(v_layout);
    m_msg_edit->setMaximumHeight(message_height);
    m_message_history->setReadOnly(true);
    m_message_history->setTextInteractionFlags(Qt::TextSelectableByMouse);
    lastCursor = m_message_history->textCursor();

    QPushButton *clear_btn = new QPushButton(tr("&Clear history"), this);
    QPushButton *send_btn = new QPushButton(tr("&Send"), this);

    connect(send_btn, SIGNAL(pressed()), SLOT(sendMessage()));
    connect(clear_btn, SIGNAL(pressed()), SLOT(clearMessageHistory()));
    connect(m_msg_edit, SIGNAL(done()), SLOT(sendMessage()));

    v_layout2->addStretch(1);

    clear_btn->setMaximumHeight(message_height / 3);
    v_layout2->addWidget(clear_btn);
    send_btn->setMaximumHeight(message_height / 3);
    v_layout2->addWidget(send_btn);

    h_layout->addWidget(m_msg_edit, 1);
    h_layout->addLayout(v_layout2);

    v_layout->addWidget(m_message_history, 3);
    v_layout->addLayout(h_layout);

    m_local_alias = b_engine->getXivoClientUser()->fullname();
    setAlias(m_remote_alias);
    addMessage("purple", tr("chat window opened with \"%1\"").arg(m_remote_alias), "gray", "system: ");
    show();
}

ChitChatWindow::~ChitChatWindow()
{}

void ChitChatWindow::addMessage(const QString &mcolor, const QString &message, const QString &ucolor, const QString &username)
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
    this->popup();
}

void ChitChatWindow::popup()
{
    this->setWindowState(this->windowState() & ~Qt::WindowMinimized);
    this->raise();
    this->show();
}

void ChitChatWindow::addMessage(const QString &mcolor, const QString &message, const QString &ucolor)
{
    QString full_desc = QString("%1: ").arg(m_remote_alias);
    this->addMessage(mcolor, message, ucolor, full_desc);
}

void ChitChatWindow::clearMessageHistory()
{
    m_message_history->clear();
}

void ChitChatWindow::sendMessage(const QString &message)
{
    addMessage("blue", message, "green", tr("you said: "));

    QVariantMap command;
    command["class"] = "chitchat";
    command["to"] = QVariantList() << m_xivo_uuid << m_user_uuid;
    command["text"] = message;
    command["alias"] = m_local_alias;

    b_engine->sendJsonCommand(command);
}

void ChitChatWindow::sendMessage()
{
    const QString &msg = m_msg_edit->toPlainText();
    if (msg.trimmed() == "") {
        return;
    }

    sendMessage(msg);
    m_msg_edit->clear();
    m_msg_edit->setFocus(Qt::OtherFocusReason);
}

void ChitChatWindow::setAlias(const QString &alias)
{
    m_remote_alias = alias;
    setWindowTitle(tr("chitchat - %1").arg(m_remote_alias));
}

void ChatEditBox::keyPressEvent(QKeyEvent *e)
{
    if (e->text() != "\r") {
        return QPlainTextEdit::keyPressEvent(e);
    }

    if (e->modifiers() == Qt::ControlModifier) {
        insertPlainText(e->text());
    } else {
        emit done();
    }
};

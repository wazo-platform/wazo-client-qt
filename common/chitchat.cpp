/* XiVO Client
 * Copyright (C) 2007-2010, Proformatique
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

/* $Revision$
 * $Date$
 */

#include "chitchat.h"
#include <stdio.h>

QHash<QString, ChitChatWindow*> ChitChatWindow::m_chat_window_opened = QHash <QString, ChitChatWindow*>();
ChitChatWindow * ChitChatWindow::chitchat_instance = NULL;
BaseEngine * ChitChatWindow::m_engine = NULL;

void ChitChatWindow::addMessage(
        const QString &mcolor,
        const QString &message,
        const QString &ucolor="",
        const QString &username="")
{
    QString time = QTime::currentTime().toString("[ HH:mm:ss ]  ");
    m_message_history->insertHtml("<span style=\"color:black\">" + time + "</span>" +
                                  "<span style=\"color:" + ucolor + "\">" + username + "</span>" +
                                  "<pre style=\"padding:0;margin:0;color:" + mcolor + "\">" +
                                  message + "\n\n</pre>");

    QScrollBar *sb = m_message_history->verticalScrollBar();
    sb->setValue(sb->maximum());
}


ChitChatWindow::ChitChatWindow(const QString &with) : QWidget(NULL)
{
    QVBoxLayout *v_layout = new QVBoxLayout;
    QHBoxLayout *h_layout = new QHBoxLayout;
    QVBoxLayout *v_layout2 = new QVBoxLayout;
    int i;

    const int message_height = 60;
    v_layout2->setSpacing(2);
    v_layout2->setSizeConstraint(QLayout::SetFixedSize);

    setLayout(v_layout);
    m_message = new MessageEdit(this);
    m_message->setMaximumHeight(message_height);
    m_message_history = new QTextEdit(this);
    m_message_history->setReadOnly(true);

    QPushButton *button[2];
    button[0]= new QPushButton(tr("&Clear history"));
    button[1]= new QPushButton(tr("&Send"));

    connect(button[1], SIGNAL(pressed()), m_message, SLOT(send_message()));
    connect(button[0], SIGNAL(pressed()), this, SLOT(clear_message_history()));

    v_layout2->addStretch(1);
    for(i=0;i<2;i++) {
        button[i]->setMaximumHeight(message_height/3);
        v_layout2->addWidget(button[i]);
    }

    h_layout->addWidget(m_message, 1);
    h_layout->addLayout(v_layout2);


    v_layout->addWidget(m_message_history, 3);
    v_layout->addLayout(h_layout);

    setWindowTitle(tr("chitchat - %0").arg(m_engine->users()[with]->fullname()));
    m_userid = with;
    show();
}


ChitChatWindow::ChitChatWindow(BaseEngine *b)
{
    m_engine = b;
    b->registerClassEvent("chitchat", ChitChatWindow::receive_message_t, this);
}


void ChitChatWindow::receive_message(const QVariantMap &p)
{
    QString from = p["from"].toString();
    QString text = p["text"].toString();

    QString chat_key = from;
    int opened = 0;
    
    if (m_chat_window_opened.contains(chat_key)) {
        m_chat_window_opened[chat_key]->show();
    } else {
        m_chat_window_opened[chat_key]= new ChitChatWindow(from);
        opened = 1;
    }

    if (opened || (!m_chat_window_opened[chat_key]->isVisible()))
        m_chat_window_opened[chat_key]->addMessage("purple",
            tr("chat window opened with user - ") + m_engine->users()[from]->fullname(), "gray", "system: ");


    m_chat_window_opened[chat_key]->addMessage("black",
        text, "red", m_engine->users()[from]->fullname() + ": ");
}


void ChitChatWindow::clear_message_history()
{
    m_message_history->setPlainText("");
}


void ChitChatWindow::send_message(const QString &message)
{
    addMessage("blue", message, "green", tr("you said: "));

    QVariantMap command;

    command["class"] = "chitchat";
    command["to"] = m_userid;
    command["text"] = message;

    m_engine->sendJsonCommand(command);
}


void ChitChatWindow::WriteMessageTo()
{
    QString astid = sender()->property("astid").toString();
    QString userid = sender()->property("userid").toString();
    
    QString chat_key = userid;
    int opened = 0;
    
    if (m_chat_window_opened.contains(chat_key)) {
        m_chat_window_opened[chat_key]->show();
    } else {
        m_chat_window_opened[chat_key]= new ChitChatWindow(userid);
        opened = 1;
    }

    if (opened || (!m_chat_window_opened[chat_key]->isVisible())) {
        m_chat_window_opened[chat_key]->addMessage("purple",
            tr("chat window opened with user - ") +  m_engine->users()[userid]->fullname(), "gray", tr("system: "));
    }
}




void MessageEdit::send_message()
{
    if (toPlainText().trimmed() == "") {
        return ;
    }

    m_dad->send_message(toPlainText());
    setPlainText("");
    setFocus(Qt::OtherFocusReason);
}


void MessageEdit::keyPressEvent(QKeyEvent * event)
{
    if (event->text() == "\r") {
        if (event->modifiers() == Qt::ControlModifier) {
            event = new QKeyEvent(event->type(), event->key(), Qt::NoModifier, "\r");
        } else {
            send_message();
            return ;
        }
    }
    QTextEdit::keyPressEvent(event);
}

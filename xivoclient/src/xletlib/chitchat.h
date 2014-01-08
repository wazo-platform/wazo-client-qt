/* XiVO Client
 * Copyright (C) 2007-2014 Avencall
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

#ifndef __CHITCHAT_H__
#define __CHITCHAT_H__

#include <QtGui>
#include <baseengine.h>
#include <ipbxlistener.h>

#include "xletlib_export.h"

class MessageEdit;


/*! \brief open a chat window with another xivo user
 */
class XLETLIB_EXPORT ChitChatWindow : public QWidget, IPBXListener
{
    Q_OBJECT

    public:
        static ChitChatWindow *chitchat_instance;

        ChitChatWindow();
        ChitChatWindow(const QString &);

        void parseCommand(const QVariantMap & map);

        void sendMessage(const QString &message);
        void addMessage(const QString &, const QString &, const QString &, const QString &);
        void receiveMessage(const QVariantMap &message);

    public slots:
        void writeMessageTo();
        void clearMessageHistory();

    private:
        QString m_userid;
        static QHash<QString, ChitChatWindow*> m_chat_window_opened;
        MessageEdit *m_message;
        QTextEdit *m_message_history;
        QTextCursor lastCursor;
};


class MessageEdit : public QTextEdit
{
    Q_OBJECT

    public:
        MessageEdit(ChitChatWindow *parent) : QTextEdit((QWidget*) parent) { m_dad = parent; };

    public slots:
        void sendMessage();

    private:
        ChitChatWindow *m_dad;

    protected:
        virtual void keyPressEvent(QKeyEvent * event);
};

#endif

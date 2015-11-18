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

#ifndef __CHITCHAT_H__
#define __CHITCHAT_H__

#include <QtGui>
#include <baseengine.h>
#include <ipbxlistener.h>
#include <QWidget>
#include <QPlainTextEdit>

#include "xletlib_export.h"

class QTextEdit;
class ChitChatWindow;

class ChatEditBox: public QPlainTextEdit
{
    Q_OBJECT

    public:
        ChatEditBox(QWidget *parent): QPlainTextEdit(parent) {}
    protected:
        virtual void keyPressEvent(QKeyEvent *e);
    signals:
         void done();
};


class XLETLIB_EXPORT ChitChatDispatcher: public QObject, IPBXListener
{
    Q_OBJECT

    public:
        ChitChatDispatcher(QObject *parent);
        virtual ~ChitChatDispatcher();
        void parseCommand(const QVariantMap & map);
        void receiveMessage(const QString &xivo_uuid, int user_id, const QString &msg);
        void writeMessageTo(const QString &name, const QString &xivo_uuid, int user_id);

    public slots:
        void writeMessageTo();

    private:
        ChitChatDispatcher();
        ChitChatWindow *findOrNew(const QString &name, const QString &xivo_uuid, int user_id);
        QHash<QString, ChitChatWindow*> m_chat_window_opened;
};

class XLETLIB_EXPORT ChitChatWindow : public QWidget
{
    Q_OBJECT

    public:
        ChitChatWindow(const QString &name, const QString &xivo_uuid, int user_id);
        virtual ~ChitChatWindow();

        void addMessage(const QString &, const QString &, const QString &, const QString &);
        void addMessage(const QString &, const QString &, const QString &);
        void sendMessage(const QString &msg);

    public slots:
        void clearMessageHistory();
        void sendMessage();

    private:
        ChitChatWindow();
        QString m_name;
        QString m_xivo_uuid;
        int m_user_id;
        ChatEditBox *m_msg_edit;
        QTextEdit *m_message_history;
        QTextCursor lastCursor;
};

extern XLETLIB_EXPORT ChitChatDispatcher *chit_chat;

#endif

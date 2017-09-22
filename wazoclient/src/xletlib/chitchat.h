/*
 * Copyright 2007-2017 The Wazo Authors  (see the AUTHORS file)
 *
 * This program is free software: you can redistribute it and/or modify
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
 * This programm is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
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
        void receiveMessage(const QString &xivo_uuid, const QString &user_uuid, const QString &alias, const QString &msg);
        void showChatWindow(const QString &alias, const QString &xivo_uuid, const QString &user_uuid);

    private:
        ChitChatDispatcher();
        ChitChatWindow *findOrNew(const QString &alias, const QString &xivo_uuid, const QString &user_uuid);
        QHash<QString, ChitChatWindow*> m_chat_window_opened;
};

class XLETLIB_EXPORT ChitChatWindow : public QWidget
{
    Q_OBJECT

    public:
        ChitChatWindow(const QString &alias, const QString &xivo_uuid, const QString &user_uuid);
        virtual ~ChitChatWindow();

        void addMessage(const QString &, const QString &, const QString &, const QString &);
        void addMessage(const QString &, const QString &, const QString &);
        void popup();
        void sendMessage(const QString &msg);
        void setAlias(const QString &alias);

    public slots:
        void clearMessageHistory();
        void sendMessage();

    private:
        QString m_remote_alias;
        QString m_local_alias;
        QString m_xivo_uuid;
        QString m_user_uuid;
        ChatEditBox *m_msg_edit;
        QTextEdit *m_message_history;
        QTextCursor lastCursor;
};

extern XLETLIB_EXPORT ChitChatDispatcher *chit_chat;

#endif

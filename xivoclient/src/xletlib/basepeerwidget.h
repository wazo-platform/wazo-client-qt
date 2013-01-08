/* XiVO Client
 * Copyright (C) 2007-2013, Avencall
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

#ifndef __BASEPEERWIDGET_H__
#define __BASEPEERWIDGET_H__

#include "xletlib_export.h"

#include <QtGui>
#include <QList>

#include <baseengine.h>
#include "chitchat.h"
#include "xivoconsts.h"
#include <userinfo.h>


class XLETLIB_EXPORT BasePeerWidget : public QWidget
{
    Q_OBJECT

    public:
        BasePeerWidget(const UserInfo *);

        virtual void setName(const QString &) = 0;
        virtual void updateAgentConfig(const QString &) = 0;
        virtual void updateAgentStatus(const QString &) = 0;
        virtual void updatePhoneConfig(const QString &) = 0;
        virtual void updatePhoneStatus(const QString &) = 0;
        virtual void removePhoneConfig(const QString &) {}
        virtual void updatePresence() = 0;

        virtual const QString& number() const { return m_number; }
        virtual QString name() const { return m_ui_remote ? m_ui_remote->fullname() : QString(); }
        virtual QString id() const;
        int maxWidthWanted() const { return m_maxWidthWanted; }
        void reloadSavedName();

    protected:
        void mouseDoubleClickEvent(QMouseEvent *);
        void mousePressEvent(QMouseEvent *);
        void mouseMoveEvent(QMouseEvent *);
        void contextMenuEvent(QContextMenuEvent *);
        void dragEnterEvent(QDragEnterEvent *);
        void dragMoveEvent(QDragMoveEvent *);
        void dropEvent(QDropEvent *);

        bool isme() const;
        void updateMenuSelf(QMenu *, QContextMenuEvent *);
        void updateMenuPeer(QMenu *, QContextMenuEvent *);
        void addChitChatMenu(QMenu *);
        void addDialMenu(QMenu *);
        void addEditMenu(QMenu *);
        void addHangupMenu(QMenu *);
        void addInterceptMenu(QMenu *);
        void addMeetmeMenu(QMenu *);
        void addTxferMenu(QMenu *, bool);
        void addTxferVmMenu(QMenu *);
    private:
        QList<const ChannelInfo *> loopOverChannels(const UserInfo *);
        bool canDrag(const QMouseEvent *event) const {
          return (isLeftClick(event) && isme() && isSwitchBoard()); }
        bool isLeftClick(const QMouseEvent *) const;
        bool isSwitchBoard() const;
        QMenu *getTransferMenu(QMenu *basemenu,
                               const QString & title,
                               bool add_sub_menu);
        QAction * newBlindTransferAction(const QString &number,
                                         const ChannelInfo &channel);
        QAction * newAttendedTransferAction(const QString &number,
                                            const ChannelInfo &channel);
        QStringList getPeerNumbers() const;
    signals:
        void selectedNumber(const QStringList &);
    protected slots:
        void dial();
        void dialMobilePhone();
        void hangup();
        void intercept();
        void transfer();
        void inviteConfRoom();
        void itransfer();
        void itransfercancel();
        void peerdial();
        void vmtransfer();

    protected:
        const UserInfo * m_ui_local;
        const UserInfo * m_ui_remote;
        QPoint m_dragstartpos;

        QAction *m_interceptAction;
        QAction *m_chitchatAction;
        QString m_number;
        bool m_editable;
        bool m_transferred;
        int m_maxWidthWanted;
        QMenu * m_contextMenu;
        QList<QMenu *> m_submenus;
};

#include "peerwidget.h"

#endif

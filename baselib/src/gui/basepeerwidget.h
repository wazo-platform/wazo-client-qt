/* XiVO Client
 * Copyright (C) 2007-2011, Proformatique
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

#ifndef __BASEPEERWIDGET_H__
#define __BASEPEERWIDGET_H__

#include "baselib_export.h"

#include <QtGui>

#include <baseengine.h>
#include <chitchat.h>

class UserInfo;

/*! \brief Abstract class for "Peer" Widgets
 *
 */
class BASELIB_EXPORT BasePeerWidget : public QWidget
{
    Q_OBJECT

    public:
        BasePeerWidget(const UserInfo *);

        virtual void setName(const QString &) = 0;  //! change username to be displayed
        virtual void updateAgentConfig(const QString &) = 0;
        virtual void updateAgentStatus(const QString &) = 0;
        virtual void updatePhoneConfig(const QString &) = 0;
        virtual void updatePhoneStatus(const QString &) = 0;
        virtual void updatePresence() = 0;  //! update presence information displayed

        virtual const QString& number() const { return m_number; };  //! Phone number
        virtual QString name() const;  //! Name
        virtual QString id() const;  //! return a unique id for the item
        int maxWidthWanted() const { return m_maxWidthWanted; };  //! maximum width for "small" items
        void reloadSavedName();  //! reload the saved name

    protected:
        void mouseDoubleClickEvent(QMouseEvent *);
        void mousePressEvent(QMouseEvent *);
        void mouseMoveEvent(QMouseEvent *);
        void contextMenuEvent(QContextMenuEvent *);
        void dragEnterEvent(QDragEnterEvent *);
        void dragMoveEvent(QDragMoveEvent *);
        void dropEvent(QDropEvent *);
        bool event(QEvent *);
        QString xletName() const;
        // Menu helper functions
        bool isme() const;
        void updateMenuSelf(QMenu *, QContextMenuEvent *);
        void updateMenuPeer(QMenu *, QContextMenuEvent *);
        void updateMenuSwitchboard(QMenu *);
        void addChitChatMenu(QMenu *);
        void addDialMenu(QMenu *);
        void addEditMenu(QMenu *);
        void addHangupMenu(QMenu *);
        void addInterceptMenu(QMenu *);
        void addMeetmeMenu(QMenu *);
        void addParkingMenu(QMenu *);
        void addSwitchboardMenu(QMenu *);
        void addTxferMenu(QMenu *, bool);
        void addTxferVmMenu(QMenu *);
    private:
        QList<const ChannelInfo *> loopOverChannels(const UserInfo *);
    signals:
        void removeFromPanel(); //! hide the widget from the containing window
        void selectedNumber(const QStringList &);
    protected slots:
        void dial();
        void dialMobilePhone();
        void peerdial();
        void hangup();
        void intercept();
        void transfer();
        void itransfer();
        void itransfercancel();
        void vmtransfer();
        void parkcall();
        void tryRemoveFromPanel();
        void rename();

    protected:
        const UserInfo * m_ui_local;  //!< user info structure for the current operator
        const UserInfo * m_ui_remote;  //!< user info structure for the widget
        QPoint m_dragstartpos;  //!< drag start position

        QAction *m_removeAction;  //!< action to remove this peer from the window
        QAction *m_interceptAction;  //!< action to intercept ringing calls to this number
        QAction *m_renameAction;  //<! action to rename the user
        QAction *m_chitchatAction;  //!< action to speak with an user
        QString m_number;  //!< phone number (filled if m_ui is NULL)
        bool m_editable;  //!< editable from a user point of view
        bool m_transfered;  //!< if this call was transfered
        int m_maxWidthWanted;  //!< maximum width for "small" items
};

#include "peerwidget.h"

#endif

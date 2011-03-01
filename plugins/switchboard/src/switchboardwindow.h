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

#ifndef __SWITCHBOARDWINDOW_H__
#define __SWITCHBOARDWINDOW_H__

#include <QtGui>
#include <xletinterface.h>
#include <xlet.h>

class PeerItem;
class PeersLayout;
class BasePeerWidget;
class UserInfo;
class Group;

/*! \brief Widget displaying PeerItems
 *
 * This widget use a PeersLayout to display PeerItems in a grid.
 */
class XletSwitchBoard : public XLet
{
    Q_OBJECT

    public:
        XletSwitchBoard(QWidget *parent=0);
        ~XletSwitchBoard();

    protected:
        void mousePressEvent(QMouseEvent *);
        void mouseMoveEvent(QMouseEvent *);
        void mouseReleaseEvent(QMouseEvent *);
        void paintEvent(QPaintEvent *event);
        void dragEnterEvent(QDragEnterEvent *);
        void dropEvent(QDropEvent *);
        void dragMoveEvent(QDragMoveEvent *);
        void contextMenuEvent(QContextMenuEvent *);

    public slots:
        void removePeer(const QString &);
        void removePeers();
        void updateUserConfig(const QString &);
        void updateUserStatus(const QString &);
        void updateAgentConfig(const QString &);
        void updateAgentStatus(const QString &);
        void updatePhoneConfig(const QString &);
        void updatePhoneStatus(const QString &);

    private slots:
        void removePeerFromLayout();
        void removeGroup();
        void changeGroupColor();
        void changeGroupName();
        void addPhoneNumberEntry();
        void drawTheGrid(bool yes = true);
        void savePositions() const;

    private:
        void saveGroups() const;
        void reloadGroups();
        void reloadExternalPhones();
        BasePeerWidget *getExternalPhonePeerWidget(const QString & number);
        BasePeerWidget *addPeerWidget(PeerItem * peeritem, const QPoint & pos);
        Group* getGroup(const QPoint &) const;

        QHash<QString, PeerItem *> m_peerhash;  //!< PeerItem hash
        PeersLayout *m_layout;  //!< Grid Layout for displaying peers
        int m_drawGrid; //!< display a grid ?
        // for the groups of people :
        bool m_trace_box;  //!< is box drawing enable
        QPoint m_first_corner;  //!< first corner of the box being drawn
        QPoint m_second_corner;  //!< second corner of the box being drawn
        QList<Group *> m_group_list;  //!< list of the groups
        Group *m_group_to_resize;  //!< group being moved/resized
        int m_group_resize_mode;
#define ETop             1
#define EBottom   (ETop<<2)
#define ERight    (ETop<<3)
#define ELeft     (ETop<<4)
#define EMove     (ETop<<5)
};

class XLetSwitchboardPlugin : public QObject, XLetInterface
{
    Q_OBJECT
    Q_INTERFACES(XLetInterface)

    public:
        XLet* newXLetInstance(QWidget *parent=0);
};

#endif

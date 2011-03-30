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

#ifndef __CALLWIDGET_H__
#define __CALLWIDGET_H__

// QT includes.
#include <QDateTime>
#include <QObject>
#include <QWidget>

#include "phoneinfo.h"

class QLabel;
class QGridLayout;

class UserInfo;

/*! \brief Widget displaying a call (channel)
 *
 * The Call is displayed with a colored square representing
 * its state, the id of the channel, the direction and
 * the source of the call
 */
class CallWidget : public QWidget
{
    Q_OBJECT

    public:
        CallWidget(UserInfo *,
                   const QString &,
                   QWidget * parent);
        void updateWidget(const QString &);
    protected:
        void mousePressEvent(QMouseEvent *);
        void mouseMoveEvent(QMouseEvent *);
        void timerEvent(QTimerEvent *);
        void contextMenuEvent(QContextMenuEvent *);
    private:
        void setActionPixmap();
        void updateCallTimeLabel();
    signals:
        void doHangUp(const QString &);  //!< hang up the channel
        void doTransferToNumber(const QString &);  //!< transfers the channel
        void doParkCall(const QString &);  //!< parks the channel
    public slots:
        void hangUp();
        void transferToNumber();
        void parkCall();
    private:
        UserInfo * m_ui;  //!< monitored user infos
        QString m_xchannel;  //!< channel identifier
        bool m_parkedCall;  //!< Is it a parked call ?

        QPoint m_dragstartpos;  //!< used for drag
        QGridLayout * m_gridlayout;  //!< monitored user infos
        QLabel * m_lbl_status;  //!< sub widget
        QLabel * m_lbl_time;  //!< sub widget
        QLabel * m_lbl_direction;  //!< sub widget
        QLabel * m_lbl_exten;  //!< sub widget
        QPixmap m_square;  //!< QPixmap used to display the status square

        QAction * m_hangUpAction;  //!< Hang Up Action
        QAction * m_transferToNumberAction;  //!< Transfer to Number Action
        QAction * m_parkCall;  //!< Park the Call Action
};

#endif

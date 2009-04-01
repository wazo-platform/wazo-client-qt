/* XIVO CTI clients
 * Copyright (C) 2007-2009  Proformatique
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License version 2 for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Linking the Licensed Program statically or dynamically with other
 * modules is making a combined work based on the Licensed Program. Thus,
 * the terms and conditions of the GNU General Public License version 2
 * cover the whole combination.
 *
 * In addition, as a special exception, the copyright holders of the
 * Licensed Program give you permission to combine the Licensed Program
 * with free software programs or libraries that are released under the
 * GNU Library General Public License version 2.0 or GNU Lesser General
 * Public License version 2.1 or any later version of the GNU Lesser
 * General Public License, and with code included in the standard release
 * of OpenSSL under a version of the OpenSSL license (with original SSLeay
 * license) which is identical to the one that was published in year 2003,
 * or modified versions of such code, with unchanged license. You may copy
 * and distribute such a system following the terms of the GNU GPL
 * version 2 for the Licensed Program and the licenses of the other code
 * concerned, provided that you include the source code of that other code
 * when and as the GNU GPL version 2 requires distribution of source code.
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

class QLabel;

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
    //! Default constructor
    CallWidget(QWidget * parent = 0);
    ~CallWidget();
    CallWidget(UserInfo *,
               const QString &,
               const QString &,
               uint,
               const QString &,
               const QString &,
               const QString &,
               QWidget * parent = 0);
    void updateWidget(const QString &,
                      uint,
                      const QString &,
                      const QString &,
                      const QString &);
    const QString & channel() const;
protected:
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    //void mouseDoubleClickEvent(QMouseEvent * event);
    //void dragEnterEvent(QDragEnterEvent * event);
    //        void dragMoveEvent(QDragMoveEvent * event);
    //        void dropEvent(QDropEvent * event);
    void timerEvent(QTimerEvent *);
    void contextMenuEvent(QContextMenuEvent *);
private:
    void setActionPixmap(const QString &);
    void updateCallTimeLabel();
signals:
    void doHangUp(const QString &);        //!< hang up the channel
    void doTransferToNumber(const QString &);        //!< transfers the channel
    void doParkCall(const QString &);        //!< parks the channel
public slots:
    void hangUp();
    void transferToNumber();
    void parkCall();
private:
    UserInfo * m_ui;            //!< monitored user infos
    QPoint m_dragstartpos;        //!< used for drag
    QString m_channelme;        //!< channel identifier
    QString m_channelpeer;      //!< peer channel identifier
    QString m_callerid;                //!< caller id
    QString m_calleridname;        //!< caller id name
    QLabel * m_lbl_status;        //!< sub widget
    QLabel * m_lbl_time;        //!< sub widget
    QLabel * m_lbl_direction;        //!< sub widget
    QLabel * m_lbl_channelpeer;        //!< sub widget
    QLabel * m_lbl_exten;        //!< sub widget
    QPixmap m_square;                //!< QPixmap used to display the status square
    static QPixmap * m_call_yellow; //!< yellow phone icon
    static QPixmap * m_call_blue;   //!< blue phone icon
    static QPixmap * m_call_red;    //!< red phone icon
    static QPixmap * m_call_gray;   //!< gray phone icon
    QDateTime m_startTime;        //!< call start date/time
    QAction * m_hangUpAction;        //!< Hang Up Action
    QAction * m_transferToNumberAction;        //!< Transfer to Number Action
    QAction * m_parkCall;        //!< Park the Call Action
};

#endif

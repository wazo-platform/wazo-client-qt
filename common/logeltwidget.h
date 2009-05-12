/* XiVO Client
 * Copyright (C) 2007-2009, Proformatique
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

#ifndef __LOGELTWIDGET_H__
#define __LOGELTWIDGET_H__

#include <QDateTime>
#include <QWidget>

class QAction;
class QContextMenuEvent;
class QMouseEvent;

/*! \brief Log element widget
 */
class LogEltWidget : public QWidget
{
    Q_OBJECT
public:
    //! Call direction (out or in)
    typedef enum { OutCall = 1, InCall = 2 } Direction;
    LogEltWidget(const QString &,
                 Direction,
                 const QDateTime &,
                 int,
                 const QString &,
                 QWidget * parent = 0 );
    const QDateTime & dateTime() const;        //! get m_dateTime
    const QString & peer() const;                //! get m_peer
    Direction direction() const;                //! get m_direction
protected:
    void contextMenuEvent(QContextMenuEvent *);
    void mouseDoubleClickEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
private slots:
    void callBackPeer();
    void doNotCallBackPeer();
signals:
    void actionCall(const QString &, const QString &, const QString &);        //!< signal to dial back.
    void copyNumber(const QString &);
private:
    QDateTime m_dateTime;        //!< date time of the call
    QString m_peer;                        //!< phone number who called/was called
    Direction m_direction;        //!< call direction (In/out)
    QAction * m_dialAction;        //!< dial action
};

#endif

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

#ifndef __LOGWIDGET_H__
#define __LOGWIDGET_H__

#include <QDateTime>
#include "xlet.h"
#include "logeltwidget.h"

class QVBoxLayout;
class QRadioButton;

class BaseEngine;
class UserInfo;

/*! \brief Call Log display widget
 */
class LogWidget : public XLet
{
    Q_OBJECT
public:
    LogWidget(BaseEngine *,
              QWidget * parent = 0);
    void addElement(const QString &,
                    LogEltWidget::Direction,
                    const QDateTime &,
                    int,
                    const QString &);
protected:
    void timerEvent(QTimerEvent *);
private:
    int mode();
public slots:
    void setGuiOptions(const QVariant &) {};
    void clear();
    void addLogEntry(const QDateTime &, int,
                     const QString &, const QString &, const QString &);
    void monitorPeer(UserInfo *);
private slots:
    void modeChanged(bool);
signals:
    void askHistory(const QString &, int, const QDateTime &);        //!< need history to be updated !
    void actionCall(const QString &, const QString &, const QString &);
private:
    QVBoxLayout * m_layout;          //!< Widget layout
    QString m_peer;                  //!< "monitored" peer
    int m_timer;                     //!< timer id for refresh
    QRadioButton * m_radioOut;       //!< "Out" radio button
    QRadioButton * m_radioIn;        //!< "In" radio button
    QRadioButton * m_radioMissed;    //!< "Missed" radio button
    QDateTime m_moreRecent;          //!< dateTime of the more recent item
};

#endif

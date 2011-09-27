/* XiVO Client
 * Copyright (C) 2011, Proformatique
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

#ifndef __INCOMINGWIDGET_H__
#define __INCOMINGWIDGET_H__

#include "pendingwidget.h"

class UserInfo;
class QPushButton;

/*! \brief A widget to display relevant information about an incoming call
 *
 *  A new widget should be created for each incoming call to the operator'S
 *  phone. The widget will display some information about this call and allow
 *  the user to interact with the call with a set of buttons.
 */
class IncomingWidget: public PendingWidget
{
    Q_OBJECT

public:
    IncomingWidget(int line, const QString &,
                   const QString &, QWidget *);
    void update();
    bool toRemove() const;
    int line() const { return m_line; };
public slots:
    void doIgnore();
    void doPickup();
protected:
    void buildui();
signals:
    void pickup(int);
    void ignore(int);
    void remove_me(int);
private:
    int m_line;
    QString m_channel_xid;
    QPushButton * m_btn_ignore;
    QPushButton * m_btn_answer;
};

#endif

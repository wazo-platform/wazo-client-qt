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

#ifndef __XLETTEST_H__
#define __XLETTEST_H__

#include <QObject>
#include "xlet.h"

class QLabel;
class QPushButton;

/*! \brief XletTest Panel
 *
 * This is a sample XLet, just displaying "Hello World!"
 */
class XletTest : public XLet
{
    Q_OBJECT
public:
    XletTest(QWidget *parent=0);
    ~XletTest();
public slots:
private slots:
    void onCallClick();
    void onHangupClick();
    void onSoftKey();
    void onHoldClick();
    void onLineOneClick();
    void onLineTwoClick();
    void onLineThreeClick();
    void onLineFourClick();
    void onPrg2Click();
private:
    QPushButton *m_button_call;
    QPushButton *m_button_hangup;
    QPushButton *m_button_dnd;
    QPushButton *m_button_hold;
    QPushButton *m_button_l1;
    QPushButton *m_button_l2;
    QPushButton *m_button_l3;
    QPushButton *m_button_l4;
    QPushButton *m_button_prg2;
};

#endif /* __XLETTEST_H__ */


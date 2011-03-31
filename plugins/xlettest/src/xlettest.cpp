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

#include <QDebug>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVariantMap>
#include <QStringList>

#include "xlettest.h"
#include "userinfo.h"
#include "phoneinfo.h"
#include "aastrasipnotify.h"

#define SPECIAL_ME "user:special:me"

/*! \brief Constructor
 *
 * Test xlet to try new features
 */
XletTest::XletTest(QWidget *parent)
    : XLet(parent)
{
    // set title for the XLet.
    setTitle(tr("Testing xlet"));
    QVBoxLayout *layout = new QVBoxLayout(this);
    m_button_call = new QPushButton(tr("Call 1001"));
    m_button_hangup = new QPushButton(tr("hangup"));
    m_button_dnd = new QPushButton(tr("DnD"));
    m_button_hold = new QPushButton("Hold");
    m_button_l1 = new QPushButton("line 1");
    m_button_l2 = new QPushButton("line 2");
    m_button_l3 = new QPushButton("line 3");
    m_button_l4 = new QPushButton("line 4");
    m_button_prg2 = new QPushButton("Prg2 (Directed call pickup)");
    layout->addWidget(m_button_call);
    layout->addWidget(m_button_hangup);
    layout->addWidget(m_button_dnd);
    layout->addWidget(m_button_hold);
    layout->addWidget(m_button_l1);
    layout->addWidget(m_button_l2);
    layout->addWidget(m_button_l3);
    layout->addWidget(m_button_l4);
    layout->addWidget(m_button_prg2);
    connect(m_button_call, SIGNAL(clicked()), this, SLOT(onCallClick()));
    connect(m_button_hangup, SIGNAL(clicked()), this, SLOT(onHangupClick()));
    connect(m_button_dnd, SIGNAL(clicked()), this, SLOT(onSoftKey()));
    connect(m_button_l1, SIGNAL(clicked()), this, SLOT(onLineOneClick()));
    connect(m_button_l2, SIGNAL(clicked()), this, SLOT(onLineTwoClick()));
    connect(m_button_l3, SIGNAL(clicked()), this, SLOT(onLineThreeClick()));
    connect(m_button_l4, SIGNAL(clicked()), this, SLOT(onLineFourClick()));
    connect(m_button_hold, SIGNAL(clicked()), this, SLOT(onHoldClick()));
    connect(m_button_prg2, SIGNAL(clicked()), this, SLOT(onPrg2Click()));
}

void XletTest::onHoldClick()
{
    emit ipbxCommand(getAastraKeyNotify(HOLD, SPECIAL_ME));
}

void XletTest::onLineOneClick()
{
    emit ipbxCommand(getAastraKeyNotify(LINE, SPECIAL_ME, 1));
}

void XletTest::onLineTwoClick()
{
    emit ipbxCommand(getAastraKeyNotify(LINE, SPECIAL_ME, 2));
}

void XletTest::onLineThreeClick()
{
    emit ipbxCommand(getAastraKeyNotify(LINE, SPECIAL_ME, 3));
}

void XletTest::onLineFourClick()
{
    emit ipbxCommand(getAastraKeyNotify(LINE, SPECIAL_ME, 4));
}

/*! \brief test button clicked */
void XletTest::onCallClick()
{
    emit ipbxCommand(getAastraDial("1001", SPECIAL_ME, true));
}

void XletTest::onHangupClick()
{
    emit ipbxCommand(getAastraKeyNotify(GOODBYE, SPECIAL_ME));
}

void XletTest::onPrg2Click()
{
    emit ipbxCommand(getAastraKeyNotify(PRG_KEY, SPECIAL_ME, 2));
}

void XletTest::onSoftKey()
{
    emit ipbxCommand(getAastraKeyNotify(PRG_KEY, SPECIAL_ME, 1));
}

/*! \brief destructor
 */
XletTest::~XletTest()
{
    qDebug() << Q_FUNC_INFO;
}


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
    QHBoxLayout *layout = new QHBoxLayout(this);
    m_button_call = new QPushButton(tr("Call 1001"));
    m_button_hangup = new QPushButton(tr("hangup"));
    m_button_dnd = new QPushButton(tr("DnD"));
    layout->addWidget(m_button_call);
    layout->addWidget(m_button_hangup);
    layout->addWidget(m_button_dnd);
    connect(m_button_call, SIGNAL(clicked()), this, SLOT(onCallClick()));
    connect(m_button_hangup, SIGNAL(clicked()), this, SLOT(onHangupClick()));
    connect(m_button_dnd, SIGNAL(clicked()), this, SLOT(onSoftKey()));
}

/*! \brief test button clicked
 */
void XletTest::onCallClick()
{
    emit ipbxCommand(getAastraDial("1001", SPECIAL_ME, true));
}

void XletTest::onHangupClick()
{
    emit ipbxCommand(getAastraKeyNotify(GOODBYE, SPECIAL_ME));
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


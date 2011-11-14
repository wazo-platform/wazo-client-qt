/* XiVO Client
 * Copyright (C) 2011, Avencall
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

#include "incomingwidget.h"

#include "baseengine.h"
#include "channelinfo.h"
#include "phoneinfo.h"

#include <QDebug>
#include <QPushButton>
#include <QHBoxLayout>

IncomingWidget::IncomingWidget(int line, const QString & channelxid,
                               const QString & phonexid, QWidget * w)
    : PendingWidget(phonexid, w), m_line(line), m_channel_xid(channelxid),
      m_btn_answer(0)
{
}

void IncomingWidget::buildui()
{
    this->PendingWidget::buildui(false);

    m_btn_answer = new QPushButton(tr("Answer"), this);
    m_btn_ignore = new QPushButton(tr("Ignore"), this);

    layout()->addWidget(m_btn_answer);
    layout()->addWidget(m_btn_ignore);

    connect(m_btn_ignore, SIGNAL(clicked()), this, SLOT(doIgnore()));
    connect(m_btn_answer, SIGNAL(clicked()), this, SLOT(doPickup()));

    connect(this, SIGNAL(ignore(int)), parent(), SLOT(hangupLine(int)));
    connect(this, SIGNAL(pickup(int)), parent(), SLOT(selectLine(int)));
    connect(this, SIGNAL(remove_me(int)), parent(), SLOT(remove_incoming(int)));
}

void IncomingWidget::update()
{
    if (! m_btn_answer) {
        buildui();
    }

    QString display;
    if (const ChannelInfo * c = b_engine->channel(m_channel_xid)) {
        display = c->peerdisplay();
    } else if (const PhoneInfo * p = b_engine->phone(phonexid())) {
        display = p->number();
    } else {
        display = tr("Incoming call");
    }
    set_string(QString("%0 %1").arg(display).arg(started_since()));
}

bool IncomingWidget::toRemove() const
{
    if (! b_engine->channel(m_channel_xid)) return true;
    return false;
}

void IncomingWidget::doIgnore()
{
    emit ignore(m_line);
    emit remove_me(m_line);
}

void IncomingWidget::doPickup()
{
    emit pickup(m_line);
    emit remove_me(m_line);
}

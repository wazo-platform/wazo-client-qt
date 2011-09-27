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

#include "holdedwidget.h"


#include "baseengine.h"
#include "channelinfo.h"
#include "phoneinfo.h"

HoldedWidget::HoldedWidget(const QString & phonexid, int line, QWidget * parent)
    : PendingWidget(phonexid, parent), m_line(line)
{
}

void HoldedWidget::update()
{
    if (!layout()) {
        buildui();
    }

    static QString base = QString("%0 %1 %2");
    static QString message = tr("on hold");
    const PhoneInfo * p = b_engine->phone(phonexid());
    if (p && p->xchannels().size()) {
        const ChannelInfo * c = b_engine->channel(p->xchannels().last());
        QString s = (QString(base)
                     .arg((c ? c->thisdisplay() : p->number()))
                     .arg(message)
                     .arg(started_since()));
        set_string(s);
    } else {
        qDebug() << Q_FUNC_INFO << "Cannot update on hold call info";
    }
}

bool HoldedWidget::toRemove() const
{
    const PhoneInfo * p = b_engine->phone(phonexid());
    if (! p || ! p->xchannels().size()) return true;

    const QString & chanxid = p->xchannels().last();
    const ChannelInfo * c = b_engine->channel(chanxid);
    if (c && c->isholded()) {
        return false;
    }
    return false;
}

void HoldedWidget::doPickup()
{
    emit pickup(m_line);
    emit remove_me(id());
}

void HoldedWidget::buildui()
{
    this->PendingWidget::buildui(true);
    connect(this, SIGNAL(pickup(int)), parent(), SLOT(selectLine(int)));
}


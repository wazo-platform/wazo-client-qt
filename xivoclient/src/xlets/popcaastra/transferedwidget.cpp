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

#include "transferedwidget.h"

#include "baseengine.h"
#include "phoneinfo.h"

#include <QDebug>

TransferedWidget::TransferedWidget(const QString & phonexid,
                                   const QString & number,
                                   QWidget * parent)
    : PendingWidget(phonexid, parent), m_called_num(number)
{
}

void TransferedWidget::buildui()
{
    this->PendingWidget::buildui(true);
    connect(this, SIGNAL(pickup(const QString &)),
            parent(), SLOT(doIntercept(const QString &)));
}

void TransferedWidget::update()
{
    if (! layout()) {
        buildui();
    }

    static QString base = QString("%0 %1 %2 %3 %4");
    static QString message = tr("transfered to");
    if (const PhoneInfo * p = b_engine->phone(phonexid())) {
        const QStringList & channels = p->xchannels();
        if (channels.size() > 0) {
            const QString & chanxid = channels.last();
            const ChannelInfo * c = b_engine->channel(chanxid);
            QString status_string;
            if (b_engine->getOptionsPhoneStatus().contains(p->hintstatus())) {
                status_string = (b_engine->getOptionsPhoneStatus()
                                 .value(p->hintstatus()).toMap()
                                 .value("longname").toString());
            }
            QString s = (QString(base)
                         .arg(c->thisdisplay())
                         .arg(message)
                         .arg(c->peerdisplay())
                         .arg(status_string)
                         .arg(started_since()));
            set_string(s);
        }
    }
}

bool TransferedWidget::toRemove() const
{
    const PhoneInfo * p = b_engine->phone(phonexid());
    if (! p || ! p->xchannels().size()) return true;
    const QString & chanxid = p->xchannels().last();

    // Matches my phone's id if it's too early to have the called's info
    const QStringList & my_identities = 
        b_engine->user(b_engine->getFullId())->identitylist();
    const ChannelInfo * c = b_engine->channel(chanxid);
    bool match = false;
    foreach (const QString & identity, my_identities) {
        if (c->talkingto_id().contains(identity)) {
            match = true;
            break;
        }
    }
    if (match) return false;

    if (c->commstatus() != "calling") return true;
    return false;
}

void TransferedWidget::doPickup()
{
    emit pickup(m_called_num);
    emit remove_me(id());
}

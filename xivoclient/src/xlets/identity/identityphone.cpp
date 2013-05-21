/* XiVO Client
 * Copyright (C) 2007-2013, Avencall
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

#include <QDebug>
#include <QLabel>
#include <QGridLayout>
#include <QContextMenuEvent>
#include <QAction>
#include <QMenu>

#include <storage/userinfo.h>
#include <storage/phoneinfo.h>
#include <storage/channelinfo.h>

#include "identityphone.h"

IdentityPhone::IdentityPhone(QWidget * parent)
    : QWidget(parent)
{
    m_layout = new QGridLayout(this);
    m_layout->setContentsMargins(0, 0, 0, 0);

    m_icon = new QLabel(this);
    m_icon->setPixmap(QPixmap(":/images/identity/identity-phone.png"));
    m_icon->setContentsMargins(20, 0, 5, 0);

    m_phone = new QLabel(this);
    m_phone->setObjectName("phone_number");
    m_phone->setContentsMargins(0, 0, 10, 0);

    m_phonecall = new QLabel(this);
    m_phonecall->setObjectName("phone_call");
    m_phonecall->setContentsMargins(0, 0, 0, 0);

    m_phonecalltxt = new QLabel(this);
    m_phonecalltxt->setObjectName("phone_calltxt");
    m_phonecalltxt->setContentsMargins(0, 0, 10, 0);

    m_phonestatustxt = new QLabel(tr("No option"), this);
    m_phonestatustxt->setScaledContents(true);
    m_phonestatustxt->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_phonestatustxt->setContentsMargins(0, 0, 10, 0);

    m_layout->addWidget(m_icon, 0, 0, 3, 1);
    m_layout->addWidget(m_phone, 0, 1, 1, 2, Qt::AlignLeft | Qt::AlignVCenter);
    m_layout->addWidget(m_phonecall, 1, 1, Qt::AlignLeft | Qt::AlignVCenter);
    m_layout->addWidget(m_phonecalltxt, 1, 2, Qt::AlignLeft | Qt::AlignVCenter);
    m_layout->addWidget(m_phonestatustxt, 2, 1, 1, 2, Qt::AlignLeft | Qt::AlignVCenter);
}

void IdentityPhone::setPhoneId(const QString & xphoneid)
{
    m_xphoneid = xphoneid;
}

void IdentityPhone::updateChannelStatus(const QString & xchannelid)
{
    const ChannelInfo * channelinfo = b_engine->channels().value(xchannelid);
    if (channelinfo == NULL)
        return;
}

void IdentityPhone::updatePhoneConfig(const QString & xphoneid)
{
    if (xphoneid != m_xphoneid)
        return;
    const PhoneInfo * phoneinfo = b_engine->phone(m_xphoneid);
    if (phoneinfo == NULL)
        return;

    QString phonenumber = phoneinfo->number();
    if (! phonenumber.isEmpty())
        m_phone->setText(tr("Phone %1").arg(phonenumber));
    else
        m_phone->setText(tr("Phone <EMPTY>"));
}

void IdentityPhone::updatePhoneStatus(const QString & xphoneid)
{
    if (xphoneid != m_xphoneid)
        return;
    const PhoneInfo * phoneinfo = b_engine->phone(xphoneid);
    if (phoneinfo == NULL)
        return;

    QPixmap square(10, 10);
    QString hintstatus = phoneinfo->hintstatus();
    QString phonenumber = phoneinfo->number();
    QString color = "black";
    QString longname;

    if (b_engine->getOptionsPhoneStatus().contains(hintstatus)) {
        QVariantMap qvmop = b_engine->getOptionsPhoneStatus().value(hintstatus).toMap();
        color = qvmop.value("color").toString();
        longname = qvmop.value("longname").toString();
    } else {
        longname = tr("Status:%1").arg(hintstatus);
    }
    if (phonenumber.isEmpty())
        longname = tr("No status\n"
                      "(no phone number)");

    square.fill(color);
    m_phonecall->setPixmap(square);
    m_phonecall->setToolTip(longname);
    m_phonecalltxt->setText(longname);
}

void IdentityPhone::svcSummary(const QVariantMap & svcstatus)
{
    if (svcstatus["enablednd"].toBool()) {
        m_phonestatustxt->setText(tr("DND"));
        m_phonestatustxt->setToolTip(tr("Do Not Disturb"));
    } else if (svcstatus["unc-enabled"].toBool()) {
        m_phonestatustxt->setText(tr("UNC %1").arg(svcstatus["unc-number"].toString()));
        m_phonestatustxt->setToolTip(tr("Unconditional Forward towards %1").arg(svcstatus["unc-number"].toString()));
    } else if (svcstatus["busy-enabled"].toBool()) {
        m_phonestatustxt->setText(tr("Busy %1").arg(svcstatus["busy-number"].toString()));
        m_phonestatustxt->setToolTip(tr("Busy Forward towards %1").arg(svcstatus["busy-number"].toString()));
    } else if (svcstatus["rna-enabled"].toBool()) {
        m_phonestatustxt->setText(tr("FNA %1").arg(svcstatus["rna-number"].toString()));
        m_phonestatustxt->setToolTip(tr("Non-Answer Forward towards %1").arg(svcstatus["rna-number"].toString()));
    } else if (svcstatus["incallfilter"].toBool()) {
        m_phonestatustxt->setText(tr("Call Filter"));
    } else {
        m_phonestatustxt->setText(tr("No option"));
        m_phonestatustxt->setToolTip(tr("No option"));
    }
}

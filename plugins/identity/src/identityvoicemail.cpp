/* XiVO Client
 * Copyright (C) 2007-2010, Proformatique
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

/* $Revision: 8993 $
 * $Date: 2010-08-24 06:41:39 -0400 (mar 24 aoû 2010) $
 */

#include <QGridLayout>
#include <QPixmap>
#include <QLabel>
#include "baseengine.h"
#include "identityvoicemail.h"
#include "userinfo.h"

/*! \brief Constructor 
 */
IdentityVoiceMail::IdentityVoiceMail(QWidget * parent)
    : QWidget(parent)
{
    m_layout = new QGridLayout( this );

    m_iconButton = new QPushButton(this);
    m_iconButton->setFocusPolicy(Qt::NoFocus);
    m_iconButton->setToolTip("call your voicemail");
    QPixmap icon = QPixmap(":/in/kthememgr.png");
    m_iconButton->setIcon(icon);
    m_iconButton->setFlat(true);
    m_iconButton->setIconSize(icon.size());
    m_layout->addWidget(m_iconButton, 0, 0, 3, 1, Qt::AlignHCenter|Qt::AlignTop);
    connect(m_iconButton, SIGNAL(clicked()), this, SLOT(callVoiceMail()));

    m_name = new QLabel(this);
    m_layout->addWidget(m_name, 0, 1, Qt::AlignLeft|Qt::AlignVCenter);

    m_old = new QLabel(this);
    m_layout->addWidget(m_old, 1, 1, Qt::AlignLeft|Qt::AlignVCenter);

    m_new = new QLabel(this);
    m_layout->addWidget(m_new, 2, 1, Qt::AlignLeft|Qt::AlignVCenter);
    m_layout->setColumnStretch(2, 1);
}

/*! \brief update voicemail box name.
 */
void IdentityVoiceMail::svcSummary(QVariantMap &svcstatus, const UserInfo * ui)
{
    if(svcstatus["enablevm"].toBool()) {
        m_name->setText(tr("<b>VoiceMailBox %1</b>").arg(ui->voicemailNumber()));
        m_name->setToolTip(tr("VoiceMail activated on %1").arg(ui->voicemailNumber()));
    } else {
        m_name->setText(tr("VoiceMailBox %1").arg(ui->voicemailNumber()));
        m_name->setToolTip(tr("VoiceMail not activated on %1").arg(ui->voicemailNumber()));
    }
}

/*! \brief update display of old/new messages
 */
void IdentityVoiceMail::setOldNew(const QString & _old, const QString & _new)
{
    m_old->setText(tr("%1 old").arg(_old));
    m_new->setText(tr("%1 new").arg(_new));
}

/*! \brief call voicemail on click
 */
void IdentityVoiceMail::callVoiceMail()
{
    b_engine->actionCall("originate", "user:special:me", "user:special:myvoicemail");
}

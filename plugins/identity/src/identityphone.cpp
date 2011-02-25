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
#include <QGridLayout>
#include <QContextMenuEvent>
#include <QAction>
#include <QMenu>
#include "identityphone.h"
#include "userinfo.h"
#include "phoneinfo.h"

/*! \brief Constructor
 */
IdentityPhoneLine::IdentityPhoneLine(int linenum, QWidget * parent)
    : QWidget(parent), m_linenum(linenum)
{
    QHBoxLayout * layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    m_action = new QLabel(this);
    m_status = new QLabel(this);

    QPixmap square(25, 3);
    square.fill(Qt::black);
    setPixmap(square);
    QString todisplay = tr("(Line %1)").arg(m_linenum);
    setText(QString("  %1  ").arg(todisplay));

    layout->addWidget(m_action);
    layout->addWidget(m_status);
}

void IdentityPhoneLine::setPhoneId(const QString & xphoneid)
{
    m_xphoneid = xphoneid;
}

void IdentityPhoneLine::setPixmap(const QPixmap & pixmap)
{
    m_action->setPixmap(pixmap);
}

void IdentityPhoneLine::setText(const QString & text)
{
    m_status->setText(text);
}

void IdentityPhoneLine::contextMenuEvent(QContextMenuEvent * event)
{
    QString thischannel;
    const PhoneInfo * phoneinfo = b_engine->phones().value(m_xphoneid);
    if(phoneinfo == NULL)
        return;
    QMapIterator<QString, QVariant> iter = QMapIterator<QString, QVariant>(phoneinfo->comms());
    while( iter.hasNext() ) {
        iter.next();
        QVariantMap callprops = iter.value().toMap();
        if(callprops.contains("linenum")) {
            int ic = callprops["linenum"].toInt();
            if(ic == m_linenum) {
                thischannel = callprops["thischannel"].toString();
            }
        }
    }

    if(!thischannel.isEmpty()) {
        QMenu contextMenu(this);

        QAction * hangupMe = new QAction(tr("Hangup"), &contextMenu);
        hangupMe->setProperty("channel", thischannel);
        connect(hangupMe, SIGNAL(triggered()),
                this, SLOT(hangup()) );
        contextMenu.addAction(hangupMe);

        QAction * replyMe = new QAction(tr("Reply"), &contextMenu);
        replyMe->setProperty("channel", thischannel);
        connect(replyMe, SIGNAL(triggered()),
                this, SLOT(answer()) );
        contextMenu.addAction(replyMe);

        QAction * refuseMe = new QAction(tr("Refuse"), &contextMenu);
        refuseMe->setProperty("channel", thischannel);
        connect(refuseMe, SIGNAL(triggered()),
                this, SLOT(refuse()) );
        contextMenu.addAction(refuseMe);

        contextMenu.exec(event->globalPos());
    }
}

void IdentityPhoneLine::hangup()
{
    const PhoneInfo * phoneinfo = b_engine->phones().value(m_xphoneid);
    if (phoneinfo == NULL)
        return;
    if (sender()) {
        QString channel = sender()->property("channel").toString();
        b_engine->actionCall("hangup",
                             QString("chan:%1/%2:%3")
                             .arg(phoneinfo->ipbxid())
                             .arg(phoneinfo->phoneid())
                             .arg(channel));
    }
}

void IdentityPhoneLine::answer()
{
    const PhoneInfo * phoneinfo = b_engine->phones().value(m_xphoneid);
    if (phoneinfo == NULL)
        return;
    if (sender()) {
        QString channel = sender()->property("channel").toString();
        b_engine->actionCall("answer",
                             QString("chan:%1/%2:%3")
                             .arg(phoneinfo->ipbxid())
                             .arg(phoneinfo->phoneid())
                             .arg(channel));
    }
}

void IdentityPhoneLine::refuse()
{
    const PhoneInfo * phoneinfo = b_engine->phones().value(m_xphoneid);
    if (phoneinfo == NULL)
        return;
    if (sender()) {
        QString channel = sender()->property("channel").toString();
        b_engine->actionCall("refuse",
                             QString("chan:%1/%2:%3")
                             .arg(phoneinfo->ipbxid())
                             .arg(phoneinfo->phoneid())
                             .arg(channel));
    }
}

/*! \brief Constructor
 */
IdentityPhone::IdentityPhone(QWidget * parent)
    : QWidget(parent)
{
    m_layout = new QGridLayout(this);
    m_layout->setContentsMargins(0, 0, 0, 0);

    m_icon = new QLabel(this);
    m_icon->setPixmap(QPixmap(":/in/identity-phone.png"));
    m_icon->setContentsMargins(20, 0, 5, 0);

    m_phone = new QLabel(this);
    m_phone->setContentsMargins(0, 0, 10, 0);

    m_phonecall = new QLabel(this);
    m_phonecall->setContentsMargins(0, 0, 0, 0);

    m_phonecalltxt = new QLabel(this);
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
    const PhoneInfo * phoneinfo = b_engine->phones().value(m_xphoneid);
    if (phoneinfo == NULL)
        return;
    m_phone->setText(tr("Phone %1").arg(phoneinfo->number()));
    m_phone->setToolTip(tr("Protocol: %1\n"
                           "Server: %2\n"
                           "Context: %3")
                        .arg(phoneinfo->protocol())
                        .arg(phoneinfo->ipbxid())
                        .arg(phoneinfo->context()));
    setPhoneLines();
    if (phoneinfo->number().isEmpty())
        hide();
    else
        show();
}

void IdentityPhone::updateChannelStatus(const QString & xchannelid)
{
    const ChannelInfo * channelinfo = b_engine->channels().value(xchannelid);
    if (channelinfo == NULL)
        return;
    qDebug() << Q_FUNC_INFO << channelinfo->channel() << channelinfo->talkingto_kind();
}

void IdentityPhone::updatePhoneConfig(const QString & xphoneid)
{
    if (xphoneid != m_xphoneid)
        return;
    setPhoneId(xphoneid);
}

void IdentityPhone::updatePhoneStatus(const QString & xphoneid)
{
    if (xphoneid != m_xphoneid)
        return;
    const PhoneInfo * phoneinfo = b_engine->phones().value(xphoneid);
    if (phoneinfo == NULL)
        return;

    QPixmap square(10, 10);
    QString color = "blue"; // XXXX function of phoneinfo->hintstatus();
    QString longname = "There"; // XXXX function of phoneinfo->hintstatus();
    square.fill(color);
    m_phonecall->setPixmap(square);
    m_phonecall->setToolTip(longname);
    m_phonecalltxt->setText(longname);

    updateLines(phoneinfo->comms());
}

void IdentityPhone::updateLines(const QVariantMap & comms)
{
    const PhoneInfo * phoneinfo = b_engine->phones().value(m_xphoneid);
    if (phoneinfo == NULL)
        return;
    QMapIterator<QString, QVariant> iter = QMapIterator<QString, QVariant>(comms);
    QList<int> busylines;
    while( iter.hasNext() ) {
        iter.next();
        QVariantMap callprops = iter.value().toMap();
        if(callprops.contains("linenum")) {
            //qDebug() << callprops;
            int ic = callprops.value("linenum").toInt() - 1;
            QString status = callprops.value("status").toString();
            QString todisplay = callprops.value("calleridname").toString();
            bool isholded = callprops.contains("time-hold");
            busylines << ic;

            QPixmap square_comm(25, 3);
            square_comm.fill(isholded ? Qt::darkGreen : Qt::green);
            if(status == "hangup") {
                todisplay = tr("(Line %1)").arg(callprops.value("linenum").toString());
                square_comm.fill(Qt::black);
            }
            if(ic < m_lines.size() && m_lines[ic]) {
                m_lines[ic]->setPixmap(square_comm);
                m_lines[ic]->setText(QString("  %1  ").arg(todisplay));
            }
        }
    }
    QPixmap square_black(25, 3);
    square_black.fill(Qt::black);
    for(int jj = 0 ; jj < phoneinfo->simultcalls() ; jj ++) {
        if(!busylines.contains(jj) && jj < m_lines.size()) {
            m_lines[jj]->setPixmap(square_black);
            QString todisplay = tr("(Line %1)").arg(jj + 1);
            m_lines[jj]->setText(QString("  %1  ").arg(todisplay));
        }
    }
}

/*! \brief display the phones' lines
 */
void IdentityPhone::setPhoneLines()
{
    const PhoneInfo * phoneinfo = b_engine->phones().value(m_xphoneid);
    if (phoneinfo == NULL)
        return;
    for(int jj = 0 ; jj < phoneinfo->simultcalls() ; jj ++) {
        qDebug() << " --- " << jj << m_lines.value(jj);
        if(jj >= m_lines.size()) {
            m_lines.insert(jj, new IdentityPhoneLine(jj + 1, this));
            m_lines[jj]->setPhoneId(m_xphoneid);
            int ix = jj / 3;
            int iy = jj % 3;
            m_layout->addWidget( m_lines[jj], iy, 3 + ix );
        }
    }
}

void IdentityPhone::svcSummary(QVariantMap & svcstatus)
{
    if(svcstatus["enablednd"].toBool()) {
        m_phonestatustxt->setText(tr("DND"));
        m_phonestatustxt->setToolTip(tr("Do Not Disturb"));
    } else {
        if(svcstatus["unc-enabled"].toBool()) {
            m_phonestatustxt->setText(tr("UNC %1").arg(svcstatus["unc-number"].toString()));
            m_phonestatustxt->setToolTip(tr("Unconditional Forward towards %1").arg(svcstatus["unc-number"].toString()));
        } else if (svcstatus["busy-enabled"].toBool()) {
            m_phonestatustxt->setText(tr("Busy %1").arg(svcstatus["busy-number"].toString()));
            m_phonestatustxt->setToolTip(tr("Busy Forward towards %1").arg(svcstatus["busy-number"].toString()));
        } else if (svcstatus["rna-enabled"].toBool()) {
            m_phonestatustxt->setText(tr("RNA %1").arg(svcstatus["rna-number"].toString()));
            m_phonestatustxt->setToolTip(tr("Non-Answer Forward towards %1").arg(svcstatus["rna-number"].toString()));
        } else if (svcstatus["callrecord"].toBool()) {
            m_phonestatustxt->setText(tr("Call Rec"));
        } else if (svcstatus["incallfilter"].toBool()) {
            m_phonestatustxt->setText(tr("Call Filter"));
        } else {
            m_phonestatustxt->setText(tr("No option"));
            m_phonestatustxt->setToolTip(tr("No option"));
        }
    }
}

/* XiVO Client
 * Copyright (C) 2007-2009, Proformatique
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
    : QWidget(parent), m_ui(0), m_linenum(linenum)
{
    QHBoxLayout * layout = new QHBoxLayout(this);
    //QGridLayout * layout = new QGridLayout(this);
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
    //layout->addWidget(m_action, 0, 0);
    //layout->addWidget(m_status, 0, 1);
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
    qDebug() << Q_FUNC_INFO;
    if(!m_ui)
        return;
    QString thischannel;
    foreach(QString phoneid, m_ui->phonelist()) {
        const PhoneInfo * p_pi = m_ui->getPhoneInfo(phoneid);
        if(p_pi == NULL)
            continue;
        QMapIterator<QString, QVariant> iter = QMapIterator<QString, QVariant>(p_pi->comms());
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

void IdentityPhoneLine::setUserInfo(const UserInfo * ui)
{
    m_ui = ui;
}

void IdentityPhoneLine::hangup()
{
    qDebug() << Q_FUNC_INFO << sender() << m_ui;
    if(sender() && m_ui) {
        QString channel = sender()->property("channel").toString();
        emit actionCall(QString("hangup"),
                        QString("chan:%1:%2").arg(m_ui->userid()).arg(channel),
                        QString());
    }
}

void IdentityPhoneLine::answer()
{
    if(sender() && m_ui) {
        QString channel = sender()->property("channel").toString();
        emit actionCall(QString("answer"),
                        QString("chan:%1:%2").arg(m_ui->userid()).arg(channel),
                        QString());
    }
}

void IdentityPhoneLine::refuse()
{
    if(sender()) {
        QString channel = sender()->property("channel").toString();
        emit actionCall(QString("refuse"),
                        QString("chan:%1:%2").arg(m_ui->userid()).arg(channel),
                        QString());
    }
}

/*! \brief Constructor
 */
IdentityPhone::IdentityPhone(QWidget * parent)
    : QWidget(parent),
    m_ui(0)
{
    m_layout = new QGridLayout(this);
    m_layout->setContentsMargins(0, 0, 0, 0);

    m_icon = new QLabel(this);
    m_icon->setPixmap(QPixmap(":/images/phone.png"));
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

    m_layout->addWidget(m_icon, 0, 0, 3, 1);    // check alignment
    m_layout->addWidget(m_phone, 0, 1, 1, 2, Qt::AlignLeft | Qt::AlignVCenter);
    m_layout->addWidget(m_phonecall, 1, 1, Qt::AlignLeft | Qt::AlignVCenter);
    m_layout->addWidget(m_phonecalltxt, 1, 2, Qt::AlignLeft | Qt::AlignVCenter);
    m_layout->addWidget(m_phonestatustxt, 2, 1, 1, 2, Qt::AlignLeft | Qt::AlignVCenter);

}

void IdentityPhone::setUserInfo(const UserInfo * ui)
{
    m_ui = ui;
    m_phone->setText(tr("Phone %1").arg(m_ui->phonenumber()));
    setPhoneLines();
}

void IdentityPhone::updateUser(UserInfo * ui)
{
    if(m_ui != ui)
        return;
    //qDebug() << Q_FUNC_INFO;
    foreach(QString phoneid, m_ui->phonelist()) {
        QPixmap square(10, 10);
        const PhoneInfo * p_pi = m_ui->getPhoneInfo(phoneid);
        if(p_pi == NULL)
            continue;
        square.fill(p_pi->hintstatus("color"));
        m_phonecall->setPixmap(square);
        m_phonecall->setToolTip(p_pi->hintstatus("longname"));
        m_phonecalltxt->setText(p_pi->hintstatus("longname"));
        QMapIterator<QString, QVariant> iter = QMapIterator<QString, QVariant>(p_pi->comms());
        QList<int> busylines;
        while( iter.hasNext() ) {
            iter.next();
            QVariantMap callprops = iter.value().toMap();
            if(callprops.contains("linenum")) {
                //qDebug() << callprops;
                int ic = callprops["linenum"].toInt() - 1;
                QString status = callprops["status"].toString();
                QString todisplay = callprops["calleridname"].toString();
                bool isholded = callprops.contains("time-hold");
                busylines << ic;
                
                QPixmap square_comm(25, 3);
                square_comm.fill(isholded ? Qt::darkGreen : Qt::green);
                if(status == "hangup") {
                    todisplay = tr("(Line %1)").arg(callprops["linenum"].toString());
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
        for(int jj = 0 ; jj < m_ui->simultcalls() ; jj ++) {
            if(!busylines.contains(jj) && jj < m_lines.size()) {
                m_lines[jj]->setPixmap(square_black);
                QString todisplay = tr("(Line %1)").arg(jj + 1);
                m_lines[jj]->setText(QString("  %1  ").arg(todisplay));
            }
        }
    }
}

/*! \brief display the phones' lines
 */
void IdentityPhone::setPhoneLines()
{
    //qDebug() << Q_FUNC_INFO;
    if(!m_ui)
        return;
    for(int jj = 0 ; jj < m_ui->simultcalls() ; jj ++) {
        //qDebug() << " --- " << jj << m_lines.value(jj);
        if(jj >= m_lines.size()) {
            m_lines.insert(jj, new IdentityPhoneLine(jj + 1, this));
            m_lines[jj]->setUserInfo(m_ui);
            connect(m_lines[jj], SIGNAL(actionCall(const QString &, const QString &, const QString &)),
                    this, SIGNAL(actionCall(const QString &, const QString &, const QString &)) );
            int ix = jj / 3;
            int iy = jj % 3;
            m_layout->addWidget( m_lines[jj], iy, 3 + ix );
        }
    }
}

void IdentityPhone::svcSummary(QMap<QString, QVariant> & svcstatus)
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


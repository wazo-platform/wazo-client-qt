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

/* $Revision$
 * $Date$
 */

#include <QAction>
#include <QDebug>
#include <QContextMenuEvent>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMenu>
#include <QMouseEvent>

#include "logeltwidget.h"

/*! \brief Constructor
 *
 * init everything and construct the sub widgets.
 */
LogEltWidget::LogEltWidget(const QString & peer, Direction d,
                           const QDateTime & dt, int duration,
                           const QString & termin,
                           QWidget * parent)
    : QWidget(parent), m_dateTime(dt), m_peer(peer), m_direction(d)
{
    // qDebug() << "  LogEltWidget::LogEltWidget()" << peer << d << dt << duration << parent;
    QGridLayout * glayout = new QGridLayout( this );
    //        QHBoxLayout * layout1 = new QHBoxLayout( this );
    //        layout1->setMargin(0);

    QLabel * lblpeer = new QLabel(this);
    if(peer.isEmpty())
        lblpeer->setText(tr("Unknown"));
    else
        lblpeer->setText(peer);
    lblpeer->setObjectName("logpeername");
    lblpeer->setFont(QFont("helvetica", 10, QFont::Bold));
    lblpeer->setMargin(0);
    glayout->addWidget(lblpeer, 0, 0);

    QLabel * lbldt = new QLabel( dt.toString(Qt::SystemLocaleDate) + "   ", this );
    lbldt->setFont(QFont("helvetica", 10, QFont::Light));
    lbldt->setMargin(0);
    glayout->addWidget(lbldt, 1, 0);
        
    QLabel * lbltermin = new QLabel();
    if(termin.split(".").size() == 4)
        lbltermin->setText(tr("(phone %1)").arg(termin.split(".")[3]));
    glayout->addWidget(lbltermin, 0, 2);
        
    QLabel * lblduration = new QLabel( this );
    int min = duration / 60;
    int sec = duration % 60;
    lblduration->setText((min?(QString::number(min) + "min "):"") + QString::number(sec) + "s");
    lblduration->setFont(QFont("helvetica", 10, QFont::Light));
    lblduration->setMargin(0);
    glayout->addWidget(lblduration, 1, 1);

    QLabel * lbldummy = new QLabel( "", this );
    lbldummy->setMargin(0);
    glayout->addWidget(lbldummy, 1, 2);

    /*        QLabel * lbldir = new QLabel( this );
              lbldir->setText((d == OutCall)?"<=":"=>");
              glayout->addWidget(lbldir);
    */

    glayout->setSpacing(0);
    glayout->setColumnStretch(0, 0);
    glayout->setColumnStretch(1, 0);
    glayout->setColumnStretch(2, 0);
    glayout->setColumnStretch(3, 1);

    m_dialAction = new QAction( tr("&Call back"), this );
    m_dialAction->setStatusTip( tr("Call back the correspondent") );
    connect( m_dialAction, SIGNAL(triggered()),
             this, SLOT(callBackPeer()) );
}

const QDateTime & LogEltWidget::dateTime() const
{
    return m_dateTime;
}

const QString & LogEltWidget::peer() const
{
    return m_peer;
}

LogEltWidget::Direction LogEltWidget::direction() const
{
    return m_direction;
}

/*! \brief call on double click
 *
 * \see callBackPeer
 */
void LogEltWidget::mouseDoubleClickEvent(QMouseEvent * /* event*/)
{
    callBackPeer();
}

/*! \brief copy number on simple click
 *
 * \see doNotCallBackPeer
 */
void LogEltWidget::mouseReleaseEvent(QMouseEvent * /* event*/)
{
    doNotCallBackPeer();
}

/*! \brief display context menu
 */
void LogEltWidget::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu contextMenu(this);
    contextMenu.addAction( m_dialAction );
    contextMenu.exec(event->globalPos());
}

/*! call the guy if dials is true, otherwise transmits
 *  the number in order to paste it
 *  somewhere, for instance in the dial input widget
 */
void LogEltWidget::callBackPeer()
{
    QStringList qsl1 = m_peer.split("<");
    QString number = m_peer;
    if (qsl1.size() > 1) {
        QStringList qsl2 = qsl1[1].split(">");
        number = qsl2[0];
    }
        
    emit actionCall("originate", "user:special:me", "ext:" + number); // Call
}

/*! \brief Copy number */
void LogEltWidget::doNotCallBackPeer()
{
    QStringList qsl1 = m_peer.split("<");
    QString number = m_peer;
    if (qsl1.size() > 1) {
        QStringList qsl2 = qsl1[1].split(">");
        number = qsl2[0];
    }

    emit copyNumber(number);
}

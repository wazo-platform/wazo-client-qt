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
#include <QHBoxLayout>
#include <QLabel>
#include <QLayoutItem>
#include <QGroupBox>
#include <QRadioButton>
#include <QScrollArea>
#include <QTabWidget>
#include <QVBoxLayout>

#include "baseengine.h"
#include "logeltwidget.h"
#include "logwidget.h"
#include "userinfo.h"

/*! \brief Constructor
 *
 * build layout and child widgets.
 */
LogWidget::LogWidget(BaseEngine * engine, QWidget * parent)
    : XLet(engine, parent), m_timer(-1)
{
    QVBoxLayout * layout = new QVBoxLayout(this);
    layout->setMargin(0);
    layout->setSpacing(0);

    QGroupBox * groupBox = new QGroupBox( this );
    groupBox->setAlignment( Qt::AlignHCenter );
    QHBoxLayout * vbox = new QHBoxLayout( groupBox );
    vbox->setMargin(0);
    vbox->setSpacing(0);

    vbox->addStretch( 1 );
    
    m_radioOut = new QRadioButton( " ", groupBox );
    m_radioOut->setIcon(QIcon(":/images/green_up.png"));
    m_radioOut->setToolTip( tr("Outgoing calls") );
    connect( m_radioOut, SIGNAL(toggled(bool)),
             this, SLOT(modeChanged(bool)) );
    vbox->addWidget( m_radioOut );
    
    m_radioIn = new QRadioButton( " ", groupBox );
    m_radioIn->setIcon(QIcon(":/images/green_down.png"));
    m_radioIn->setToolTip( tr("Incoming calls") );
    connect( m_radioIn, SIGNAL(toggled(bool)),
             this, SLOT(modeChanged(bool)) );
    vbox->addWidget( m_radioIn );
    
    m_radioMissed = new QRadioButton( " ", groupBox );
    m_radioMissed->setIcon(QIcon(":/images/red_down.png"));
    m_radioMissed->setToolTip( tr("Missed calls") );
    connect( m_radioMissed, SIGNAL(toggled(bool)),
             this, SLOT(modeChanged(bool)) );
    vbox->addWidget( m_radioMissed );
    vbox->addStretch( 1 );
    
    m_radioMissed->setChecked( true );
    
    layout->addWidget( groupBox );
    QScrollArea * scrollArea = new QScrollArea( this );
    scrollArea->setWidgetResizable( true );

    QWidget * widget = new QWidget( this );
    scrollArea->setWidget( widget );
    widget->setObjectName("scroller"); // in order for the style settings to be set accordingly

    m_layout = new QVBoxLayout( widget );
    m_layout->setMargin(0);
    m_layout->setSpacing(0);
    m_layout->addStretch(0);
    layout->addWidget( scrollArea );

    // connect slots
    connect( m_engine, SIGNAL(updateLogEntry(const QDateTime &, int,
                                             const QString &, const QString &, const QString &)),
             this, SLOT(addLogEntry(const QDateTime &, int,
                                    const QString &, const QString &, const QString &)) );
    connect( this, SIGNAL(askHistory(const QString &, int, const QDateTime &)),
             m_engine, SLOT(requestHistory(const QString &, int, const QDateTime &)) );
    connect( m_engine, SIGNAL(delogged()),
             this, SLOT(clear()) );
    connect( m_engine, SIGNAL(monitorPeer(UserInfo *)),
             this, SLOT(monitorPeer(UserInfo *)) );
}

/*! \brief add an entry
 *
 * \sa addLogEntry
 */
void LogWidget::addElement(const QString & peer, LogEltWidget::Direction d,
                           const QDateTime & dt, int duration, const QString & termin)
{
    //qDebug() << "LogWidget::addElement()" << peer << d << dt << duration;
    int i, index = 0;
    for(i = 0; i < m_layout->count(); i++) {
        QWidget * widget = m_layout->itemAt( i )->widget();
        if(widget) {
            LogEltWidget * logelt = qobject_cast<LogEltWidget *>(widget);
            if(logelt) {
                if((dt == logelt->dateTime())
                   && (peer == logelt->peer())
                   && (d == logelt->direction()))
                    return;
                else if(dt > logelt->dateTime())
                    break;
                index = i + 1;
            }
        }
    }
    LogEltWidget * logelt = new LogEltWidget(peer, d, dt, duration, termin, this);
    connect( logelt, SIGNAL(actionCall(const QString &, const QString &, const QString &)),
             m_engine, SLOT(actionCall(const QString &, const QString &, const QString &)) );
    connect( logelt, SIGNAL(copyNumber(const QString &)),
             m_engine, SLOT(copyNumber(const QString &)) );
    m_layout->insertWidget(index, logelt);
    if(dt > m_moreRecent)
        m_moreRecent = dt;
}

/*! \brief remove all child widgets
 */
void LogWidget::clear()
{
    QLayoutItem * child;
    while ((child = m_layout->itemAt(0)) != 0)
    {
        if(child->widget())
        {
            m_layout->removeItem(child);
            delete child->widget();
            delete child;
        }
        else
            break;
    }
    m_moreRecent = QDateTime();
    //m_layout->addStretch(1);
}

/*! \brief add an entry
 */
void LogWidget::addLogEntry(const QDateTime & dt, int duration,
                            const QString & peer, const QString & direction, const QString & termin)
{
    LogEltWidget::Direction d;
    d = (direction == "IN") ? LogEltWidget::InCall : LogEltWidget::OutCall;
    // TODO: manage the list !
    addElement(peer, d, dt, duration, termin);
}

/*! \brief change the monitored peer
 */
void LogWidget::monitorPeer(UserInfo * ui)
{
    // qDebug() << "LogWidget::monitorPeer()" << ui->userid();
    clear();
    m_peer = ui->userid();
    if(m_peer.size() > 0) {
        emit askHistory(m_peer, mode(), m_moreRecent);
        if(m_timer < 0)
            m_timer = startTimer(10000);
    }
}

/*! \brief timer event : ask for update
 */
void LogWidget::timerEvent(QTimerEvent *)
{
    // qDebug() << "LogWidget::timerEvent() id=" << event->timerId();
    if(m_peer.size() > 0)
        emit askHistory(m_peer, mode(), m_moreRecent);
}

/*! \brief return the mode (out/in or missed)
 */
int LogWidget::mode()
{
    int r = -1;
    if(m_radioOut->isChecked())
        r = 0;
    else if(m_radioIn->isChecked())
        r = 1;
    else if(m_radioMissed->isChecked())
        r = 2;
    return r;
}

/*! \brief triggered when mode is changed.
 *
 * clear the list and ask an update.
 */
void LogWidget::modeChanged(bool b)
{
    // qDebug() << "LogWidget::modeChanged()" << b << mode();
    if(b && m_peer.size() > 0) {
        clear();
        emit askHistory(m_peer, mode(), m_moreRecent);
    }
}

/* XIVO CTI clients
 * Copyright (C) 2007-2009  Proformatique
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License version 2 for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Linking the Licensed Program statically or dynamically with other
 * modules is making a combined work based on the Licensed Program. Thus,
 * the terms and conditions of the GNU General Public License version 2
 * cover the whole combination.
 *
 * In addition, as a special exception, the copyright holders of the
 * Licensed Program give you permission to combine the Licensed Program
 * with free software programs or libraries that are released under the
 * GNU Library General Public License version 2.0 or GNU Lesser General
 * Public License version 2.1 or any later version of the GNU Lesser
 * General Public License, and with code included in the standard release
 * of OpenSSL under a version of the OpenSSL license (with original SSLeay
 * license) which is identical to the one that was published in year 2003,
 * or modified versions of such code, with unchanged license. You may copy
 * and distribute such a system following the terms of the GNU GPL
 * version 2 for the Licensed Program and the licenses of the other code
 * concerned, provided that you include the source code of that other code
 * when and as the GNU GPL version 2 requires distribution of source code.
 */

/* $Revision$
 * $Date$
 */

#include <QDebug>
#include <QVariant>
#include <QPainter>
#include <QSizePolicy>
#include "basicpeerwidget.h"
#include "userinfo.h"
#include "phoneinfo.h"
#include "baseengine.h"
#include "xivoconsts.h"

/*! \brief Constructor
 *
 * initialize members and tooltip
 */
BasicPeerWidget::BasicPeerWidget(BaseEngine * engine, UserInfo * ui)
    : BasePeerWidget(engine, ui), m_color(0xcc, 0xcc, 0xcc),
      m_presenceColor(0xcc, 0xcc, 0xcc)
{
    // can grow horizontaly but not verticaly
    setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Fixed );
    setText( ui->fullname().isEmpty() ? tr("(No callerid yet)") : ui->fullname() );
    setToolTip( ui->phonenumber() );
}

void BasicPeerWidget::setText(const QString & text)
{
    m_text = text;
    // calculate size
    QFontMetrics fontMetrics( font() );
    QSize size = fontMetrics.size(0, m_text);
    // maximum width for PeerWidget
    if(size.width() > maxWidthWanted())
        size.setWidth(maxWidthWanted());
    setMinimumSize( size );
    update();
}

/*! \brief paint this widget
 *
 * Fill the whole widget with a rectange of color
 * which indicate the status of the phone.
 * Draw a small square for user presence indicator
 * Then write the name of the user.
 */
void BasicPeerWidget::paintEvent(QPaintEvent * /*event*/)
{
    QRect rectangle = contentsRect();
    QPainter painter( this );
    // draw the color rectangle
    painter.setBrush( m_color );
    painter.setPen( Qt::NoPen );
    //painter.drawRect( rectangle );
    painter.drawRect( rectangle.adjusted( 1, 0, -1, 0 ) );
    painter.drawRect( rectangle.adjusted( 0, 1, 0, -1 ) );
    // small square
    if(! m_ui->ctilogin().isEmpty())
    {
        painter.setBrush( m_presenceColor );
        painter.drawRect( QRect(rectangle.width() - 5, rectangle.height() - 5, 4, 4) );
    }
    // write the text
    painter.setPen(Qt::SolidLine);
    if(m_color.value() < 128)
        painter.setPen( QColor(0xcc, 0xcc, 0xcc) );
    painter.drawText( rectangle, Qt::AlignVCenter | Qt::AlignHCenter, m_text );
}

void BasicPeerWidget::setAgentToolTip(const QString & /*agentnum*/, const QStringList & /*queues*/)
{
    // do nothing !
}
 
void BasicPeerWidget::setAgentState(const QString & /*color*/)
{
    // do nothing !
}

void BasicPeerWidget::updatePresence()
{
    QString text = m_ui->phonenumber();
    if(! m_ui->ctilogin().isEmpty())
        {
            text.append(" ");
            text.append(m_ui->availstate()["longname"]);
        }
    setToolTip( text );
    m_presenceColor.setNamedColor( m_ui->availstate()["color"] );
}

void BasicPeerWidget::updatePhonesStates()
{
    // set the color according to the 1st phone
    if( !m_ui->phonelist().isEmpty() )
        {
            const PhoneInfo * pi = m_ui->getPhoneInfo(m_ui->phonelist()[0]);
            if(pi)
                {
                    m_color.setNamedColor( pi->hintstatus("color") );
                    update();
                }
        }
}


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
    m_presenceSquareSize = m_engine->getGuiOptions("merged_gui").value("presenceindicatorsize").toInt();
    if(m_presenceSquareSize<=0||m_presenceSquareSize>20)
        m_presenceSquareSize = 5;
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
        painter.drawRect( QRect(rectangle.width() - m_presenceSquareSize - 1,
                                rectangle.height() - m_presenceSquareSize - 1,
                                m_presenceSquareSize, m_presenceSquareSize) );
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


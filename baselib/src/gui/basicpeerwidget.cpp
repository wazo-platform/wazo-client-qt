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
BasicPeerWidget::BasicPeerWidget(UserInfo * ui)
    : BasePeerWidget(ui), m_color(0xcc, 0xcc, 0xcc),
      m_presenceColor(0xcc, 0xcc, 0xcc)
{
    // can grow horizontaly but not verticaly
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    QString name = (!ui || ui->fullname().isEmpty()) ? tr("(No callerid yet)") : ui->fullname();
    setText(name);
    setToolTip(ui->phoneNumber());
    m_presenceSquareSize = b_engine->getGuiOptions("merged_gui").value("presenceindicatorsize").toInt();
    if ((m_presenceSquareSize<=0)||(m_presenceSquareSize>20)) {
        m_presenceSquareSize = 5;
    }
    reloadSavedName();
}

void BasicPeerWidget::setText(const QString &text)
{
    m_text = text;
    QFontMetrics fontMetrics(font());
    QSize size = fontMetrics.size(0, m_text);
    size.rwidth() += m_presenceSquareSize;
    // maximum width for PeerWidget
    if (size.width() > maxWidthWanted()) {
        size.setWidth(maxWidthWanted());
    }
    setMinimumSize(size);
    update();
}

/*! \brief paint this widget
 *
 * Fill the whole widget with a rectange of color
 * which indicate the status of the phone.
 * Draw a small square for user presence indicator
 * Then write the name of the user.
 */
void BasicPeerWidget::paintEvent(QPaintEvent *)
{
    bool hasPresenceIndicator = !m_ui->ctilogin().isEmpty();
    QRect rectangle = contentsRect();
    QPainter painter(this);
    // draw the color rectangle
    painter.setBrush(m_color);
    painter.setPen(Qt::NoPen);
    painter.drawRect(QRect( 1, 0, rectangle.width() - 2, 1));
    painter.drawRect(rectangle.adjusted( 0, 1, 0, -1 ));
    painter.drawRect(QRect( 1, rectangle.height() - 1, rectangle.width() - 2, 1));
    // draw presence indicator
    if(hasPresenceIndicator) {
        painter.setBrush( m_presenceColor );
        // square
//        painter.drawRect( QRect(rectangle.width() - m_presenceSquareSize - 1,
//                                rectangle.height() - m_presenceSquareSize - 1,
//                                m_presenceSquareSize, m_presenceSquareSize) );
        // vertical bar
        painter.drawRect(QRect( 1, 0, m_presenceSquareSize + 1, rectangle.height()));
    }
    // write the text
    painter.setPen(Qt::SolidLine);
    if(m_color.value() < 128) {
        painter.setPen(QColor(0xcc, 0xcc, 0xcc));
    }
    if(hasPresenceIndicator) {
        rectangle.adjust(m_presenceSquareSize + 1, 0, 0, 0);
    }
    painter.drawText( rectangle, Qt::AlignVCenter | Qt::AlignHCenter, m_text );
}

void BasicPeerWidget::setAgentToolTip(const QString &, const QStringList &)
{
    // do nothing !
}

void BasicPeerWidget::setAgentState(const QString &)
{
    // do nothing !
}

void BasicPeerWidget::updatePresence()
{
    QString text = m_ui->phoneNumber();
    if (!m_ui->ctilogin().isEmpty()) {
        text.append(" ");
        text.append(m_ui->availstate()["longname"]);
    }
    setToolTip(text);
    m_presenceColor.setNamedColor(m_ui->availstate()["color"]);
}

void BasicPeerWidget::updatePhonesStates()
{
    // set the color according to the 1st phone
    QString ipbxid = m_ui->ipbxid();
    if (! m_ui->phonelist().isEmpty()) {
        QString xphoneid = QString("%1/%2").arg(ipbxid).arg(m_ui->phonelist()[0]);
        const PhoneInfo * pi = b_engine->phones().value(xphoneid);
        if (pi) {
            m_color.setNamedColor(pi->hintstatus("color"));
            update();
        }
    }
}

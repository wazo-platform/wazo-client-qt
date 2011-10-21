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
BasicPeerWidget::BasicPeerWidget(const UserInfo * ui)
    : BasePeerWidget(ui),
      m_color(0xcc, 0xcc, 0xcc),
      m_presenceColor(0xcc, 0xcc, 0xcc)
{
    qDebug() << Q_FUNC_INFO;
    // can grow horizontaly but not verticaly
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    setToolTip(tr("Phone Numbers : %1").arg(b_engine->phonenumbers(ui).join(", ")));
    getConfig();
    
    // setText must be called after getConfig, as it assigns m_presenceSquareSize
    QString name = (ui->fullname().isEmpty()) ? tr("(No callerid yet)") : ui->fullname();
    setText(name);
    reloadSavedName();
    connect(b_engine, SIGNAL(settingsChanged()),
            this, SLOT(updateConfig()));
}

void BasicPeerWidget::setText(const QString &text)
{
    // qDebug() << Q_FUNC_INFO;
    m_text = text;
    updateWidth();
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
    bool hasPresenceIndicator = m_ui_remote->enableclient();
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
        painter.drawRect(QRect( 1, 0, m_presenceSquareSize, rectangle.height()));
    }
    // write the text
    painter.setPen(Qt::SolidLine);
    if(m_color.value() < 128) {
        painter.setPen(QColor(0xcc, 0xcc, 0xcc));
    }
    if(hasPresenceIndicator) {
        /*                      square        + space between square and text*/
        rectangle.adjust(m_presenceSquareSize + 2, 0, 0, 0);
    }
    painter.drawText( rectangle, Qt::AlignVCenter | Qt::AlignHCenter, m_text );
}

void BasicPeerWidget::updatePresence()
{
    QString text = tr("User Name : %1\n"
                      "Phone Numbers : %2")
        .arg(m_ui_remote->fullname())
        .arg(b_engine->phonenumbers(m_ui_remote).join(", "));
    QString availstate = m_ui_remote->availstate();
    QVariantMap presencedetails = b_engine->getOptionsUserStatus().value(availstate).toMap();
    if (m_ui_remote->enableclient()) {
        text.append("\n");
        text.append(tr("Status : %1").arg(presencedetails.value("longname").toString()));
    }
    setToolTip(text);
    m_presenceColor.setNamedColor(presencedetails.value("color").toString());
}

void BasicPeerWidget::updatePhoneConfig(const QString & /*xphoneid*/)
{
//     qDebug() << Q_FUNC_INFO;
//     QString ipbxid = m_ui_remote->ipbxid();
//     if (! m_ui_remote->phonelist().isEmpty()) {
//         QString xphoneid = QString("%1/%2").arg(ipbxid).arg(m_ui_remote->phonelist()[0]);
//         const PhoneInfo * phoneinfo = b_engine->phone(xphoneid);
//         if (phoneinfo != NULL) {
//             QString color = "white"; // XXXX function of phoneinfo->hintstatus();
//             m_color.setNamedColor(color);
//             update();
//         }
//     }
}

void BasicPeerWidget::updatePhoneStatus(const QString & /*xphoneid*/)
{
}

void BasicPeerWidget::getConfig()
{
    m_presenceSquareSize = b_engine->getConfig("guioptions.presenceindicatorsize").toInt();
    if ((m_presenceSquareSize<=0)||(m_presenceSquareSize>20)) {
        m_presenceSquareSize = 5;
    }
}

void BasicPeerWidget::updateWidth()
{
    QFontMetrics fontMetrics(font());
    QSize size = fontMetrics.size(0, m_text);
    size.rwidth() += 1 + m_presenceSquareSize + 2;
    // maximum width for PeerWidget
    if (size.width() > maxWidthWanted()) {
        size.setWidth(maxWidthWanted());
    }
    setMinimumSize(size);
}

void BasicPeerWidget::updateConfig()
{
    getConfig();
    updateWidth();
    update();
}

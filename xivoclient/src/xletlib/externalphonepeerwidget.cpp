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
#include <QPainter>
#include <QLabel>
#include "externalphonepeerwidget.h"
#include "externalphonedialog.h"
#include "baseengine.h"

ExternalPhonePeerWidget::ExternalPhonePeerWidget(
    const QString & label,
    const QString & number)
    : BasePeerWidget(NULL)
{
    m_number = number;
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    setText(label);
    setToolTip(tr("Label : %1\n"
                  "Phone Number : %2")
               .arg(label)
               .arg(number));
    m_editable = true;  // allow "edit" option in context menu
}

void ExternalPhonePeerWidget::setText(const QString & text)
{
    m_text = text;
    // calculate size
    QFontMetrics fontMetrics( font() );
    QSize size = fontMetrics.size(0, m_text);
    // set a maximum width PeerWidget
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
void ExternalPhonePeerWidget::paintEvent(QPaintEvent * /*event*/)
{
    QRect rectangle = contentsRect();
    QPainter painter( this );
    // draw the color rectangle
    painter.setBrush( QColor(192, 160, 160) );
    painter.setPen( Qt::NoPen );
    //painter.drawRect( rectangle );
    painter.drawRect( rectangle.adjusted(1, 0, -1, 0) );
    painter.drawRect( rectangle.adjusted(0, 1, 0, -1) );
    // write the text
    painter.setPen(Qt::SolidLine);
    painter.drawText( rectangle, Qt::AlignVCenter | Qt::AlignHCenter, m_text );
}

/*! \brief run a dialog to edit number and label
 *
 *
 */
void ExternalPhonePeerWidget::edit()
{
    ExternalPhoneDialog dialog(this, m_text, m_number);
    if(dialog.exec()) {
        if(!dialog.number().isEmpty())
            m_number = dialog.number();
        if(!dialog.label().isEmpty())
            setText( dialog.label() );
    }
}

/* XiVO Client
 * Copyright (C) 2015 Avencall
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

#include <QPainter>
#include <QStyleOptionTab>

#include "tabber_style.h"

QSize tab_size = QSize(60, 55);
QColor tab_icon_active_color = QColor("#2c2927");
QColor tab_icon_inactive_color = QColor("#6c6763");


TabberStyle::TabberStyle(QStyle *style)
    : QProxyStyle(style),
      selected_tab_indicator(":/images/selected-tab.png")
{
}

QSize TabberStyle::sizeFromContents(ContentsType type,
                                    const QStyleOption *option,
                                    const QSize &size,
                                    const QWidget *widget) const
{
    if (type == QStyle::CT_TabBarTab) {
        return tab_size;
    }
    return QProxyStyle::sizeFromContents(type, option, size, widget);
}

void TabberStyle::drawControl(ControlElement element,
                              const QStyleOption *option,
                              QPainter *painter,
                              const QWidget *widget) const
{
    if (element == CE_TabBarTabLabel) {
        if (const QStyleOptionTab *option_tab = qstyleoption_cast<const QStyleOptionTab *>(option)) {
            QColor icon_color;
            if (option_tab->state & State_MouseOver || option_tab->state & State_Selected) {
                icon_color = tab_icon_active_color;
            } else {
                icon_color = tab_icon_inactive_color;
            }

            QPixmap icon_image = option_tab->icon.pixmap(option_tab->iconSize);
            QPainter tint_painter(&icon_image);
            tint_painter.setCompositionMode(QPainter::CompositionMode_SourceAtop);
            tint_painter.fillRect(icon_image.rect(), icon_color);
            tint_painter.end();

            QRect icon_rect = QRect();
            icon_rect.setSize(option_tab->iconSize);
            icon_rect.moveCenter(option_tab->rect.center());

            painter->drawPixmap(icon_rect, icon_image);

            if (option_tab->state & State_Selected) {
                // We need a SVG image, because this will break on high-DPI displays
                QRect select_indicator_rect;
                select_indicator_rect.setSize(this->selected_tab_indicator.size());
                select_indicator_rect.moveCenter(option_tab->rect.center());
                select_indicator_rect.setX(option_tab->rect.right() - select_indicator_rect.width() / 2);
                painter->drawPixmap(select_indicator_rect, this->selected_tab_indicator);
            }
            return;
        }
    } else if (element == CE_TabBarTabShape) {
        // Draw nothing, leave the tab transparent
        return;
    }
    QProxyStyle::drawControl(element, option, painter, widget);
}

void TabberStyle::drawPrimitive(PrimitiveElement element,
                                const QStyleOption * option,
                                QPainter * painter,
                                const QWidget * widget) const
{
    if (element == QStyle::PE_IndicatorTabTear) {
        // This is useful when some tabs are hidden because the window is not high enough.
        // Draw nothing
        return;
    }
    QProxyStyle::drawPrimitive(element, option, painter, widget);
}

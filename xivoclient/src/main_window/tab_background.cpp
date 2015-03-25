/* Copyright (C) 2015, Avencall
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

#include "tab_background.h"

#include <QDebug>
#include <QPainter>
#include <QTabBar>
#include <QTabWidget>
#include <QVBoxLayout>

TabBackground::TabBackground(QTabWidget *tab_widget, QWidget *parent)
    : QWidget(parent)
{
    this->tab_widget = tab_widget;
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(tab_widget);

    this->gradient.setColorAt(0, QColor("#9E9995"));
    this->gradient.setColorAt(1, QColor("#8C8783"));
}

void TabBackground::paintEvent(QPaintEvent */*event*/)
{
    QRect tab_bar_rect = tab_widget->tabBar()->rect();
    this->gradient.setStart(tab_bar_rect.topLeft());
    this->gradient.setFinalStop(tab_bar_rect.topRight());

    QPainter painter(this);
    QRect paint_rect = this->rect();
    paint_rect.setRight(tab_widget->tabBar()->rect().right());
    painter.fillRect(paint_rect, QBrush(this->gradient));
    painter.end();
}

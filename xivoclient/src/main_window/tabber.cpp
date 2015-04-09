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

#include "tabber.h"

#include <QDebug>
#include <QPainter>
#include <QTabBar>
#include <QTabWidget>
#include <QVBoxLayout>

/* This class aims to paint the background for QTabWidget, because QTabWidget is
   unable to paint the background of the zone where there are no tabs */

Tabber::Tabber(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    this->m_gradient.setColorAt(0, QColor("#9E9995"));
    this->m_gradient.setColorAt(1, QColor("#8C8783"));

    this->m_tab_widget = new QTabWidget(this);
    this->m_tab_widget->setObjectName("tabber");
    layout->addWidget(this->m_tab_widget);
    this->m_tab_widget->setTabPosition(QTabWidget::West);
    // A little more space before the first tab
    this->m_tab_widget->setStyleSheet("#tabber::tab-bar {top: 17px;}"
                                      "#tabber::pane {border: 0px;}");

    this->m_tab_widget->tabBar()->setAttribute(Qt::WA_Hover);
    this->m_tab_widget->tabBar()->setStyle(&this->m_tabber_style);
    this->m_tab_widget->tabBar()->setIconSize(QSize(30, 30));
    this->m_tab_widget->tabBar()->setDrawBase(true);
}

void Tabber::paintEvent(QPaintEvent */*event*/)
{
    QRect tab_bar_rect = this->m_tab_widget->tabBar()->rect();
    this->m_gradient.setStart(tab_bar_rect.topLeft());
    this->m_gradient.setFinalStop(tab_bar_rect.topRight());

    QPainter painter(this);
    QRect paint_rect = this->rect();
    paint_rect.setRight(tab_bar_rect.right());
    painter.fillRect(paint_rect, QBrush(this->m_gradient));
    painter.end();
}

QTabWidget *Tabber::tabWidget()
{
    return this->m_tab_widget;
}

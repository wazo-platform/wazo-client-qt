/* XiVO Client
 * Copyright (C) 2007-2011, Avencall
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

#include <QHBoxLayout>
#include <QMovie>
#include <QLabel>

// Template class, the header includes the implementation
// #include "waitingwidget.h"

template <class WidgetType>
WaitingWidget<WidgetType>::WaitingWidget(WidgetType * widget, bool showanim)
    : QWidget(), m_widget(widget), m_locked(false), m_showanim(showanim)
{
    QHBoxLayout * layout = new QHBoxLayout();
    layout->setMargin(0);
    layout->setSpacing(5);
    layout->setAlignment(Qt::AlignLeft);
    setLayout(layout);

    // Waiting animation
    m_waitanim = new QLabel(this);
    // waitanim.gif comes from http://ajaxload.info/ (color XiVO #f37224)
    m_waitanim->setMovie(new QMovie(":/images/services/waitanim.gif"));
    m_waitanim->hide();

    layout->addWidget(widget);
    layout->addWidget(m_waitanim);
}

template <class WidgetType>
WidgetType * WaitingWidget<WidgetType>::widget()
{
    return m_widget;
}

template <class WidgetType>
void WaitingWidget<WidgetType>::lock()
{
    m_widget->clearFocus();
    m_widget->setEnabled(false);
    if (m_showanim) {
        m_waitanim->show();
        m_waitanim->movie()->start();
    }
    m_locked = true;
}

template <class WidgetType>
void WaitingWidget<WidgetType>::unlock(bool enabled)
{
    m_widget->setEnabled(enabled);
    if (m_showanim) {
        m_waitanim->hide();
        m_waitanim->movie()->stop();
    }
    m_locked = false;
}

template <class WidgetType>
bool WaitingWidget<WidgetType>::locked()
{
    return m_locked;
}

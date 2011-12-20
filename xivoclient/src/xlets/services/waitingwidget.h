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

#ifndef __WAITINGWIDGET_H__
#define __WAITINGWIDGET_H__

#include <QWidget>

class QString;
class QLabel;

/*! \brief Wrapper to display a warning icon beside a QWidget*/
template <class WidgetType>
class WaitingWidget : public QWidget
{
    // Q_OBJECT does not support template classes

    public:
    WaitingWidget(WidgetType * = NULL, bool = true);
        void lock();
        void unlock(bool = true);
        WidgetType * widget();
        bool locked();

    private:
        WidgetType * m_widget;
        QLabel * m_waitanim;
        bool m_locked;
        bool m_showanim;
};

/* Template class need to be implemented in the same place that it is defined.
 * This is because the compiler can't know with which type the template will be
 * used, this will be detected on compilation.
 */
#include "waitingwidget.cpp"

#endif

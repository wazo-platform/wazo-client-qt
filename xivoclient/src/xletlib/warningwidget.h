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

#ifndef __WARNINGWIDGET_H__
#define __WARNINGWIDGET_H__

#include <QObject>
#include <QWidget>

#include "xletlib_export.h"

class QLabel;
class QString;

/*! \brief Wrapper to display a warning icon beside a QWidget*/
template <class WidgetType>
class WarningWidget : public QWidget
{
    // Q_OBJECT does not support template classes

    public:
        WarningWidget(WidgetType *, QString s = "", bool visible = true);
        void showWarning();
        void hideWarning();
        void setWarningVisible(bool);
        WidgetType * widget();
    private:
        WidgetType * m_widget;
        QLabel * m_warning;
};

/* Template class need to be implemented in the same place that it is defined.
 * This is because the compiler can't know with which type the template will be
 * used, this will be detected on compilation.
 */
#include "warningwidget.cpp"

#endif

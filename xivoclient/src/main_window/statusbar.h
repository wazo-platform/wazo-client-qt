/* XiVO Client
 * Copyright (C) 2013, Avencall
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

#ifndef __STATUSBAR_H__
#define __STATUSBAR_H__

#include <QtGui>
#include <QList>
#include <QStatusBar>

#include <xletlib/functests.h>

#include "main_window/main_window.h"

class MainWindow;

class Statusbar : public QStatusBar
{
    Q_OBJECT
    FUNCTESTED

    public:
        Statusbar(MainWindow *parent);
        ~Statusbar();

    private slots:
        void setStatusLogged();
        void setStatusNotLogged();
        void confUpdated();

    protected:

    private:
        QStatusBar *m_statusbar;

        QLabel *m_status;  //!< status indicator
        QLabel *m_padlock; //!< padlock icon (SSL indicator)
        QLabel *m_config_profile; //!< profile indicator (status bar)

        QPixmap m_pixmap_disconnected;
        QPixmap m_pixmap_connected;
};

#endif

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

#ifndef __BASICPEERWIDGET_H__
#define __BASICPEERWIDGET_H__

#include "gui_export.h"

#include <QtGui>

#include "basepeerwidget.h"

/*! \brief Simple widget to display a Peer
 *
 * Only display the name of the user in a color rectangle
 * which gives the status of its telephone :
 * Green for available, blue for ringing, red for online.
 * More informations are given by the tool tip. */
class GUI_EXPORT BasicPeerWidget : public BasePeerWidget
{
    Q_OBJECT

    public:
        BasicPeerWidget(const UserInfo *);
        void updateAgentConfig(const QString &) {};
        void updateAgentStatus(const QString &) {};
        void updatePhoneConfig(const QString &);
        void updatePhoneStatus(const QString &);
        void updatePresence();
        void setName(const QString &name) { setText(name); };
    
    public slots:
        void updateConfig();

    protected:
        void paintEvent(QPaintEvent *);

    private:
        void setText(const QString &);  //!< Set displayed text
        void getConfig(); //!< Get config from BaseEngine
        void updateWidth(); //!< Set new width, when text or presence square changed

    private:
        QString m_text;  //!< Text to display
        QColor m_color;  //!< color
        QColor m_presenceColor;  //!< color of presence indicator
        int m_presenceSquareSize;  //!< size of the presence indicator
};

#endif

/* XiVO Client
 * Copyright (C) 2007-2010, Proformatique
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

/* $Revision: 8992 $
 * $Date: 2010-08-24 05:59:38 -0400 (mar 24 aoû 2010) $
 */

#ifndef __GROUP_H__
#define __GROUP_H__
#include <QObject>
#include <QRect>
#include <QString>
#include <QColor>
#include <QPoint>
#include <QList>

#include "peerslayout.h"
#include "switchboardwindow.h"

class Group ;

class Group : public QObject
{
    public:
        Group(QObject * parent=0);
        void setName(const QString &name) { m_name = name; };
        const QString& name() const { return m_name; };
        void setRect(const QRect &rect) { m_rect = rect; };
        const QRect& rect() const { return m_rect; };
        QRect& rect() { return m_rect; };
        void setColor(const QColor &color) { m_color = color; };
        const QColor& color() const { return m_color; };

        int move(QList<Group *> &groups, PeersLayout *on, QPoint &deltaGrid);
        int resize(QList<Group *> &groups, PeersLayout *on, QPoint &deltaGrid, int direction);

    private:
        QString m_name;
        QRect m_rect;
        QColor m_color;
};

#endif

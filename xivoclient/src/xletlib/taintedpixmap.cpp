/* XiVO Client
 * Copyright (C) 2007-2013, Avencall
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

#include "taintedpixmap.h"

QHash<QString, QPixmap> TaintedPixmap::m_pixmap_cache = QHash <QString, QPixmap>();

TaintedPixmap::TaintedPixmap(const QString &pixmap_path, const QColor &bg_color)
{
    m_pixmap_hash = pixmap_path + bg_color.name();

    if (!m_pixmap_cache.contains(m_pixmap_hash)) {
        QPixmap pix = QPixmap(pixmap_path);
        QPixmap pixs = QPixmap(pix);
        QPainter painter;

        painter.begin(&pixs);
        painter.fillRect(painter.viewport(), bg_color);
        painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
        painter.drawPixmap(painter.viewport(), pix);
        painter.setCompositionMode(QPainter::CompositionMode_DestinationIn);
        painter.drawPixmap(painter.viewport(), pix);
        painter.end();

        m_pixmap_cache.insert(m_pixmap_hash, pixs);
    }
}

QPixmap TaintedPixmap::getPixmap()
{
    return m_pixmap_cache.value(m_pixmap_hash);
}

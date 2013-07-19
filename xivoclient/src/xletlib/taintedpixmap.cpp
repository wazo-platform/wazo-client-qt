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

TaintedPixmap::TaintedPixmap(const QString &pixmap_path, const QColor &tint_color)
{
    m_pixmap_hash = pixmap_path + tint_color.name();

    if (!m_pixmap_cache.contains(m_pixmap_hash)) {
        QPixmap shape(pixmap_path);
        QPixmap result(shape);
        QPainter painter;

        painter.begin(&result);
        painter.fillRect(painter.viewport(), tint_color);
        // Apply the shape on top of the tint
        painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
        painter.drawPixmap(painter.viewport(), shape);
        // Remove unused background
        painter.setCompositionMode(QPainter::CompositionMode_DestinationIn);
        painter.drawPixmap(painter.viewport(), shape);
        painter.end();

        m_pixmap_cache.insert(m_pixmap_hash, result);
    }
}

QPixmap TaintedPixmap::getPixmap()
{
    return m_pixmap_cache.value(m_pixmap_hash);
}

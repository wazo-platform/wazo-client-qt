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

// Initialize static members
QHash<QString, QPixmap*> TaintedPixmap::m_pixmap_cache = QHash <QString, QPixmap*>();

/*! \brief Constructor create if needed a tainted pixmap
 */
TaintedPixmap::TaintedPixmap(const QString &pixmap_path, const QColor &bg_color)
{
    m_pixmap_hash = pixmap_path + bg_color.name();

    if (!m_pixmap_cache.contains(m_pixmap_hash)) {
        QPixmap tainted_pixmap = this->createTaintedPixmap(pixmap_path, tint_color);
        m_pixmap_cache.insert(m_pixmap_hash, tainted_pixmap);
    }
}

QPixmap TaintedPixmap::createTaintedPixmap(const QString &pixmap_path, const QColor &tint_color)
{
    #if defined(Q_WS_MAC)
    return this->createTaintedPixmapMacOnly(pixmap_path, tint_color);
    #else
    return this->createTaintedPixmapNotMac(pixmap_path, tint_color);
    #endif
}

QPixmap TaintedPixmap::createTaintedPixmapNotMac(const QString &pixmap_path, const QColor &tint_color)
{
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

    return result;
}

QPixmap TaintedPixmap::createTaintedPixmapMacOnly(const QString &pixmap_path, const QColor &tint_color)
{
    QPixmap shape(pixmap_path);
    QPixmap result(shape);
    QPainter painter;

    painter.begin(&result);
    painter.drawPixmap(painter.viewport(), shape);
    // Create a color mask
    painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
    painter.fillRect(painter.viewport(), tint_color);
    // Apply the shape on top of the mask
    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
    painter.drawPixmap(painter.viewport(), shape);
    painter.end();

    return result;
}

QPixmap TaintedPixmap::getPixmap()
{
    return * m_pixmap_cache.value(m_pixmap_hash);
}

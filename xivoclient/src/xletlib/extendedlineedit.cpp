/*
 * Copyright 2007-2017 The Wazo Authors  (see the AUTHORS file)
 *
 * This program is free software: you can redistribute it and/or modify
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
 * This programm is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QDropEvent>
#include <QDebug>
#include <QLineEdit>
#include <QUrl>
#include <QMimeData>

#include "extendedlineedit.h"

/*! \brief Constructor
 */
ExtendedLineEdit::ExtendedLineEdit(QWidget * parent)
    : QLineEdit(parent)
{
    setAcceptDrops(true);
}

/*! \brief receive drop event
 */
void ExtendedLineEdit::dropEvent(QDropEvent *)
{
    // qDebug() << Q_FUNC_INFO << event->mimeData()->text() << event->pos();
}


FileNameLineEdit::FileNameLineEdit(QWidget * parent)
    : QLineEdit( parent )
{
}

void FileNameLineEdit::dropEvent(QDropEvent * event)
{
    // qDebug() << Q_FUNC_INFO << event;
    QList<QUrl> urls = event->mimeData()->urls();
    if (urls.size() > 0) {
        QUrl url = urls[0];
        QLineEdit::setText(url.toLocalFile());
    }
    // QLineEdit::dropEvent(event);
}

void FileNameLineEdit::dragEnterEvent(QDragEnterEvent * event)
{
    // qDebug() << Q_FUNC_INFO << event;
    event->acceptProposedAction();
}

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

#ifndef __EXTENDEDTABLEWIDGET_H__
#define __EXTENDEDTABLEWIDGET_H__

#include "xletlib_export.h"

#include <QtGui>
#include <baseengine.h>


/*! \brief Table class inheriting QTableWidget with contextMenu added.
 */
class XLETLIB_EXPORT ExtendedTableWidget : public QTableWidget
{
    Q_OBJECT

    public:
        ExtendedTableWidget(QWidget *parent=0);
        ExtendedTableWidget(int rows, int columns, QWidget *parent=0);
        void setEditable(bool editable) { m_editable = editable; };

    protected:
        void contextMenuEvent(QContextMenuEvent *);
        void dragEnterEvent(QDragEnterEvent *);
        void dragMoveEvent(QDragMoveEvent *);
        void dropEvent(QDropEvent *);
        void mouseMoveEvent(QMouseEvent *);

    private slots:
        void dialNumber();
        void sendMail();
        void itransfer();
        void dtransfer();
        void remove();

    private:
        bool m_editable;
};

#endif

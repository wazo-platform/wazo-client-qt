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

/* $Revision$
 * $Date$
 */

#ifndef __DIRECTORYPANEL_H__
#define __DIRECTORYPANEL_H__

#include <QContextMenuEvent>
#include <QDebug>
#include <QDesktopServices>
#include <QDropEvent>
#include <QFocusEvent>
#include <QHBoxLayout>
#include <QLabel>
#include <QMenu>
#include <QPushButton>
#include <QUrl>
#include <QVBoxLayout>

#include "directorypanel.h"
#include <baseengine.h>
#include <extendedtablewidget.h>
#include <extendedlineedit.h>
#include <userinfo.h>
#include <phoneinfo.h>
#include <xivoconsts.h>
#include <xlet.h>


/*! \brief Directory allowing search
 */
class DirectoryPanel : public XLet
{
    Q_OBJECT

    public:
        DirectoryPanel(QWidget *parent=0);

    protected:
        void dropEvent(QDropEvent *);
        void focusInEvent(QFocusEvent *);
    private slots:
        void startSearch();
        void itemClicked(QTableWidgetItem *);
        void itemDoubleClicked(QTableWidgetItem *);

    public slots:
        void setSearchResponse(const QStringList &, const QStringList &);
        void stop();

    private:
        ExtendedLineEdit *m_searchText;  //!< search text input
        ExtendedTableWidget *m_table;  //!< table to display results
        QPushButton *m_searchButton;  //!< button
        QString m_mailAddr;  //!< used to store email address
};

#endif

/* XiVO Client
 * Copyright (C) 2007-2009, Proformatique
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

#include "xlet.h"

class QDropEvent;
class QFocusEvent;
class QPushButton;
class QTableWidget;
class QTableWidgetItem;

class BaseEngine;
class ExtendedLineEdit;
class ExtendedTableWidget;
class PeerChannel;
class UserInfo;

/*! \brief Directory allowing search
 */
class DirectoryPanel : public XLet
{
    Q_OBJECT
public:
    DirectoryPanel(BaseEngine * engine, QWidget * parent = 0);
    ~DirectoryPanel();
protected:
    void dropEvent(QDropEvent *);
    void focusInEvent(QFocusEvent *);
//    void contextMenuEvent(QContextMenuEvent *);
signals:
    //! start a search
    void searchDirectory(const QString &);
    void copyNumber(const QString &);
    void actionCall(const QString &,
                    const QString &,
                    const QString &);
private slots:
    void startSearch();
    void itemClicked(QTableWidgetItem *);
    void itemDoubleClicked(QTableWidgetItem *);
public slots:
    void setGuiOptions(const QVariant &) {};
    void setSearchResponse(const QStringList &, const QStringList &);
    void stop();
private:
    ExtendedLineEdit * m_searchText;        //!< search text input
    ExtendedTableWidget * m_table;                //!< table to display results
    QPushButton * m_searchButton;        //!< button
    QString m_numberToDial;                //!< used to store number to dial or to transfer to
    QString m_mailAddr;                //!< used to store email address
    QRegExp m_re_number;    //!< regexp used to recognize phone numbers
};

#endif

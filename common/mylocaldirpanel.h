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

#ifndef __MYLOCALDIRPANEL_H__
#define __MYLOCALDIRPANEL_H__

#include <QObject>
#include <QFile>
#include "xlet.h"

class QLineEdit;
class BaseEngine;
class UserInfo;
class ExtendedTableWidget;
class SearchDialog;

/*! \brief Local Directory Panel
 *
 * Displays a local contact base that can be imported/exported
 * from/to .csv files.
 */
class MyLocalDirPanel : public XLet
{
    Q_OBJECT
public:
    MyLocalDirPanel(BaseEngine *, QWidget * parent = 0);
    ~MyLocalDirPanel();
public slots:
    void openNewContactDialog();
    void importContacts();
    void exportContacts();
    void findNext();
    void itemSelectionChanged();
    void removeAllContacts();
signals:
    void actionCall(const QString &, const QString &, const QString &);
private:
    void loadFromFile(QFile & file);
    void saveToFile(QFile & file);
    int findCol(QStringList, QStringList);
    QString getSaveFile() const;

    ExtendedTableWidget * m_table;  //!< Table widget to display contacts
    SearchDialog * m_searchBox; //!< Search dialog box
};

#endif /* __MYLOCALDIRPANEL_H__ */


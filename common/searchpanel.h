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

/* $Revision$
 * $Date$
 */

#ifndef __SEARCHPANEL_H__
#define __SEARCHPANEL_H__

#include <QHash>
#include <QVariant>

#include "xlet.h"
#include "peeritem.h"

class QGridLayout;
class QVBoxLayout;

class BaseEngine;
class ExtendedLineEdit;
class UserInfo;

/*! \brief search panel widget
 */
class SearchPanel : public XLet
{
    Q_OBJECT
public:
    SearchPanel(BaseEngine *,
                QWidget * parent = 0);
    ~SearchPanel();        //!< Destructor
public slots:
    void affTextChanged(const QString &);
    void updatePeerAgent(double,
                         const QString &,
                         const QString &,
                         const QVariant &);
    void removePeer(const QString &);
    void removePeers();
    void updateDisplay();
    void updateUser(UserInfo *);
signals:
    void askCallerIds();
private:
    QHash<QString, PeerItem *> m_peerhash;        //!< PeerItem hash
    QGridLayout * m_peerlayout;        //!< layout object
    ExtendedLineEdit * m_input;        //!< widget for search string input
        
    int m_maxdisplay;        //!< max number of peers displayed on the search panel
    int m_ncolumns;
    QString m_searchpattern;
};

#endif

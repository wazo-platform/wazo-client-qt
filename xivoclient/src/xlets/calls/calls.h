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

#ifndef __CALLSTACKWIDGET_H__
#define __CALLSTACKWIDGET_H__

#include <QtGui>

#include "xlet.h"
#include "xletinterface.h"
#include "baseengine.h"
#include "callwidget.h"
#include "xivoconsts.h"


/*! \brief Widget displaying the current open channels for a phone line.
 */
class XletCalls : public XLet
{
    Q_OBJECT

    public:
        XletCalls(QWidget *parent=0); //!< Constructor

    public slots:
        void updateDisplay();
        void updatePhoneStatus(const QString &);
        void updateChannelStatus(const QString &);
        void hupchan(const QString &);
        void transftonumberchan(const QString &);
        void monitorPeerChanged();

    protected:
        void dragEnterEvent(QDragEnterEvent *event);
        void dropEvent(QDropEvent *event);

    signals:
        //! originate, transfer & atxfer signals
        void changeTitle(const QString &);  //!< change Title
        void monitorPeerRequest(const QString &);  //!< send the userid of the new monitored peer

    private:
        QVBoxLayout * m_layout;  //!< Vertical Layout used
        QHash<QString, CallWidget *> m_affhash;  //!< List of CallWidget Widgets
};

class XLetCallsPlugin : public QObject, XLetInterface
{
    Q_OBJECT
    Q_INTERFACES(XLetInterface)

    public:
        XLet *newXLetInstance(QWidget *parent=0);
};

#endif

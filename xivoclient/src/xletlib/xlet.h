/* XiVO Client
 * Copyright (C) 2007-2015 Avencall
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

#ifndef __XLET_H__
#define __XLET_H__

#include <QVariantMap>
#include <QtWidgets>
#include <QtPlugin>

#include <storage/userinfo.h>

#include "xletlib_export.h"

/*! \brief base class for all XLets
 *
 * To implement a new XLet, you need to inherit from
 * this class. Use setTitle() in the constructor to set
 * the name of your XLet. The constructor should also connect
 * appropriate signals and slots to the BaseEngine object.
 *
 * doGUIConnects() can be superclassed for xlet needing to connect
 * signal/slots from the MainWidget class.
 */
class XLETLIB_EXPORT XLet : public QWidget
{
    Q_OBJECT

    public:
        XLet(QWidget *parent=0, const QString &title=QString(""), const QString &icon_path=QString(""));
        const QString & title() const;
        const QString & iconPath() const;

        /*! \brief connects signals/slots to the main GUI window
         *
         * Reimplement this method for connecting signals/slots to the
         * top level window.
         * The default implementation does nothing.
         * \see MainWidget
         */
        virtual void doGUIConnects(QWidget *) {};
    signals:
        void ipbxCommand(const QVariantMap &);
    public slots:
        void localUserInfoDefined();
        void monitoredUserInfoDefined();
    protected:
        QString m_xuserid;
        const UserInfo * m_ui;
        const UserInfo * m_monitored_ui;
    private:
        QString m_title;
        QString m_icon_path;
};

#endif

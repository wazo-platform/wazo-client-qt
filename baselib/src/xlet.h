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

#ifndef __XLET_H__
#define __XLET_H__

#include <baselib_export.h>
#include <QVariantMap>
#include <QWidget>
#include <baseengine.h>


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
class BASELIB_EXPORT XLet : public QWidget
{
    Q_OBJECT

    public:
        XLet(QWidget *parent=0);  //! Constructor
        const QString& title() const { return m_title; };  //! title accessor

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
        void logAction(const QString &);
        void shouldNotOccur(const QString &, const QString &); //!< signal to log tricky situations
    protected:
        void setTitle(const QString &title) { m_title = title; }; //! set title of the XLet
    private:
        QString m_title;    //!< title of the XLet
};
#endif


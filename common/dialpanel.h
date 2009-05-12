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

#ifndef __DIALPANEL_H__
#define __DIALPANEL_H__

#include <QObject>
#include <QWidget>
#include <QList>

class QComboBox;
class QLabel;
class QMouseEvent;
class QVBoxLayout;

class BaseEngine;
class UserInfo;

/*! \brief Simple widget to enter a number and dial it
 */
class DialPanel : public QWidget
{
    Q_OBJECT
public:
    DialPanel(BaseEngine *,
              const QVariant &,
              QWidget * parent = 0);
    ~DialPanel();
protected:
    /*void mouseMoveEvent(QMouseEvent *);        //!< Catch mouse press events */
    /*void mousePressEvent(QMouseEvent *); */
    void dragEnterEvent(QDragEnterEvent *);
    void dropEvent(QDropEvent *);
public slots:
    void setGuiOptions(const QVariant &) {};
    void setUserInfo(const UserInfo *) {};
    void inputValidated();
    void clearlist();
    void setNumberToDial(const QString &);
signals:
    void textEdited(const QString &);   //!< text has changed
    void actionCall(const QString &,
                    const QString &,
                    const QString &);   //!< originates a number
private:
    BaseEngine * m_engine;      //!< base engine
    QLabel * m_lbl;             //!< label
    QComboBox * m_input;        //!< input widget
};

#endif

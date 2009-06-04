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

#ifndef __BASEPEERWIDGET_H__
#define __BASEPEERWIDGET_H__

#include <QWidget>
#include <QMouseEvent>

class UserInfo;
class BaseEngine;

/*! \brief Abstract class for "Peer" Widgets
 *
 */
class BasePeerWidget : public QWidget
{
    Q_OBJECT
public:
    BasePeerWidget(BaseEngine *, UserInfo *);
    //! change username to be displayed
    virtual void setName(const QString &) = 0;
    //! set Tooltip for agent
    virtual void setAgentToolTip(const QString &, const QStringList &) = 0;
    //! set agent state
    virtual void setAgentState(const QString & color) = 0;
    //! update presence information displayed
    virtual void updatePresence() = 0;
    //! update phones information displayed
    virtual void updatePhonesStates() = 0;
    //! Phone number
    virtual const QString & number() const { return m_number; };
    //! Name
    virtual QString name() const;
    //! maximum width for "small" items
    int maxWidthWanted() const { return m_maxWidthWanted; };
protected:
    void mouseDoubleClickEvent(QMouseEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void contextMenuEvent(QContextMenuEvent *);
    void dragEnterEvent(QDragEnterEvent *);
    void dragMoveEvent(QDragMoveEvent *);
    void dropEvent(QDropEvent *);
signals:
    //! originate, transfer, etc...
    void actionCall(const QString &,
                    const QString &,
                    const QString &dst = "");
    //! hide the widget from the containing window
    void removeFromPanel();
protected slots:
    void dial();
    void peerdial();
    void hangup();
    void intercept();
    void intercept2();
    void transfer();
    void itransfer();
    void vmtransfer();
    void parkcall();
protected:
    BaseEngine * m_engine;      //!< Xivo CTI Client Engine
    UserInfo * m_ui;            //!< user infos
    QPoint m_dragstartpos;            //!< drag start position
    QAction * m_dialAction;     //!< action to dial this number
    QAction * m_removeAction;   //!< action to remove this peer from the window
    QAction * m_interceptAction;    //!< action to intercept ringing calls to this number
    QString m_number;   //!< phone number (filled if m_ui is NULL)
    bool m_editable;    //!< editable from a user point of view
private:
    int m_maxWidthWanted;   //!< maximum width for "small" items
};

#endif


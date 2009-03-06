/* XIVO CTI clients
 * Copyright (C) 2007-2009  Proformatique
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License version 2 for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Linking the Licensed Program statically or dynamically with other
 * modules is making a combined work based on the Licensed Program. Thus,
 * the terms and conditions of the GNU General Public License version 2
 * cover the whole combination.
 *
 * In addition, as a special exception, the copyright holders of the
 * Licensed Program give you permission to combine the Licensed Program
 * with free software programs or libraries that are released under the
 * GNU Library General Public License version 2.0 or GNU Lesser General
 * Public License version 2.1 or any later version of the GNU Lesser
 * General Public License, and with code included in the standard release
 * of OpenSSL under a version of the OpenSSL license (with original SSLeay
 * license) which is identical to the one that was published in year 2003,
 * or modified versions of such code, with unchanged license. You may copy
 * and distribute such a system following the terms of the GNU GPL
 * version 2 for the Licensed Program and the licenses of the other code
 * concerned, provided that you include the source code of that other code
 * when and as the GNU GPL version 2 requires distribution of source code.
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
    BasePeerWidget(BaseEngine *, UserInfo *, const QVariant &);
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
    void hangup();
    void intercept();
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
    QString m_number;   //!< phone number (filled if m_ui is NULL)
    bool m_editable;    //!< editable from a user point of view
private:
    int m_maxWidthWanted;   //!< maximum width for "small" items
};

#endif


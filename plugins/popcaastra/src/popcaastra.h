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

#ifndef __POPCAASTRA_H__
#define __POPCAASTRA_H__

#include <QObject>
#include <QVBoxLayout>
#include "xlet.h"

namespace Ui {
    class PopcAastra;
}
class IncomingWidget;

/*! \brief POPC xlet for Aastra phones
 *
 * This xlet allow the operator to "drive" his phone from an xlet using SIP
 * sotify messages that are available to Aastra
 */
class PopcAastra : public XLet
{
    Q_OBJECT
public:
    PopcAastra(QWidget *parent=0);
    ~PopcAastra();
    int findFirstAvailableLine() const;
    void debugIncomingCalls() const;
private:
    void removeDefuncWidgets();
public slots:
    void updateDisplay();
    /*! \brief When a name is clicked on the destination list */
    void destinationClicked(int col, int row);
    /*! \brief refresh the list containing call destinations */
    void refreshDestination(const QString &);
    /*! \brief retrieves the channel list from the base engine and refresh the
     *  lines list
     */
    void refreshLines(const QString &);
    /*! \brief turns up the volume of the phone */
    void volUp();
    /*! \brief turns down the volume of the phone */
    void volDown();
    void navRight();
    /*!  \brief update the peer to be monitored */
    void monitorPeer(UserInfo *);
    void updateUserConfig(const QString &);
    void updateUserStatus(const QString &);
    void updatePhoneConfig(const QString &);
    void updatePhoneStatus(const QString &);
    void updateChannelStatus(const QString &);
    void hupline(int);
    void transftonumberchan(const QString &);
    void blindTransfer(int);
    void parkcall(const QString &);
private:
    UserInfo * m_monitored_ui;  //!< currently monitored user info
    Ui::PopcAastra * m_ui;
    QHash<QString, IncomingWidget *> m_incomingcalls;  //!< List of IncomingWidget
    QVBoxLayout * m_calls_list; //!< Container layout for the incoming calls widget

    QAction * m_hangupAction;
    QAction * m_transferToNumberAction;
    QAction * m_parkCallAction;

};

#endif /* __POPCAASTRA_H__ */


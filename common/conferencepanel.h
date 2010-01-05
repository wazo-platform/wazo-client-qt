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

#ifndef __CONFERENCEPANEL_H__
#define __CONFERENCEPANEL_H__

#include <QHash>
#include <QLabel>
#include <QList>
#include <QTreeWidget>
#include <QHeaderView>
#include <QObject>
#include <QVariant>
#include "xlet.h"
#include <stdio.h>

class QGridLayout;
class QProgressBar;
class QPushButton;
class QTabWidget;
class QTimerEvent;

class UserInfo;

/*! \brief Conference Panel
 */
class ConferencePanel : public XLet
{
    Q_OBJECT
public:
    ConferencePanel(BaseEngine *,
                    QWidget * parent = 0);
    ~ConferencePanel();
protected:
    void timerEvent(QTimerEvent *);
public slots:
    void setGuiOptions(const QVariantMap &);
    void meetmeEvent(double, const QVariant &);
    void meetmeInit(double, const QVariant &);
    void doMeetMeAction();
    void changeRoomPauseState();
    void doMeetMeAdminOnlyAction(QTreeWidgetItem *, int );
signals:
    void meetmeAction(const QString &, const QString &);    //!< action
private:
    void setProperties(double,
                       const QString &,
                       const QString &,
                       const QString &,
                       const QString &,
                       const QString &,
                       const QVariantMap &,
                       const QString &,
                       const bool &
                       );
    void addRoomTab(const QString &,
                    const QString &,
                    const QString &,
                    const QString &,
                    int are_we_inside=1);
    void delRoomTab(const QString &,
                    const QString &);
    void updateSummary();
    QWidget * createLeftUserList(QWidget *parent, const QString &,const QString &);
    void updateButtons(const QString &, const QString &);
    
    QHash<QString, QLabel *> m_infos;   //!< widgets for displaying name and phone number
    QHash<QString, QLabel *> m_timespent;   //!< call durations
    QHash<QString, QPushButton *> m_action_kick;      //!< kick buttons
    QHash<QString, QPushButton *> m_action_record;    //!< record buttons
    QHash<QString, QPushButton *> m_action_mute;      //!< mute buttons
    QHash<QString, QGridLayout *> m_layout;           //!< layouts
    QHash<QString, QTreeWidget *> m_user_not_authed_list;  //!< list of user not authed by admin
    QHash<QString, QPushButton *> m_action_chamber_toggle_pause;  //!< button to pause a conf chamber
    QHash<QString, QTreeWidgetItem *> m_user_not_authed;   //!< list of user not authed by admin

    QTabWidget * m_tw;              //!< Tab container
    QGridLayout * m_glayout;        //!< gridlayout
    QLabel * m_summary;
    bool m_show_record;
};

#endif

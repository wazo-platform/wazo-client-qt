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

#ifndef __CONFERENCEPANEL_H__
#define __CONFERENCEPANEL_H__

#include <QHash>
#include <QLabel>
#include <QList>
#include <QObject>
#include <QWidget>
#include <QVariant>

class QGridLayout;
class QProgressBar;
class QPushButton;
class QTabWidget;
class QTimerEvent;

class BaseEngine;
class UserInfo;

/*! \brief Conference Panel
 */
class ConferencePanel : public QWidget
{
    Q_OBJECT
public:
    ConferencePanel(BaseEngine *,
                    QWidget * parent = 0);
    ~ConferencePanel();
protected:
    void timerEvent(QTimerEvent *);
public slots:
    //! does nothing...
    void setGuiOptions(const QVariant &) {};
    //! does nothing...
    void setUserInfo(const UserInfo *) {};
    void meetmeEvent(double, const QVariant &);
    void meetmeInit(double, const QVariant &);
    void doMeetMeAction();
signals:
    void meetmeAction(const QString &, const QString &);    //!< action
private:
    void setProperties(double,
                       const QString &,
                       const QString &,
                       const QString &,
                       const QString &,
                       const QString &,
                       const QVariantMap &);
    void addRoomTab(const QString &,
                    const QString &,
                    const QString &);
    void delRoomTab(const QString &,
                    const QString &);
    void updateSummary();
    
    BaseEngine * m_engine;  //!< BaseEngine pointer
    QHash<QString, QLabel *> m_infos;   //!< widgets for displaying name and phone number
    QHash<QString, QLabel *> m_timespent;   //!< call durations
    QHash<QString, QPushButton *> m_action_kick;    //!< kick buttons
    QHash<QString, QPushButton *> m_action_record;  //!< record buttons
    QHash<QString, QPushButton *> m_action_mute;    //!< mute buttons
    QHash<QString, QGridLayout *> m_layout;         //!< layouts
    QTabWidget * m_tw;              //!< Tab container
    QGridLayout * m_glayout;        //!< gridlayout
    QLabel * m_summary;
};

#endif

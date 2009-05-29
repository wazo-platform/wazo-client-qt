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

#ifndef __IDENTITYDISPLAY_H__
#define __IDENTITYDISPLAY_H__

#include <QHash>
#include <QList>
#include <QObject>
#include <QVariant>
#include "xlet.h"

class QComboBox;
class QContextMenuEvent;
class QGridLayout;
class QProgressBar;
class QPushButton;
class QLabel;
class QFrame;

class UserInfo;
class IdentityAgent;

/*! \brief Identity Display
 */
class IdentityDisplay : public XLet
{
    Q_OBJECT
public:
    IdentityDisplay(BaseEngine *,
                    QWidget * parent = 0);
    void doGUIConnects(QWidget *);
public slots:
    void setGuiOptions(const QVariantMap &);
    void setUserInfo(const UserInfo *);
    void newAgentList(const QStringList &);
    void newQueueList(const QStringList &);
    void idxChanged(const QString &);
    void updatePresence(const QVariant &);
    void setOpt(const QString &, bool);
    void setForward(const QString &, const QVariant &);
    void updateUser(UserInfo *);
private slots:
    void contextMenuEvent(QContextMenuEvent *);
    void contextMenuAction();
signals:
    void setAvailState(const QString &, bool);
    void changeWatchedAgent(const QString &, bool);
    void setSystrayIcon(const QString &);
    void actionCall(const QString &,
                    const QString &,
                    const QString & dst = "");   //!< to hangup
    void ipbxCommand(const QVariantMap &);
private:
    void svcSummary();
    void setPhoneLines();
    void setupIcons();
    
    QGridLayout * m_glayout;
    
    QLabel * m_icon_user;
    QLabel * m_icon_phone;
    QLabel * m_icon_voicemail;
    
    QFrame * m_qvline1;
    QFrame * m_qvline2;
    
    QLabel * m_user;
    QLabel * m_phone;
    QLabel * m_voicemail_old;
    QLabel * m_voicemail_new;
    QLabel * m_voicemail_name;
    QLabel * m_phonenum;
    QLabel * m_phonecall;
    QLabel * m_phonecalltxt;
    QComboBox * m_presencevalue;
    QLabel * m_phonestatustxt;

    IdentityAgent * m_agent;
    
    QHash<QString, QString> m_presence_names;
    QHash<QString, QLabel *> m_lineaction;
    QHash<QString, QLabel *> m_linestatus;
    const UserInfo * m_ui;
    
    int m_col_user;
    int m_col_agent;
    int m_col_phone;
    int m_col_vm;
    int m_col_last;
    int m_nlines;
    Qt::Alignment m_iconAlign;
    Qt::Alignment m_textAlignVCenter;
    QFont m_gui_font;
    quint32 m_gui_buttonsize;
    quint32 m_loginkind;
    QStringList m_functions;
    QMap<QString, QVariant> m_comms;
    QMap<QString, QVariant> m_svcstatus;
};

#endif

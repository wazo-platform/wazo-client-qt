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

#ifndef __STATUSPANEL_H__
#define __STATUSPANEL_H__

#include <QHash>
#include <QList>
#include <QMap>
#include <QObject>
#include <QWidget>

#include "userinfo.h"

class QFrame;
class QGridLayout;
class QLabel;
class QLineEdit;
class QPushButton;

class UserInfo;

/*! \brief Operator widget used to answer and transfer calls
 */
class StatusPanel : public QWidget
{
    Q_OBJECT
        public:
    StatusPanel(QWidget * parent = 0);
    ~StatusPanel();
    enum Line {Ready, Ringing, Hangup, Wait, Transfer, WDTransfer, WITransfer, Online};
    public slots:
    void setGuiOptions(const QVariant &);
    void setUserInfo(const UserInfo *);
    void functionKeyPressed(int);
    void xferPressed();
    void clicked();
    void updateUser(UserInfo *);
 signals:
    void actionCall(const QString &,
                    const QString & src = "",
                    const QString & dst = "");
 private:
    void updateLine(const QString &, const QStringList &);
    void newCall(const QString &);
    void dtransfer();
    void itransfer();
    void changeCurrentChannel(const QString &, const QString &);
    QString getPeerChan(const QString & chan) const;
    void removeLine(const QString &);
        
    QGridLayout * m_glayout;
    const UserInfo * m_ui;
    QLabel * m_lbl;
        
    QHash<QString, QFrame *> m_vlinesl;
    QHash<QString, QFrame *> m_vlinesr;
    QHash<QString, QLabel *> m_statuses;
    QHash<QString, QHash<QString, QPushButton *> > m_actions;
    QHash<QString, QLineEdit *> m_tnums;
    QHash<QString, Line> m_linestatuses;
    QHash<QString, int> m_row;
        
    QMap<int, QStringList> m_actionkey;
        
    QString m_currentchannel;
    QStringList m_callchannels;
    QString m_tferchannel;
};

#endif

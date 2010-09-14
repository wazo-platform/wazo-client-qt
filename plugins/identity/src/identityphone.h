/* XiVO Client
 * Copyright (C) 2007-2010, Proformatique
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

/* $Revision: 8993 $
 * $Date: 2010-08-24 06:41:39 -0400 (mar 24 aoû 2010) $
 */

#ifndef __IDENTITYPHONE_H__
#define __IDENTITYPHONE_H__

#include <QWidget>
#include <QList>

#include <baseengine.h>

class QGridLayout;
class QLabel;
class QPixmap;
class UserInfo;

/*! \brief phone line display
 */
class IdentityPhoneLine : public QWidget
{
    Q_OBJECT

    public:
        IdentityPhoneLine(int linenum, QWidget *parent=0);
        void setPixmap(const QPixmap &pixmap);
        void setText(const QString &text);

    public slots:
        void setUserInfo(const UserInfo *);
        void hangup();
        void answer();
        void refuse();

    protected:
        void contextMenuEvent(QContextMenuEvent *);

    private:
        const UserInfo *m_ui;  //!< pointer to UserInfo
        int m_linenum;
        QLabel *m_action;
        QLabel *m_status;
};

/*! \brief Display Phone informations
 *
 * \see IdentityDisplay
 */
class IdentityPhone : public QWidget
{
    Q_OBJECT

    public:
        IdentityPhone(QWidget *parent=0);
        void svcSummary(QMap<QString, QVariant> &svcstatus);
    public slots:
        void setUserInfo(const UserInfo *);
        void updateUser(UserInfo *);
    private:
        void setPhoneLines();
        const UserInfo *m_ui;  //!< pointer to UserInfo

        QGridLayout *m_layout;  //!< layout
        QLabel *m_icon;  //!< icon
        QLabel *m_phone;  //!< Phone name label
        QLabel *m_phonecall;
        QLabel *m_phonecalltxt;
        QLabel *m_phonestatustxt;
        QList<IdentityPhoneLine *> m_lines;
};

#endif


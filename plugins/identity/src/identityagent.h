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

#ifndef __IDENTITYAGENT_H__
#define __IDENTITYAGENT_H__

#include <QDebug>
#include <QFrame>
#include <QVariant>
#include <QLabel>
#include <QGridLayout>
#include <QAction>
#include <QMenu>
#include <QContextMenuEvent>

/*! \brief Display Agent information
 *
 * \see IdentityDisplay
 */
class IdentityAgent : public QFrame
{
    Q_OBJECT

    public:
        IdentityAgent(QWidget *parent=0);
        void setText(const QString &);
        void updateStatus(const QVariantMap &properties);
        void setAllowedActions(bool allow_logagent, bool allow_pauseagent);
    protected:
        void contextMenuEvent(QContextMenuEvent *);
    signals:
        void setSystrayIcon(const QString &);
        void ipbxCommand(const QVariantMap &);
    private slots:
        void logout();
        void login();
        void pause();
        void unpause();
    private:
        void setStatusColors(const QString &);
        void setPausedColors(int, int);

        QGridLayout *m_layout;
        QLabel *m_icon;
        QLabel *m_text;
        QLabel *m_status;
        QLabel *m_statustxt;
        QLabel *m_pause;
        QLabel *m_pausetxt;
        QString m_agstatus; //!< agent status string
        bool m_allow_logagent;
        bool m_allow_pauseagent;
};

#endif

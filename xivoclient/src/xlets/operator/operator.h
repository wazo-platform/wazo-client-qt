/* XiVO Client
 * Copyright (C) 2007-2011, Avencall
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

#ifndef __STATUSPANEL_H__
#define __STATUSPANEL_H__

#include <QtGui>

#include <xlet.h>
#include <xletinterface.h>
#include <baseengine.h>
#include <xivoconsts.h>

/*! \brief Operator widget used to answer and transfer calls
 */
class XletOperator : public XLetExperimental
{
    Q_OBJECT

    public:
        XletOperator(QWidget *parent=0);

        enum Line {Ready, Ringing, Hangup, Wait, Transfer, WDTransfer, WITransfer, Online};
        void doGUIConnects(QWidget *mainwindow);

    public slots:
        void functionKeyPressed(int);
        void xferPressed();
        void clicked();

        void updateUserConfig(const QString &);
        void updateUserStatus(const QString &);
        void updatePhoneConfig(const QString &);
        void updatePhoneStatus(const QString &);
        void updateChannelStatus(const QString &);

    private:
        void updateLine(const QString &, const QStringList &);
        void newCall(const QString &);
        void dtransfer();
        void itransfer();
        void changeCurrentChannel(const QString &, const QString &);
        QString getPeerChan(const QString &chan) const;
        void removeLine(const QString &);

        QString m_xphoneid;
        QGridLayout * m_glayout;
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
};

class XLetOperatorPlugin : public QObject, XLetInterface
{
    Q_OBJECT
    Q_INTERFACES(XLetInterface)

    public:
        XLet *newXLetInstance(QWidget *parent=0);
};


#endif

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

#ifndef __SEARCHPANEL_H__
#define __SEARCHPANEL_H__

#include <QHash>
#include <QVariant>

#include <QPaintEvent>
#include "xlet.h"
#include "peeritem.h"

class QGridLayout;
class QVBoxLayout;

class BaseEngine;
class ExtendedLineEdit;
class UserInfo;

class SearchPanel : public XLet
{
    Q_OBJECT
    public:
        SearchPanel(QWidget *parent=0);
        void resizeEvent(QResizeEvent *);
        ~SearchPanel();
    public slots:
        void affTextChanged(const QString &);
        void removePeer(const QString &);
        void removePeers();
        void updateDisplay();
        void updateUserConfig(const QString &);
        void updateUserStatus(const QString &);
        void removeUserConfig(const QString &);
        void updatePhoneConfig(const QString &);
        void updatePhoneStatus(const QString &);
        void removePhoneConfig(const QString &);
        void initializationStarting();
        void initializationComplete();
    private:
        void disableLiveUpdate();
        void enableLiveUpdate();
        BasePeerWidget *findWidgetByPhoneXid(const QString &);
        bool isShown(const QString &) const;
        unsigned int maxDisplay() const { return b_engine->getConfig("guioptions.contacts-max").toUInt(); };
        QHash<QString, PeerItem *> m_peerhash;  //!< PeerItem hash
        QGridLayout *m_peerlayout;
        ExtendedLineEdit *m_input; //!< widget for search string input
        QScrollArea * m_scrollarea; //!< widget to see all or part of the contacts list
        
        QString m_searchpattern;
        static const unsigned peer_spacing = 6;
        bool m_live_reload_enabled;
    protected:
        virtual void paintEvent(QPaintEvent *event);
};

#endif

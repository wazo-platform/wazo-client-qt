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

#include <QVBoxLayout>
#include <QLabel>
#include <QScrollArea>
#include <QDebug>

#include "searchpanel.h"
#include "peerwidget.h"
#include "baseengine.h"
#include "extendedlineedit.h"
#include "userinfo.h"
#include "chitchat.h"

SearchPanel::SearchPanel(QWidget *parent)
    : XLet(parent)
{
    // qDebug() << Q_FUNC_INFO;
    setTitle(tr("Contacts"));
    ChitChatWindow::chitchat_instance = new ChitChatWindow();
    
    updateConf();

    QVBoxLayout *vlayout = new QVBoxLayout(this);
    vlayout->setMargin(0);
    QLabel *lbl = new QLabel(tr("N&ame or number to search :"), this);
    vlayout->addWidget(lbl, 0, Qt::AlignCenter);
    m_input = new ExtendedLineEdit(this);
    lbl->setBuddy(m_input);
    connect(m_input, SIGNAL(textChanged(const QString &)),
            this, SLOT(affTextChanged(const QString &)));
    vlayout->addWidget(m_input);

    QScrollArea *scrollarea = new QScrollArea(this);
    scrollarea->setWidgetResizable(true);
    QWidget *widget = new QWidget(scrollarea);
    widget->setObjectName("scroller");
    scrollarea->setWidget(widget);
    QVBoxLayout *scrollarealayout = new QVBoxLayout(widget);
    m_peerlayout = new QGridLayout;
    m_peerlayout->setMargin(0);
    m_peerlayout->setSpacing(6);
    scrollarealayout->addLayout(m_peerlayout);
    scrollarealayout->addStretch(1);
    vlayout->addWidget(scrollarea);

    m_searchpattern = "";

    // connect signal/slots
    connect(b_engine, SIGNAL(updateUserConfig(const QString &)),
            this, SLOT(updateUserConfig(const QString &)));
    connect(b_engine, SIGNAL(updateUserStatus(const QString &)),
            this, SLOT(updateUserStatus(const QString &)));
    connect(b_engine, SIGNAL(removeUserConfig(const QString &)),
            this, SLOT(removeUserConfig(const QString &)));
    connect(b_engine, SIGNAL(updatePhoneConfig(const QString &)),
            this, SLOT(updatePhoneConfig(const QString &)));
    connect(b_engine, SIGNAL(updatePhoneStatus(const QString &)),
            this, SLOT(updatePhoneStatus(const QString &)));
    connect(b_engine, SIGNAL(updateAgentConfig(const QString &)),
            this, SLOT(updateAgentConfig(const QString &)));
    connect(b_engine, SIGNAL(updateAgentStatus(const QString &)),
            this, SLOT(updateAgentStatus(const QString &)));

    connect(b_engine, SIGNAL(peersReceived()),
            this, SLOT(updateDisplay()));
    connect(b_engine, SIGNAL(delogged()),
            this, SLOT(removePeers()));
    
    connect(b_engine, SIGNAL(settingsChanged()),
            this, SLOT(updateConf()));
}

SearchPanel::~SearchPanel()
{
    // qDebug() << Q_FUNC_INFO;
    removePeers();
}

/*! \brief apply the search
 */
void SearchPanel::affTextChanged(const QString & text)
{
    m_searchpattern = text;
    updateDisplay();
}

/*! \brief update the list of Persons displayed
 */
void SearchPanel::updateDisplay()
{
    // first hide/delete everyonedisplayed
    QHashIterator<QString, PeerItem *> i(m_peerhash);
    while (i.hasNext()) {
        i.next();
        PeerItem *peeritem = i.value();
        BasePeerWidget *peerwidget = peeritem->getWidget();
        if ((peerwidget != NULL) &&
            (m_peerlayout->indexOf(peerwidget) > -1)) {
            m_peerlayout->removeWidget(peerwidget);
            peerwidget->hide();
            // originate
            // if switchboard : transfer, atxfer, hangup, intercept
            peeritem->setWidget(NULL);
            peerwidget->deleteLater();
        }
    }

    // then display all users whose name match the search pattern
    int naff = 0;
    i.toFront();
    while (i.hasNext()) {
        i.next();
        PeerItem * peeritem = i.value();
        BasePeerWidget * peerwidget = peeritem->getWidget();
        const UserInfo * userinfo = peeritem->userinfo();
        if (userinfo == NULL)
            continue;
        bool num_match = false;
        foreach (const QString & phonexid, userinfo->phonelist()) {
            if (const PhoneInfo * p = b_engine->phone(phonexid)) {
                if (p->number().contains(m_searchpattern)) {
                    num_match = true;
                    break;
                }
            }
        }
        bool name_match = userinfo->fullname().contains(
            m_searchpattern, Qt::CaseInsensitive);
        if ((name_match || num_match) && (naff < m_maxdisplay)) {
            if (peerwidget == NULL) {
                peerwidget = new PeerWidget(userinfo);
                peerwidget->updateAgentConfig(userinfo->xagentid());
                peerwidget->updateAgentStatus(userinfo->xagentid());
                foreach (QString xphoneid, userinfo->phonelist()) {
                    peerwidget->updatePhoneConfig(xphoneid);
                    peerwidget->updatePhoneStatus(xphoneid);
                }
                peeritem->setWidget(peerwidget);
                peeritem->updateDisplayedStatus();
                peeritem->updateDisplayedName();

                m_peerlayout->addWidget(peerwidget,
                                        naff / m_ncolumns,
                                        naff % m_ncolumns);

                naff++;
                peerwidget->show();
                // if switchboard : transfer, atxfer, hangup, intercept
            }
        }
    }
}

/*! \brief update display according to changes
 */
void SearchPanel::updateUserConfig(const QString & xuserid)
{
    PeerItem *peeritem = NULL;

    if (m_peerhash.contains(xuserid)) {
        peeritem = m_peerhash.value(xuserid);
    } else {
        const UserInfo * ui = b_engine->user(xuserid);
        peeritem = new PeerItem(ui);
        m_peerhash.insert(xuserid, peeritem);
    }
    peeritem->updateStatus();
}

void SearchPanel::removeUserConfig(const QString & xuserid)
{
    removePeer(xuserid);
}

void SearchPanel::updateUserStatus(const QString & xuserid)
{
    // qDebug() << Q_FUNC_INFO << xuserid << m_peerhash.keys();
    PeerItem * peeritem = NULL;
    if (m_peerhash.contains(xuserid)) {
        peeritem = m_peerhash.value(xuserid);
        peeritem->updateStatus();
    }
}

void SearchPanel::updateAgentConfig(const QString & xagentid)
{
    qDebug() << Q_FUNC_INFO << xagentid;
}

void SearchPanel::updateAgentStatus(const QString & xagentid)
{
    qDebug() << Q_FUNC_INFO << xagentid;
    // XXXX find xuserid
//     if (m_peerhash.contains(xuserid))
//         m_peerhash.value(xuserid)->updateAgentStatus(xagentid);
}

void SearchPanel::updatePhoneConfig(const QString & xphoneid)
{
    foreach (QString peerkey, m_peerhash.keys()) {
        PeerItem * peeritem = m_peerhash.value(peerkey);
        if (peeritem == NULL)
            continue;
        BasePeerWidget * peerwidget = peeritem->getWidget();
        if (peerwidget == NULL)
            continue;
        const UserInfo * userinfo = b_engine->user(peerkey);
        if (userinfo == NULL)
            continue;

        if (userinfo->phonelist().contains(xphoneid))
            peerwidget->updatePhoneConfig(xphoneid);
    }
}

void SearchPanel::updatePhoneStatus(const QString & xphoneid)
{
    foreach (QString peerkey, m_peerhash.keys()) {
        PeerItem * peeritem = m_peerhash.value(peerkey);
        if (peeritem == NULL)
            continue;
        BasePeerWidget * peerwidget = peeritem->getWidget();
        if (peerwidget == NULL)
            continue;
        const UserInfo * userinfo = b_engine->user(peerkey);
        if (userinfo == NULL)
            continue;

        if (userinfo->phonelist().contains(xphoneid))
            peerwidget->updatePhoneStatus(xphoneid);
    }
}

/*! \brief remove a peer
 */
void SearchPanel::removePeer(const QString & xuserid)
{
    // qDebug() << Q_FUNC_INFO << ext;
    if (m_peerhash.contains(xuserid)) {
        PeerItem * peeritem = m_peerhash.value(xuserid);
        BasePeerWidget * peerwidget = peeritem->getWidget();
        if (m_peerlayout->indexOf(peerwidget) > -1) {
            m_peerlayout->removeWidget(peerwidget);
        }
        m_peerhash.remove(xuserid);
        delete peerwidget; // peerwidget->deleteLater();
        return;
    }
}

/*! \brief clear the widget
 */
void SearchPanel::removePeers()
{
    // qDebug() << Q_FUNC_INFO;
    foreach (QString peerkey, m_peerhash.keys()) {
        PeerItem * peeritem = m_peerhash[peerkey];
        BasePeerWidget * peerwidget = peeritem->getWidget();
        if (peerwidget) {
                if (m_peerlayout->indexOf(peerwidget) > -1) {
                    m_peerlayout->removeWidget(peerwidget);
                }
                delete peerwidget;
        }
        delete peeritem;
    }
    m_peerhash.clear();
}

/*! \brief force the widget to be redrawn a least two time
 * to fix a bug occurring on qt ~4.5 where the contact list is not correctly redraw after one expose event
 */
void SearchPanel::paintEvent(QPaintEvent *)
{
    static int i = 0;

    if (i) {
        i = 0;
    } else {
        update();
        i = 1;
    }
}

/*! \brief reads the configuration from BaseEngine
 */
void SearchPanel::updateConf()
{
    m_maxdisplay = b_engine->getConfig("guioptions.contacts-max").toUInt();
    m_ncolumns = b_engine->getConfig("guioptions.contacts-width").toUInt();
    updateDisplay();
}

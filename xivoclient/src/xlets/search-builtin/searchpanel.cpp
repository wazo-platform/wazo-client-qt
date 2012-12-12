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

#include <QVBoxLayout>
#include <QLabel>
#include <QScrollArea>
#include <QDebug>

#include <peerwidget.h>
#include <baseengine.h>
#include <extendedlineedit.h>
#include <phoneinfo.h>
#include <userinfo.h>
#include <chitchat.h>
#include <peeritem.h>

#include "searchpanel.h"

SearchPanel::SearchPanel(QWidget *parent) :
    XLet(parent),
    m_live_reload_enabled(false)
{
    setTitle(tr("Contacts"));
    ChitChatWindow::chitchat_instance = new ChitChatWindow();

    QVBoxLayout *vlayout = new QVBoxLayout(this);
    vlayout->setMargin(0);
    QLabel *lbl = new QLabel(tr("N&ame or number to search :"), this);
    vlayout->addWidget(lbl, 0, Qt::AlignCenter);
    m_input = new ExtendedLineEdit(this);
    lbl->setBuddy(m_input);
    connect(m_input, SIGNAL(textChanged(const QString &)),
            this, SLOT(affTextChanged(const QString &)));
    vlayout->addWidget(m_input);

    m_scrollarea = new QScrollArea(this);
    m_scrollarea->setWidgetResizable(true);
    QWidget *widget = new QWidget(m_scrollarea);
    widget->setObjectName("scroller");
    m_scrollarea->setWidget(widget);
    QVBoxLayout *scrollarealayout = new QVBoxLayout(widget);
    m_peerlayout = new QGridLayout;
    m_peerlayout->setMargin(0);
    m_peerlayout->setSpacing(SearchPanel::peer_spacing);
    scrollarealayout->addLayout(m_peerlayout);
    scrollarealayout->addStretch(10);
    vlayout->addWidget(m_scrollarea);

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
    connect(b_engine, SIGNAL(removePhoneConfig(const QString &)),
            this, SLOT(removePhoneConfig(const QString &)));

    connect(b_engine, SIGNAL(delogged()),
            this, SLOT(removePeers()));

    connect(b_engine, SIGNAL(settingsChanged()),
            this, SLOT(updateDisplay()));

    connect(b_engine, SIGNAL(initialized()),
            this, SLOT(initializationComplete()));
    connect(b_engine, SIGNAL(initializing()),
            this, SLOT(initializationStarting()));
}

SearchPanel::~SearchPanel()
{
    removePeers();
}

void SearchPanel::initializationStarting()
{
    qDebug() << "DISABLING UPDATES";
    this->disableLiveUpdate();
}

void SearchPanel::initializationComplete()
{
    qDebug() << "ENABLING UPDATES";
    this->enableLiveUpdate();
}

void SearchPanel::enableLiveUpdate()
{
    qDebug() << "Initialization complete received...";
    this->m_live_reload_enabled = true;
    this->updateDisplay();
}

void SearchPanel::disableLiveUpdate()
{
    this->m_live_reload_enabled = false;
}

/*! \brief apply the search
 */
void SearchPanel::affTextChanged(const QString & text)
{
    m_searchpattern = text;
    updateDisplay();
}

void SearchPanel::resizeEvent(QResizeEvent *) {
    updateDisplay();
}

bool SearchPanel::isShown(const QString &xuserid) const
{
    PeerItem *peer = m_peerhash.value(xuserid, NULL);
    if (peer) {
        if (BasePeerWidget *widget = peer->getWidget()) {
            return widget->isVisible();
        }
    }
    return false;
}

/*! \brief update the list of Persons displayed
 */
void SearchPanel::updateDisplay()
{
    if (m_live_reload_enabled == false) {
        return;
    }

    // max number of peers displayed on the search panel
    unsigned maxdisplay = maxDisplay();
    // number of columns (0 = auto)
    unsigned ncolumns = b_engine->getConfig("guioptions.contacts-width").toUInt();
    if (ncolumns == 0) {
        ncolumns = m_scrollarea->width() /
            (PeerWidget::max_width + 2 * SearchPanel::peer_spacing);
    }
    // Prevent arithmetic exception
    if (ncolumns == 0) {
        ncolumns = 1;
    }

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
            peeritem->setWidget(NULL);
            peerwidget->deleteLater();
        }
    }

    // then display all users whose name match the search pattern
    unsigned naff = 0;
    i.toFront();
    while (i.hasNext()) {
        i.next();
        PeerItem * peeritem = i.value();
        BasePeerWidget * peerwidget = peeritem->getWidget();
        const UserInfo * userinfo = peeritem->userinfo();
        if (userinfo == NULL)
            continue;
        if (peeritem->matchPattern(m_searchpattern) && (naff < maxdisplay)) {
            if (peerwidget == NULL) {
                peerwidget = new PeerWidget(userinfo);
                if (! userinfo->agentid().isEmpty()) {
                    peerwidget->updateAgentConfig(userinfo->xagentid());
                    peerwidget->updateAgentStatus(userinfo->xagentid());
                }
                foreach (const QString &xphoneid, userinfo->phonelist()) {
                    peerwidget->updatePhoneConfig(xphoneid);
                    peerwidget->updatePhoneStatus(xphoneid);
                }
                peeritem->setWidget(peerwidget);
                peeritem->updateDisplayedStatus();
                peeritem->updateDisplayedName();

                m_peerlayout->addWidget(peerwidget,
                                        naff / ncolumns,
                                        naff % ncolumns);

                naff++;
                peerwidget->show();
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
        if (isShown(xuserid))
            updateDisplay();
    } else {
        const UserInfo * ui = b_engine->user(xuserid);
        peeritem = new PeerItem(ui);
        m_peerhash.insert(xuserid, peeritem);
        if (peeritem && (((unsigned int) m_peerlayout->count() < maxDisplay())
                         && peeritem->matchPattern(m_searchpattern))) {
            updateDisplay();
        }
    }
    peeritem->updateStatus();
}

void SearchPanel::removeUserConfig(const QString & xuserid)
{
    removePeer(xuserid);
}

void SearchPanel::updateUserStatus(const QString & xuserid)
{
    PeerItem * peeritem = NULL;
    if (m_peerhash.contains(xuserid)) {
        peeritem = m_peerhash.value(xuserid);
        peeritem->updateStatus();
    }
}

BasePeerWidget *SearchPanel::findWidgetByPhoneXid(const QString &xphoneid)
{
    if (const PhoneInfo * phone = b_engine->phone(xphoneid)) {
        QString userxid = phone->ipbxid() + "/" + phone->iduserfeatures();
        if (PeerItem * peeritem = m_peerhash.value(userxid)) {
            return peeritem->getWidget();
        }
    }
    return NULL;
}

void SearchPanel::updatePhoneConfig(const QString & xphoneid)
{
    if (BasePeerWidget * peerwidget = findWidgetByPhoneXid(xphoneid)) {
        peerwidget->updatePhoneConfig(xphoneid);
    }
}

void SearchPanel::updatePhoneStatus(const QString & xphoneid)
{
    if (BasePeerWidget * peerwidget = findWidgetByPhoneXid(xphoneid)) {
        peerwidget->updatePhoneStatus(xphoneid);
    }
}

void SearchPanel::removePhoneConfig(const QString & xphoneid) {
    foreach (PeerItem *peeritem, m_peerhash) {
        BasePeerWidget * peerwidget = peeritem->getWidget();
        if (peerwidget == NULL)
            continue;
        peerwidget->removePhoneConfig(xphoneid);
    }
}

void SearchPanel::removePeer(const QString & xuserid)
{
    if (m_peerhash.contains(xuserid)) {
        PeerItem * peeritem = m_peerhash.value(xuserid);
        BasePeerWidget * peerwidget = peeritem->getWidget();
        m_peerhash.remove(xuserid);
        if (this->isShown(xuserid)) {
            m_peerlayout->removeWidget(peerwidget);
            this->updateDisplay();
        }
        delete peerwidget;
        delete peeritem;
    }
}

void SearchPanel::removePeers()
{
    foreach (const QString &peerkey, m_peerhash.keys()) {
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

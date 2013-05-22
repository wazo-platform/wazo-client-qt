/* XiVO Client
 * Copyright (C) 2013, Avencall
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

#include <baseengine.h>
#include <xivoconsts.h>
#include <xletfactory.h>

#include "menu_availability.h"


MenuAvailability::MenuAvailability(MainWindow *parent)
    : m_menu_availability(parent->ui->menu_availability),
      m_availability_action_group(new QActionGroup(parent))
{
    this->m_availability_action_group->setExclusive(true);

    this->connect(b_engine, SIGNAL(localUserInfoDefined()), SLOT(updatePresence()));
    this->connect(b_engine, SIGNAL(updateUserStatus(const QString &)), SLOT(updateUserStatus(const QString &)));
    this->connect(b_engine, SIGNAL(logged()), SLOT(setStatusLogged()));
    this->connect(b_engine, SIGNAL(delogged()), SLOT(setStatusNotLogged()));
    this->connect(b_engine, SIGNAL(settingsChanged()), SLOT(confUpdated()));
}

MenuAvailability::~MenuAvailability()
{
}

void MenuAvailability::setStatusLogged()
{
    this->setMenuAvailabilityEnabled(true);
}

void MenuAvailability::setStatusNotLogged()
{
    this->setMenuAvailabilityEnabled(false);
    this->clearPresence();
}

void MenuAvailability::confUpdated()
{
    this->setMenuAvailabilityEnabled(true);
}

/*!
 * Fills the list of available availability states and select the current state
 */
void MenuAvailability::updatePresence()
{
    const QVariantMap & presencemap = b_engine->getOptionsUserStatus();
    foreach (const QString & presencestate, presencemap.keys()) {
        const QVariantMap & pdetails = presencemap.value(presencestate).toMap();
        const QString & longname = pdetails.value("longname").toString();
            if (! this->m_availabilitys.contains(presencestate)) {
                this->m_availabilitys[presencestate] = new QAction(longname, this);
                this->m_availabilitys[presencestate]->setProperty("availstate", presencestate);
                connect(this->m_availabilitys[presencestate], SIGNAL(triggered()), this, SLOT(setAvailability()));
                this->m_availability_action_group->addAction(this->m_availabilitys[presencestate]);
            }
        }
        this->m_menu_availability->addActions(this->m_availability_action_group->actions());
    this->syncPresence();
}

/*!
 * Disconnect signals for the availability menu and select the current state
 */
void MenuAvailability::syncPresence()
{
    if (const UserInfo * u = b_engine->getXivoClientUser()) {
        const QString & state = u->availstate();
        if (this->m_availabilitys.contains(state)) {
            this->checksAvailState();
        }
    }
}

void MenuAvailability::clearPresence()
{
    foreach (QAction *action, this->m_availabilitys) {
        disconnect(action, SIGNAL(triggered()), this, SLOT(setAvailability()));
        this->m_availability_action_group->removeAction(action);
        delete action;
    }
    this->m_availabilitys.clear();
    this->m_menu_availability->clear();
}

/*!
 * Change the selected state in the availability menu
 *
 * If this method is called without while the connections between the
 * m_avact[state] and b_engine are up, this will also update the state
 * on the server
 */
void MenuAvailability::checksAvailState()
{
    if (const UserInfo * u = b_engine->getXivoClientUser()) {
        const QString & state = u->availstate();
        if (! state.isEmpty() && this->m_availabilitys.contains(state)) {
            this->setEnabledMenus(state);
            this->m_availabilitys[state]->setChecked(true);
        }
    }
}

/*!
 * Enable or disable menu item according to the state
 * \param state The new state
 */
void MenuAvailability::setEnabledMenus(const QString & state)
{
    const QVariantMap & states = b_engine->getOptionsUserStatus();
    if (states.contains(state)) {
        const QStringList & allowed = states.value(state).toMap().value("allowed").toStringList();
        foreach (const QString & presence, this->m_availabilitys.keys()) {
            bool enabled = allowed.contains(presence);
            this->m_availabilitys[presence]->setCheckable(enabled);
            this->m_availabilitys[presence]->setEnabled(enabled);
        }
    }
}

/*!
 * \brief send the availability action clicked in the menu
 */
void MenuAvailability::setAvailability()
{
    QString availstate = sender()->property("availstate").toString();
    b_engine->setAvailState(availstate, false);
}

/*!
 * enables or disables the Availability menu
 */
void MenuAvailability::setMenuAvailabilityEnabled(bool enabled)
{
    if (enabled) {
        bool presence_enabled = b_engine->getConfig("checked_function.presence").toBool();
        this->m_menu_availability->setEnabled(presence_enabled);
    } else {
        this->m_menu_availability->setEnabled(false);
    }
}

/*!
 * Update the presence of the current user
 * \param userxid The user's XiVO id
 */
void MenuAvailability::updateUserStatus(const QString & userxid)
{
    if (b_engine->getFullId() == userxid) {
        const UserInfo * user = b_engine->getXivoClientUser();
        if (user && b_engine->getOptionsUserStatus().size()) {
            this->syncPresence();
        }
    }
}

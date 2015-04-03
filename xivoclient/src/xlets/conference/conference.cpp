/* XiVO Client
 * Copyright (C) 2007-2015 Avencall
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

#include "conference.h"
#include "conflist.h"
#include "conftab.h"

Conference::Conference(QWidget *parent)
    : XLet(parent, tr("Conference"), ":/images/tab-conference.svg")
{
    this->ui.setupUi(this);

    QFile qssFile(QString(":/default.qss"));
    if(qssFile.open(QIODevice::ReadOnly)) {
        this->setStyleSheet(qssFile.readAll());
    }

    this->ui.conf_tab->addTab(new ConfList(this->ui.conf_tab), tr("Room list"));
    this->ui.conf_tab->setTabsClosable(true);
    this->ui.conf_tab->tabBar()->setTabButton(0, QTabBar::RightSide, NULL);
    this->ui.conf_tab->tabBar()->setTabButton(0, QTabBar::LeftSide, NULL);

    registerMeetmeUpdate();
}

void Conference::registerMeetmeUpdate() const
{
    QVariantMap command;

    command["class"] = "subscribe";
    command["message"] = "meetme_update";

    b_engine->sendJsonCommand(command);
}


XLet* XLetConferencePlugin::newXLetInstance(QWidget *parent)
{
    b_engine->registerTranslation(":/obj/conference_%1");
    return new Conference(parent);
}

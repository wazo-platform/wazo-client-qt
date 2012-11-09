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

#include "conference.h"

Q_EXPORT_PLUGIN2(xletconferenceplugin, XLetConferencePlugin);

ConfTab::ConfTab(QWidget *parent)
    : QTabWidget(parent)
{
}

void ConfTab::closeTab(QWidget *w)
{
    int index;

    if (!w) {
        w = sender()->property("index").value<QWidget*>();
    }

    index = indexOf(w);
    removeTab(index);
    w->deleteLater();
}

int ConfTab::addClosableTab(QWidget *w, const QString &number)
{
    int index = addTab(w, number);

    QPushButton *p = new QPushButton("X");
    p->setFlat(true);
    p->setMaximumSize(12, 20);
    p->setFocusPolicy(Qt::NoFocus);
    p->setProperty("index", qVariantFromValue(w));
    widget(index)->setProperty("number", number);
    connect(p, SIGNAL(clicked()), this, SLOT(closeTab()));
    tabBar()->setTabButton(index, QTabBar::RightSide, p);

    return index;
}

void ConfTab::showConfRoom(const QString &number, const QVariantMap &members, bool force)
{
    int index = indexOf(number);

    if (index == -1) {
        index = addClosableTab(new ConfRoom(this, this, number, members), number);
    }

    setCurrentIndex(index);
}

int ConfTab::indexOf(const QString &number)
{
    for (int i = 1; i < count(); i++) {
        if (widget(i)->property("number").toString() == number) {
            return i;
        }
    }
    return -1;
}

XLet* XLetConferencePlugin::newXLetInstance(QWidget *parent)
{
    b_engine->registerTranslation(":/obj/conference_%1");
    return new XletConference(parent);
}

XletConference::XletConference(QWidget *parent)
    : XLet(parent)
{
    setTitle(tr("Conference"));

    QVBoxLayout *vLayout = new QVBoxLayout();
    setLayout(vLayout);

    m_tab = new ConfTab(this);
    m_tab->addTab(new ConfList(this), tr("Conference room list"));
    vLayout->addWidget(m_tab);

    registerMeetmeUpdate();
}

void XletConference::registerMeetmeUpdate() const
{
    QVariantMap command;

    command["class"] = "subscribe";
    command["message"] = "meetme_update";

    b_engine->sendJsonCommand(command);
}

void XletConference::openConfRoom(const QString &number, const QVariantMap &members, bool force)
{
    m_tab->showConfRoom(number, members, force);
}

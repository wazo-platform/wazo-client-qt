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

int ConfTab::addClosableTab(QWidget *w, const QString &title)
{
    int index = addTab(w, title);

    QPushButton *p = new QPushButton("X");
    p->setFlat(true);
    p->setMaximumSize(12, 20);
    p->setFocusPolicy(Qt::NoFocus);
    p->setProperty("index", qVariantFromValue(w));
    connect(p, SIGNAL(clicked()), this, SLOT(closeTab()));
    tabBar()->setTabButton(index, QTabBar::RightSide, p);
    return index;
}

/*! \brief Create and show a new tab
 * \param meetme_id The XiVO id of the meetme room
 * \param force Will add a new if true
 */
void ConfTab::showConfRoom(const QString & meetme_id, bool force)
{
    // qDebug() << Q_FUNC_INFO << meetme_id;
    int index = indexOf(meetme_id);
    if (index == -1) {
        const MeetmeInfo * m = b_engine->meetme(meetme_id);
        if (m) {
            if ((! force) && m->channels().size() == 0) {
                return;
            }
            QString title = QString("%0 (%1)").arg(m->name()).arg(m->number());
            index = addClosableTab(new ConfRoom(this, this, meetme_id), title);
        }
    }
    setCurrentIndex(index);
}

/*! \brief Finds the index of a tab for a given meetme XiVO id
 * \param xid The XiVO id of the meetme room
 * \return the index of the matching tab or -1
 */
int ConfTab::indexOf(const QString & xid)
{
    for (int i = 1; i < count(); i++) {
        if (widget(i)->property("id").toString() == xid) {
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
    // qDebug() << Q_FUNC_INFO;
    setTitle(tr("Conference"));

    QVBoxLayout *vLayout = new QVBoxLayout();
    setLayout(vLayout);

    m_tab = new ConfTab(this);
    m_tab->addTab(new ConfList(this), tr("Conference room list"));
    vLayout->addWidget(m_tab);

    connect(b_engine, SIGNAL(updateMeetmesStatus(const QString &)),
            this, SLOT(updateMeetmesStatus(const QString &)));
}

void XletConference::openConfRoom(const QString &id, bool force)
{
    m_tab->showConfRoom(id, force);
}

void XletConference::updateMeetmesStatus(const QString & meetme_id)
{
    // qDebug() << Q_FUNC_INFO << meetme_id;
    const MeetmeInfo * m = b_engine->meetme(meetme_id);
    if (m) {
        foreach (const QString & xcid, m->channels().keys()) {
            const UserInfo * chan_owner = b_engine->getUserForXChannelId(xcid);
            if (chan_owner && chan_owner->xid() == b_engine->getFullId()) {
                openConfRoom(meetme_id);
            }
        }
    }
}
